#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <set>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <memory>

#include <libutils/rasserts.h>
#include <libutils/fast_random.h>


// Эта функция говорит нам правда ли пиксель отмаскирован, т.е. отмечен как "удаленный", т.е. белый
bool isPixelMasked(cv::Mat mask, int j, int i) {
    rassert(j >= 0 && j < mask.rows, 372489347280017);
    rassert(i >= 0 && i < mask.cols, 372489347280018);
    rassert(mask.type() == CV_8UC3, 2348732984792380019);

    // TODO проверьте белый ли пиксель
    return mask.at<cv::Vec3b>(j, i) == cv::Vec3b(255, 255, 255);
}

double estimateQuality(cv::Mat mask, cv::Mat image, int j, int i, int ny, int nx, int height, int width){
    double distance = 0;
    for (int a = - height / 2; a <= height / 2; a++){
        for (int b = - width / 2; b <= width / 2; b++){
            if (j + a < 0 || j + a >= image.rows || i + b < 0 || i + b >= image.cols || ny + a < 0 || ny + a >= image.rows || nx + b < 0 || nx + b >= image.cols){
                distance += 10000000;
            } else if (isPixelMasked(mask, ny + a, nx + b)){
                distance += 100000000000;
            }
            else {
                cv::Vec3b d = image.at<cv::Vec3b>(j + a, i + b) - image.at<cv::Vec3b>(ny + a, nx + b);
                distance += cv::norm(d);
            }
        }
    }
    return distance;
}

