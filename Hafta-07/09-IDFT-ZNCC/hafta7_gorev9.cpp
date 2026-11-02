
#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


// ----------------------------------------------------
// Manuel 2D DFT
// ----------------------------------------------------

vector<vector<complex<double>>> DFT2D(
    const Mat& resim)
{
    int M = resim.rows;
    int N = resim.cols;

    vector<vector<complex<double>>> harmonikler(
        M,
        vector<complex<double>>(N)
    );


    for (int u = 0; u < M; u++)
    {
        for (int v = 0; v < N; v++)
        {
            complex<double> toplam(0.0, 0.0);


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


                    toplam +=
                        (double)resim.at<uchar>(x, y)
                        * harmonik;
                }
            }


            harmonikler[u][v] = toplam;
        }
    }


    return harmonikler;
}


// ----------------------------------------------------
// Manuel 2D IDFT
// ----------------------------------------------------

Mat IDFT2D(
    const vector<vector<complex<double>>>& harmonikler)
{
    int M = harmonikler.size();
    int N = harmonikler[0].size();

    Mat sonuc(
        M,
        N,
        CV_8UC1
    );


    for (int x = 0; x < M; x++)
    {
        for (int y = 0; y < N; y++)
        {
            complex<double> toplam(0.0, 0.0);


            for (int u = 0; u < M; u++)
            {
                for (int v = 0; v < N; v++)
                {
                    double aci =
                        2.0 * M_PI *
                        (
                            ((double)u * x / M)
                            +
                            ((double)v * y / N)
                        );


                    complex<double> harmonik(
                        cos(aci),
                        sin(aci)
                    );


                    toplam +=
                        harmonikler[u][v]
                        * harmonik;
                }
            }


            // IDFT'de N*M ile bolunur
            toplam =
                toplam /
                (double)(M * N);


            int deger =
                round(toplam.real());


            if (deger < 0)
                deger = 0;

            if (deger > 255)
                deger = 255;


            sonuc.at<uchar>(x, y) =
                deger;
        }
    }


    return sonuc;
}


// ----------------------------------------------------
// ZNCC
// Zero Mean Normalized Cross Correlation
// ----------------------------------------------------

double ZNCC(
    const Mat& resim1,
    const Mat& resim2)
{
    int toplamPiksel =
        resim1.rows * resim1.cols;


    double ortalama1 = 0;
    double ortalama2 = 0;


    // Ortalamalari hesapla
    for (int y = 0; y < resim1.rows; y++)
    {
        for (int x = 0; x < resim1.cols; x++)
        {
            ortalama1 +=
                resim1.at<uchar>(y, x);

            ortalama2 +=
                resim2.at<uchar>(y, x);
        }
    }


    ortalama1 /= toplamPiksel;
    ortalama2 /= toplamPiksel;


    double pay = 0;
    double payda1 = 0;
    double payda2 = 0;


    for (int y = 0; y < resim1.rows; y++)
    {
        for (int x = 0; x < resim1.cols; x++)
        {
            double fark1 =
                resim1.at<uchar>(y, x)
                - ortalama1;


            double fark2 =
                resim2.at<uchar>(y, x)
                - ortalama2;


            pay +=
                fark1 * fark2;


            payda1 +=
                fark1 * fark1;


            payda2 +=
                fark2 * fark2;
        }
    }


    double payda =
        sqrt(payda1 * payda2);


    if (payda == 0)
        return 0;


    return pay / payda;
}


// ----------------------------------------------------
// MSE
// ----------------------------------------------------

double MSE(
    const Mat& resim1,
    const Mat& resim2)
{
    double toplam = 0;


    for (int y = 0; y < resim1.rows; y++)
    {
        for (int x = 0; x < resim1.cols; x++)
        {
            double fark =
                (double)resim1.at<uchar>(y, x)
                -
                (double)resim2.at<uchar>(y, x);


            toplam +=
                fark * fark;
        }
    }


    return toplam /
        (resim1.rows * resim1.cols);
}


// ----------------------------------------------------
// MAE
// ----------------------------------------------------

