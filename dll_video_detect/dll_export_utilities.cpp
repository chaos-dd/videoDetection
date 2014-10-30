#define _EXPORTING

#include "video_dll.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>

namespace vd
{
    using std::string;
    using std::cout;
    using std::endl;
    using std::stringstream;
    using std::shared_ptr;
    using boost::filesystem::exists;
    using boost::filesystem::create_directory;
    using boost::filesystem::directory_iterator;
    using boost::property_tree::ptree;

    string configFilePath;
    DLL_EXPORTS int initModule(string _configFilePath)
    {
        boost::filesystem::path p(_configFilePath);
        if (!is_regular_file(p))
        {
            return -1;
        }
        configFilePath = _configFilePath;

        return 0;
    }


    int loadTarVideoNames(string configFilePath, vector<string> &tarVideoNames)
    {
        ptree pt;
        vector<boost::filesystem::path>  vecPath_tar;

        try
        {
            read_xml(configFilePath, pt, boost::property_tree::xml_parser::trim_whitespace);


            for (auto v : pt.get_child("config.videoPath"))
            {
                // v.first is the name of the child.
                // v.second is the child tree.

                string videoPath = v.second.get<string>("");
                boost::filesystem::path videoPathObj(videoPath);

                if (!exists(videoPathObj) || !is_directory(videoPathObj))
                {
                    //cout << endl << "视频文件路径有误" << endl;
                    continue;
                }
                std::copy_if(directory_iterator(videoPathObj), directory_iterator(), std::back_inserter(vecPath_tar), [](boost::filesystem::path p){return is_regular_file(p); });

            }
        }
        catch (boost::property_tree::ptree_error &e)
        {
            cout << endl << "解析配置文件错误，信息如下：";
            cout << endl << e.what() << endl;

            return -1;
        }
        catch (boost::filesystem::filesystem_error &e)
        {
            cout << e.what() << endl;
            return -1;
        }

        tarVideoNames.reserve(vecPath_tar.size());
        for_each(vecPath_tar.begin(), vecPath_tar.end(), [&tarVideoNames](boost::filesystem::path &p){tarVideoNames.push_back(p.string()); });

        return 0;

    }


    DLL_EXPORTS int detectSingleVideo(string videoName, string &strJsonResult, vector<string> &tarVideoNames)
    {
        if (tarVideoNames.size() == 0 && loadTarVideoNames(configFilePath, tarVideoNames) == -1)
        {
            return -1;
        }

        Video_det_param param;
        if (param.loadFromFile(configFilePath) != 0)
            param = Video_det_param();

        ptree json_array;

        for (auto tarVidoe : tarVideoNames)
        {
            string strJson;
            if (detectVideoCitation(videoName, tarVidoe, param, strJson) == -1)
                continue;

            if (strJson.size() == 0)
            {
                continue;
            }

            ptree json_array_elem;
            stringstream str_json_array_elem(strJson);

            read_json(str_json_array_elem, json_array_elem);
            json_array.push_back(std::make_pair("", json_array_elem));
        }

        ptree json_result;
        if (json_array.size() == 0)
        {
            return 0;
        }
        json_result.put_child("result", json_array);

        stringstream strstream;
        write_json(strstream, json_result);
        strJsonResult = strstream.str();


        return 0;
    }

    DLL_EXPORTS int detectMultipeVideos(vector<string> videoNames, vector<string> &jsonResult, vector<string> &tarVideoNames)
    {

        for (auto vn : videoNames)
        {
            string single_json_result;
            detectSingleVideo(vn, single_json_result, tarVideoNames);    //失败就加个空的字符串
            jsonResult.push_back(single_json_result);
        }
        return 0;
    }

  
    int procSingleVideo(string videoPath, Video_det_param param
        , Mat &colorFeat, vector<vector<KeyPoint>> & keypoints, Mat &descriptors
        , vector<int> &feat2FrameMaps, shared_ptr< FeatIndexType > &desc_index)
    {
        int ret = exactFeatAndKFrame(videoPath, param, colorFeat, keypoints, descriptors,feat2FrameMaps, true);

        if (ret == -1)
        {
            return -1;
        }
        int ret1 = buildVideoFeatIndex(descriptors, videoPath, desc_index);
        if (ret == -1)
        {
            return -1;
        }

        return 0;
    }

    DLL_EXPORTS int processSingleVideo(string videoPath)
    {
        Mat colorFeat;
        vector<vector<KeyPoint>> keypoints;
        Mat descriptors;
        vector<int> feat2FrameMaps;

        shared_ptr< FeatIndexType > desc_index;                    


        extern string configFilePath;
        Video_det_param param;
        if (param.loadFromFile(configFilePath) != 0)
            param = Video_det_param();

        param.loadFromFile(configFilePath);
        return procSingleVideo(videoPath, param, colorFeat, keypoints, descriptors, feat2FrameMaps, desc_index);

    }

    DLL_EXPORTS int getVideoFrame(string videoName, int second, string frameName)
    {

        VideoCapture cap(videoName);

        if (!cap.isOpened())
        {
            return -1;
        }

        double fps = cap.get(CV_CAP_PROP_FPS);
        double totalFrameCnt = cap.get(CV_CAP_PROP_FRAME_COUNT);

        double frameInd = std::floor(fps*second);

        if (frameInd<0 || frameInd> totalFrameCnt + 50)
        {
            return -1;
        }
        else if (frameInd >= totalFrameCnt)
        {
            frameInd = totalFrameCnt - 1;
        }

        cap.set(CV_CAP_PROP_POS_FRAMES, frameInd);

        Mat img;
        cap.read(img);
        if (img.empty())
        {
            return -1;
        }

        cv::imwrite(frameName, img);

        return 0;
    }
    DLL_EXPORTS int procBatchVideo(vector<string>  folders, string configFilePath)
    {
        ptree pt;
        vector<boost::filesystem::path>  vecPathObj;
        for (auto v : folders)
        {
            boost::filesystem::path videoPathObj(v);

            if (!exists(videoPathObj) || !is_directory(videoPathObj))
            {
                //cout << endl << "视频文件路径有误" << endl;
                continue;
            }
            std::copy_if(directory_iterator(videoPathObj), directory_iterator(), std::back_inserter(vecPathObj), [](boost::filesystem::path p){return is_regular_file(p); });

        }
        Video_det_param param;
        if (param.loadFromFile(configFilePath) != 0)
            param = Video_det_param();


        for (auto vn : vecPathObj)
        {
            Mat colorFeat;
            vector<vector<KeyPoint>> keypoints;
            Mat descriptors;
            vector<int> feat2FrameMaps;

            shared_ptr< FeatIndexType > desc_index;
            if (procSingleVideo(vn.string(), param, colorFeat, keypoints, descriptors, feat2FrameMaps, desc_index) == -1)
                continue;
        }

        return 0;
    }
}
