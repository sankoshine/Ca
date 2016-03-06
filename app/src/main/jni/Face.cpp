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

vector<Rect> detectFaces( Mat& img,CascadeClassifier& cascade)
{
    LOGD("detect faces");
    vector<Rect> faces;
    Mat gray, smallImg( cvRound (img.rows), cvRound(img.cols), CV_8UC1 );
    imwrite("/storage/emulated/0/1000/check.jpg",img);

    cvtColor( img, gray, CV_BGR2GRAY );
    resize( gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );
    equalizeHist( smallImg, smallImg );

    cascade.detectMultiScale( smallImg, faces,
                              1.1, 2, 0|CV_HAAR_FIND_BIGGEST_OBJECT,
                              Size(30, 30) );
    LOGD("No of faces:%d",faces.size());
    return faces;
}

Mat normalizeImage(string path)
{
    Mat cur_img = imread(path,CV_LOAD_IMAGE_GRAYSCALE);
    Mat des_img;
    normalize(cur_img,des_img,0, 255, NORM_MINMAX, CV_8UC1);
    imwrite(path+"_normal.jpg",des_img);
    return des_img;
}

extern "C" {

JNIEXPORT jint JNICALL
Java_andy_ca_FaceHelper_detect(JNIEnv *env, jclass type, jstring parentDir_, jint photoId) {
    const char *parentDir = env->GetStringUTFChars(parentDir_, 0);
    string stdDir(parentDir);
    int stdId(photoId);
    LOGD("start detect");

    CascadeClassifier haar_cascade;
    haar_cascade.load(stdDir + "haarcascade_frontalface_alt.xml");
    stringstream pathss;
    pathss << stdDir << stdId << ".jpg";
    LOGD(pathss.str().c_str());
    Mat original = imread(pathss.str(),1), roiImg, normalImg;
    Rect roiRect;
    vector<Rect> faces = detectFaces(original,haar_cascade);
    pathss.str("");
    pathss << stdDir << stdId << "_face.jpg";
    LOGD("image write: %s",pathss.str().c_str());
    if(faces.size()==1){
        roiRect = faces[0];
        roiImg = original(roiRect);
        imwrite(pathss.str(),roiImg);
    }
    else{
        imwrite(pathss.str(),original);
    }

    env->ReleaseStringUTFChars(parentDir_, parentDir);
    return faces.size();
}

JNIEXPORT jint JNICALL
Java_andy_ca_FaceHelper_train(JNIEnv *env, jclass type, jstring parentDir_, jint countPhoto) {
    const char *parentDir = env->GetStringUTFChars(parentDir_, 0);
    string stdDir(parentDir);
    int stdCountPhoto(countPhoto);
    LOGD("start train");

    vector<Mat> images;
    vector<int> labels;
    Mat original, roiImg, normalImg;
    vector<Rect> faces;
    Rect roiRect;
    stringstream pathss;

    CascadeClassifier haar_cascade;
    haar_cascade.load(stdDir + "haarcascade_frontalface_alt.xml");

    int i = 0;
    for (i = 1; i <= stdCountPhoto; i++) {
        pathss.str("");
        pathss << stdDir << i << "_face.jpg";
        LOGD("normalization of %d",i);
        normalImg = normalizeImage(pathss.str());
        images.push_back(normalImg);
        labels.push_back(0);
    }

    LOGD("training");
    Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
    model->train(images, labels);
    LOGD("train finish");
    model->save(stdDir + "f.xml");

    env->ReleaseStringUTFChars(parentDir_, parentDir);
    return 0;
}

JNIEXPORT jint JNICALL
Java_andy_ca_FaceHelper_predict(JNIEnv *env, jclass type, jstring parentDir_) {
    const char *parentDir = env->GetStringUTFChars(parentDir_, 0);
    string stdDir(parentDir);
    LOGD("start predict");

    string imgPath = stdDir + "9_face.jpg";
    Mat normalImg = normalizeImage(imgPath);
    LOGD("nomalization done");
    string xmlPath = stdDir + "f.xml";
    double predicted_confidence = 0.0;
    int prediction = -9;
    Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
    model->load(xmlPath);
    LOGD("load xml done");
    model->predict(normalImg, prediction, predicted_confidence);
    LOGD("Prediction = %d Predicted Confidence = %Lf", prediction, predicted_confidence);
    int confidence = predicted_confidence;

    env->ReleaseStringUTFChars(parentDir_, parentDir);
    return confidence;
}

}