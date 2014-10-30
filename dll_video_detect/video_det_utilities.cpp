#include <iostream>
#include <sstream>
#include <numeric>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/opencv.hpp>

#include <opencv2/nonfree/features2d.hpp>
#include <time.h>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>


#define _EXPORTING
#include "video_dll.h"


namespace vd
{
    using std::stringstream;
    using std::cout;
    using std::endl;
    using std::to_string;
    using std::make_pair;
    using std::map;
    using std::pair;
    using std::tuple;
    using std::make_tuple;

    using cv::Mat;
    using cv::RNG;
    using cv::Point;
    using cv::Point2f;
    using cv::getTickCount;
    using cv::Scalar;
    using cv::BFMatcher;
    using cv::getTextSize;
    using cv::NORM_HAMMING;
    using cv::waitKey;
    using cv::SparseMat;
    using boost::filesystem::exists;
    using boost::filesystem::create_directory;
    using boost::filesystem::directory_iterator;
    using boost::property_tree::ptree;
    using boost::property_tree::read_json;
    using boost::property_tree::write_json;

    Mat drawDetResult(int frameNum1, vector<pair<int, int>> matchResult_tar, int frameNum2, vector<pair<int, int>> matchResult_det)
    {

        //show result

        int max_frames = frameNum1 > frameNum2 ? frameNum1 : frameNum2;

        int height = 400;
        int width = 1000;
        Mat showImg(height, width, CV_8UC3, Scalar(30, 30, 30));


        RNG  rng(getTickCount());

        //Scalar line_color = Scalar(rng(255), rng(255), rng(255));
        Scalar line_color = Scalar(170, 75, 17);
        int lineType = CV_AA;

        line(showImg, Point(0, 100), Point(width*(float)frameNum1 / max_frames, 100), line_color, 3, lineType);
        line(showImg, Point(0, 300), Point(width * (float)frameNum2 / max_frames, 300), line_color, 3, lineType);

        int fontFace = CV_FONT_HERSHEY_DUPLEX;

        double fontScale = 1;
        const int bufSize = 100;
        int thickness = 1;

        int baseline = 0;

        Scalar font_color = Scalar(rng(255), rng(255), rng(255));
        char text[bufSize] = { 0 };
        Point outPt;
        Size sz;

        //original video information
        sprintf(text, "detect:%ds ", frameNum1);
        sz = getTextSize(text, fontFace, fontScale, thickness, &baseline);

        outPt = Point(width - sz.width, 0 + sz.height*1.2);
        putText(showImg, text, outPt, fontFace, fontScale, font_color, thickness, lineType);
        memset(text, 0, bufSize);

        //detecting video information
        sprintf(text, "target:%ds ", frameNum2);
        sz = getTextSize(text, fontFace, fontScale, thickness, &baseline);
        outPt = Point(width - sz.width, height - sz.height);
        putText(showImg, text, outPt, fontFace, fontScale, font_color, thickness, lineType);
        memset(text, 0, bufSize);


        fontFace = CV_FONT_HERSHEY_COMPLEX_SMALL;
        fontScale = 0.7;
        lineType = 4;
        for (int i = 0; i != matchResult_tar.size(); ++i)
        {
            Scalar color(rng(255), rng(255), rng(255));

            Point pt[4];
            pt[0] = Point(width * (float)matchResult_tar[i].first / max_frames, 100);

            sprintf(text, "%d", matchResult_tar[i].first);
            sz = getTextSize(text, fontFace, fontScale, thickness, &baseline);
            outPt = Point(pt[0].x - sz.width / 2, pt[0].y - sz.height);
            putText(showImg, text, outPt, fontFace, fontScale, font_color, thickness, lineType);
            memset(text, 0, bufSize);


            pt[1] = Point(width * (float)matchResult_det[i].first / max_frames, 300);

            sprintf(text, "%d", matchResult_det[i].first);
            sz = getTextSize(text, fontFace, fontScale, thickness, &baseline);
            outPt = Point(pt[1].x - sz.width / 2, pt[1].y + sz.height * 2);
            putText(showImg, text, outPt, fontFace, fontScale, font_color, thickness, lineType);
            memset(text, 0, bufSize);


            pt[2] = Point(width * (float)matchResult_det[i].second / max_frames, 300);

            sprintf(text, "%d", matchResult_det[i].second);
            sz = getTextSize(text, fontFace, fontScale, thickness, &baseline);
            outPt = Point(pt[2].x - sz.width / 2, pt[2].y + sz.height * 2);
            putText(showImg, text, outPt, fontFace, fontScale, font_color, thickness, lineType);
            memset(text, 0, bufSize);


            pt[3] = Point(width * (float)matchResult_tar[i].second / max_frames, 100);

            sprintf(text, "%d", matchResult_tar[i].second);
            sz = getTextSize(text, fontFace, fontScale, thickness, &baseline);
            outPt = Point(pt[3].x - sz.width / 2, pt[3].y - sz.height);
            putText(showImg, text, outPt, fontFace, fontScale, font_color, thickness, lineType);
            memset(text, 0, bufSize);


            fillConvexPoly(showImg, pt, 4, color, CV_AA);

            waitKey();

        }

        return showImg;
    }
    

