#ifndef LAYER_H
#define LAYER_H

#include <EIGEN_SETTINGS.h>
#include "image_buffer.h"
#include <limits>

enum BlendMode{
	BLEND_NORMAL
};


class Layer{
	float _in_point, _out_point;
public:
	Layer(){
		_in_point = std::numeric_limits<float>::min();
		_out_point = std::numeric_limits<float>::max();
	}

	Layer(float in, float out){
		_in_point = in;
		_out_point = out;
	}

	float getInPoint(){return _in_point;}
	void setInPoint(float in_point){_in_point = in_point;}
	float getOutPoint(){return _out_point;}
	void setOutPoint(float out_point){_out_point = out_point;}

	virtual void render(ImageBuffer * target, float frame_num) = 0;
};


#endif // LAYER_H