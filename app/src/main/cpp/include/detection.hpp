#pragma once
#ifndef DETECTION_HPP
#define DETECTION_HPP
#include <vector>
#include <opencv2/core.hpp>

typedef struct
{
    float rou;
    float theta;
} Line;

/**
 * @brief Convert the RGB image into gray scale and reduce it to a suitable size
 * @param src Source image
 * @param hRatio The scale of the image in the horizontal direction
 * @param vRatio The scale of the image in the vertical direction
 * @return A single-channel square matrix
 */
cv::Mat preProcess(const cv::Mat &src, float &hRatio, float &vRatio);

/**
 * @brief Do morphological operations to remove the handwriting on the blackboard
 * @param image The input and output image
 */
void erase(cv::InputOutputArray image);

/**
 * @brief Detect lines in src by the Hough Line Transform Algorithm
 * @param src Source image
 * @param hLines Horizontal lines
 * @param vLines Vertical lines
 */
void detectLines(cv::InputArray src, std::vector<Line> &hLines, std::vector<Line> &vLines);

/**
 * @brief Solving intersections of horizontal lines and vertical lines
 * @param hLines Horizontal lines
 * @param vLines Vertical lines
 * @return The intersections of horizontal lines and vertical lines
 */
std::vector<cv::Point2f> solveIntersections(const std::vector<Line> &hLines, const std::vector<Line> &vLines);

/**
 * @brief Calibration intersections of lines by image
 * @param src Source image
 * @param points The result of points
 */
void calibratePoints(cv::InputArray src, std::vector<cv::Point2f> &points);

/**
 * @brief Filter the corners of the blackboard among the input points
 * @param points Intersections of horizontal lines and vertical lines
 * @param result An array with four corners
 * @param cols The width of the image
 * @param rows The height of the image
 * @return True if there are four corners that meet the requirements
 */
bool sortPoints(const std::vector<cv::Point2f> &points, cv::Point2f result[4], int cols, int rows);
#endif
