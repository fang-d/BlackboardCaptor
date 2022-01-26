#include <jni.h>
//#include <android/bitmap.h>
#include "detection.hpp"
#include "transformation.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

enum {
    CUT_IMAGE = 1,
    SHARPENED_IMAGE = 2,
    BINARIZED_IMAGE = 4
};

int process(cv::Mat &image, const int FAVOUR)
{
    float hRatio, vRatio;
    cv::Mat grayImage = preProcess(image, hRatio, vRatio);
    cv::Mat cleanImage;
    grayImage.copyTo(cleanImage);
    erase(cleanImage);
    if (FAVOUR & CUT_IMAGE)
    {
        std::vector<Line> hLines, vLines;
        detectLines(cleanImage, hLines, vLines);
        std::vector<cv::Point2f> intersections = solveIntersections(hLines, vLines);
        //calibratePoints(lineImg, intersections);

        cv::Point2f corner[4];
        if (!sortPoints(intersections, corner, grayImage.cols, grayImage.rows))
            return 1;

        for (cv::Point2f &point : corner) {
            point.x *= hRatio;
            point.y *= vRatio;
        }

        transform(image, corner);
    }
    if (FAVOUR & SHARPENED_IMAGE)
    {
        sharpen(image);
    }
    if (FAVOUR & BINARIZED_IMAGE)
    {
        binarize(image);
    }
    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_net_fangd_bbc_MainActivity_process
(JNIEnv *env, jobject, jstring src, jstring dst, jint favour)
{
    const char *const srcPath = env->GetStringUTFChars(src, nullptr);
    cv::Mat image = cv::imread(srcPath);
    jint result = -1;
    if (!image.empty())
    {
        if (!(result = process(image, favour)))
            cv::imwrite(env->GetStringUTFChars(dst, nullptr), image);
    }
    return result;
}
