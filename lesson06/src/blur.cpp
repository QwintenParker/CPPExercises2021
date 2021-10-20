#include "blur.h"

#include <libutils/rasserts.h>
#include <iostream>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

cv::Mat blur(cv::Mat img, double sigma) {
    // TODO формулу весов можно найти тут:
    // https://ru.wikipedia.org/wiki/%D0%A0%D0%B0%D0%B7%D0%BC%D1%8B%D1%82%D0%B8%D0%B5_%D0%BF%D0%BE_%D0%93%D0%B0%D1%83%D1%81%D1%81%D1%83

    int width = img.cols;
    int height = img.rows;
    cv::Mat blurImg(height, width, CV_8UC3);
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {

        }
    }


    return blurImg;
}
