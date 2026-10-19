
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <cuda_runtime.h>
#include <iostream>

using namespace std;
using namespace cv;


// 3x3 Laplacian filtresi
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

        // Laplacian çekirdeği:
        //
        //  0   1   0
        //  1  -4   1
        //  0   1   0

        deger += resim[(y - 1) * genislik + x];

        deger += resim[y * genislik + (x - 1)];

        deger += -4 * resim[y * genislik + x];

        deger += resim[y * genislik + (x + 1)];

        deger += resim[(y + 1) * genislik + x];


        // Negatif değerleri pozitife çevir
        if (deger < 0)
            deger = -deger;

        // 0-255 aralığında tut
        if (deger > 255)
            deger = 255;

        sonuc[y * genislik + x] =
            (unsigned char)deger;
    }
}


int main()
{
    string giris =
        "/content/goruntu-isleme-dersi/Hafta-04/input/girdi.png";

    string gaussianCikis =
        "/content/goruntu-isleme-dersi/Hafta-04/05-Laplacian/gaussian.png";

    string laplacianCikis =
        "/content/goruntu-isleme-dersi/Hafta-04/05-Laplacian/laplacian.png";


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


    // Önce Gaussian filtre
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
        genislik * yukseklik *
        sizeof(unsigned char);


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
    laplacianFiltre<<<grid, block>>>(
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


    // GPU'dan sonucu geri al
    cudaMemcpy(
        sonuc.data,
        gpuSonuc,
        boyut,
        cudaMemcpyDeviceToHost
    );


    imwrite(
        laplacianCikis,
        sonuc
    );


    cudaFree(gpuResim);
    cudaFree(gpuSonuc);


    cout << "Laplacian filtresi uygulandi." << endl;
    cout << "Sonuc kaydedildi." << endl;


    return 0;
}
