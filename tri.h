#ifndef TRI_H
#define TRI_H

#include <stdio.h>
#include "layer.h"
#include "pixel.h"
#include "funmath.h"
#include "keyframe.h"


class Geometry{
	
public:
	Geometry(){

	}
	virtual ~Geometry(){}
	virtual bool getColor(const VEC2& pos, VEC3 * col_out) = 0;
};


class Tri : public Geometry{
	VEC2 P0, P1, P2;
	VEC2 TEX0, TEX1, TEX2;
	VEC3 col;
public:
	Tri(){}

	Tri(VEC2 P0_in, VEC2 P1_in, VEC2 P2_in): 
		P0(P0_in), P1(P1_in), P2(P2_in){
			col = VEC3(255, 0, 0);
		}

	VEC2 getUV(const VEC3& barryCoords){
		return TEX0*barryCoords[0] + TEX1*barryCoords[1] + TEX2*barryCoords[2];
	}

	void setTexCoords(const VEC2& TEX0_in, const VEC2& TEX1_in, const VEC2& TEX2_in){
		TEX0 = TEX0_in;
		TEX1 = TEX1_in;
		TEX2 = TEX2_in;
	}

	void setColor(VEC3 col_in){
		col = col_in;
	}

	void getBarryCoords(const VEC2& Q, VEC3 * coords){
		VEC3 qa = extend(Q-P0, 0);
		VEC3 qb = extend(Q-P1, 0);
		VEC3 qc = extend(Q-P2, 0);

		VEC3 pa = extend(P2-P1, 0);
		VEC3 pb = extend(P2-P0, 0);
		VEC3 pc = extend(P1-P0, 0);

		float area = triArea(pc, pb);
		float A = triArea(pa, qb);
		float B = triArea(qa, pb);
		float C = triArea(pc, qa);

		*coords = VEC3(A/area, B/area, C/area);
	}

	// Same as above, just returns a value
	bool barryCoordIntersectTest(const VEC2& Q, VEC3 * coords){
		getBarryCoords(Q, coords);
		return ((*coords)[0] >= 0 && (*coords)[1] >= 0 && (*coords)[2] >= 0);
	}

	bool getColor(const VEC2& pos, VEC3 * col_out){
		VEC3 coords;
		getBarryCoords(pos, &coords);
		if(coords[0] >= 0 && coords[1] >= 0 && coords[2] >= 0){
			*col_out = col;
			return true;
		}else{
			return false;
		}
	}
};

class Quad : public Geometry{
	Tri t1, t2;
public:
	Quad(VEC2 P0, VEC2 P1){
		if(P1[0] <= P0[0] || P1[1] <= P0[1]){
			ERROR("ERROR - QUAD - P1 must be greater than P0");
			return;
		}

		t1 = Tri(P1, P0, VEC2(P0[0], P1[1]));
		t1.setTexCoords(VEC2(1, 0), VEC2(0, 1), VEC2(0, 0));
		t2 = Tri(P1, VEC2(P1[0], P0[1]), P0);
		t2.setTexCoords(VEC2(1, 0), VEC2(1, 1), VEC2(0, 1));
	}

	bool getColor(const VEC2& pos, VEC3 * col_out){
		if(t1.getColor(pos, col_out)){
			return true;
		}else if(t2.getColor(pos, col_out)){
			return true;
		}else{
			return false;
		}
	}
};

class Texture : public Geometry{
	Tri t1, t2;
	ImageBuffer * sourceTexture;
public:
	Texture(VEC2 P0, VEC2 P1, char * source){
		if(P1[0] <= P0[0] || P1[1] <= P0[1]){
			ERROR("ERROR - TEXTURE - P1 must be greater than P0");
			return;
		}

		t1 = Tri(P1, P0, VEC2(P0[0], P1[1]));
		t1.setTexCoords(VEC2(1, 0), VEC2(0, 1), VEC2(0, 0));
		t2 = Tri(P1, VEC2(P1[0], P0[1]), P0);
		t2.setTexCoords(VEC2(1, 0), VEC2(1, 1), VEC2(0, 1));

		sourceTexture = new ImageBuffer(source);
	}

	virtual ~Texture(){
		delete sourceTexture;
	}

	bool getColor(const VEC2& pos, VEC3 * col_out){
		VEC3 barryCoords;
		Tri * hit;
		bool intersects = false;
		if(t1.barryCoordIntersectTest(pos, &barryCoords)){
			intersects = true;
			hit = &t1;
		}else if(t2.barryCoordIntersectTest(pos, &barryCoords)){
			intersects = true;
			hit = &t2;
		}

		if(intersects){
			int xRes, yRes;
			sourceTexture->getDimensions(xRes, yRes);
			VEC2 uv = hit->getUV(barryCoords);

			int x_sample = (int) (uv[0]*(xRes-1));
			int y_sample = (int) (uv[1]*(yRes-1));

			*col_out = sourceTexture->getPixel(xRes-x_sample-1, yRes-y_sample-1)->toVec3();
			return true;
		}else{
			return false;
		}
	}
};

class Shapes : public Layer{
	std::vector<Geometry *> geo;
public:
	// Each object should manage its own memory... think about that...
	// Maybe addTri should allocate its own memory for a tri object
	Shapes(){}
	~Shapes(){
		for(int i = 0; i < geo.size(); i++){
			delete geo[i];
		}
	}


	void render(ImageBuffer * target, float frame_num){
		int xRes, yRes;
		target->getDimensions(xRes, yRes);
		for(int y = 0; y < yRes; y++){
			for(int x = 0; x < xRes; x++){
				for(int i = 0; i < geo.size(); i++){

					VEC3 col;
					Geometry * geometry = geo[i];
					if(geometry->getColor(VEC2(x, y), &col)){
						target->getPixel(x, y)->set(col);
					}

				}
			}
		}
	}

	void addTri(VEC2 P0, VEC2 P1, VEC2 P2, VEC3 col){
		Tri * tri = new Tri(P0, P1, P2);
		tri->setColor(col);
		geo.push_back(tri);
	}

	void addQuad(VEC2 P0, VEC2 P1){
		Quad * quad = new Quad(P0, P1);
		geo.push_back(quad);
	}

	void addTexture(VEC2 P0, VEC2 P1, char * source){
		Texture * tex = new Texture(P0, P1, source);
		geo.push_back(tex);
	}

};

#endif // TRI_H