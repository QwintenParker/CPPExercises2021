#include "helper_functions.h"
#include "random"
#include <libutils/rasserts.h>
#include <iostream>


cv::Mat makeAllBlackPixelsBlue(cv::Mat image) {
    // TODO реализуйте функцию которая каждый черный пиксель картинки сделает синим

    // ниже приведен пример как узнать цвет отдельного пикселя - состоящий из тройки чисел BGR (Blue Green Red)
    // чем больше значение одного из трех чисел - тем насыщеннее его оттенок
    // всего их диапазон значений - от 0 до 255 включительно
    // т.е. один байт, поэтому мы используем ниже тип unsigned char - целое однобайтовое неотрицательное число

    //cv::Vec3b color = image.at<cv::Vec3b>(13, 5); // взяли и узнали что за цвет в пикселе в 14-ом ряду (т.к. индексация с нуля) и 6-ой колонке
    //unsigned char blue = color[0]; // если это число равно 255 - в пикселе много синего, если равно 0 - в пикселе нет синего
    //unsigned char green = color[1];
    //unsigned char red = color[2];
//
    // как получить белый цвет? как получить черный цвет? как получить желтый цвет?
    // поэкспериментируйте! например можете всю картинку заполнить каким-то одним цветом

    // пример как заменить цвет по тем же координатам
    //red = 0;
    //green = 0;
    //blue = 250;
    // запустите эту версию функции и посмотрите на получившуюся картинку - lesson03/resultsData/01_blue_unicorn.jpg
    // какой пиксель изменился? почему он не чисто красный?
    for (int i = 0; i < image.cols; ++i) {
        for (int j = 0; j < image.rows; ++j) {
            cv::Vec3b color1 = image.at<cv::Vec3b>(j, i);
            if (color1 == cv::Vec3b(0, 0, 0))
                image.at<cv::Vec3b>(j, i) = cv::Vec3b(0, 255, 0);
        }
    }

    //image.at<cv::Vec3b>(13, 5) = cv::Vec3b(blue, green, red);

    return image;
}

cv::Mat invertImageColors(cv::Mat image) {
    // TODO реализуйте функцию которая каждый цвет картинки инвертирует:
    // т.е. пусть ночь станет днем, а сумрак рассеется
    // иначе говоря замените каждое значение яркости x на (255-x) (т.к находится в диапазоне от 0 до 255)
    for (int i = 0; i < image.cols; ++i) {
        for (int j = 0; j < image.rows; ++j) {
            cv::Vec3b color1 = image.at<cv::Vec3b>(j, i);
            char blue = color1[0];
            char green = color1[1];
            char red = color1[2];
            image.at<cv::Vec3b>(j, i) = cv::Vec3b(255 - blue, 255 - green, 255 - red);
        }
    }


    return image;
}

cv::Mat addBackgroundInsteadOfBlackPixels(cv::Mat object, cv::Mat background) {
    // TODO реализуйте функцию которая все черные пиксели картинки-объекта заменяет на пиксели с картинки-фона
    // т.е. что-то вроде накладного фона получится

    // гарантируется что размеры картинок совпадают - проверьте это через rassert, вот например сверка ширины:
    //rassert(object.rows == background.rows, 341241251251351);

    //cv::Vec3b color = object.at<cv::Vec3b>(13, 5);
    //unsigned char blue = color[0];
    //unsigned char green = color[1];
    //unsigned char red = color[2];

    for (int i = 0; i < object.cols; ++i) {
        for (int j = 0; j < object.rows; ++j) {
            cv::Vec3b color1 = object.at<cv::Vec3b>(j, i);
            if (color1 == cv::Vec3b(0, 0, 0))
                object.at<cv::Vec3b>(j, i) = background.at<cv::Vec3b>(j, i);
        }
    }

    return object;
}

cv::Mat addBackgroundInsteadOfBlackPixelsLargeBackground(cv::Mat object, cv::Mat largeBackground) {
    // теперь вам гарантируется что largeBackground гораздо больше - добавьте проверок этого инварианта (rassert-ов)

    // TODO реализуйте функцию так, чтобы нарисовался объект ровно по центру на данном фоне, при этом черные пиксели объекта не должны быть нарисованы
    //cv::Vec3b color = object.at<cv::Vec3b>(13, 5);
    //unsigned char blue = color[0];
    //unsigned char green = color[1];
    //unsigned char red = color[2];

    int centerWidth = (int) ((largeBackground.cols - object.cols) / 2);
    int centerHeight = (int) ((largeBackground.rows - object.rows) / 2);

    for (int i = centerWidth; i < centerWidth + object.cols; ++i) {
        for (int j = centerHeight; j < centerHeight + object.rows; ++j) {
            cv::Vec3b color1 = object.at<cv::Vec3b>(j - centerHeight, i - centerWidth);
            if (color1 != cv::Vec3b(0, 0, 0))
                largeBackground.at<cv::Vec3b>(j, i) = object.at<cv::Vec3b>(j - centerHeight, i - centerWidth);
        }
    }


    return largeBackground;
}

cv::Mat addNUnicornRandom(cv::Mat object, cv::Mat background) {
    //cv::Vec3b color = object.at<cv::Vec3b>(13, 5);
    //unsigned char blue = color[0];
    //unsigned char green = color[1];
    //unsigned char red = color[2];

    srand((unsigned)time(0));
    int n;
    n = (rand()%100);
    std::cout << "n:" << n  << std::endl;

    for (int l = 0; l < n; ++l) {
        int w = (rand()%1173);
        int h = (rand()%490);
        for (int i = w; i < w + object.cols; ++i) {
            for (int j = h; j < h + object.rows; ++j) {
                cv::Vec3b color1 = object.at<cv::Vec3b>(j - h, i - w);
                if (color1 != cv::Vec3b(0, 0, 0))
                    background.at<cv::Vec3b>(j, i) = object.at<cv::Vec3b>(j - h, i - w);
            }
        }
    }

    return background;
}

cv::Mat addStretchedUnicorn(cv::Mat object, cv::Mat background) {

    cv::Mat newUnicorn(591, 1274, CV_8UC3, cv::Scalar(0, 0, 0));

    for (int i = 0; i < newUnicorn.cols; ++i) {
        for (int j = 0; j < newUnicorn.rows; ++j) {
            int x = j * 1.0 /newUnicorn.rows * object.rows;
            int y = i * 1.0 /newUnicorn.cols * object.cols;
            newUnicorn.at<cv::Vec3b>(j, i) = object.at<cv::Vec3b>(x, y);
        }
    }

    for (int i = 0; i < newUnicorn.cols; ++i) {
        for (int j = 0; j < newUnicorn.rows; ++j) {
            cv::Vec3b color1 = newUnicorn.at<cv::Vec3b>(j, i);
            if (color1 != cv::Vec3b(0, 0, 0))
                background.at<cv::Vec3b>(j, i) = newUnicorn.at<cv::Vec3b>(j, i);
        }
    }


    return background;
}