void run(int caseNumber, std::string caseName) {
    std::cout << "_________Case #" << caseNumber << ": " <<  caseName << "_________" << std::endl;

    cv::Mat original = cv::imread("lesson18/data/" + std::to_string(caseNumber) + "_" + caseName + "/" + std::to_string(caseNumber) + "_original.jpg");
    cv::Mat mask = cv::imread("lesson18/data/" + std::to_string(caseNumber) + "_" + caseName + "/" + std::to_string(caseNumber) + "_mask.png");
    rassert(!original.empty(), 324789374290018);
    rassert(!mask.empty(), 378957298420019);

    // TODO напишите rassert сверяющий разрешение картинки и маски
    // TODO выведите в консоль это разрешение картинки
    rassert(original.size == mask.size, 2222223489765);
     std::cout << "Image resolution: " << original.size << std::endl;

    // создаем папку в которую будем сохранять результаты - lesson18/resultsData/ИМЯ_НАБОРА/
    std::string resultsDir = "lesson18/resultsData/";
    if (!std::filesystem::exists(resultsDir)) { // если папка еще не создана
        std::filesystem::create_directory(resultsDir); // то создаем ее
    }
    resultsDir += std::to_string(caseNumber) + "_" + caseName + "/";
    if (!std::filesystem::exists(resultsDir)) { // если папка еще не создана
        std::filesystem::create_directory(resultsDir); // то создаем ее
    }

    // сохраняем в папку с результатами оригинальную картинку и маску
    cv::imwrite(resultsDir + "0original.png", original);
    cv::imwrite(resultsDir + "1mask.png", mask);

    // TODO замените белым цветом все пиксели в оригинальной картинке которые покрыты маской
    // TODO сохраните в папку с результатами то что получилось под названием "2_original_cleaned.png"
    // TODO посчитайте и выведите число отмаскированных пикселей (числом и в процентах) - в таком формате:
    // Number of masked pixels: 7899/544850 = 1%

    int numberOfMasked = 0;
    cv::Mat originalCleaned = original.clone();
    for (int j = 0; j < original.rows; j++){
        for (int i = 0; i < original.cols; i++){
            if (isPixelMasked(mask, j, i)){
                originalCleaned.at<cv::Vec3b>(j, i) = cv::Vec3b(255, 255, 255);
                numberOfMasked++;
            }
        }
    }
    cv::imwrite(resultsDir + "2_original_cleaned.png", originalCleaned);

    {
        int numberOfPixels = original.cols * original.rows;
        int percentage = numberOfMasked * 100 / numberOfPixels;
        std::cout << "Number of masked pixels: " + std::to_string(numberOfMasked) + "/" + std::to_string(numberOfPixels) + " = " + std::to_string(percentage) + "%" + '\n';
    }


    FastRandom random(32542341); // этот объект поможет вам генерировать случайные гипотезы

    // TODO 10 создайте картинку хранящую относительные смещения - откуда брать донора для заплатки, см. подсказки про то как с нею работать на сайте
    // TODO 11 во всех отмаскированных пикселях: заполните эту картинку с относительными смещениями - случайными смещениями (но чтобы они и их окрестность 5х5 не выходила за пределы картинки)
    // TODO 12 во всех отмаскированных пикселях: замените цвет пиксела А на цвет пикселя Б на который указывает относительное смещение пикселя А
    // TODO 13 сохраните получившуюся картинку на диск
    // TODO 14 выполняйте эти шаги 11-13 много раз, например 1000 раз (оберните просто в цикл, сохраняйте картинку на диск только на каждой десятой или сотой итерации)
    // TODO 15 теперь давайте заменять значение относительного смещения на новой только если новая случайная гипотеза - лучше старой, добавьте оценку "насколько смещенный патч 5х5 похож на патч вокруг пикселя если их наложить"
    //
    // Ориентировочный псевдокод-подсказка получившегося алгоритма:
     cv::Mat shifts(original.rows, original.cols, CV_32SC2); // матрица хранящая смещения, изначально заполнена парами нулей
     cv::Mat image = original; // текущая картинка
     for (int j = 0; j < image.rows; j++) {
         for (int i = 0; i < image.cols; i++) {
             if (!isPixelMasked(mask, j, i))
                 continue; // пропускаем т.к. его менять не надо
             cv::Vec2i dxy = shifts.at<cv::Vec2i>(j, i);
             cv::Point pointD = cv::Point(i + dxy[1], j + dxy[0]);
 //            int (nx, ny) = (i + dxy.x, j + dxy.y); // ЭТО НЕ КОРРЕКТНЫЙ КОД, но он иллюстрирует как рассчитать координаты пикселя-донора из которого мы хотим брать цвет
             double currentQuality = estimateQuality(mask, image, j, i, pointD.y, pointD.x, 5, 5); // эта функция (создайте ее) считает насколько похож квадрат 5х5 приложенный центром к (i, j)
  //                                                                                                                          на квадрат 5х5 приложенный центром к (nx, ny)

//             int (rx, ry) = random.... // создаем случайное смещение относительно нашего пикселя, воспользуйтесь функцией random.next(...);
            // (окрестность вокруг пикселя на который укажет смещение - не должна выходить за пределы картинки и не должна быть отмаскирована)

            cv::Point randomD;
            bool goodP = false;
            while (!goodP){
                goodP = true;
                randomD = cv::Point(random.next(3, image.rows - 2), random.next(3, image.cols - 2));
                for (int a = - 5 / 2; a <= 5 / 2; a++){
                    for (int b = - 5 / 2; b <= 5 / 2; b++){
                        if (mask.at<cv::Vec3b>(randomD.y + a, randomD.x + b) == cv::Vec3b(255, 255, 255))
                            goodP = false;
                    }
                }
            }

             double randomQuality = estimateQuality(mask, image, j, i, randomD.y, randomD.x, 5, 5); // оцениваем насколько похоже будет если мы приложим эту случайную гипотезу которую только что выбрали

             int rx = i - randomD.x;
             int ry = j - randomD.y;

             if (randomQuality < currentQuality) {
                 shifts.at<cv::Vec2i>(j, i) = (ry, rx);
                 image.at<cv::Vec3b>(j, i) = image.at<cv::Vec3b>(randomD.y, randomD.x);
                 //(т.е. мы не весь патч сюда кладем, а только его центральный пиксель)
             } else {
                 image.at<cv::Vec3b>(j, i) = image.at<cv::Vec3b>(pointD.y, pointD.x);
             }
         }
         //не забываем сохранить на диск текущую картинку
         //а как численно оценить насколько уже хорошую картинку мы смогли построить? выведите в консоль это число
     }
}


int main() {
    try {
        run(1, "mic");
        // TODO протестируйте остальные случаи:
//        run(2, "flowers");
//        run(3, "baloons");
//        run(4, "brickwall");
//        run(5, "old_photo");
//        run(6, "your_data"); // TODO придумайте свой случай для тестирования (рекомендуется не очень большое разрешение, например 300х300)

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
