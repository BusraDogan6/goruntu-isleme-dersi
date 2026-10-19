
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <cuda_runtime.h>
#include <iostream>

using namespace std;
using namespace cv;


// 7x7 LoG çekirdeği
__device__ __constant__ int logKernel[49] =
{
     0,  0, -1, -1, -1,  0,  0,
     0, -1, -3, -3, -3, -1,  0,
    -1, -3,  0,  7,  0, -3, -1,
    -1, -3,  7, 24,  7, -3, -1,
    -1, -3,  0,  7,  0, -3, -1,
     0, -1, -3, -3, -3, -1,  0,
     0,  0, -1, -1, -1,  0,  0
};


// 7x7 LoG filtresi
__global__ void logFiltre(
    unsigned char* resim,
    unsigned char* sonuc,
    int genislik,
    int yukseklik)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    // 7x7 olduğu için kenarlardan 3 piksel içeri giriyoruz
    if (x >= 3 && x < genislik - 3 &&
        y >= 3 && y < yukseklik - 3)
    {
        int toplam = 0;

        for (int ky = -3; ky <= 3; ky++)
        {
            for (int kx = -3; kx <= 3; kx++)
            {
                int piksel =
                    resim[(y + ky) * genislik + (x + kx)];

                int kernelDegeri =
                    logKernel[(ky + 3) * 7 + (kx + 3)];

                toplam += piksel * kernelDegeri;
            }
        }

        if (toplam < 0)
            toplam = -toplam;

        if (toplam > 255)
            toplam = 255;

        sonuc[y * genislik + x] =
            (unsigned char)toplam;
    }
}


// Prewitt - yatay ve dikey sonucu birlikte
__global__ void prewittFiltre(
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
        int gx = 0;
        int gy = 0;

        gx += -resim[(y - 1) * genislik + (x - 1)];
        gx +=  resim[(y - 1) * genislik + (x + 1)];

        gx += -resim[y * genislik + (x - 1)];
        gx +=  resim[y * genislik + (x + 1)];

        gx += -resim[(y + 1) * genislik + (x - 1)];
        gx +=  resim[(y + 1) * genislik + (x + 1)];


        gy += -resim[(y - 1) * genislik + (x - 1)];
        gy += -resim[(y - 1) * genislik + x];
        gy += -resim[(y - 1) * genislik + (x + 1)];

        gy += resim[(y + 1) * genislik + (x - 1)];
        gy += resim[(y + 1) * genislik + x];
        gy += resim[(y + 1) * genislik + (x + 1)];


        if (gx < 0) gx = -gx;
        if (gy < 0) gy = -gy;

        int deger = gx + gy;

        if (deger > 255)
            deger = 255;

        sonuc[y * genislik + x] =
            (unsigned char)deger;
    }
}


// Sobel - yatay ve dikey sonucu birlikte
__global__ void sobelFiltre(
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
        int gx = 0;
        int gy = 0;

        gx += -resim[(y - 1) * genislik + (x - 1)];
        gx +=  resim[(y - 1) * genislik + (x + 1)];

        gx += -2 * resim[y * genislik + (x - 1)];
        gx +=  2 * resim[y * genislik + (x + 1)];

        gx += -resim[(y + 1) * genislik + (x - 1)];
        gx +=  resim[(y + 1) * genislik + (x + 1)];


        gy += -resim[(y - 1) * genislik + (x - 1)];
        gy += -2 * resim[(y - 1) * genislik + x];
        gy += -resim[(y - 1) * genislik + (x + 1)];

        gy += resim[(y + 1) * genislik + (x - 1)];
        gy += 2 * resim[(y + 1) * genislik + x];
        gy += resim[(y + 1) * genislik + (x + 1)];


        if (gx < 0) gx = -gx;
        if (gy < 0) gy = -gy;

        int deger = gx + gy;

        if (deger > 255)
            deger = 255;

        sonuc[y * genislik + x] =
            (unsigned char)deger;
    }
}


