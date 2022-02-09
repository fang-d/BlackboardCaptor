#pragma once
#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <jni.h>

extern "C" JNIEXPORT jint JNICALL
Java_net_fangd_bbc_MainActivity_process
(JNIEnv *env, jobject, jstring src, jstring dst, jint favour);

/**
 * @brief The main process of detecting and correcting the given blackboard
 * @param image Source image
 * @param FAVOUR Specify what to do with detected blackboards
 * @return 0 if a blackboard can be detected
 */
int process(cv::Mat &image, const int FAVOUR);
#endif
