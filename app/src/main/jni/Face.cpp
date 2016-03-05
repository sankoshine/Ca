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

vector<Rect> detectAndDraw( Mat& img,CascadeClassifier& cascade,
                            double scale)
{
    LOGD("detect and draw");
    int i = 0;
    vector<Rect> faces;
    Mat gray, smallImg( cvRound (img.rows/scale), cvRound(img.cols/scale), CV_8UC1 );

    cvtColor( img, gray, CV_BGR2GRAY );
    resize( gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );
    equalizeHist( smallImg, smallImg );

    cascade.detectMultiScale( smallImg, faces,
                              1.1, 2, 0|CV_HAAR_FIND_BIGGEST_OBJECT,
                              Size(30, 30) );
    LOGD("No of faces:%d",faces.size());
    return faces;
}

void resizeandtogray(string stdDir,int photoId)
{
    stringstream pathss;
    pathss << stdDir << photoId;
    string path = pathss.str();
    Mat cur_img = imread(path+".jpg",CV_LOAD_IMAGE_GRAYSCALE);
    Mat des_img;
    normalize(cur_img,des_img,0, 255, NORM_MINMAX, CV_8UC1);
    imwrite(path+"0.jpg",des_img);
}

Mat DetectandExtract(Mat& img, CascadeClassifier& cascade,
                                      CascadeClassifier& nestedCascade,
                                      double scale, bool tryflip)
{
    LOGD("DetectandExtract in");
    vector<Rect> Rvec = detectAndDraw(img,cascade,scale);
    Rect roi_rect = Rvec[0];
    Mat roi = img(roi_rect);
    LOGD("DetectandExtract: out");
    return roi;
}

void PreHelper()
{
    int i;
}

extern "C" {

JNIEXPORT jint JNICALL
Java_andy_ca_FaceHelper_detect(JNIEnv *env, jclass type, jstring parentDir_, jint photoId) {
    const char *parentDir = env->GetStringUTFChars(parentDir_, 0);
    string stdDir(parentDir);
    vector<Mat> images;
    vector<int> labels;
    string path, label;

    CascadeClassifier haar_cascade;
    haar_cascade.load(stdDir + "haarcascade_frontalface_alt.xml");

    int i;

    LOGD("start train");
    for (i = 1; i <= 4; i++) {
        stringstream pathss, paths;
        pathss << stdDir << 0 << i;
        paths << stdDir << i << ".jpg";
        path = paths.str();
        Mat original = imread(path, 1);
        Mat img = DetectandExtract(original, haar_cascade, haar_cascade, 1, false);
        path = pathss.str();
        imwrite(path + ".jpg", img);
        resizeandtogray(stdDir + "0", i);
    }
/*    Mat gray = original.clone();
    cvtColor(original,gray,CV_BGR2GRAY);
    vector< Rect_<int> > faces;
    haar_cascade.detectMultiScale(gray, faces, 1.1, 3, 0, Size(20,60));
    LOGD("No of faces = %d",faces.size());*/

    env->ReleaseStringUTFChars(parentDir_, parentDir);
    return 1;
}

JNIEXPORT jint JNICALL
Java_andy_ca_FaceHelper_train(JNIEnv *env, jclass type, jstring parentDir_, jint countPhoto) {
    const char *parentDir = env->GetStringUTFChars(parentDir_, 0);
    string stdDir(parentDir);

    vector<Mat> images;
    vector<int> labels;
    string path, label;






//    Mat original = imread(path, 1);
//    Mat gray = original.clone();
//    cvtColor(original,gray,CV_BGR2GRAY);
//    imwrite(stdDir+"11.jpg",gray);
//    vector< Rect_<int> > faces;
//    haar_cascade.detectMultiScale(gray, faces, 1.1, 3, 0, Size(20,60));
//    LOGD("No of faces = %d",faces.size());
//    if(faces.size()==1) {
//        Mat face = gray(faces[0]);
//        Mat face_resized;
//        cv::resize(face, face_resized,Size(face.cols,face.rows), 1.0, 1.0, INTER_CUBIC);
//        imwrite(stdDir+"resize.jpg",face_resized);
//    }



    LOGD("start train");
    int i = 0;
    for (i = 1; i <= countPhoto; i++) {
        stringstream pathss;
        pathss << stdDir << 0 << i << "0.jpg";
        path = pathss.str();
        LOGD(path.c_str());
        label = "0";
        images.push_back(imread(path, 0));
        labels.push_back(atoi(label.c_str()));
    }

    try {
        LOGD("training");
        Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
        model->train(images, labels);
        LOGD("train finish");
        model->save(stdDir + "f.xml");
    } catch (cv::Exception e) {
        LOGD("nativeCreateObject catched cv::Exception: %s", e.what());
        jclass je = env->FindClass("org/opencv/core/CvException");
        if (!je)
            je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
    }
    catch (...) {
        LOGD("nativeCreateObject catched unknown exception");
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je,
                      "Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
    }

    env->ReleaseStringUTFChars(parentDir_, parentDir);
}


JNIEXPORT jint JNICALL
Java_andy_ca_FaceHelper_predict(JNIEnv *env, jclass type, jstring parentDir_) {
    const char *parentDir = env->GetStringUTFChars(parentDir_, 0);
    string stdDir(parentDir);
    LOGD("strat predict");

    CascadeClassifier haar_cascade;
    haar_cascade.load(stdDir + "haarcascade_frontalface_alt.xml");
    string path;
    stringstream pathss, paths;
    pathss << stdDir << 0 << 9;
    paths << stdDir << 9 << ".jpg";
    path = paths.str();
    Mat original = imread(path, 1);
    Mat img = DetectandExtract(original, haar_cascade, haar_cascade, 1, false);
    path = pathss.str();
    imwrite(path + ".jpg", img);
    resizeandtogray(stdDir + "0", 9);
    img = imread(stdDir + "090.jpg", 0);
    string xmlPath = stdDir + "f.xml";
    double predicted_confidence = 0.0;
    int prediction;
    Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
    model->load(xmlPath);
    model->predict(img, prediction, predicted_confidence);
    LOGD("Prediction = %d Predicted Confidence = %Lf", prediction, predicted_confidence);
    return prediction;

    LOGD("predict finish");
    env->ReleaseStringUTFChars(parentDir_, parentDir);
}
}