int main()
{
    string klasor =
        "/content/goruntu-isleme-dersi/Hafta-04/06-LoG/";

    string giris =
        "/content/goruntu-isleme-dersi/Hafta-04/input/girdi.png";


    Mat resim =
        imread(giris, IMREAD_GRAYSCALE);

    if (resim.empty())
    {
        cout << "Resim okunamadi!" << endl;
        return -1;
    }


    cout << "Resim boyutu: "
         << resim.cols << " x "
         << resim.rows << endl;


    // Gaussian uygulanmış görüntü
    Mat gaussian;

    GaussianBlur(
        resim,
        gaussian,
        Size(5, 5),
        1.0
    );

    imwrite(
        klasor + "gaussian.png",
        gaussian
    );


    int genislik = resim.cols;
    int yukseklik = resim.rows;

    int boyut =
        genislik * yukseklik *
        sizeof(unsigned char);


    unsigned char* gpuOrijinal;
    unsigned char* gpuGaussian;

    unsigned char* gpuLoG;
    unsigned char* gpuPrewittHam;
    unsigned char* gpuPrewittGauss;
    unsigned char* gpuSobelHam;
    unsigned char* gpuSobelGauss;


    cudaMalloc((void**)&gpuOrijinal, boyut);
    cudaMalloc((void**)&gpuGaussian, boyut);

    cudaMalloc((void**)&gpuLoG, boyut);
    cudaMalloc((void**)&gpuPrewittHam, boyut);
    cudaMalloc((void**)&gpuPrewittGauss, boyut);
    cudaMalloc((void**)&gpuSobelHam, boyut);
    cudaMalloc((void**)&gpuSobelGauss, boyut);


    cudaMemcpy(
        gpuOrijinal,
        resim.data,
        boyut,
        cudaMemcpyHostToDevice
    );

    cudaMemcpy(
        gpuGaussian,
        gaussian.data,
        boyut,
        cudaMemcpyHostToDevice
    );


    cudaMemset(gpuLoG, 0, boyut);
    cudaMemset(gpuPrewittHam, 0, boyut);
    cudaMemset(gpuPrewittGauss, 0, boyut);
    cudaMemset(gpuSobelHam, 0, boyut);
    cudaMemset(gpuSobelGauss, 0, boyut);


    dim3 block(16, 16);

    dim3 grid(
        (genislik + block.x - 1) / block.x,
        (yukseklik + block.y - 1) / block.y
    );


    // 7x7 LoG
    logFiltre<<<grid, block>>>(
        gpuOrijinal,
        gpuLoG,
        genislik,
        yukseklik
    );


    // Prewitt - Gaussian yok
    prewittFiltre<<<grid, block>>>(
        gpuOrijinal,
        gpuPrewittHam,
        genislik,
        yukseklik
    );


    // Prewitt - Gaussian var
    prewittFiltre<<<grid, block>>>(
        gpuGaussian,
        gpuPrewittGauss,
        genislik,
        yukseklik
    );


    // Sobel - Gaussian yok
    sobelFiltre<<<grid, block>>>(
        gpuOrijinal,
        gpuSobelHam,
        genislik,
        yukseklik
    );


    // Sobel - Gaussian var
    sobelFiltre<<<grid, block>>>(
        gpuGaussian,
        gpuSobelGauss,
        genislik,
        yukseklik
    );


    cudaDeviceSynchronize();


    Mat logSonuc =
        Mat::zeros(yukseklik, genislik, CV_8UC1);

    Mat prewittHam =
        Mat::zeros(yukseklik, genislik, CV_8UC1);

    Mat prewittGauss =
        Mat::zeros(yukseklik, genislik, CV_8UC1);

    Mat sobelHam =
        Mat::zeros(yukseklik, genislik, CV_8UC1);

    Mat sobelGauss =
        Mat::zeros(yukseklik, genislik, CV_8UC1);


    cudaMemcpy(
        logSonuc.data,
        gpuLoG,
        boyut,
        cudaMemcpyDeviceToHost
    );

    cudaMemcpy(
        prewittHam.data,
        gpuPrewittHam,
        boyut,
        cudaMemcpyDeviceToHost
    );

    cudaMemcpy(
        prewittGauss.data,
        gpuPrewittGauss,
        boyut,
        cudaMemcpyDeviceToHost
    );

    cudaMemcpy(
        sobelHam.data,
        gpuSobelHam,
        boyut,
        cudaMemcpyDeviceToHost
    );

    cudaMemcpy(
        sobelGauss.data,
        gpuSobelGauss,
        boyut,
        cudaMemcpyDeviceToHost
    );


    imwrite(klasor + "log_7x7.png", logSonuc);

    imwrite(
        klasor + "prewitt_gauss_yok.png",
        prewittHam
    );

    imwrite(
        klasor + "prewitt_gauss_var.png",
        prewittGauss
    );

    imwrite(
        klasor + "sobel_gauss_yok.png",
        sobelHam
    );

    imwrite(
        klasor + "sobel_gauss_var.png",
        sobelGauss
    );


    cudaFree(gpuOrijinal);
    cudaFree(gpuGaussian);

    cudaFree(gpuLoG);
    cudaFree(gpuPrewittHam);
    cudaFree(gpuPrewittGauss);
    cudaFree(gpuSobelHam);
    cudaFree(gpuSobelGauss);


    cout << "7x7 LoG tamamlandi." << endl;
    cout << "Prewitt karsilastirmalari tamamlandi." << endl;
    cout << "Sobel karsilastirmalari tamamlandi." << endl;
    cout << "Sonuclar kaydedildi." << endl;

    return 0;
}
