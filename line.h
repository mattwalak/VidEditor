#ifndef LINE_H
#define LINE_H

#include <stdio.h>
#include "layer.h"
#include "pixel.h"
#include "keyframe.h"

struct Bresenham{
	int x0, y0, x1, y1;
};


struct AnimatedBresenham{
	Float_Animator *x0, *y0, *x1, *y1;
};

class Lines: public Layer{
	std::vector<Bresenham> bresenhams;
	std::vector<AnimatedBresenham> animBresenhams;
public:
	Lines(){}
	~Lines(){}

	void addAnimatedBresenham(Float_Animator * x0, Float_Animator * y0, Float_Animator * x1, Float_Animator * y1){
		AnimatedBresenham b;
		b.x0 = x0;
		b.y0 = y0;
		b.x1 = x1;
		b.y1 = y1;
		animBresenhams.push_back(b);
	}

	void addBresenham(int x0, int y0, int x1, int y1){
		Bresenham b;
		b.x0 = x0;
		b.y0 = y0;
		b.x1 = x1;
		b.y1 = y1;
		bresenhams.push_back(b);
	}

	void paintPixel(ImageBuffer * target, int x, int y){
		int xRes, yRes;
		target->getDimensions(xRes, yRes);
		if(!(x < 0 || x >= xRes || y < 0 || y >= yRes)){
			target->getPixel(x, yRes - y - 1)->set(255, 0, 0);
		}
	}

	void renderBresenhams(ImageBuffer * target, float frame_num){
		for(int i = 0; i < bresenhams.size(); i++){
			// We don't want to mess with the data every time render is called
			int x0 = bresenhams[i].x0;
			int x1 = bresenhams[i].x1;
			int y0 = bresenhams[i].y0;
			int y1 = bresenhams[i].y1;

			// Make sure slope <= 1
			float slope = (float)(y1-y0)/(x1-x0);
			bool steep = (abs(slope) > 1);
			if(steep){
				std::swap(x0, y0);
				std::swap(x1, y1);
				slope = 1.0f/slope;
			}

			// Make sure we go left to right
			if(x1 < x0){
				std::swap(x1, x0);
				std::swap(y1, y0);
			}

			int target_x;
			int target_y;
			int direction;

			if(slope < 0){
				target_x = x1;
				target_y = y1;
				direction = -1;
				slope = -slope;
			}else{
				target_x = x0;
				target_y = y0;
				direction = 1;
			}

			int range = abs(x1 - x0);
			float error = 0.5f;
			for(int i = 0; i < range; i++){
				if(error >= 1){
					error -= 1;
					target_y++;
				}

				if(steep)
					paintPixel(target, target_y, target_x);
				else
					paintPixel(target, target_x, target_y);	

				target_x += direction;
				error += slope;
			}
		}
	}

	void renderAnimBresenhams(ImageBuffer * target, float frame_num){
		for(int i = 0; i < animBresenhams.size(); i++){
			// We don't want to mess with the data every time render is called
			int x0 = animBresenhams[i].x0->interpolate(frame_num);
			int x1 = animBresenhams[i].x1->interpolate(frame_num);
			int y0 = animBresenhams[i].y0->interpolate(frame_num);
			int y1 = animBresenhams[i].y1->interpolate(frame_num);

			// Make sure slope <= 1
			float slope = (float)(y1-y0)/(x1-x0);
			bool steep = (abs(slope) > 1);
			if(steep){
				std::swap(x0, y0);
				std::swap(x1, y1);
				slope = 1.0f/slope;
			}

			// Make sure we go left to right
			if(x1 < x0){
				std::swap(x1, x0);
				std::swap(y1, y0);
			}

			int target_x;
			int target_y;
			int direction;

			if(slope < 0){
				target_x = x1;
				target_y = y1;
				direction = -1;
				slope = -slope;
			}else{
				target_x = x0;
				target_y = y0;
				direction = 1;
			}

			int range = abs(x1 - x0);
			float error = 0.5f;
			for(int i = 0; i < range; i++){
				if(error >= 1){
					error -= 1;
					target_y++;
				}

				if(steep)
					paintPixel(target, target_y, target_x);
				else
					paintPixel(target, target_x, target_y);	

				target_x += direction;
				error += slope;
			}
		}
	}

	void render(ImageBuffer * target, float frame_num){
		for(int i = 0; i < bresenhams.size(); i++){
			renderBresenhams(target, frame_num);
		}

		for(int i = 0; i < animBresenhams.size(); i++){
			renderAnimBresenhams(target, frame_num);
		}

	}
};

#endif // LINE_H