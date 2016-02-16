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
Java_andy_ca_FaceHelper_Find(JNIEnv *env, jclass type, jstring imageName_, jstring FileName_,
                             jstring Csv_) {
    const char *imageName = env->GetStringUTFChars(imageName_, 0);
    const char *FileName = env->GetStringUTFChars(FileName_, 0);
    const char *Csv = env->GetStringUTFChars(Csv_, 0);

    string stdFileName(FileName);
    string stdImageName(imageName);
    string stdCsv(Csv);

    vector<Mat> images;
    vector<int> labels;
    std::ifstream file(stdCsv.c_str(), ifstream::in);

    LOGD("Image = %s ",stdImageName.c_str());

    string line, path, classlabel;
    while (getline(file, line)) {
        stringstream liness(line);
        getline(liness, path, ';');
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty()) {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }

    int im_width = images[0].cols;
    int im_height = images[0].rows;
    // Create a FaceRecognizer and train it on the given images:
    Ptr<FaceRecognizer> model = createEigenFaceRecognizer();
    model->train(images, labels);

    env->ReleaseStringUTFChars(imageName_, imageName);
    env->ReleaseStringUTFChars(FileName_, FileName);
    env->ReleaseStringUTFChars(Csv_, Csv);
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