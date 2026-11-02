
#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


// 2 Boyutlu DFT
vector<vector<complex<double>>> DFT2D(
    const Mat& resim)
{
    int M = resim.rows;
    int N = resim.cols;


    vector<vector<complex<double>>> harmonikler(
        M,
        vector<complex<double>>(N)
    );


    // Frekans koordinatlari
    for (int u = 0; u < M; u++)
    {
        for (int v = 0; v < N; v++)
        {
            complex<double> toplam(0.0, 0.0);


            // Goruntu koordinatlari
            for (int x = 0; x < M; x++)
            {
                for (int y = 0; y < N; y++)
                {
                    double aci =
                        -2.0 * M_PI *
                        (
                            ((double)u * x / M)
                            +
                            ((double)v * y / N)
                        );


                    complex<double> harmonik(
                        cos(aci),
                        sin(aci)
                    );


                    double piksel =
                        resim.at<uchar>(x, y);


                    toplam +=
                        piksel * harmonik;
                }
            }


            harmonikler[u][v] =
                toplam;
        }


        cout
            << "Satir tamamlandi: "
            << u + 1
            << " / "
            << M
            << endl;
    }


    return harmonikler;
}



void harmonikKaydet(
    const vector<vector<complex<double>>>& harmonikler,
    string dosyaAdi)
{
    ofstream dosya(dosyaAdi);


    int M = harmonikler.size();
    int N = harmonikler[0].size();


    for (int u = 0; u < M; u++)
    {
        for (int v = 0; v < N; v++)
        {
            double gercek =
                harmonikler[u][v].real();


            double sanal =
                harmonikler[u][v].imag();


            double genlik =
                abs(harmonikler[u][v]);


            dosya
                << u << " "
                << v << " "
                << gercek << " "
                << sanal << " "
                << genlik
                << endl;
        }
    }


    dosya.close();
}



int main()
{
    string klasor =
        "/content/goruntu-isleme-dersi/Hafta-07/07-2D-DFT/";


    Mat resim =
        imread(
            klasor + "input/girdi.png"
        );


    if (resim.empty())
    {
        cout << "Resim bulunamadi!" << endl;
        return -1;
    }


    // Gri tona cevir
    Mat gri;

    cvtColor(
        resim,
        gri,
        COLOR_BGR2GRAY
    );


    // Manuel DFT cok maliyetli oldugu icin
    // resmi 64x64 boyutuna getiriyoruz
    Mat kucukResim;

    resize(
        gri,
        kucukResim,
        Size(64, 64)
    );


    imwrite(
        klasor + "gri_64.png",
        kucukResim
    );


    cout << "Resim boyutu: "
         << kucukResim.cols
         << " x "
         << kucukResim.rows
         << endl;


    cout << "2D DFT basladi..."
         << endl;


    vector<vector<complex<double>>> harmonikler =
        DFT2D(kucukResim);


    harmonikKaydet(
        harmonikler,
        klasor + "harmonikler.txt"
    );


    cout << endl;

    cout << "2D DFT tamamlandi."
         << endl;

    cout << "Harmonik vektor kaydedildi."
         << endl;


    return 0;
}
