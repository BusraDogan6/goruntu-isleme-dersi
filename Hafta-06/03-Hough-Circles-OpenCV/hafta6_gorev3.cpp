
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

int main()
{
    string giris =
        "/content/goruntu-isleme-dersi/Hafta-06/input/girdi_cember.png";

    string cikis =
        "/content/goruntu-isleme-dersi/Hafta-06/03-Hough-Circles-OpenCV/";


    Mat resim = imread(giris);

    if (resim.empty())
    {
        cout << "Resim okunamadi!" << endl;
        return -1;
    }


    // 1. Gri tonlama
    Mat gri;

    cvtColor(
        resim,
        gri,
        COLOR_BGR2GRAY
    );


    // 2. Canny
    Mat canny;

    Canny(
        gri,
        canny,
        50,
        150
    );


    // 3. Median filtre
    Mat medianSonuc;

    medianBlur(
        canny,
        medianSonuc,
        5
    );


    // 4. Hough Circles
    vector<Vec3f> cemberler;

    HoughCircles(
        medianSonuc,
        cemberler,
        HOUGH_GRADIENT,
        1,
        30,
        150,
        20,
        10,
        200
    );


    Mat sonuc =
        resim.clone();


    // Bulunan çemberleri çiz
    for (int i = 0; i < cemberler.size(); i++)
    {
        int merkezX =
            cvRound(cemberler[i][0]);

        int merkezY =
            cvRound(cemberler[i][1]);

        int yaricap =
            cvRound(cemberler[i][2]);


        Point merkez(
            merkezX,
            merkezY
        );


        // Çemberi yeşil çiz
        circle(
            sonuc,
            merkez,
            yaricap,
            Scalar(0, 255, 0),
            2
        );


        // Merkezi kırmızı çiz
        circle(
            sonuc,
            merkez,
            3,
            Scalar(0, 0, 255),
            -1
        );
    }


    // Sonuçları kaydet
    imwrite(
        cikis + "01_gri.png",
        gri
    );

    imwrite(
        cikis + "02_canny.png",
        canny
    );

    imwrite(
        cikis + "03_median.png",
        medianSonuc
    );

    imwrite(
        cikis + "04_hough_circles.png",
        sonuc
    );


    cout << "Bulunan cember sayisi: "
         << cemberler.size()
         << endl;

    cout << "Sonuclar kaydedildi."
         << endl;


    return 0;
}
