#include <opencv2\opencv.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <functional>


#include "../dll_video_detect/video_dll.h"

#include "../JNI/jni/videoDetectionJNI_VideoDetecion.h"

using namespace std;
using namespace cv;
using namespace std::placeholders;

JNIEXPORT jint JNICALL Java_videoDetectionJNI_VideoDetecion_initModule
(JNIEnv *env, jclass, jstring _configFilePath)
{
    shared_ptr<const char> path(env->GetStringUTFChars(_configFilePath, NULL), bind(&JNIEnv::ReleaseStringUTFChars, env, _configFilePath, _1));
    return vd::initModule(string(path.get()));
}

JNIEXPORT jint JNICALL Java_videoDetectionJNI_VideoDetecion_getFrame
(JNIEnv *env, jclass, jstring videoName, jint second, jstring frameName)
{
    shared_ptr<const char> vn(env->GetStringUTFChars(videoName, NULL), bind(&JNIEnv::ReleaseStringUTFChars, *env, videoName, _1));
    shared_ptr<const char> fn(env->GetStringUTFChars(frameName, NULL), bind(&JNIEnv::ReleaseStringUTFChars, *env, frameName, _1));


    if (videoName == NULL || frameName == NULL)
    {
        return -1;
    }

    vd::getVideoFrame(string(vn.get()), second, string(fn.get()));

    return 0;
}

JNIEXPORT jint JNICALL Java_videoDetectionJNI_VideoDetecion_processSingleVideo
(JNIEnv *env, jclass, jstring videoPath)
{
    shared_ptr<const char> vp(env->GetStringUTFChars(videoPath, NULL), bind(&JNIEnv::ReleaseStringUTFChars, env, videoPath, _1));

    return vd::processSingleVideo(string(vp.get()));

}

JNIEXPORT jstring JNICALL Java_videoDetectionJNI_VideoDetecion_detectSingleVideo__Ljava_lang_String_2
(JNIEnv *env, jclass, jstring videoPath)
{

    shared_ptr<const char> vp(env->GetStringUTFChars(videoPath, NULL), bind(&JNIEnv::ReleaseStringUTFChars,env, videoPath,_1));
   
    
    string strResult;
    //if (vd::detectSingleVideo(string(vp.get()), strResult)==-1)
    //{
    //    return -1;
    //}
    vd::detectSingleVideo(string(vp.get()), strResult);

    jstring jsonResult = env->NewStringUTF(strResult.c_str());
    //env->DeleteLocalRef(jsonResult);
    return jsonResult;

}

JNIEXPORT jstring JNICALL Java_videoDetectionJNI_VideoDetecion_detectSingleVideo__Ljava_lang_String_2_3Ljava_lang_String_2
(JNIEnv *env, jclass, jstring videoName, jobjectArray stringArray)
{   
    jint tarVideoNum = env->GetArrayLength(stringArray);
    vector<string> tarVideoNames;
    tarVideoNames.reserve(tarVideoNum);

    for (int i = 0; i != tarVideoNum; ++i)
    {
        jstring jstrElem = (jstring)env->GetObjectArrayElement(stringArray, i);
        const char * vn = env->GetStringUTFChars(jstrElem, NULL);
        if (vn==0)
        {
           continue;
        }
        tarVideoNames.push_back(string(vn));
        env->ReleaseStringUTFChars(jstrElem, vn);
    }


    shared_ptr<const char> vn(env->GetStringUTFChars(videoName, NULL), bind(&JNIEnv::ReleaseStringUTFChars, env, videoName, _1));

    string strJsonResult;
    vd::detectSingleVideo(string(vn.get()), strJsonResult, tarVideoNames);
    jstring jsonResult = env->NewStringUTF(strJsonResult.c_str());
    //env->DeleteLocalRef(jsonResult);
    return jsonResult;
}

JNIEXPORT jobjectArray JNICALL Java_videoDetectionJNI_VideoDetecion_detectMultipleVideos___3Ljava_lang_String_2
(JNIEnv *env, jclass, jobjectArray stringArray)
{
    if (stringArray==0)
    {
        return jobjectArray();
    }
    jint videoNum = env->GetArrayLength(stringArray);

    jclass strArrclass = env->FindClass("Ljava/lang/String;");
    jobjectArray jsonResults = env->NewObjectArray(videoNum, strArrclass,NULL);    
   

    for (int i = 0; i != videoNum;++i)
    {
        jstring jstrElem = (jstring)env->GetObjectArrayElement(stringArray, i);
        const char * vn= env->GetStringUTFChars(jstrElem, NULL);
       // if (vn==0)
        //{
        //    continue;
        //}
        string strJsonResult;
        vd::detectSingleVideo(string(vn), strJsonResult);
        jstring jsonResultElem = env->NewStringUTF(strJsonResult.c_str());
        env->SetObjectArrayElement(jsonResults, i, jsonResultElem);
      
        //env->DeleteLocalRef(jsonResultElem);

        env->ReleaseStringUTFChars(jstrElem, vn);
    }
    
    return jsonResults;
}

JNIEXPORT jobjectArray JNICALL Java_videoDetectionJNI_VideoDetecion_detectMultipleVideos___3Ljava_lang_String_2_3Ljava_lang_String_2
(JNIEnv *env, jclass, jobjectArray detVideoNamesJstrings, jobjectArray tarVideoNamesJstrings)
{
    if (detVideoNamesJstrings == 0 )
    {
        return jobjectArray();
    }


    jint tarVideoNum = env->GetArrayLength(tarVideoNamesJstrings);
    vector<string> tarVideoNames;
    tarVideoNames.reserve(tarVideoNum);

    for (int i = 0; i != tarVideoNum; ++i)
    {
        jstring jstrElem = (jstring)env->GetObjectArrayElement(tarVideoNamesJstrings, i);
        const char * vn = env->GetStringUTFChars(jstrElem, NULL);
        if (vn == 0)
        {
            continue;
        }
        tarVideoNames.push_back(string(vn));
        env->ReleaseStringUTFChars(jstrElem, vn);
    }


    jint detVideoNum = env->GetArrayLength(detVideoNamesJstrings);

    jclass strArrclass = env->FindClass("Ljava/lang/String;");
    jobjectArray jsonResults = env->NewObjectArray(detVideoNum, strArrclass, NULL);


    for (int i = 0; i != detVideoNum; ++i)
    {
        jstring jstrElem = (jstring)env->GetObjectArrayElement(detVideoNamesJstrings, i);
        const char * vn = env->GetStringUTFChars(jstrElem, NULL);

        string strJsonResult;
        vd::detectSingleVideo(string(vn), strJsonResult,tarVideoNames);

        jstring jsonResultElem = env->NewStringUTF(strJsonResult.c_str());
        env->SetObjectArrayElement(jsonResults, i, jsonResultElem);

        env->DeleteLocalRef(jsonResultElem);
        env->ReleaseStringUTFChars(jstrElem, vn);
    }

    return jsonResults;
}

