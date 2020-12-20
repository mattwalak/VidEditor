#ifndef KEYFRAME_H
#define KEYFRAME_H

#include <EIGEN_SETTINGS.h>
#include <stdio.h>
#include "funmath.h"

#define BEZIER_LOOPS 16

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////BEZIER STUFF////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

class Bezier2{
	VEC2 P0 = VEC2(0.0f, 0.0f);
	VEC2 P1;
	VEC2 P2 = VEC2(1.0f, 1.0f);
public:
	Bezier2(){
		P1 = VEC2(0,0);
	}

	Bezier2(VEC2 control_1){
		init(control_1);
	}

	void init(VEC2 control_1){
		if(control_1[0] > 1 || control_1[0] < 0){
			ERROR("ERROR - BEZIER2 - Can not set control_1 to (" << control_1[0] << ", " << control_1[1] << ")");
		}

		P1 = control_1;
	}

	float tFromX(float x){
		if(x < 0 || x > 1){
			ERROR("x is not in [0, 1]");
			return -1;
		}

		float a = P1[0];
		if(a == 0.5f)
			return a;
		else
			return (-2*a + sqrt(4*pow(a, 2) + 4*(1-2*a)*x))/(2-4*a);
	}

	VEC2 bezier(float t){
		if(t < 0 || t > 1){
			ERROR("t is not in [0, 1]");
			return VEC2(0, 0);
		}

		return pow(t, 2)*(P0 - 2*P1 + P2) + t*(2*P1) + P0;
	}
	
	float yFromX(float x){
		if(x == 0)
			return 0;
		else if(x == 1)
			return 1;

		// Binary search method now woooooooohooo
		float t = 0.5;
		for(int i = 1; i <= BEZIER_LOOPS; i++){
			VEC2 sample = bezier(t);
			if(sample[0] > x){
				t -= pow(0.5f, i+1);
			}else{
				t += pow(0.5f, i+1);
			}
		}

		return bezier(t)[1];
	}

	void renderPreview(char * name){
		int res = 250;
		ImageBuffer buffer(res, res);
		for(int i = 0; i < res; i++){
			float x = i/(res-1.0f);
			float y = yFromX(x);
			buffer.getPixel(i, (res - 1)-y*(res-1))->set(255, 0, 0);
		}
		buffer.writeTIFF(name);
	}
};

class Bezier3{
	VEC2 P0 = VEC2(0, 0);
	VEC2 P1;
	VEC2 P2;
	VEC2 P3 = VEC2(1, 1);
public:
	Bezier3(){
		P1 = VEC2(0, 0);
		P2 = VEC2(1, 1);
	}

	Bezier3(VEC2 control_1, VEC2 control_2){
		init(control_1, control_2);
	}

	void init(VEC2 control_1, VEC2 control_2){
		if(control_1[0] > 1 || control_1[0] < 0){
			ERROR("ERROR - BEZIER2 - Can not set control_1 to (" << control_1[0] << ", " << control_1[1] << ")");
		}

		if(control_2[0] > 1 || control_2[0] < 0){
			ERROR("ERROR - BEZIER2 - Can not set control_2 to (" << control_2[0] << ", " << control_2[1] << ")");
		}

		P1 = control_1;
		P2 = control_2;
	}

	VEC2 bezier(float t){
		return 3*pow(1-t, 2)*t*P1 + 3*(1-t)*pow(t, 2)*P2+pow(t, 3)*P3;
	}

	float yFromX(float x){
		if(x == 0)
			return 0;
		else if(x == 1)
			return 1;

		// Binary search method now woooooooohooo
		float t = 0.5;
		for(int i = 1; i <= BEZIER_LOOPS; i++){
			VEC2 sample = bezier(t);
			if(sample[0] > x){
				t -= pow(0.5f, i+1);
			}else{
				t += pow(0.5f, i+1);
			}
		}

		return bezier(t)[1];
	}

