
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <cuda_runtime.h>

#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;
using namespace cv;


// ----------------------
// PREWITT
// ----------------------

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


// ----------------------
// SOBEL
// ----------------------

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


// ----------------------
// LAPLACIAN
// ----------------------

__global__ void laplacianFiltre(
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

        deger += resim[(y - 1) * genislik + x];
        deger += resim[y * genislik + (x - 1)];
        deger += -4 * resim[y * genislik + x];
        deger += resim[y * genislik + (x + 1)];
        deger += resim[(y + 1) * genislik + x];

        if (deger < 0)
            deger = -deger;

        if (deger > 255)
            deger = 255;

        sonuc[y * genislik + x] =
            (unsigned char)deger;
    }
}


// ----------------------
// 7x7 LoG
// ----------------------

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


__global__ void logFiltre(
    unsigned char* resim,
    unsigned char* sonuc,
    int genislik,
    int yukseklik)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

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



int main()
{
    string klasor =
        "/content/goruntu-isleme-dersi/Hafta-04/07-Canny/";

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


    // Gürültüyü azaltmak için Gaussian
    Mat gaussian;

    GaussianBlur(
        resim,
        gaussian,
        Size(5, 5),
        1.0
    );


    // ==================================
    // CANNY - OpenCV
    // ==================================

    Mat cannySonuc;

    auto cannyBaslangic =
        chrono::high_resolution_clock::now();

    Canny(
        gaussian,
        cannySonuc,
        50,
        150
    );

    auto cannyBitis =
        chrono::high_resolution_clock::now();


    double cannySure =
        chrono::duration<double, milli>(
            cannyBitis - cannyBaslangic
        ).count();


    imwrite(
        klasor + "canny.png",
        cannySonuc
    );


    // ==================================
    // CUDA HAZIRLIK
    // ==================================

    int genislik = gaussian.cols;
    int yukseklik = gaussian.rows;

    int boyut =
        genislik * yukseklik *
        sizeof(unsigned char);


    unsigned char* gpuResim;

    unsigned char* gpuPrewitt;
    unsigned char* gpuSobel;
    unsigned char* gpuLaplacian;
    unsigned char* gpuLoG;


    cudaMalloc((void**)&gpuResim, boyut);

    cudaMalloc((void**)&gpuPrewitt, boyut);
    cudaMalloc((void**)&gpuSobel, boyut);
    cudaMalloc((void**)&gpuLaplacian, boyut);
    cudaMalloc((void**)&gpuLoG, boyut);


    cudaMemcpy(
        gpuResim,
        gaussian.data,
        boyut,
        cudaMemcpyHostToDevice
    );


    cudaMemset(gpuPrewitt, 0, boyut);
    cudaMemset(gpuSobel, 0, boyut);
    cudaMemset(gpuLaplacian, 0, boyut);
    cudaMemset(gpuLoG, 0, boyut);


    dim3 block(16, 16);

    dim3 grid(
        (genislik + block.x - 1) / block.x,
        (yukseklik + block.y - 1) / block.y
    );


    // CUDA süre ölçümü için event
    cudaEvent_t basla;
    cudaEvent_t bitir;

    cudaEventCreate(&basla);
    cudaEventCreate(&bitir);


    float prewittSure = 0;
    float sobelSure = 0;
    float laplacianSure = 0;
    float logSure = 0;


    // ==================================
    // PREWITT SÜRESİ
    // ==================================

    cudaEventRecord(basla);

    prewittFiltre<<<grid, block>>>(
        gpuResim,
        gpuPrewitt,
        genislik,
        yukseklik
    );

    cudaEventRecord(bitir);
    cudaEventSynchronize(bitir);

    cudaEventElapsedTime(
        &prewittSure,
        basla,
        bitir
    );


    // ==================================
    // SOBEL SÜRESİ
    // ==================================

    cudaEventRecord(basla);

    sobelFiltre<<<grid, block>>>(
        gpuResim,
        gpuSobel,
        genislik,
        yukseklik
    );

    cudaEventRecord(bitir);
    cudaEventSynchronize(bitir);

    cudaEventElapsedTime(
        &sobelSure,
        basla,
        bitir
    );


    // ==================================
    // LAPLACIAN SÜRESİ
    // ==================================

    cudaEventRecord(basla);

    laplacianFiltre<<<grid, block>>>(
        gpuResim,
        gpuLaplacian,
        genislik,
        yukseklik
    );

    cudaEventRecord(bitir);
    cudaEventSynchronize(bitir);

    cudaEventElapsedTime(
        &laplacianSure,
        basla,
        bitir
    );


    // ==================================
    // LoG SÜRESİ
    // ==================================

    cudaEventRecord(basla);

    logFiltre<<<grid, block>>>(
        gpuResim,
        gpuLoG,
        genislik,
        yukseklik
    );

    cudaEventRecord(bitir);
    cudaEventSynchronize(bitir);

    cudaEventElapsedTime(
        &logSure,
        basla,
        bitir
    );


    // ==================================
    // SONUÇLARI GERİ AL
    // ==================================

    Mat prewitt =
        Mat::zeros(yukseklik, genislik, CV_8UC1);

    Mat sobel =
        Mat::zeros(yukseklik, genislik, CV_8UC1);

    Mat laplacian =
        Mat::zeros(yukseklik, genislik, CV_8UC1);

    Mat logSonuc =
        Mat::zeros(yukseklik, genislik, CV_8UC1);


    cudaMemcpy(
        prewitt.data,
        gpuPrewitt,
        boyut,
        cudaMemcpyDeviceToHost
    );

    cudaMemcpy(
        sobel.data,
        gpuSobel,
        boyut,
        cudaMemcpyDeviceToHost
    );

    cudaMemcpy(
        laplacian.data,
        gpuLaplacian,
        boyut,
        cudaMemcpyDeviceToHost
    );

    cudaMemcpy(
        logSonuc.data,
        gpuLoG,
        boyut,
        cudaMemcpyDeviceToHost
    );


    imwrite(klasor + "prewitt.png", prewitt);
    imwrite(klasor + "sobel.png", sobel);
    imwrite(klasor + "laplacian.png", laplacian);
    imwrite(klasor + "log_7x7.png", logSonuc);


    // ==================================
    // SÜRELERİ DOSYAYA YAZ
    // ==================================

    ofstream dosya(
        klasor + "sureler.txt"
    );

    dosya << "Canny: "
          << cannySure
          << " ms" << endl;

    dosya << "Prewitt CUDA: "
          << prewittSure
          << " ms" << endl;

    dosya << "Sobel CUDA: "
          << sobelSure
          << " ms" << endl;

    dosya << "Laplacian CUDA: "
          << laplacianSure
          << " ms" << endl;

    dosya << "LoG CUDA: "
          << logSure
          << " ms" << endl;

    dosya.close();


    cout << endl;

    cout << "Canny: "
         << cannySure
         << " ms" << endl;

    cout << "Prewitt CUDA: "
         << prewittSure
         << " ms" << endl;

    cout << "Sobel CUDA: "
         << sobelSure
         << " ms" << endl;

    cout << "Laplacian CUDA: "
         << laplacianSure
         << " ms" << endl;

    cout << "LoG CUDA: "
         << logSure
         << " ms" << endl;


    cudaFree(gpuResim);

    cudaFree(gpuPrewitt);
    cudaFree(gpuSobel);
    cudaFree(gpuLaplacian);
    cudaFree(gpuLoG);

    cudaEventDestroy(basla);
    cudaEventDestroy(bitir);


    cout << endl;
    cout << "Tum sonuclar kaydedildi." << endl;

    return 0;
}