    int ransac(vector<KeyPoint> &keys1, vector<KeyPoint> &keys2, vector<DMatch> &matches, vector<DMatch> &fined_matches, int threshold, int iteration)
    {
        if (matches.size() < 3)
        {
            return -1;
        }
        vector<Point2f> pt1;
        vector<Point2f> pt2;

        vector<size_t> orginal_index_pt1;
        vector<size_t> orginal_index_pt2;

        for (size_t i = 0; i != matches.size(); ++i)
        {
            pt1.push_back(keys1[matches[i].queryIdx].pt);
            pt2.push_back(keys2[matches[i].trainIdx].pt);

            orginal_index_pt1.push_back(matches[i].queryIdx);
            orginal_index_pt2.push_back(matches[i].trainIdx);

        }
        
        //vector<DMatch> fined_matches;

        int max_cnt = 0;
        //int n = 100;
        //RNG rng = cv::theRNG();
        RNG rng;
        while (iteration-- > 0)
        {
            vector<Point2f> srcPt;
            vector<Point2f> dstPt;
            size_t id1 = rng(pt1.size());
            size_t id2 = rng(pt1.size());
            size_t id3 = rng(pt1.size());

            srcPt.push_back(pt1[id1]);
            srcPt.push_back(pt1[id2]);
            srcPt.push_back(pt1[id3]);

            dstPt.push_back(pt2[id1]);
            dstPt.push_back(pt2[id2]);
            dstPt.push_back(pt2[id3]);

            Mat affineMat = getAffineTransform(srcPt, dstPt);


            vector<Point2f> outPt;
            transform(pt1, outPt, affineMat);

            vector<DMatch> temp_match;
            int inLier = 0;
            for (int i = 0; i != pt2.size(); ++i)
            {
                if (std::fabs(pt2[i].x - outPt[i].x) < threshold && std::fabs(pt2[i].y - outPt[i].y) <threshold)
                {
                    inLier++;
                    temp_match.push_back(DMatch(orginal_index_pt1[i], orginal_index_pt2[i], 0));
                }
            }

            if (inLier> max_cnt)
            {
                max_cnt = inLier;
                fined_matches = temp_match;
            }

        }

        return max_cnt;
    }

    

    int loadVideoIndex(string videoPath, Mat &descriptors, shared_ptr<FeatIndexType> &index, vector<int> &feat2FrameMap)
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



