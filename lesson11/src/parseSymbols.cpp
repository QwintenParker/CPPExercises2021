#include <opencv2/imgproc.hpp>
#include "parseSymbols.h"

std::vector<cv::Mat> splitSymbols(cv::Mat img)
{
    std::vector<cv::Mat> symbols;
    std::vector<std::vector<cv::Point>> contoursPoints;
    cv::findContours(img, contoursPoints, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
    for (int contourI = 0; contourI < contoursPoints.size(); ++contourI) {
        std::vector<cv::Point> points = contoursPoints[contourI];
        cv::Rect box = cv::boundingRect(points);
        cv::Mat imgBox(img, cv::Rect(box.tl(), box.br()));
        symbols.push_back(imgBox);
    }
    // TODO 101: чтобы извлечь кусок картинки (для каждого прямоугольника cv::Rect вокруг символа) - загуглите "opencv how to extract subimage"

    return symbols;
}
