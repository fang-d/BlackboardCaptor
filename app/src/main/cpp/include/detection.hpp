#pragma once
#ifndef DETECTION_HPP
#define DETECTION_HPP
#include <opencv2/core.hpp>
#include <vector>

typedef struct
{
    float rou;
    float theta;
} Line;

cv::Mat preProcess(const cv::Mat &src, float &hRatio, float &vRatio);

void erase(cv::Mat &image);

void detectLines(const cv::Mat &src, std::vector<Line> &hLines, std::vector<Line> &vLines);

std::vector<cv::Point2f> solveIntersections(const std::vector<Line> &hLines, const std::vector<Line> &vLines);

void calibratePoints(const cv::Mat &image, std::vector<cv::Point2f> &points);

bool sortPoints(const std::vector<cv::Point2f> &points, cv::Point2f result[4], int cols, int rows);
#endif
