#include "detection.hpp"
#include <algorithm>
#include <cfloat>
#include <opencv2/imgproc.hpp>

cv::Mat preProcess(const cv::Mat &src, float &hRatio, float &vRatio)
{
    // Convert the RGB image into gray scale
    cv::Mat dst;
    cv::cvtColor(src, dst, cv::COLOR_BGR2GRAY);

    // Reduce the image to a suitable size
    constexpr int IMAGE_SIZE = 1024;
    hRatio = static_cast<float>(src.cols) / IMAGE_SIZE;
    vRatio = static_cast<float>(src.rows) / IMAGE_SIZE;
    cv::resize(dst, dst, cv::Size(IMAGE_SIZE, IMAGE_SIZE), 0, 0, cv::INTER_AREA);

    // Return the preprocessed image
    return dst;
}

void erase(cv::InputOutputArray image)
{
    // Do morphological operations to remove the handwriting on the blackboard
    for (int size : {5, 7, 7}) {
        cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(size, size));
        cv::morphologyEx(image, image, cv::MORPH_OPEN, structuringElement);
        cv::morphologyEx(image, image, cv::MORPH_CLOSE, structuringElement);
    }
}

void detectLines(cv::InputArray src, std::vector<Line> &hLines, std::vector<Line> &vLines)
{
    // Edge detection
    cv::Mat edges;
    cv::Canny(src, edges, 53, 113);

    // Line detection
    std::vector<cv::Vec2f> lines;
    cv::HoughLines(edges, lines, 1, CV_PI / 360, 180);

    // Separate horizontal and vertical lines
    for (cv::Vec2f &line : lines)
    {
        constexpr auto RANGE = static_cast<float>(CV_PI / 7); // A value between 0 and CV_PI / 4
        const float upperAngle = cosf(RANGE), lowerAngle = sinf(RANGE);

        const float sinTheta = sinf(line[1]);
        if (sinTheta < lowerAngle)
            hLines.push_back({line[0], line[1]});
        else if (sinTheta > upperAngle)
            vLines.push_back({line[0], line[1]});
    }
}

std::vector<cv::Point2f> solveIntersections(const std::vector<Line> &hLines, const std::vector<Line> &vLines)
{
    std::vector<cv::Point2f> points;
    for (Line line1 : hLines)
    {
        const float sin1 = sinf(line1.theta), cos1 = cosf(line1.theta);
        for (Line line2 : vLines)
        {
            const float sin2 = sinf(line2.theta), cos2 = cosf(line2.theta);
            const float det = cos1 * sin2 - sin1 * cos2;
            if (std::fabs(det) < 1e-5f)
                continue;
            points.emplace_back((line1.rou * sin2 - line2.rou * sin1) / det, (line2.rou * cos1 - line1.rou * cos2) / det);
        }
    }
    return points;
}

void calibratePoints(cv::InputArray src, std::vector<cv::Point2f> &points)
{
    cv::Mat edges;
    constexpr auto cmpPoints = [](const cv::Point2f &a, const cv::Point2f &b) -> bool {
        return (a.x < b.x) || ((a.x <= b.x) && a.y < b.y);
    };
    constexpr auto eqlPoints = [](const cv::Point2f &a, const cv::Point2f &b) -> bool {
        return std::fabs(a.x - b.x) < 0.5f && std::fabs(a.y - b.y) < 0.5f;
    };
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 100, 0.001);
    for (int s : {7, 5})
    {
        cornerSubPix(src, points, cv::Size(s, s), cv::Size(-1, -1), criteria);
        sort(points.begin(), points.end(), cmpPoints);
        points.erase(std::unique(points.begin(), points.end(), eqlPoints), points.end());
    }
}

bool sortPoints(const std::vector<cv::Point2f> &points, cv::Point2f result[4], int cols, int rows)
{
    cols >>= 1;
    rows >>= 1;
    float distances[4] = {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX};
    for (const cv::Point2f& point : points)
    {
        const float distance = hypotf(point.x - static_cast<float>(cols), point.y - static_cast<float>(cols));
        const auto id = static_cast<size_t>((static_cast<float>(cols) < point.x) | ((static_cast<float>(rows) < point.y) << 1));
        if (distance < distances[id])
        {
            result[id] = point;
            distances[id] = distance;
        }
    }
    return std::none_of(distances, distances + (sizeof(distances) / sizeof(int)),
                        [](const float x) -> bool { return fabs(x - FLT_MAX) < 1e-5f; });
}
