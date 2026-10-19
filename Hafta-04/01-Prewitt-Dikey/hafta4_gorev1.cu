
#include <opencv2/opencv.hpp>
#include <cuda_runtime.h>
#include <iostream>

using namespace std;
using namespace cv;


// Dikey Prewitt filtresi
__global__ void prewittDikey(
    unsigned char* resim,
    unsigned char* sonuc,
    int genislik,
    int yukseklik)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    // Kenar pikselleri işlemiyoruz
    if (x > 0 && x < genislik - 1 &&
        y > 0 && y < yukseklik - 1)
    {
        int deger = 0;

        deger += -1 * resim[(y - 1) * genislik + (x - 1)];
        deger +=  1 * resim[(y - 1) * genislik + (x + 1)];

        deger += -1 * resim[y * genislik + (x - 1)];
        deger +=  1 * resim[y * genislik + (x + 1)];

        deger += -1 * resim[(y + 1) * genislik + (x - 1)];
        deger +=  1 * resim[(y + 1) * genislik + (x + 1)];

        // Negatif değerleri pozitife çevir
        if (deger < 0)
            deger = -deger;

        // 0-255 aralığında tut
        if (deger > 255)
            deger = 255;

        sonuc[y * genislik + x] = (unsigned char)deger;
    }
}


int main()
{
    string giris =
        "/content/goruntu-isleme-dersi/Hafta-04/input/girdi.png";

    string gaussianCikis =
        "/content/goruntu-isleme-dersi/Hafta-04/01-Prewitt-Dikey/gaussian.png";

    string prewittCikis =
        "/content/goruntu-isleme-dersi/Hafta-04/01-Prewitt-Dikey/prewitt_dikey.png";


    // Resmi gri tonlamalı aç
    Mat resim = imread(giris, IMREAD_GRAYSCALE);

    if (resim.empty())
    {
        cout << "Resim okunamadi!" << endl;
        return -1;
    }

    cout << "Resim boyutu: "
         << resim.cols << " x "
         << resim.rows << endl;


    // Önce Gaussian filtre ile hafif yumuşatma
    Mat gaussian;

    GaussianBlur(
        resim,
        gaussian,
        Size(5, 5),
        1.0
    );

    imwrite(
        gaussianCikis,
        gaussian
    );

    cout << "Gaussian filtre uygulandi." << endl;


    int genislik = gaussian.cols;
    int yukseklik = gaussian.rows;

    int boyut =
        genislik * yukseklik * sizeof(unsigned char);


    // GPU için değişkenler
    unsigned char* gpuResim;
    unsigned char* gpuSonuc;


    cudaMalloc(
        (void**)&gpuResim,
        boyut
    );

    cudaMalloc(
        (void**)&gpuSonuc,
        boyut
    );


    // Görüntüyü GPU'ya gönder
    cudaMemcpy(
        gpuResim,
        gaussian.data,
        boyut,
        cudaMemcpyHostToDevice
    );


    // Sonuç belleğini sıfırla
    cudaMemset(
        gpuSonuc,
        0,
        boyut
    );


    // CUDA blok ve grid ayarları
    dim3 block(16, 16);

    dim3 grid(
        (genislik + block.x - 1) / block.x,
        (yukseklik + block.y - 1) / block.y
    );


    // Prewitt kernelini GPU'da çalıştır
    prewittDikey<<<grid, block>>>(
        gpuResim,
        gpuSonuc,
        genislik,
        yukseklik
    );


    cudaDeviceSynchronize();


    // Sonuç görüntüsü
    Mat sonuc =
        Mat::zeros(
            yukseklik,
            genislik,
            CV_8UC1
        );


    // GPU'dan sonucu geri al
    cudaMemcpy(
        sonuc.data,
        gpuSonuc,
        boyut,
        cudaMemcpyDeviceToHost
    );


    imwrite(
        prewittCikis,
        sonuc
    );


    cudaFree(gpuResim);
    cudaFree(gpuSonuc);


    cout << "Dikey Prewitt filtresi uygulandi." << endl;
    cout << "Sonuc kaydedildi." << endl;

    return 0;
}
