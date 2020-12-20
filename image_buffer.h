#ifndef IMAGE_BUFFER_H
#define IMAGE_BUFFER_H

#include "pixel.h"
#include <EIGEN_SETTINGS.h>

class ImageBuffer{
	int _xRes, _yRes;
	Pixel * _pixels;
public:
	// Must be tiff
	ImageBuffer(std::string filename){
		TinyTIFFReaderFile * tif = TinyTIFFReader_open(filename.c_str());
		if(!tif){
			ERROR("Could not open file " << filename << " ...bailing");
			exit(0);
		}

		uint32_t wwidth=TinyTIFFReader_getWidth(tif);
		uint32_t hheight=TinyTIFFReader_getHeight(tif);
		if(!(wwidth>0 && hheight>0)){
			ERROR("File " << filename << " too small ...bailing");
			exit(0);
		}

		_xRes = wwidth;
		_yRes = hheight;
		_pixels = new Pixel[_xRes*_yRes];

		uint16_t sformat=TinyTIFFReader_getSampleFormat(tif);
    uint16_t bits=TinyTIFFReader_getBitsPerSample(tif, 0); // Assume sample 0 is representative (it should be)
    uint16_t samples = TinyTIFFReader_getSamplesPerPixel(tif);
    PRINT("Reading file: " << filename);
    PRINT("bits per sample = " << bits << ", sformat = " << sformat << ", Samples per pixel = " << samples);
    for(uint16_t sample = 0; sample < samples; sample++){
    	float * data = new float[_xRes*_yRes];

    	if (sformat==TINYTIFF_SAMPLEFORMAT_UINT) {
    		if (bits==8) TinyTIFFReader_readFrame<uint8_t, float>(tif, data, sample);
    		else if (bits==16) TinyTIFFReader_readFrame<uint16_t, float>(tif, data, sample);
    		else if (bits==32) TinyTIFFReader_readFrame<uint32_t, float>(tif, data, sample);
    		else {
    			ERROR("Could not read file: " << filename << " ...bailing");
    			exit(0);
    		}
    	} else if (sformat==TINYTIFF_SAMPLEFORMAT_INT) {
    		if (bits==8) TinyTIFFReader_readFrame<int8_t, float>(tif, data, sample);
    		else if (bits==16) TinyTIFFReader_readFrame<int16_t, float>(tif, data, sample);
    		else if (bits==32) TinyTIFFReader_readFrame<int32_t, float>(tif, data, sample);
    		else {
    			ERROR("Could not read file: " << filename << " ...bailing");
    			exit(0);
    		}
    	} else if (sformat==TINYTIFF_SAMPLEFORMAT_FLOAT) {
    		if (bits==32) TinyTIFFReader_readFrame<float, float>(tif, data, sample);
    		else {
    			ERROR("Could not read file: " << filename << " ...bailing");
    			exit(0);
    		}
    	} else {
    		ERROR("File " << filename << " has an unknown sformat ...bailing");
    		exit(0);
    	}


    	float max = pow(2, bits) - 1;
    	for(int y = 0; y < _yRes; y++){
    		for(int x = 0; x < _xRes; x++){
    			int index = y*_xRes+x;
    			if(sample == 0){
    				getPixel(x, y)->setR(data[index]/max);
		    	}else if(sample == 1){
		    		getPixel(x, y)->setG(data[index]/max);
		    	}else if(sample == 2){
		    		getPixel(x, y)->setB(data[index]/max);
		    	}else if(sample == 3){
		    		getPixel(x, y)->setA(data[index]/max);
		    	}
    		}
    	}

    	delete[] data;
    }

    if (TinyTIFFReader_wasError(tif)) {
    	ERROR("Encountered an error:");
    	ERROR(TinyTIFFReader_getLastError(tif));
    	ERROR("...bailing");
    	exit(0);
    }

    TinyTIFFReader_close(tif);
    PRINT("file " << filename << " read successfully")
	}

	ImageBuffer(int xRes, int yRes): _xRes(xRes), _yRes(yRes){
		_pixels = new Pixel[_xRes*_yRes]();
	}

	~ImageBuffer(){
		delete[] _pixels;
	}

	void clear(){
		for(int i = 0; i < _xRes*_yRes; i++){
			_pixels[i].clear();
		}
	}

	void getDimensions(int& x, int& y){
		x = _xRes;
		y = _yRes;
	}

	Pixel * getPixel(int x, int y){
		if(x < 0 || x >= _xRes || y < 0 || y >= _yRes){
			ERROR("ERROR - IMAGE_BUFFER - Can not get pixel (" << x << ", " << y << ") in ImageBuffer of size " << _xRes << "x" << _yRes);
			return nullptr;
		}

		return &_pixels[y*_xRes + x];
	}

	void setPixel(int x, int y, Pixel * pix){
		if(x < 0 || x >= _xRes || y < 0 || y >= _yRes){
			ERROR("ERROR - IMAGE_BUFFER - Can not set pixel (" << x << ", " << y << ") in ImageBuffer of size " << _xRes << "x" << _yRes);
			return;
		}

		_pixels[y*_xRes + x] = *pix;
	}

	void writeTIFF(const std::string& filename){
		TinyTIFFWriterFile * tiffw = TinyTIFFWriter_open(filename.c_str(), 8, TinyTIFFWriter_UInt, 3, _xRes, _yRes, TinyTIFFWriter_RGB);
		int totalCells = _xRes*_yRes;
		uint8_t * pixels = new uint8_t[3*totalCells];
		for(int i = 0; i < totalCells; i++){
			pixels[i*3 + 0] = clamp(0, 255, _pixels[i][0]*255);
			pixels[i*3 + 1] = clamp(0, 255, _pixels[i][1]*255);
			pixels[i*3 + 2] = clamp(0, 255, _pixels[i][2]*255);
		}

		TinyTIFFWriter_writeImage(tiffw, pixels);
		TinyTIFFWriter_close(tiffw);
		delete[] pixels;

		PRINT("Wrote file " << filename << " successfully");
	}
};



#endif // IMAGE_BUFFER_H