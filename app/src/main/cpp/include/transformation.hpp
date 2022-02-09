#pragma once
#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP
#include <vector>
#include <opencv2/core.hpp>

/**
 * @brief Correct blackboard by perspective transformation
 * @param image Source image
 * @param points Four corners of blackboard in image
 * @param ratio The ratio of preProcess
 */
void transform(cv::InputOutputArray image, const cv::Point2f points[4], const float ratio = 16.f / 10);

/**
 * @brief Sharpen the given image
 * @param image Source image
 */
void sharpen(cv::InputOutputArray image);

/**
 * @brief Binarize the given image
 * @param image Source image
 */
void binarize(cv::InputOutputArray image);
#endif
