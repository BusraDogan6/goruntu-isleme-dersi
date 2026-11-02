
#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

typedef vector<vector<complex<double>>> KarmaMatris;


// =====================================================
// 1. MANUEL 2D DFT
// =====================================================

KarmaMatris DFT2D(const Mat& resim)
{
    int M = resim.rows;
    int N = resim.cols;

    KarmaMatris harmonikler(
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


                    double piksel =
                        resim.at<uchar>(x, y);


                    toplam +=
                        piksel * harmonik;
                }
            }


            harmonikler[u][v] =
                toplam;
        }
    }


    return harmonikler;
}


// =====================================================
// 2. MANUEL 2D IDFT
// =====================================================

Mat IDFT2D(const KarmaMatris& harmonikler)
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
                        *
                        harmonik;
                }
            }


            toplam =
                toplam /
                (double)(M * N);


            int deger =
                round(
                    toplam.real()
                );


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


// =====================================================
// 3. FREKANSLARI MERKEZE KAYDIR
//
// Fonksiyon iki kez uygulanirsa eski haline doner.
// =====================================================

KarmaMatris frekansKaydir(
    const KarmaMatris& F)
{
    int M = F.size();
    int N = F[0].size();


    KarmaMatris sonuc(
        M,
        vector<complex<double>>(N)
    );


    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int yeniI =
                (i + M / 2) % M;

            int yeniJ =
                (j + N / 2) % N;


            sonuc[yeniI][yeniJ] =
                F[i][j];
        }
    }


    return sonuc;
}


// =====================================================
// 4. YUMUSAK GAUSSIAN FREKANS FILTRESI
//
// Eski kod gibi yuksek frekanslari tamamen SIFIRLAMAZ.
//
// minKoruma sayesinde yuksek frekanslarin bir kismi
// korunur. Bu nedenle detay kaybi ve bulaniklik azalir.
// =====================================================

void yumusakGaussianFiltre(
    KarmaMatris& F,
    double sigma,
    double minKoruma)
{
    int M = F.size();
    int N = F[0].size();


    int merkezX = M / 2;
    int merkezY = N / 2;


    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            double dx =
                i - merkezX;

            double dy =
                j - merkezY;


            double uzaklikKare =
                dx * dx + dy * dy;


            // Normal Gaussian katsayisi
            double gaussian =
                exp(
                    -uzaklikKare /
                    (2.0 * sigma * sigma)
                );


            // Yuksek frekanslari tamamen silme.
            // Bir miktarini koru.
            double H =
                minKoruma
                +
                (1.0 - minKoruma)
                * gaussian;


            F[i][j] =
                F[i][j] * H;
        }
    }
}


// =====================================================
// 5. ZNCC
// =====================================================

double ZNCC(
    const Mat& resim1,
    const Mat& resim2)
{
    int pikselSayisi =
        resim1.rows
        *
        resim1.cols;


    double ortalama1 = 0.0;
    double ortalama2 = 0.0;


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


    ortalama1 /=
        pikselSayisi;

    ortalama2 /=
        pikselSayisi;


    double pay = 0.0;

    double payda1 = 0.0;
    double payda2 = 0.0;


    for (int y = 0; y < resim1.rows; y++)
    {
        for (int x = 0; x < resim1.cols; x++)
        {
            double fark1 =
                resim1.at<uchar>(y, x)
                -
                ortalama1;


            double fark2 =
                resim2.at<uchar>(y, x)
                -
                ortalama2;


            pay +=
                fark1 * fark2;


            payda1 +=
                fark1 * fark1;


            payda2 +=
                fark2 * fark2;
        }
    }


    double payda =
        sqrt(
            payda1 * payda2
        );


    if (payda == 0.0)
    {
        return 0.0;
    }


    return pay / payda;
}


// =====================================================
// 6. MSE
// =====================================================

double MSE(
    const Mat& resim1,
    const Mat& resim2)
{
    double toplam = 0.0;


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
        (
            resim1.rows
            *
            resim1.cols
        );
}


// =====================================================
// 7. MAE
// =====================================================

double MAE(
    const Mat& resim1,
    const Mat& resim2)
{
    double toplam = 0.0;


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
        (
            resim1.rows
            *
            resim1.cols
        );
}


// =====================================================
// 8. PSNR
// =====================================================

double PSNRHesapla(
    const Mat& resim1,
    const Mat& resim2)
{
    double mse =
        MSE(
            resim1,
            resim2
        );


    if (mse == 0)
    {
        return 100.0;
    }


    return
        10.0 *
        log10(
            (255.0 * 255.0)
            /
            mse
        );
}


// =====================================================
// 9. BIR GURULTULU GORUNTUYU ISLE
// =====================================================

Mat resmiTemizle(
    const Mat& gurultulu,
    double sigma,
    double minKoruma)
{
    // DFT
    KarmaMatris F =
        DFT2D(
            gurultulu
        );


    // Dusuk frekanslari merkeze getir
    KarmaMatris merkezli =
        frekansKaydir(
            F
        );


    // Yumusayarak filtrele
    yumusakGaussianFiltre(
        merkezli,
        sigma,
        minKoruma
    );


    // Frekanslari eski konumuna getir
    KarmaMatris geri =
        frekansKaydir(
            merkezli
        );


    // IDFT ile goruntuye don
    Mat sonuc =
        IDFT2D(
            geri
        );


    return sonuc;
}


