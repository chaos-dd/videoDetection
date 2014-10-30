
CXX=g++

CXXFLAGS= -std=c++11 -I$(OPENCVINCLUDE)
LDFLAGS=-L/home/ufo/cpp_code/videoDetection/dll_video_detect/ -ldll_video_detect #-Wl,rpath=../dll_video_detect/
OPENCVINCLUDE=/usr/local/include/opencv

ALL:main


main:main.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

.PHONY:clean
clean:
	-rm -f main