    static int retrieval_frames(Mat &descriptors, shared_ptr<FeatIndexType> &feat_index, size_t knn_num, vector<int> &feat2FrameMap,vector<int> & GPos2LPos
        , vector<pair<int, int>> &votesMap,vector<vector<DMatch>> &desc_matches)
    {
        Mat knn_indices;
        Mat knn_dists;

        //clock_t tp1, tp2;
        //double time2=0;
        //tp1 = clock();       
       
        feat_index->knnSearch(descriptors, knn_indices, knn_dists, knn_num, cv::flann::SearchParams());
       // tp2 = clock();
        //time2 += (double)(tp2 - tp1) / CLOCKS_PER_SEC;

        //printf(" ----%f----\n", time2);
        map<int, int> votes;
        map<int, vector<DMatch>> matches_maps;

        for (int i = 0; i != knn_indices.rows; ++i)
        {
            for (int j = 0; j != knn_indices.cols; ++j)
            {
                int pos_inTotalFeat = ((int*)knn_indices.data)[i*knn_num + j];
                size_t frameIndex = feat2FrameMap[pos_inTotalFeat];
                votes[frameIndex] += 1;

                matches_maps[frameIndex].push_back(DMatch(i, GPos2LPos[pos_inTotalFeat],0.0));
            }
        }
        //vector<pair<int, int>> vec_votes(votes.begin(), votes.end());

        votesMap.clear();
        votesMap = vector<pair<int, int>>(votes.begin(), votes.end());
        std::sort(votesMap.begin(), votesMap.end(), [](const pair<int, int> &v1, const pair<int, int> &v2){ return v1.second > v2.second; });

        desc_matches = vector<vector<DMatch>>(votesMap.size());
        for (int i = 0; i != votesMap.size();++i)
        {
            desc_matches[i] = matches_maps[votesMap[i].first];
        }
        return 0;
    }

    static tuple<int, int> go_bottom_right(Mat &recordMap, SparseMat & flagMat, int r, int c, int interval)
    {
        //int gap_len = 0;

        int valid_r = r;
        int valid_c = c;

        //while loop avoid recursion
        while (true)
        {
            //given a (valid_r,valid_c) find for next one, if success,break out for, 
            //use the found (valid_r,valid_c) to start with
            //if for loop end normally, then no next one, so return
            // in the for loop visit the element in diagonal line first,the method found on stackoverflow.com
            bool next = false;
            for (int diag = 1; diag < interval; ++diag)
            {
                    auto pred = [&](int pos_r, int pos_c)
                {
                    return pos_r < recordMap.rows && pos_c
                        && recordMap.data[pos_r * recordMap.cols + pos_c] == 0
                        && flagMat.ptr(pos_r, pos_c, false) != NULL;
                };

                int test_row = valid_r + diag;
                int test_col = valid_c + diag;
                if (pred(test_row, test_col))
                {
                    next = true;
                    valid_r = test_row;
                    valid_c = test_col;
                }
                else
                {
                    for (int delta = 1; delta <= diag; ++delta)
                    {

                        test_row = valid_r + diag + delta;
                        test_col = valid_c + diag;
                        if (pred(test_row, test_col))
                        {
                            next = true;
                            valid_r = test_row;
                            valid_c = test_col;
                            break;
                        }

                        test_row = valid_r + diag - delta;
                        test_col = valid_c + diag;
                        if (pred(test_row, test_col))
                        {
                            next = true;
                            valid_r = test_row;
                            valid_c = test_col;
                            break;
                        }
                    }//end of for delta

                    
                }
                if (next == true)
                {
                    break;
                }
            }//end of for diag

            if (next ==false)
            {
                return make_tuple(valid_r, valid_c);
            }
            
        }//end of while
                
    };
    static void assembleMatches(Video_det_param &param, SparseMat &flagMat, int beg0, int end0, int beg1, int end1
        , vector<pair<int, int>> &matches0, vector<pair<int, int>> &matches1)
    {

        Mat recordMap = Mat::zeros(end0 - beg0, end1 - beg1, CV_8UC1);
        const int interval = param.timeInterval*param.used_fps;

        for (int i = beg0; i < end0; ++i)
        {

            for (int j = beg1; j < end1; ++j)
            {
                if (recordMap.data[i*recordMap.cols + j] == 1 || flagMat.ptr(i, j, false) == NULL)
                {
                    continue;
                }
                int valid_r, valid_c;
                std::tie(valid_r, valid_c) = go_bottom_right(recordMap, flagMat, i, j, interval);

                //record visited location
                for (int m = i; m < valid_r + 1; ++m)
                {
                    for (int n = j; n < valid_c + 1; ++n)
                    {
                        recordMap.data[m*recordMap.cols + n] = 1;
                    }
                }
                if (valid_r - i + 1 < interval || valid_c - j + 1 < interval)
                {
                    continue;
                }
                matches0.push_back(make_pair(i, valid_r));
                matches1.push_back(make_pair(j, valid_c));
            }
        }

    }