	void renderPreview(char * name){
		int res = 250;
		ImageBuffer buffer(res, res);
		for(int i = 0; i < res; i++){
			float x = i/(res-1.0f);
			float y = yFromX(x);
			buffer.getPixel(i, (res - 1)-y*(res-1))->set(255, 0, 0);
		}
		buffer.writeTIFF(name);
	}

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////INTERPOLATOR STUFF/////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


class Interpolator{
public:
	virtual float interpolate(float t) = 0;
};

class Interpolator_Linear : public Interpolator{
public:
	float interpolate(float t){
		return t;
	}
};

class Interpolator_Bezier2 : public Interpolator{
	Bezier2 bezier2;
public:
	Interpolator_Bezier2(VEC2 control_1){
		bezier2.init(control_1);
	}

	float interpolate(float t){
		float result = bezier2.yFromX(t);
		return result;
	}
};

class Interpolator_Bezier3 : public Interpolator{
	Bezier3 bezier3;
public:
	Interpolator_Bezier3(VEC2 control_1, VEC2 control_2){
		bezier3.init(control_1, control_2);
	}

	float interpolate(float t){
		float result = bezier3.yFromX(t);
		return result;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////KEYFRAME STUF////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Float_Keyframe{
	float _frame;
	float _value;
	Interpolator * _interpolator;

	Float_Keyframe(float frame, float value, Interpolator * interpolator): _frame(frame), _value(value){
		_interpolator = interpolator;
	}
	~Float_Keyframe(){
		delete _interpolator;
	}

	bool operator<(const Float_Keyframe& b){
		return _frame < b._frame;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////ANIMATOR STUF////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

class Float_Animator{
	std::vector<Float_Keyframe *> _keyframes;
public:
	Float_Animator(){}
	~Float_Animator(){
		for(int i = 0; i < _keyframes.size(); i++){
			delete _keyframes[i];
		}
	}

	// Bezier3 -------------------------------------------------------
	void addBezier3Keyframe(float frame, float value, VEC2 control_1, VEC2 control_2){
		Interpolator * interp = new Interpolator_Bezier3(control_1, control_2);
		_keyframes.push_back(new Float_Keyframe(frame, value, interp));
		std::sort(_keyframes.begin(), _keyframes.end());
	}

	void addBezier3Keyframe(float frame, float value, float influence_1, float influence_2){
		Interpolator * interp = new Interpolator_Bezier3(VEC2(influence_1, 0), VEC2(1-influence_2, 1));
		_keyframes.push_back(new Float_Keyframe(frame, value, interp));
		std::sort(_keyframes.begin(), _keyframes.end());
	}

	// Bezier2 -------------------------------------------------------
	void addBezier2Keyframe(float frame, float value, VEC2 control_1){
		Interpolator * interp = new Interpolator_Bezier2(control_1);
		_keyframes.push_back(new Float_Keyframe(frame, value, interp));
		std::sort(_keyframes.begin(), _keyframes.end());
	}

	void addBezier2Keyframe(float frame, float value, float influence_1){
		Interpolator * interp = new Interpolator_Bezier2(VEC2(influence_1, 0));
		_keyframes.push_back(new Float_Keyframe(frame, value, interp));
		std::sort(_keyframes.begin(), _keyframes.end());
	}

	// Linear -------------------------------------------------------
	void addLinearKeyframe(float frame, float value){
		Interpolator * interp = new Interpolator_Linear();
		_keyframes.push_back(new Float_Keyframe(frame, value, interp));
		std::sort(_keyframes.begin(), _keyframes.end());
	}

	float interpolate(float frame){
		if(_keyframes.size() == 0){
			ERROR("ERROR - KEYFRAME - Can't interpolate with no keyframes");
			return -1;
		}

		int i = 0;
		while(i < _keyframes.size() && frame >= _keyframes[i]->_frame)
			i++;
		int next_frame = i;
		if(next_frame == _keyframes.size())
			next_frame--;
		int previous_frame = i-1;
		if(previous_frame < 0)
			previous_frame = 0;

		float percent;
		Float_Keyframe * previous = _keyframes[previous_frame];
		Float_Keyframe * next = _keyframes[next_frame];

		if(next_frame != previous_frame){
			percent = (frame - previous->_frame) / (next->_frame - previous->_frame);
			percent = previous->_interpolator->interpolate(percent);
		}else{
			percent = 1;
		}

		return LERP(previous->_value, next->_value, percent);
	}
};

#endif // KEYFRAME_H