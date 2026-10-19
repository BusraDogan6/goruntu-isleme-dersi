
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <cuda_runtime.h>
#include <iostream>

using namespace std;
using namespace cv;


// Yatay Prewitt filtresi
__global__ void prewittYatay(
    unsigned char* resim,
    unsigned char* sonuc,
    int genislik,
    int yukseklik)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x > 0 && x < genislik - 1 &&
        y > 0 && y < yukseklik - 1)
    {
        int deger = 0;

        // Üst satır
        deger += -1 * resim[(y - 1) * genislik + (x - 1)];
        deger += -1 * resim[(y - 1) * genislik + x];
        deger += -1 * resim[(y - 1) * genislik + (x + 1)];

        // Alt satır
        deger += 1 * resim[(y + 1) * genislik + (x - 1)];
        deger += 1 * resim[(y + 1) * genislik + x];
        deger += 1 * resim[(y + 1) * genislik + (x + 1)];

        // Negatif değeri pozitife çevir
        if (deger < 0)
            deger = -deger;

        // 255'ten büyükse sınırla
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
        "/content/goruntu-isleme-dersi/Hafta-04/02-Prewitt-Yatay/gaussian.png";

    string prewittCikis =
        "/content/goruntu-isleme-dersi/Hafta-04/02-Prewitt-Yatay/prewitt_yatay.png";


    Mat resim = imread(giris, IMREAD_GRAYSCALE);

    if (resim.empty())
    {
        cout << "Resim okunamadi!" << endl;
        return -1;
    }

    cout << "Resim boyutu: "
         << resim.cols << " x "
         << resim.rows << endl;


    // Gaussian filtre
    Mat gaussian;

    GaussianBlur(
        resim,
        gaussian,
        Size(5, 5),
        1.0
    );

    imwrite(gaussianCikis, gaussian);

    cout << "Gaussian filtre uygulandi." << endl;


    int genislik = gaussian.cols;
    int yukseklik = gaussian.rows;

    int boyut =
        genislik * yukseklik * sizeof(unsigned char);


    unsigned char* gpuResim;
    unsigned char* gpuSonuc;


    cudaMalloc((void**)&gpuResim, boyut);
    cudaMalloc((void**)&gpuSonuc, boyut);


    // Resmi GPU'ya gönder
    cudaMemcpy(
        gpuResim,
        gaussian.data,
        boyut,
        cudaMemcpyHostToDevice
    );


    cudaMemset(
        gpuSonuc,
        0,
        boyut
    );


    dim3 block(16, 16);

    dim3 grid(
        (genislik + block.x - 1) / block.x,
        (yukseklik + block.y - 1) / block.y
    );


    // CUDA kernel
    prewittYatay<<<grid, block>>>(
        gpuResim,
        gpuSonuc,
        genislik,
        yukseklik
    );


    cudaDeviceSynchronize();


    Mat sonuc =
        Mat::zeros(
            yukseklik,
            genislik,
            CV_8UC1
        );


    // Sonucu GPU'dan geri al
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


    cout << "Yatay Prewitt filtresi uygulandi." << endl;
    cout << "Sonuc kaydedildi." << endl;

    return 0;
}
