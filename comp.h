#ifndef COMP_H
#define COMP_H

#include <EIGEN_SETTINGS.h>
#include <stdio.h>
#include "layer.h"

class Comp : public Layer{
	int _xRes, _yRes;
	float _frame_rate;
	std::vector<Layer *> _layers; // How to free this? can I use delete[]?
public:
	Comp(int xRes, int yRes, float frame_rate): _xRes(xRes), _yRes(yRes), _frame_rate(frame_rate){}
	~Comp(){
		// Not sure about this one
		for(int i = 0; i < _layers.size(); i++){
			// delete _layers[i];
		}
	}

	void getDimensions(int& x, int& y){
		x = _xRes;
		y = _yRes;
	}

	void render(ImageBuffer * target, float frame_num){
		for(int i = 0; i < _layers.size(); i++){
			ImageBuffer layerBuffer(_xRes, _yRes);
			_layers[i]->render(&layerBuffer, frame_num);

			for(int y = 0; y < _yRes; y++){
				for(int x = 0; x < _xRes; x++){
					Pixel * pix_new = layerBuffer.getPixel(x, y);
					Pixel * pix_target = target->getPixel(x, y);

					VEC3 vec_new = pix_new->toVec3();
					VEC3 vec_target = pix_target->toVec3();

					// You would do blending stuff here but not right now
					pix_target->set(vec_new + vec_target);
				}
			}
		}

		

	}

	void addLayer(Layer * layer){
		_layers.push_back(layer);
	}

};

void renderCompToFolder(Comp * comp, int start_frame, int end_frame, char * folder){
	if(end_frame <= start_frame){
		ERROR("ERROR - LAYER - Need at least 1 frame to render layer");
		return;
	}

	int xRes, yRes;
	comp->getDimensions(xRes, yRes);
	ImageBuffer * buffer = new ImageBuffer(xRes, yRes);

	for(int frame = start_frame; frame < end_frame; frame++){
		buffer->clear();
		comp->render(buffer, frame);

		char name[100];
		sprintf(name, "%s/%04i.tif", folder, frame);
		buffer->writeTIFF(name);
	}

	// cleanup
	delete buffer;
}

#endif // COMP_H