// =====================================================
// MAIN
// =====================================================

int main()
{
    string gorev8 =
        "/content/goruntu-isleme-dersi/Hafta-07/08-Gurultu-DFT/";


    string cikis =
        "/content/goruntu-isleme-dersi/Hafta-07/09-IDFT-ZNCC/";


    // -------------------------------------------------
    // Resimleri oku
    // -------------------------------------------------

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
        orijinal.empty()
        ||
        gaussian.empty()
        ||
        saltPepper.empty()
    )
    {
        cout << "Resimler bulunamadi!"
             << endl;

        return -1;
    }


    // -------------------------------------------------
    // Filtre ayarlari
    //
    // Buyuk sigma =
    // daha fazla detay korunur.
    //
    // Buyuk minKoruma =
    // yuksek frekanslar daha fazla korunur.
    // -------------------------------------------------

    double gaussianSigma =
        24.0;

    double gaussianMinKoruma =
        0.25;


    double saltSigma =
        26.0;

    double saltMinKoruma =
        0.30;


    cout << "Gaussian goruntu isleniyor..."
         << endl;


    Mat gaussianTemiz =
        resmiTemizle(
            gaussian,
            gaussianSigma,
            gaussianMinKoruma
        );


    cout << "Salt & Pepper goruntu isleniyor..."
         << endl;


    Mat saltTemiz =
        resmiTemizle(
            saltPepper,
            saltSigma,
            saltMinKoruma
        );


    // -------------------------------------------------
    // Sonuclari kaydet
    // -------------------------------------------------

    imwrite(
        cikis + "01_gaussian_temiz.png",
        gaussianTemiz
    );


    imwrite(
        cikis + "02_salt_pepper_temiz.png",
        saltTemiz
    );


    // -------------------------------------------------
    // Gurultulu goruntulerin metrikleri
    // -------------------------------------------------

    double gaussianZNCCOnce =
        ZNCC(
            orijinal,
            gaussian
        );


    double saltZNCCOnce =
        ZNCC(
            orijinal,
            saltPepper
        );


    // -------------------------------------------------
    // Filtre sonrasi metrikler
    // -------------------------------------------------

    double gaussianZNCCSonra =
        ZNCC(
            orijinal,
            gaussianTemiz
        );


    double saltZNCCSonra =
        ZNCC(
            orijinal,
            saltTemiz
        );


    double gaussianMSE =
        MSE(
            orijinal,
            gaussianTemiz
        );


    double saltMSE =
        MSE(
            orijinal,
            saltTemiz
        );


    double gaussianMAE =
        MAE(
            orijinal,
            gaussianTemiz
        );


    double saltMAE =
        MAE(
            orijinal,
            saltTemiz
        );


    double gaussianPSNR =
        PSNRHesapla(
            orijinal,
            gaussianTemiz
        );


    double saltPSNR =
        PSNRHesapla(
            orijinal,
            saltTemiz
        );


    // -------------------------------------------------
    // Terminal sonucu
    // -------------------------------------------------

    cout << endl;

    cout << "================================"
         << endl;

    cout << "GAUSSIAN SONUCLARI"
         << endl;

    cout << "================================"
         << endl;


    cout << "Gurultulu ZNCC: "
         << gaussianZNCCOnce
         << endl;


    cout << "Temiz ZNCC    : "
         << gaussianZNCCSonra
         << endl;


    cout << "MSE           : "
         << gaussianMSE
         << endl;


    cout << "MAE           : "
         << gaussianMAE
         << endl;


    cout << "PSNR          : "
         << gaussianPSNR
         << endl;


    cout << endl;


    cout << "================================"
         << endl;

    cout << "SALT & PEPPER SONUCLARI"
         << endl;

    cout << "================================"
         << endl;


    cout << "Gurultulu ZNCC: "
         << saltZNCCOnce
         << endl;


    cout << "Temiz ZNCC    : "
         << saltZNCCSonra
         << endl;


    cout << "MSE           : "
         << saltMSE
         << endl;


    cout << "MAE           : "
         << saltMAE
         << endl;


    cout << "PSNR          : "
         << saltPSNR
         << endl;


    // -------------------------------------------------
    // TXT raporu
    // -------------------------------------------------

    ofstream rapor(
        cikis + "sonuclar.txt"
    );


    rapor
        << "GAUSSIAN GURULTU"
        << endl;


    rapor
        << "Gurultulu ZNCC: "
        << gaussianZNCCOnce
        << endl;


    rapor
        << "Temiz ZNCC: "
        << gaussianZNCCSonra
        << endl;


    rapor
        << "MSE: "
        << gaussianMSE
        << endl;


    rapor
        << "MAE: "
        << gaussianMAE
        << endl;


    rapor
        << "PSNR: "
        << gaussianPSNR
        << endl;


    rapor << endl;


    rapor
        << "SALT & PEPPER GURULTU"
        << endl;


    rapor
        << "Gurultulu ZNCC: "
        << saltZNCCOnce
        << endl;


    rapor
        << "Temiz ZNCC: "
        << saltZNCCSonra
        << endl;


    rapor
        << "MSE: "
        << saltMSE
        << endl;


    rapor
        << "MAE: "
        << saltMAE
        << endl;


    rapor
        << "PSNR: "
        << saltPSNR
        << endl;


    rapor.close();


    cout << endl;

    cout << "Tum islemler tamamlandi."
         << endl;


    return 0;
}
