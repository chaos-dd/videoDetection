#include <iostream>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/opencv.hpp>

#include <opencv2/nonfree/features2d.hpp>
#include <time.h>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <opencv2/flann/flann.hpp>
#include <stdio.h>
#define _EXPORTING
#include "video_dll.h"

namespace vd
{
    using std::cout;
    using std::endl;
    using std::to_string;


    using cv::Point;
    using cv::Vec3b;
    using cv::Mat;
    using cv::imread;
    using cv::KeyPoint;
    using cv::DMatch;
    using cv::Size;
    using cv::ORB;
    using cv::waitKey;
    using cv::RNG;
    using cv::Point2f;
    using cv::BFMatcher;
    using cv::NORM_HAMMING;

    using boost::filesystem::exists;
    using boost::filesystem::create_directory;
    using boost::filesystem::directory_iterator;
    using boost::property_tree::ptree;


    /************************************************************************/
    /* 颜色直方图类实现                                                                     */
    /************************************************************************/
    void ColorHist::initSteps()
    {
        m_steps[0] = std::ceil((float)256 / m_binsNum[0]);
        m_steps[1] = std::ceil((float)256 / m_binsNum[1]);
        m_steps[2] = std::ceil((float)256 / m_binsNum[2]);
    }

    void ColorHist::computeFeat(Mat &src, Mat &hist, int colorSpaceType, int normType)
    {
        assert(src.channels() == 3);
        assert(src.type() == CV_8UC3);

        hist = Mat::zeros(1, getFeatDim(), CV_32FC1);

        Mat img;

        if (colorSpaceType == COLOR_SPACE_HLS)
        {
            cvtColor(src, img, CV_BGR2HLS);
            for (int r = 0; r < img.rows; ++r)
            {
                for (int c = 0; c < img.cols; ++c)
                {
                    int tmp = img.at<cv::Vec3b>(r, c)[0];
                    img.at<cv::Vec3b>(r, c)[0] = (float)tmp * 2 / 360 * 255;
                }
            }
        }
        else
        {
            img = src;
        }

        initSteps();


        for (int r = 0; r < img.rows; ++r)
        {
            for (int c = 0; c < img.cols; ++c)
            {

                cv::Vec3b index = img.at<cv::Vec3b>(r, c);
                index[0] = index[0] / m_steps[0];
                index[1] = index[1] / m_steps[1];
                index[2] = index[2] / m_steps[2];

                //根据颜色量化bin确定在结果直方图中的位置
                int location = index[0] * m_binsNum[1] * m_binsNum[2] + index[1] * m_binsNum[2] + index[2];
                hist.at<float>(location) += 1;
            }
        }

        //L1 normalization

        if (normType == -1)
        {
            return;
        }
        normalize(hist, hist, 1.0, 0, normType);
}


    int ColorHist::getFeatDim()
    {
        return m_binsNum[0] * m_binsNum[1] * m_binsNum[2];
    }


    /*****************************************************************
    parameter class of video detection algorithm

    ****************************************************************/
    int Video_det_param::loadFromFile(string configFilePath)
    {
        if (configFilePath.empty())
        {
            cout << endl << "未指定 配置文件 ";

            return -1;
        }

        ptree pt;
        read_xml(configFilePath, pt, boost::property_tree::xml_parser::trim_whitespace);

        try
        {
            colorThres = pt.get<float>("config.colorThres");
            if (colorThres<0.0 || colorThres>0.5f)
            {
                colorThres = 0.2;
            }

            used_fps = pt.get<float>("config.used_fps");
            if (used_fps > 10 || used_fps<0.2)
            {
                used_fps = 1;
            }

            orbThres = pt.get<float>("config.orbThres");

            if (orbThres > 200 || orbThres < 10)
            {
                orbThres = 50;
            }
            timeInterval = pt.get<int>("config.timeInterval");
            if (timeInterval>10 || timeInterval<2)
            {
                timeInterval = 2;
            }
        }
        catch (boost::property_tree::ptree_error &e)
        {
            cout << endl << "读取检测算法参数错误！\n";
            cout << endl << e.what() << endl;

            return -1;
        }

        return 0;
    }

