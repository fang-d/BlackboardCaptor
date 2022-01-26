#include "transformation.hpp"
#include <algorithm>
#include <cmath>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

void transform(cv::Mat &image, const cv::Point2f points[4], const float ratio)
{
    constexpr auto dis = [](const cv::Point2f &a, const cv::Point2f &b) -> float {
        const float dx = (a.x - b.x), dy = (a.y - b.y);
        return sqrtf(dx * dx + dy * dy);
    };
    const float imgWidth = std::max(dis(points[0], points[1]), dis(points[2], points[3]));
    const float imgRowHeight = std::max(dis(points[0], points[2]), dis(points[1], points[3]));
    const float imgHeight = (imgWidth < imgRowHeight) ? imgWidth * ratio : imgWidth / ratio;
    cv::Point2f dst[4] = {{.0f, .0f}, {imgWidth, .0f}, {.0f, imgHeight}, {imgWidth, imgHeight}};
    cv::warpPerspective(image, image, cv::getPerspectiveTransform(points, dst, cv::DECOMP_LU),
                        cv::Size(static_cast<int>(imgWidth), static_cast<int>(imgHeight)));
}

void sharpen(cv::Mat &image)
{
    constexpr float STRENGTH = 5.1f; // Between 4 and 6
    cv::Mat kernel = (cv::Mat_<float>(3, 3) << 0, -1, 0, -1, STRENGTH, 0, -1, 0, -1);
    cv::filter2D(image, image, image.depth(), kernel);
    cv::Mat copy;
}

void binarize(cv::Mat &image)
{
    cv::Mat copy;
    cv::cvtColor(image, copy, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(copy, copy);
    image.release();
    cv::threshold(copy, image, 245, 255, cv::THRESH_BINARY_INV);
}
