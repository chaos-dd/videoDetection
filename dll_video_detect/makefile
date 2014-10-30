
CXX=g++
CXXFLAGS= -std=c++11 -fPIC  -I$(OPENCVINCLUDE)
LDFLAGS= -L/usr/local/lib $(foreach var,$(OPENCVLIBS),-l$(var))  $(foreach var,$(BOOSTLIBS),-l$(var))

SRC=dll_export_utilities.cpp video_det_utilities.cpp video_proc_utilites.cpp

OPENCVINCLUDE=/usr/local/include/opencv
OPENCVLIBS=opencv_imgproc opencv_features2d opencv_highgui
BOOSTLIBS=boost_system boost_filesystem


.PHONY:ALL
ALL: libdll_video_detect.so

libdll_video_detect.so:$(SRC)
	$(CXX) $(CXXFLAGS) -shared $(LDFLAGS) $(SRC) -o $@

#$(OBJS):$(SRC)
#	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY:clean
clean:
	-rm -f $(OBJS) libdll_video_detect.so
