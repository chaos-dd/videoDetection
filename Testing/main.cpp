#include "../dll_video_detect/video_dll.h"

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <time.h>

using namespace std;
using namespace boost::property_tree;

int main(int argc,char **argv)
{
    if(argc<3)
    {
        cout<<"argument less 3"<<endl;
        return -1;
    }
    //string det_video_path = "D:\\videos\\test4\\det.mp4";
    //string tar_video_path = "D:\\videos\\test4\\tar.mp4";
    
    string det_video_path=argv[1];
    string tar_video_path=argv[2];

    string configFilePath = "../videoDetection/JNI/det_config.xml";

    //vd::initModule(configFilePath);
    //vd::processSingleVideo(det_video_path);
    //vd::processSingleVideo(tar_video_path);   

    
    vd::Video_det_param param;

    if (param.loadFromFile(configFilePath) != 0)
        param = vd::Video_det_param();
    string jsonResult;

    clock_t start, end;
    start = clock();
    int ret = vd::detectVideoCitation(det_video_path, tar_video_path, param, jsonResult);
    //cout << ret << endl;
    end = clock();
    cout <<"total time -----time------"<< (double)(end - start) / CLOCKS_PER_SEC << endl;

    if (jsonResult.size()==0)
    {
        return 0;
    }
    stringstream strstream(jsonResult);
    ptree pt;
    read_json(strstream, pt);

    write_json("1.json", pt);

    return 0;
}