    int exactFeatAndKFrame(string videoPath, Video_det_param param, Mat &colorFeat,
                           vector<vector<KeyPoint>> & keypoints, Mat &descriptors,vector<int> &feat2FrameMap, bool saveData)
    {
        boost::filesystem::path vn(videoPath);
        string nm = vn.string();
        VideoCapture cap(nm);

        if (!cap.isOpened())
        {
            cout << nm << "  打开失败" << endl;
            return -1;
        }
        double fps = cap.get(CV_CAP_PROP_FPS);
        double totalFrameNum = cap.get(CV_CAP_PROP_FRAME_COUNT);

        ColorHist colorHist;

        int MaxKeyPointNum = 500;
        ORB orb_detector(MaxKeyPointNum);

        //clock_t start, end;

        //start = clock();
        //exact color feature and orb features of frames

        vector<Mat> vec_colorFeat;
        vector<Mat> vec_desc;
        vec_colorFeat.reserve(totalFrameNum / fps*param.used_fps);
        vec_desc.reserve(totalFrameNum / fps *param.used_fps);

        keypoints.clear();
        feat2FrameMap.clear();

        int totalDescNum = 0;
        Mat frame;
        int validFrameInd= 0;
        for (int frameInd = 0; frameInd < totalFrameNum; frameInd += fps / param.used_fps)
        {
            cap.set(CV_CAP_PROP_POS_FRAMES, frameInd);
            if (!cap.read(frame) && frame.empty())
            {
                continue;
            }

            vector<KeyPoint> keys;
            Mat desc;
            orb_detector(frame, Mat(), keys, desc);

            if (keys.size()==0)
            {
                continue;
            }
            keypoints.push_back(keys);
            vec_desc.push_back(desc);

            //char fileName[100] = { 0 };
            //sprintf(fileName, "%s_%d.jpg", videoPath.c_str(), validFrameInd);
            //imwrite(fileName, frame);


            Mat hist;
            colorHist.computeFeat(frame, hist, COLOR_SPACE_BGR, cv::NORM_L1);
            vec_colorFeat.push_back(hist);


            // prepare for feat map
            for (int i = 0; i!= keys.size();++i)
            {
                feat2FrameMap.push_back(validFrameInd);
            }
            validFrameInd++;
            totalDescNum += desc.rows;
        }

        if (keypoints.size() < 1)
        {
            return -1;
        }

        colorFeat.push_back(vec_colorFeat[0]);
        descriptors.push_back(vec_desc[0]);

        colorFeat.reserve(keypoints.size());
        descriptors.reserve(totalDescNum);


        for (int i = 1; i != keypoints.size(); ++i)
        {
            colorFeat.push_back(vec_colorFeat[i]);
            descriptors.push_back(vec_desc[i]);
        }

        if (saveData == true)
        {
            string dir = string(vn.parent_path().string() + "_data");
            if (!exists(dir))
            {
                create_directory(dir);
            }

            string fileName = string(dir + '/' + vn.filename().string() + ".data");
            FILE *file = fopen(fileName.c_str(), "wb");

            if (!file)
            {
                cout << "can not open out put file    :" << fileName << endl;
                return -1;
            }

            //save color feat
            fwrite(&colorFeat.rows, sizeof(colorFeat.rows), 1, file);
            fwrite(&colorFeat.cols, sizeof(colorFeat.cols), 1, file);
            fwrite(colorFeat.data, sizeof(float), colorFeat.rows*colorFeat.cols, file);

            //save keypoints
            int n = keypoints.size();
            fwrite(&n, sizeof(n), 1, file);
            for (auto sub_vec : keypoints)
            {
                n = sub_vec.size();
                fwrite(&n, sizeof(n), 1, file);
                for (auto k : sub_vec)
                {
                    fwrite(&k.angle, sizeof(k.angle), 1, file);
                    fwrite(&k.class_id, sizeof(k.class_id), 1, file);
                    fwrite(&k.octave, sizeof(k.octave), 1, file);
                    fwrite(&k.pt.x, sizeof(k.pt.x), 1, file);
                    fwrite(&k.pt.y, sizeof(k.pt.y), 1, file);
                    fwrite(&k.response, sizeof(k.response), 1, file);
                    fwrite(&k.size, sizeof(k.size), 1, file);
                }
            }

            //save descriptors
            fwrite(&descriptors.rows, sizeof(int), 1, file);
            fwrite(&descriptors.cols, sizeof(int), 1, file);
            fwrite(descriptors.data, sizeof(uchar), descriptors.total(), file);

            fwrite(&totalDescNum, sizeof(totalDescNum), 1, file);
            fwrite(feat2FrameMap.data(), sizeof(int), totalDescNum, file);


            fclose(file);
        }

        return 0;
    }

