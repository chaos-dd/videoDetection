
#ifndef UTILITIES_H_H
#define UTILITIES_H_H


#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>
#include "defines.h"
#include <opencv2/flann/flann.hpp>

#if (defined WIN32 || defined _WIN32)
#pragma warning(disable:4996)
#ifdef _EXPORTING
#define DLL_EXPORTS __declspec(dllexport)
#else
#define DLL_EXPORTS __declspec(dllimport)
#endif
#else
#define DLL_EXPORTS
#endif

namespace vd
{

    using std::vector;
    using std::pair;
    using std::string;
    using std::shared_ptr;
    using cv::Mat;
    using cv::KeyPoint;
    using cv::VideoCapture;
    using cv::DMatch;
    using cv::Size;
    
#define COLOR_SPACE_BGR 1
#define COLOR_SPACE_HLS 2
    
#define COLOR_SPACE_BGR 1
#define COLOR_SPACE_HLS 2

    class ColorHist
    {
    public:
        ColorHist()
        {
            m_binsNum[0] = 6;
            m_binsNum[1] = 3;
            m_binsNum[2] = 4;
        }
        ColorHist(int binsNum1, int binsNum2, int binsNum3)
        {
            m_binsNum[0] = binsNum1;
            m_binsNum[1] = binsNum2;
            m_binsNum[2] = binsNum3;

        }
        void computeFeat(Mat &src, Mat &hist, int colorSpaceType = COLOR_SPACE_BGR, int normType = cv::NORM_L1);
        int getFeatDim();

    private:
        void initSteps();

    public:
        int m_binsNum[3];
        int m_steps[3];
    };


    class DLL_EXPORTS Video_det_param
    {
    public:
        Video_det_param() :timeInterval(2), colorThres(0.2), orbThres(50), used_fps(2)
        {
        }
        int loadFromFile(string configFilePath);
    public:

        int timeInterval;
        float colorThres;
        float orbThres;
        float used_fps;
    };
       
    typedef cv::flann::Index FeatIndexType;


    DLL_EXPORTS int initModule(string _configFilePath);
    DLL_EXPORTS int detectSingleVideo(string videoName, string &strJsonResult, vector<string> &tarVideoNames );
    DLL_EXPORTS int detectMultipeVideos(vector<string> videoNames, vector<string> &jsonResult, vector<string> &tarVideoNames);
    DLL_EXPORTS int getVideoFrame(string videoName, int second, string frameName);
    DLL_EXPORTS int processSingleVideo(string videoName);
    DLL_EXPORTS int procBatchVideo(vector<string>  folders, string configFilePath);
    
    
    DLL_EXPORTS int detectVideoCitation(string detectVideoName, string targetVideoName, Video_det_param param, string &strJsonResult);

   
    //int ransac(vector<KeyPoint> &keys1, vector<KeyPoint> &keys2, vector<DMatch> &matches, vector<DMatch> &fined_matches, int threshold=5, int iteration=100);


    int buildVideoFeatIndex(Mat &descriptors, string videoPath, shared_ptr< FeatIndexType > &desc_index);
    int loadVideoIndex(string videoPath, Mat &descriptors, shared_ptr<FeatIndexType> &index);
    int exactFeatAndKFrame(string videoPath, Video_det_param param, Mat &colorFeat,
        vector<vector<KeyPoint>> & keypoints, Mat &descriptors, vector<int> &feat2FrameMap, bool saveData);
    int loadVideoFeat(string videoName, Mat &colorFeat, vector<vector<KeyPoint>> &keypoints, Mat & descriptors, vector<int> &feat2FrameMap);
}

#endif
