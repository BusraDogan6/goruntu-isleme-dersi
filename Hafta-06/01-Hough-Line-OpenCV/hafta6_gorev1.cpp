
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

int main()
{
    string giris =
        "/content/goruntu-isleme-dersi/Hafta-06/input/girdi.png";

    string cikis =
        "/content/goruntu-isleme-dersi/Hafta-06/01-Hough-Line-OpenCV/";

    Mat resim = imread(giris);

    if (resim.empty())
    {
        cout << "Resim okunamadi!" << endl;
        return -1;
    }

    // Gri tonlama
    Mat gri;
    cvtColor(resim, gri, COLOR_BGR2GRAY);

    // Canny
    Mat canny;
    Canny(gri, canny, 50, 150);

    // Median filtre
    Mat medianSonuc;
    medianBlur(canny, medianSonuc, 3);


    // ------------------------
    // HoughLines
    // ------------------------

    vector<Vec2f> dogrular;

    HoughLines(
        medianSonuc,
        dogrular,
        1,
        CV_PI / 180,
        100
    );

    Mat hough1 = resim.clone();

    for (int i = 0; i < dogrular.size(); i++)
    {
        float rho = dogrular[i][0];
        float theta = dogrular[i][1];

        double a = cos(theta);
        double b = sin(theta);

        double x0 = a * rho;
        double y0 = b * rho;

        Point nokta1;
        Point nokta2;

        nokta1.x = cvRound(x0 + 1000 * (-b));
        nokta1.y = cvRound(y0 + 1000 * a);

        nokta2.x = cvRound(x0 - 1000 * (-b));
        nokta2.y = cvRound(y0 - 1000 * a);

        // Kırmızı
        line(
            hough1,
            nokta1,
            nokta2,
            Scalar(0, 0, 255),
            2
        );
    }


    // ------------------------
    // HoughLinesP
    // ------------------------

    vector<Vec4i> dogrularP;

    HoughLinesP(
        medianSonuc,
        dogrularP,
        1,
        CV_PI / 180,
        50,
        50,
        10
    );

    Mat hough2 = resim.clone();

    for (int i = 0; i < dogrularP.size(); i++)
    {
        Vec4i d = dogrularP[i];

        // Yeşil
        line(
            hough2,
            Point(d[0], d[1]),
            Point(d[2], d[3]),
            Scalar(0, 255, 0),
            2
        );
    }


    // Sonuçları kaydet
    imwrite(cikis + "01_gri.png", gri);
    imwrite(cikis + "02_canny.png", canny);
    imwrite(cikis + "03_median.png", medianSonuc);
    imwrite(cikis + "04_hough_lines.png", hough1);
    imwrite(cikis + "05_hough_lines_p.png", hough2);


    cout << "HoughLines dogru sayisi: "
         << dogrular.size()
         << endl;

    cout << "HoughLinesP dogru sayisi: "
         << dogrularP.size()
         << endl;

    cout << "Sonuclar kaydedildi." << endl;

    return 0;
}
