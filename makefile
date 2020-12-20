all: effect

effect: effect.cpp
	g++ -std=c++14 -w effect.cpp -o effect -I/mnt/c/Include -L/usr/local/lib -lTinyTIFF_Release
	touch effect.cpp

clean:
	rm effect