double MAE(
    const Mat& resim1,
    const Mat& resim2)
{
    double toplam = 0;


    for (int y = 0; y < resim1.rows; y++)
    {
        for (int x = 0; x < resim1.cols; x++)
        {
            double fark =
                abs(
                    (double)resim1.at<uchar>(y, x)
                    -
                    (double)resim2.at<uchar>(y, x)
                );


            toplam += fark;
        }
    }


    return toplam /
        (resim1.rows * resim1.cols);
}



int main()
{
    string gorev8 =
        "/content/goruntu-isleme-dersi/Hafta-07/08-Gurultu-DFT/";

    string cikis =
        "/content/goruntu-isleme-dersi/Hafta-07/09-IDFT-ZNCC/";


    Mat orijinal =
        imread(
            gorev8 + "01_orijinal.png",
            IMREAD_GRAYSCALE
        );


    Mat gaussian =
        imread(
            gorev8 + "02_gaussian_gurultulu.png",
            IMREAD_GRAYSCALE
        );


    Mat saltPepper =
        imread(
            gorev8 + "03_salt_pepper_gurultulu.png",
            IMREAD_GRAYSCALE
        );


    if (
        orijinal.empty() ||
        gaussian.empty() ||
        saltPepper.empty()
    )
    {
        cout << "Girdi resimleri bulunamadi!" << endl;
        return -1;
    }


    cout << "Gaussian goruntu icin DFT hesaplaniyor..."
         << endl;


    vector<vector<complex<double>>> gaussianDFT =
        DFT2D(gaussian);


    cout << "Gaussian goruntu icin IDFT hesaplaniyor..."
         << endl;


    Mat gaussianYeniden =
        IDFT2D(gaussianDFT);


    imwrite(
        cikis + "01_gaussian_idft.png",
        gaussianYeniden
    );


    cout << endl;

    cout << "Salt & Pepper goruntu icin DFT hesaplaniyor..."
         << endl;


    vector<vector<complex<double>>> saltDFT =
        DFT2D(saltPepper);


    cout << "Salt & Pepper goruntu icin IDFT hesaplaniyor..."
         << endl;


    Mat saltYeniden =
        IDFT2D(saltDFT);


    imwrite(
        cikis + "02_salt_pepper_idft.png",
        saltYeniden
    );


    // -----------------------------------
    // ZNCC
    // -----------------------------------

    double gaussianZNCC =
        ZNCC(
            orijinal,
            gaussianYeniden
        );


    double saltZNCC =
        ZNCC(
            orijinal,
            saltYeniden
        );


    // Ek olarak MSE ve MAE de hesaplayalim

    double gaussianMSE =
        MSE(
            orijinal,
            gaussianYeniden
        );


    double saltMSE =
        MSE(
            orijinal,
            saltYeniden
        );


    double gaussianMAE =
        MAE(
            orijinal,
            gaussianYeniden
        );


    double saltMAE =
        MAE(
            orijinal,
            saltYeniden
        );


    cout << endl;

    cout << "-------------------------" << endl;
    cout << "SONUCLAR" << endl;
    cout << "-------------------------" << endl;


    cout << endl;

    cout << "Gaussian Gurultu:" << endl;

    cout << "ZNCC = "
         << gaussianZNCC
         << endl;

    cout << "MSE  = "
         << gaussianMSE
         << endl;

    cout << "MAE  = "
         << gaussianMAE
         << endl;


    cout << endl;


    cout << "Salt & Pepper Gurultu:" << endl;

    cout << "ZNCC = "
         << saltZNCC
         << endl;

    cout << "MSE  = "
         << saltMSE
         << endl;

    cout << "MAE  = "
         << saltMAE
         << endl;


    // Sonuclari dosyaya yaz

    ofstream dosya(
        cikis + "sonuclar.txt"
    );


    dosya << "Gaussian Gurultu" << endl;

    dosya << "ZNCC: "
          << gaussianZNCC
          << endl;

    dosya << "MSE: "
          << gaussianMSE
          << endl;

    dosya << "MAE: "
          << gaussianMAE
          << endl;


    dosya << endl;


    dosya << "Salt & Pepper Gurultu" << endl;

    dosya << "ZNCC: "
          << saltZNCC
          << endl;

    dosya << "MSE: "
          << saltMSE
          << endl;

    dosya << "MAE: "
          << saltMAE
          << endl;


    dosya.close();


    return 0;
}