    int loadVideoFeat(string videoName, Mat &colorFeat, vector<vector<KeyPoint>> &keypoints, Mat & descriptors, vector<int> &feat2FrameMap)
    {
        boost::filesystem::path p(videoName);
        string dir = string(p.parent_path().string() + "_data");
        if (!exists(dir))
        {
            return -1;
        }

        string fileName = string(dir + '/' + p.filename().string() + ".data");
        FILE *file = fopen(fileName.c_str(), "rb");

        if (!file)
        {
            cout << "can not open  file :         " << fileName << endl;
            return -1;
        }


        //load color feature
        int r, c;
        fread(&r, sizeof(r), 1, file);
        fread(&c, sizeof(c), 1, file);
        colorFeat = Mat::zeros(r, c, CV_32FC1);
        fread(colorFeat.data, sizeof(float), r*c, file);

        //load keypoints
        int n;
        fread(&n, sizeof(n), 1, file);
        keypoints.resize(n);
        for (auto &sub_vec : keypoints)
        {
            fread(&n, sizeof(n), 1, file);
            sub_vec.resize(n);
            for (auto &k : sub_vec)
            {
                fread(&k.angle, sizeof(k.angle), 1, file);
                fread(&k.class_id, sizeof(k.class_id), 1, file);
                fread(&k.octave, sizeof(k.octave), 1, file);
                fread(&k.pt.x, sizeof(k.pt.x), 1, file);
                fread(&k.pt.y, sizeof(k.pt.y), 1, file);
                fread(&k.response, sizeof(k.response), 1, file);
                fread(&k.size, sizeof(k.size), 1, file);
            }
        }

        //load descriptors
        fread(&r, sizeof(r), 1, file);
        fread(&c, sizeof(c), 1, file);
        descriptors = Mat(r, c, CV_8UC1);
        fread(descriptors.data, sizeof(uchar), descriptors.total(), file);


        //load image size
        //fread(&imageSz.width, sizeof(imageSz.width), 1, file);
        //fread(&imageSz.height, sizeof(imageSz.height), 1, file);
        fread(&n, sizeof(n), 1, file);
        feat2FrameMap.resize(n);
        fread(feat2FrameMap.data(), sizeof(int), n, file);

        fclose(file);
        return 0;
    }

    int buildVideoFeatIndex(Mat &descriptors, string videoPath, shared_ptr< FeatIndexType > &desc_index)
    {
        if (descriptors.rows == 0)
        {
            return 0;
        }
        //const int ORB_FEAT_DIM = 32;

        FeatIndexType * index = new FeatIndexType();
        index->build(descriptors, cv::flann::HierarchicalClusteringIndexParams(), cvflann::FLANN_DIST_HAMMING);
        desc_index.reset(index);

        //save the index
        boost::filesystem::path vn(videoPath);
        string dir = string(vn.parent_path().string() + "_data");
        if (!exists(dir))
        {
            create_directory(dir);
        }

        string fileName = string(dir + '/' + vn.filename().string() + '_' + ".index");
        desc_index->save(fileName);

        return 0;
    }

    int loadVideoIndex(string videoPath, Mat &descriptors, shared_ptr<FeatIndexType> &index)
    {

        boost::filesystem::path p(videoPath);
        string dir = string(p.parent_path().string() + "_data");


        string fileName = string(dir + '/' + p.filename().string() + '_' + ".index");

        if (!exists(boost::filesystem::path(fileName)))
        {
            return -1;
        }

        try
        {
            index.reset(new FeatIndexType(descriptors, cv::flann::SavedIndexParams(fileName), cvflann::FLANN_DIST_HAMMING));
        }
        catch (cv::Exception e)
        {
            cout << e.what() << endl;
            return -1;
        }
        return 0;
    }



}