    int detectVideoCitation(string detectVideoName, string targetVideoName, Video_det_param param, string &strJsonResult)
    {
        Mat tar_colorFeat;
        vector<vector<KeyPoint>> tar_keypoints;
        Mat tar_descriptors;
        vector<int> tar_feat2FrameMap;
        shared_ptr<FeatIndexType> tar_desc_index;
        if (loadVideoFeat(targetVideoName, tar_colorFeat, tar_keypoints, tar_descriptors, tar_feat2FrameMap) == -1)
        {
            if (exactFeatAndKFrame(targetVideoName, param, tar_colorFeat, tar_keypoints, tar_descriptors, tar_feat2FrameMap, true) == -1)
            {
                cout << "load(exact) target video features failed" << endl;
                return -1;
            }
        }

        //prepare a map of features in target video from  global position to local position
        vector<int> tar_featGPos2LPos(tar_feat2FrameMap.size());
        int LPos = 0;
        for (int i = 0; i< tar_feat2FrameMap.size();++i)
        {
            if (i>0 && tar_feat2FrameMap[i] !=tar_feat2FrameMap[i-1])
            {
                LPos = 0;
            }
            tar_featGPos2LPos[i] = LPos++;
        }
        
        vector<vector<size_t>> tar_feat2FrameMaps;
        if (loadVideoIndex(targetVideoName, tar_descriptors, tar_desc_index) == -1)
        {
            if (buildVideoFeatIndex(tar_descriptors, targetVideoName, tar_desc_index) == -1)
            {
                cout << "build feature index failed" << endl;
                return -1;
            }

        }


        //load detection video data
        Mat det_colorFeat;
        vector<vector<KeyPoint>> det_keypoints;
        Mat det_descriptors;
        vector<int> det_feat2FrameMap;

        if (loadVideoFeat(detectVideoName, det_colorFeat, det_keypoints, det_descriptors, det_feat2FrameMap) == -1)
        {
            if (exactFeatAndKFrame(detectVideoName, param, det_colorFeat, det_keypoints, det_descriptors, det_feat2FrameMap, true) == -1)
            {
                cout << "load(exact) target video features failed" << endl;
                return -1;
            }
        }

        // map of frame index to descriptors index
        vector<pair<int, int>> det_frame2FeatMap(det_keypoints.size());
        int acc_feat_num = 0;
        for (int i = 0; i != det_frame2FeatMap.size(); ++i)
        {

            det_frame2FeatMap[i].first =acc_feat_num;
            det_frame2FeatMap[i].second =acc_feat_num + det_keypoints[i].size();
            acc_feat_num += det_keypoints[i].size();
        }
        
        size_t knn_num = 10;

        BFMatcher bfMatcher(NORM_HAMMING, true);
       
        vector<tuple<int, int>> frameMatches;

        
//#pragma omp parallel for
        for (int i = 0; i < det_keypoints.size(); ++i)
        {
            vector<pair<int, int>> votes_map;
            vector<vector<DMatch>> desc_matches;
         
            Mat det_cur_desc = det_descriptors.rowRange(det_frame2FeatMap[i].first, det_frame2FeatMap[i].second);
           
            retrieval_frames(det_cur_desc, tar_desc_index, knn_num, tar_feat2FrameMap, tar_featGPos2LPos,votes_map, desc_matches);   
        
            int test_knn = knn_num;
            if (votes_map.size()<knn_num)
            {
                //votes_map.resize(knn_num);
                test_knn = votes_map.size();
            }
            for (int k = 0; k != test_knn; ++k)
            {
                vector<DMatch> matches=desc_matches[k];       
                vector<DMatch> fined_matches;
                int inLiers = ransac(det_keypoints[i], tar_keypoints[votes_map[k].first], matches, fined_matches,5,100);

                double dist = norm(det_colorFeat.row(i), tar_colorFeat.row(votes_map[k].first), cv::NORM_L1);

                if (inLiers > param.orbThres && dist < param.colorThres)
                {
//#pragma omp critical
                    {
                        frameMatches.push_back(make_pair(i, votes_map[k].first));
                    }                    
                }
            }//end for k
        }

        // fuse the match result
        // for convenience :: vector<tuple<int, int, float>> shotMatches;  

        vector<pair<int, int>> det_initFrameMatches;
        vector<pair<int, int>> tar_initFrameMatches;

        int sparseMatSize[2] = { det_keypoints.size(), tar_keypoints.size() };
        SparseMat flagMat(2, sparseMatSize, CV_8UC1);

        for (size_t i = 0; i != frameMatches.size(); ++i)
        {
            flagMat.ref<uchar>(std::get<0>(frameMatches[i]), std::get<1>(frameMatches[i])) = 1;
        }
        
        assembleMatches(param, flagMat, 0, det_keypoints.size(), 0, tar_keypoints.size(), det_initFrameMatches, tar_initFrameMatches);


        vector<pair<int, int>> finalMatchResult_det(det_initFrameMatches.size());
        vector<pair<int, int>> finalMatchResult_tar(tar_initFrameMatches.size());

        for (int i = 0; i != det_initFrameMatches.size(); ++i)
        {
            finalMatchResult_det[i].first = det_initFrameMatches[i].first / param.used_fps;
            finalMatchResult_det[i].second = det_initFrameMatches[i].second / param.used_fps;

            finalMatchResult_tar[i].first = tar_initFrameMatches[i].first / param.used_fps;
            finalMatchResult_tar[i].second = tar_initFrameMatches[i].second / param.used_fps;
        }


        if (finalMatchResult_tar.size() == 0)
        {
            return 0;
        }
        
        //generate detect result image

        int totalFrameCntTar;
        int totalFrameCntDet;

        VideoCapture capTarget(targetVideoName);
        VideoCapture capDetect(detectVideoName);

        if (!capTarget.isOpened() || !capDetect.isOpened())
        {
            //cout << " 目标视频打开失败" << endl;
            totalFrameCntTar = totalFrameCntDet = 100;
        }
        else
        {
            totalFrameCntTar = std::ceil(capTarget.get(CV_CAP_PROP_FRAME_COUNT) / capTarget.get(CV_CAP_PROP_FPS));
            totalFrameCntDet = std::ceil(capDetect.get(CV_CAP_PROP_FRAME_COUNT) / capDetect.get(CV_CAP_PROP_FPS));
        }
        
        Mat resultImg = drawDetResult(totalFrameCntDet, finalMatchResult_det, totalFrameCntTar, finalMatchResult_tar);


        //save results
        boost::filesystem::path det_path(detectVideoName);

        string dir = string(det_path.parent_path().string() + "_data");
        if (!exists(dir))
        {
            create_directory(dir);
        }

        boost::filesystem::path tar_path(targetVideoName);


        string resultImgFileName = string(dir + '/' + det_path.filename().string() + tar_path.filename().string() + ".jpg");
        imwrite(resultImgFileName, resultImg);


        ptree json_result;

        json_result.put("filename", targetVideoName);

        ptree json_array;
        for (size_t i = 0; i != finalMatchResult_det.size(); ++i)
        {
            ptree temp_pt;
            pair<int, int> & tempTar = finalMatchResult_tar[i];
            pair<int, int> & tempDet = finalMatchResult_det[i];

            temp_pt.put("det_beg", tempDet.first);
            temp_pt.put("det_end", tempDet.second);
            temp_pt.put("tar_beg", tempTar.first);
            temp_pt.put("tar_end", tempTar.second);


            json_array.push_back(std::make_pair("", temp_pt));
        }

        json_result.put_child("time", json_array);

        json_result.put("pic_path", resultImgFileName);


        stringstream strstream;
        write_json(strstream, json_result);
        //write_json("1.json", json_result);
        strJsonResult = strstream.str();

        return 0;
    }




}
