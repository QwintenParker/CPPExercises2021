#include "hough.h"

#include <libutils/rasserts.h>

double toRadians(double degrees)
{
    const double PI = 3.14159265358979323846264338327950288;
    return degrees * PI / 180.0;
}

double estimateR(double x0, double y0, double theta0radians)
{
    double r0 = x0 * cos(theta0radians) + y0 * sin(theta0radians);
    return r0;
}

cv::Mat buildHough(cv::Mat sobel) {// единственный аргумент - это результат свертки Собелем изначальной картинки
    // проверяем что входная картинка - одноканальная и вещественная:
    rassert(sobel.type() == CV_32FC1, 237128273918006);

    // TODO
    // Эта функция по картинке с силами градиентов (после свертки оператором Собеля) строит пространство Хафа
    // Вы можете либо взять свою реализацию из прошлого задания, либо взять эту заготовку:

    int width = sobel.cols;
    int height = sobel.rows;

    // решаем какое максимальное значение у параметра theta в нашем пространстве прямых
    int max_theta = 360;

    // решаем какое максимальное значение у параметра r в нашем пространстве прямых:
    int max_r = width + height;

    // создаем картинку-аккумулятор, в которой мы будем накапливать суммарные голоса за прямые
    // так же известна как пространство Хафа
    cv::Mat accumulator(max_r, max_theta, CV_32FC1, 0.0f); // зануление аккумулятора через указание значения по умолчанию в конструкторе

    // проходим по всем пикселям нашей картинки (уже свернутой оператором Собеля)
    for (int y0 = 0; y0 < height; ++y0) {
        for (int x0 = 0; x0 < width; ++x0) {
            // смотрим на пиксель с координатами (x0, y0)
            float strength = sobel.at<float>(y0, x0);

            // теперь для текущего пикселя надо найти все возможные прямые которые через него проходят
            // переберем параметр theta по всему возможному диапазону (в градусах):
            for (int theta0 = 0; theta0 + 1 < max_theta; ++theta0) {

                double theta0radians = toRadians(theta0);
                double theta1radians = toRadians(theta0 + 1);
                int r0 = (int) round(estimateR(x0, y0, theta0radians)); // оцениваем r0 и округляем его до целого числа
                int r1 = (int) round(estimateR(x0, y0, theta1radians));
                if (r0 < 0 || r0 >= max_r)
                    continue;

                if (r1 < 0 || r1 >= max_r)
                    continue;

                int r_max = std::max(r1, r0);
                int r_min = std::min(r1, r0);

                double length =r_max - r_min;
                double k = 1;

                for (int r = r_min; r <= r_max; ++r) {
                    accumulator.at<float>(r, theta0) += (strength * k);
                    accumulator.at<float>(r, theta0 + 1) += strength * (1 - k);

                    k -= 1.0 / length;
                }

                // TODO надо определить в какие пиксели i,j надо внести наш голос с учетом проблемы "Почему два экстремума?" обозначенной на странице:
                // https://www.polarnick.com/blogs/239/2021/school239_11_2021_2022/2021/11/09/lesson9-hough2-interpolation-extremum-detection.html

//                        // чтобы проверить не вышли ли мы за пределы картинки-аккумулятора - давайте явно это проверим
//                        rassert(i >= 0, 237891731289044);
//                        rassert(i < accumulator.cols, 237891731289045);
//                        rassert(j >= 0, 237891731289046);
//                        rassert(j < accumulator.rows, 237891731289047);
//                        // теперь легко отладить случай выхода за пределы картинки
//                        // просто поставьте точку остановки внутри rassert:
//                        // нажмите Ctrl+Shift+N -> rasserts.cpp
//                        // и поставьте точку остановки на 8 строке: "return line;"
//
//                        // и добоавляем в картинку-аккумулятор наш голос с весом strength (взятый из картинки свернутой Собелем)
//                        accumulator.at<float>(j, i) += strength;
            }
        }
    }

    return accumulator;
}

std::vector<PolarLineExtremum> findLocalExtremums(cv::Mat houghSpace)
{
    rassert(houghSpace.type() == CV_32FC1, 234827498237080);

    const int max_theta = 360;
    rassert(houghSpace.cols == max_theta, 233892742893082);
    const int max_r = houghSpace.rows;

    std::vector<PolarLineExtremum> winners;

    for (int theta = 1; theta < max_theta - 1; ++theta) {
        for (int r = 1; r < max_r - 1; ++r) {

            float localExtr = houghSpace.at<float>(r, theta);
            bool ok = true;

            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    if (houghSpace.at<float>(r + j, theta + i) >= localExtr && (i != 0 && j !=0))
                        ok = false;
                }
            }

            if (ok) {
                PolarLineExtremum line(theta, r, localExtr);
                winners.push_back(line);
            }
        }
    }

    return winners;
}

std::vector<PolarLineExtremum> filterStrongLines(std::vector<PolarLineExtremum> allLines, double thresholdFromWinner)
{
    std::vector<PolarLineExtremum> strongLines;

    PolarLineExtremum strongestLine(0, 0, 0);
    for (int i = 0; i < allLines.size(); ++i) {
        if (allLines[i].votes > strongestLine.votes)
            strongestLine = allLines[i];
    }

    for (int i = 0; i < allLines.size(); ++i) {
        if (allLines[i].votes >= (strongestLine.votes * thresholdFromWinner))
            strongLines.push_back(allLines[i]);
    }
    // Эта функция по множеству всех найденных локальных экстремумов (прямых) находит самую популярную прямую
    // и возвращает только вектор из тех прямых, что не сильно ее хуже (набрали хотя бы thresholdFromWinner голосов от победителя, т.е. например половину)

    // TODO

    return strongLines;
}
