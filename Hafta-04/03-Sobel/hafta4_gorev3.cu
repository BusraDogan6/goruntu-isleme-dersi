
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <cuda_runtime.h>
#include <iostream>

using namespace std;
using namespace cv;


// Dikey Sobel
__global__ void sobelDikey(
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

        deger += -1 * resim[(y - 1) * genislik + (x - 1)];
        deger +=  1 * resim[(y - 1) * genislik + (x + 1)];

        deger += -2 * resim[y * genislik + (x - 1)];
        deger +=  2 * resim[y * genislik + (x + 1)];

        deger += -1 * resim[(y + 1) * genislik + (x - 1)];
        deger +=  1 * resim[(y + 1) * genislik + (x + 1)];

        if (deger < 0)
            deger = -deger;

        if (deger > 255)
            deger = 255;

        sonuc[y * genislik + x] = (unsigned char)deger;
    }
}


// Yatay Sobel
__global__ void sobelYatay(
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

        deger += -1 * resim[(y - 1) * genislik + (x - 1)];
        deger += -2 * resim[(y - 1) * genislik + x];
        deger += -1 * resim[(y - 1) * genislik + (x + 1)];

        deger += 1 * resim[(y + 1) * genislik + (x - 1)];
        deger += 2 * resim[(y + 1) * genislik + x];
        deger += 1 * resim[(y + 1) * genislik + (x + 1)];

        if (deger < 0)
            deger = -deger;

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
        "/content/goruntu-isleme-dersi/Hafta-04/03-Sobel/gaussian.png";

    string dikeyCikis =
        "/content/goruntu-isleme-dersi/Hafta-04/03-Sobel/sobel_dikey.png";

    string yatayCikis =
        "/content/goruntu-isleme-dersi/Hafta-04/03-Sobel/sobel_yatay.png";


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
    unsigned char* gpuDikey;
    unsigned char* gpuYatay;


    cudaMalloc((void**)&gpuResim, boyut);
    cudaMalloc((void**)&gpuDikey, boyut);
    cudaMalloc((void**)&gpuYatay, boyut);


    cudaMemcpy(
        gpuResim,
        gaussian.data,
        boyut,
        cudaMemcpyHostToDevice
    );


    cudaMemset(gpuDikey, 0, boyut);
    cudaMemset(gpuYatay, 0, boyut);


    dim3 block(16, 16);

    dim3 grid(
        (genislik + block.x - 1) / block.x,
        (yukseklik + block.y - 1) / block.y
    );


    // Dikey Sobel
    sobelDikey<<<grid, block>>>(
        gpuResim,
        gpuDikey,
        genislik,
        yukseklik
    );


    // Yatay Sobel
    sobelYatay<<<grid, block>>>(
        gpuResim,
        gpuYatay,
        genislik,
        yukseklik
    );


    cudaDeviceSynchronize();


    Mat dikey =
        Mat::zeros(yukseklik, genislik, CV_8UC1);

    Mat yatay =
        Mat::zeros(yukseklik, genislik, CV_8UC1);


    cudaMemcpy(
        dikey.data,
        gpuDikey,
        boyut,
        cudaMemcpyDeviceToHost
    );

    cudaMemcpy(
        yatay.data,
        gpuYatay,
        boyut,
        cudaMemcpyDeviceToHost
    );


    imwrite(dikeyCikis, dikey);
    imwrite(yatayCikis, yatay);


    cudaFree(gpuResim);
    cudaFree(gpuDikey);
    cudaFree(gpuYatay);


    cout << "Dikey Sobel filtresi uygulandi." << endl;
    cout << "Yatay Sobel filtresi uygulandi." << endl;
    cout << "Sonuclar kaydedildi." << endl;

    return 0;
}
