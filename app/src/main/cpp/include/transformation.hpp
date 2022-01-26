#pragma once
#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP
#include <opencv2/core.hpp>
#include <vector>
void transform(cv::Mat &image, const cv::Point2f points[4], const float ratio = 16.f / 10);

void sharpen(cv::Mat &image);

void binarize(cv::Mat &image);
#endif
