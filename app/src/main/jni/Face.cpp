#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <fstream>
#include <sstream>
#include <android/log.h>

#define LOG_TAG "Andy/FaceCPP"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))


using namespace cv;
using namespace std;

extern "C" {

JNIEXPORT jint JNICALL
Java_andy_ca_FaceHelper_train(JNIEnv *env, jclass type, jstring parentDir_, jint countPhoto) {
    const char *parentDir = env->GetStringUTFChars(parentDir_, 0);
    string stdDir(parentDir);

    vector<Mat> images;
    vector<int> labels;
    string path, label;
    LOGD("start train");
    int i = 0;
    for(i = 1;i<=countPhoto;i++){
        stringstream pathss;
        pathss << stdDir << i << ".jpg";
        path = pathss.str();
        LOGD(path.c_str());
        label = "0";
        images.push_back(imread(path, 0));
        labels.push_back(atoi(label.c_str()));
    }

    try{
        Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
        model->train(images, labels);
        LOGD("train finish");
        model->save(stdDir+"f.csv");
    }catch(cv::Exception e){
        LOGD("nativeCreateObject catched cv::Exception: %s", e.what());
        jclass je = env->FindClass("org/opencv/core/CvException");
        if(!je)
            je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
    }
    catch (...){
        LOGD("nativeCreateObject catched unknown exception");
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
    }

    env->ReleaseStringUTFChars(parentDir_, parentDir);
}

JNIEXPORT jint JNICALL
Java_andy_ca_FaceHelper_predict(JNIEnv *env, jclass type, jstring parentDir_) {
    const char *parentDir = env->GetStringUTFChars(parentDir_, 0);

    string stdDir(parentDir);
    LOGD("strat predict");
    string imgPath = stdDir + "p.jpg";
    string xmlPath = stdDir + "f.csv";
    Mat img = imread(imgPath.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
    double predicted_confidence = 0.0;
    int prediction;
    Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
    model->load(xmlPath);
    model->predict(img,prediction,predicted_confidence);
    LOGD("Prediction = %d Predicted Confidence = %Lf",prediction,predicted_confidence);
    if(prediction>=0)
        return prediction;

    env->ReleaseStringUTFChars(parentDir_, parentDir);
}

JNIEXPORT jint JNICALL
Java_andy_ca_FaceHelper_Find(JNIEnv *env, jclass type, jstring imageName_, jstring FileName_,
                             jstring Csv_) {
    const char *imageName = env->GetStringUTFChars(imageName_, 0);
    const char *FileName = env->GetStringUTFChars(FileName_, 0);
    const char *Csv = env->GetStringUTFChars(Csv_, 0);
    try{

        string stdXmlName(FileName);
        string stdImageName(imageName);
        string stdCsvName(Csv);

        vector<Mat> images, images2;
        vector<int> labels, labels2;
//        std::ifstream file(stdCsvName.c_str(), ifstream::in);
//        LOGD("csv = %s",stdCsvName.c_str());
//
        string line, path, classlabel;
//        while (getline(file, line)) {
//            stringstream liness(line);
//            getline(liness, path, ';');
//            getline(liness, classlabel);
//            if(!path.empty() && !classlabel.empty()) {
//                LOGD("push_back");
//                LOGD("path = %s", path.c_str());
//                LOGD("class = %d",atoi(classlabel.c_str()) );
//                images.push_back(imread(path, 0));
//                labels.push_back(atoi(classlabel.c_str()));
//            }
//        }
        LOGD("start train");
        Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
        path = "/storage/emulated/0/0010/1.jpg";
        classlabel = "0";
        images.push_back(imread(path, 0));
        labels.push_back(atoi(classlabel.c_str()));
        model->train(images, labels);
        path = "/storage/emulated/0/0010/4.jpg";
        classlabel = "1";
        images2.push_back(imread(path, 0));
        labels2.push_back(atoi(classlabel.c_str()));
        model->train(images, labels);
        LOGD("train finish");

        model->save(stdXmlName);




    } catch(cv::Exception e)
    {
        LOGD("nativeCreateObject catched cv::Exception: %s", e.what());
        jclass je = env->FindClass("org/opencv/core/CvException");
        if(!je)
            je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeCreateObject catched unknown exception");
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
    }

    env->ReleaseStringUTFChars(imageName_, imageName);
    env->ReleaseStringUTFChars(FileName_, FileName);
    env->ReleaseStringUTFChars(Csv_, Csv);

    return -9;
}

JNIEXPORT jint JNICALL
Java_andy_ca_FaceHelper_Recog(JNIEnv *env, jclass type, jstring imageName_, jstring Filename_) {
    const char *imageName = env->GetStringUTFChars(imageName_, 0);
    const char *Filename = env->GetStringUTFChars(Filename_, 0);
    string stdXmlName(Filename);
    string stdImageName(imageName);
    LOGD("strat predict");
    Mat img = imread(stdImageName.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
    double predicted_confidence = 0.0;
    int prediction;
    Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
    model->load(stdXmlName);
    model->predict(img,prediction,predicted_confidence);
    LOGD("Prediction = %d Predicted Confidence = %Lf",prediction,predicted_confidence);
    if(prediction>=0)
        return prediction;

    env->ReleaseStringUTFChars(imageName_, imageName);
    env->ReleaseStringUTFChars(Filename_, Filename);
}


JNIEXPORT jintArray JNICALL Java_andy_ca_FaceHelper_gray(
        JNIEnv *env, jclass obj, jintArray buf, int w, int h) {

    jint *cbuf;
    cbuf = env->GetIntArrayElements(buf, JNI_FALSE );
    if (cbuf == NULL) {
        return 0;
    }

    Mat imgData(h, w, CV_8UC4, (unsigned char *) cbuf);

    uchar* ptr = imgData.ptr(0);
    for(int i = 0; i < w*h; i ++){
        //计算公式：Y(亮度) = 0.299*R + 0.587*G + 0.114*B
        //对于一个int四字节，其彩色值存储方式为：BGRA
        int grayScale = (int)(ptr[4*i+2]*0.299 + ptr[4*i+1]*0.587 + ptr[4*i+0]*0.114);
        ptr[4*i+1] = grayScale;
        ptr[4*i+2] = grayScale;
        ptr[4*i+0] = grayScale;
    }

    int size = w * h;
    jintArray result = env->NewIntArray(size);
    env->SetIntArrayRegion(result, 0, size, cbuf);
    env->ReleaseIntArrayElements(buf, cbuf, 0);
    return result;
}
}