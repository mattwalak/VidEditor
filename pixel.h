#ifndef PIXEL_H
#define PIXEL_H

#include <EIGEN_SETTINGS.h>

class Pixel{
	float _r, _g, _b, _a;
public:
	Pixel(float r, float g, float b, float a): _r(r), _g(g), _b(b), _a(a) {}
	Pixel(float r, float g, float b): _r(r), _g(g), _b(b), _a(1) {}
	Pixel(): _r(0), _g(0), _b(0), _a(1) {}

	float getR() {return _r;}
	float getG() {return _g;}
	float getB() {return _b;}
	float getA() {return _a;}
	void setR(float r) {_r = r;}
	void setG(float g) {_g = g;}
	void setB(float b) {_b = b;}
	void setA(float a) {_a = a;}

	void clear(){
		_r = 0;
		_g = 0;
		_b = 0;
		_a = 1;
	}

	float operator[](int i){
		if(i == 0){
			return _r;
		}else if(i == 1){
			return _g;
		}else if(i == 2){
			return _b;
		}else if(i == 3){
			return _a;
		}else{
			ERROR("ERROR - PIXEL - Can not retrieve i = " << i << " with [] operator");
			return -1;
		}
	}

	void set(float r, float g, float b){
		_r = r;
		_g = g;
		_b = b;
	}

	void set(float r, float g, float b, float a){
		_r = r;
		_g = g;
		_b = b;
		_a = a;
	}

	void set(VEC3 col){
		_r = col[0];
		_g = col[1];
		_b = col[2];
		_a = 1;
	}

	/*
	void set(VEC4 col){
		_r = col[0];
		_g = col[1];
		_b = col[2];
		_a = col[3];
	}*/

	VEC3 toVec3(){
		return VEC3(_r, _g, _b);
	}

	VEC4 toVec4(){
		return VEC4(_r, _g, _b, _a);
	}
};

#endif // PIXEL_H