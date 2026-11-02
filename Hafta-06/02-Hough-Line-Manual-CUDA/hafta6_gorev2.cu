
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <cuda_runtime.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

using namespace std;
using namespace cv;


// Her CUDA thread'i bir pikseli kontrol eder
__global__ void houghOyla(
    unsigned char* kenar,
    int* oylama,
    int genislik,
    int yukseklik,
    int rhoMax,
    int rhoSayisi,
    int thetaSayisi,
    float* sinDegerleri,
    float* cosDegerleri)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;


    if (x >= genislik || y >= yukseklik)
        return;


    // Sadece kenar pikseliyse oy ver
    if (kenar[y * genislik + x] > 0)
    {
        for (int theta = 0; theta < thetaSayisi; theta++)
        {
            float rho =
                x * cosDegerleri[theta] +
                y * sinDegerleri[theta];


            int rhoIndex =
                (int)roundf(rho) + rhoMax;


            if (rhoIndex >= 0 &&
                rhoIndex < rhoSayisi)
            {
                int index =
                    rhoIndex * thetaSayisi +
                    theta;


                // Birden fazla thread aynı yere oy verebilir
                atomicAdd(
                    &oylama[index],
                    1
                );
            }
        }
    }
}



int main()
{
    string giris =
        "/content/goruntu-isleme-dersi/Hafta-06/input/girdi.png";

    string cikis =
        "/content/goruntu-isleme-dersi/Hafta-06/02-Hough-Line-Manual-CUDA/";


    Mat resim =
        imread(giris, IMREAD_COLOR);


    if (resim.empty())
    {
        cout << "Resim okunamadi!" << endl;
        return -1;
    }


    int genislik = resim.cols;
    int yukseklik = resim.rows;


    cout << "Resim boyutu: "
         << genislik
         << " x "
         << yukseklik
         << endl;


    // -----------------------------
    // 1. Gri görüntü
    // -----------------------------

    Mat gri;

    cvtColor(
        resim,
        gri,
        COLOR_BGR2GRAY
    );


    // -----------------------------
    // 2. Canny
    // -----------------------------

    Mat canny;

    Canny(
        gri,
        canny,
        50,
        150
    );


    // -----------------------------
    // 3. Median filtre
    // -----------------------------

    Mat medianSonuc;

    medianBlur(
        canny,
        medianSonuc,
        3
    );


    imwrite(
        cikis + "01_canny.png",
        canny
    );

    imwrite(
        cikis + "02_median.png",
        medianSonuc
    );


    // -----------------------------
    // 4. Sin ve cos değerlerini
    //    baştan hesapla
    // -----------------------------

    int thetaSayisi = 180;

    vector<float> sinDegerleri(thetaSayisi);
    vector<float> cosDegerleri(thetaSayisi);


    for (int theta = 0; theta < thetaSayisi; theta++)
    {
        float radyan =
            theta * CV_PI / 180.0f;


        sinDegerleri[theta] =
            sin(radyan);

        cosDegerleri[theta] =
            cos(radyan);
    }


    cout << "Sin ve cos degerleri hesaplandi." << endl;


    // -----------------------------
    // 5. Rho aralığı
    // -----------------------------

    int rhoMax =
        (int)ceil(
            sqrt(
                genislik * genislik +
                yukseklik * yukseklik
            )
        );


    // rho değerleri -rhoMax ile +rhoMax arasında
    int rhoSayisi =
        2 * rhoMax + 1;


    cout << "Theta sayisi: "
         << thetaSayisi
         << endl;

    cout << "Rho sayisi: "
         << rhoSayisi
         << endl;


    // -----------------------------
    // 6. Oylama matrisi
    // -----------------------------

    int hucreSayisi =
        rhoSayisi * thetaSayisi;


    vector<int> oylama(
        hucreSayisi,
        0
    );


    // -----------------------------
    // 7. GPU belleği
    // -----------------------------

    unsigned char* gpuKenar;

    int* gpuOylama;

    float* gpuSin;
    float* gpuCos;


    int resimBoyutu =
        genislik *
        yukseklik *
        sizeof(unsigned char);


    int oylamaBoyutu =
        hucreSayisi *
        sizeof(int);


    cudaMalloc(
        (void**)&gpuKenar,
        resimBoyutu
    );

    cudaMalloc(
        (void**)&gpuOylama,
        oylamaBoyutu
    );

    cudaMalloc(
        (void**)&gpuSin,
        thetaSayisi * sizeof(float)
    );

    cudaMalloc(
        (void**)&gpuCos,
        thetaSayisi * sizeof(float)
    );


    // -----------------------------
    // 8. Verileri GPU'ya gönder
    // -----------------------------

    cudaMemcpy(
        gpuKenar,
        medianSonuc.data,
        resimBoyutu,
        cudaMemcpyHostToDevice
    );


    cudaMemcpy(
        gpuSin,
        sinDegerleri.data(),
        thetaSayisi * sizeof(float),
        cudaMemcpyHostToDevice
    );


    cudaMemcpy(
        gpuCos,
        cosDegerleri.data(),
        thetaSayisi * sizeof(float),
        cudaMemcpyHostToDevice
    );


    cudaMemset(
        gpuOylama,
        0,
        oylamaBoyutu
    );


    // -----------------------------
    // 9. CUDA thread yapısı
    // -----------------------------

    dim3 block(
        16,
        16
    );


    dim3 grid(
        (genislik + block.x - 1) / block.x,
        (yukseklik + block.y - 1) / block.y
    );


    // -----------------------------
    // 10. Süre ölçümü
    // -----------------------------

    cudaEvent_t baslangic;
    cudaEvent_t bitis;

    cudaEventCreate(&baslangic);
    cudaEventCreate(&bitis);


    cudaEventRecord(baslangic);


    houghOyla<<<grid, block>>>(
        gpuKenar,
        gpuOylama,
        genislik,
        yukseklik,
        rhoMax,
        rhoSayisi,
        thetaSayisi,
        gpuSin,
        gpuCos
    );


    cudaEventRecord(bitis);

    cudaEventSynchronize(bitis);


    float sure = 0;


    cudaEventElapsedTime(
        &sure,
        baslangic,
        bitis
    );


    cout << "CUDA oylama suresi: "
         << sure
         << " ms"
         << endl;


    // -----------------------------
    // 11. Oylama matrisini geri al
    // -----------------------------

    cudaMemcpy(
        oylama.data(),
        gpuOylama,
        oylamaBoyutu,
        cudaMemcpyDeviceToHost
    );


    // -----------------------------
    // 12. En yüksek oy
    // -----------------------------

    int enYuksekOy = 0;


    for (int i = 0; i < hucreSayisi; i++)
    {
        if (oylama[i] > enYuksekOy)
        {
            enYuksekOy =
                oylama[i];
        }
    }


    cout << "En yuksek oy: "
         << enYuksekOy
         << endl;


    // -----------------------------
    // 13. Oylama matrisini resim yap
    // -----------------------------

    Mat oylamaResmi(
        rhoSayisi,
        thetaSayisi,
        CV_8UC1,
        Scalar(0)
    );


    for (int rho = 0; rho < rhoSayisi; rho++)
    {
        for (int theta = 0; theta < thetaSayisi; theta++)
        {
            int index =
                rho * thetaSayisi +
                theta;


            int oy =
                oylama[index];


            int deger = 0;


            if (enYuksekOy > 0)
            {
                deger =
                    oy * 255 /
                    enYuksekOy;
            }


            oylamaResmi.at<uchar>(
                rho,
                theta
            ) =
                (uchar)deger;
        }
    }


    imwrite(
        cikis + "03_oylama_matrisi.png",
        oylamaResmi
    );


    // -----------------------------
    // 14. Eşiğe göre doğruları bul
    // -----------------------------

    int esik = 100;

    int bulunanDogru = 0;


    Mat sonuc =
        resim.clone();


    for (int rhoIndex = 1;
         rhoIndex < rhoSayisi - 1;
         rhoIndex++)
    {
        for (int theta = 1;
             theta < thetaSayisi - 1;
             theta++)
        {
            int index =
                rhoIndex * thetaSayisi +
                theta;


            int oy =
                oylama[index];


            if (oy >= esik)
            {
                // Komşularından da büyük mü?
                // Aynı doğrunun çok kez çizilmesini azaltır
                bool maksimum = true;


                for (int dr = -1; dr <= 1; dr++)
                {
                    for (int dt = -1; dt <= 1; dt++)
                    {
                        if (dr == 0 && dt == 0)
                            continue;


                        int komsuIndex =
                            (rhoIndex + dr) *
                            thetaSayisi +
                            (theta + dt);


                        if (oylama[komsuIndex] > oy)
                        {
                            maksimum = false;
                        }
                    }
                }


                if (!maksimum)
                    continue;


                int rho =
                    rhoIndex - rhoMax;


                double a =
                    cosDegerleri[theta];

                double b =
                    sinDegerleri[theta];


                double x0 =
                    a * rho;

                double y0 =
                    b * rho;


                Point nokta1;
                Point nokta2;


                nokta1.x =
                    cvRound(
                        x0 + 1000 * (-b)
                    );

                nokta1.y =
                    cvRound(
                        y0 + 1000 * a
                    );


                nokta2.x =
                    cvRound(
                        x0 - 1000 * (-b)
                    );

                nokta2.y =
                    cvRound(
                        y0 - 1000 * a
                    );


                line(
                    sonuc,
                    nokta1,
                    nokta2,
                    Scalar(0, 0, 255),
                    2
                );


                bulunanDogru++;
            }
        }
    }


    cout << "Esik degeri: "
         << esik
         << endl;

    cout << "Bulunan dogru sayisi: "
         << bulunanDogru
         << endl;


    imwrite(
        cikis + "04_hough_cuda_sonuc.png",
        sonuc
    );


    // -----------------------------
    // 15. Sonuçları dosyaya yaz
    // -----------------------------

    ofstream dosya(
        cikis + "sonuclar.txt"
    );


    dosya << "Theta sayisi: "
          << thetaSayisi
          << endl;

    dosya << "Rho sayisi: "
          << rhoSayisi
          << endl;

    dosya << "En yuksek oy: "
          << enYuksekOy
          << endl;

    dosya << "Esik degeri: "
          << esik
          << endl;

    dosya << "Bulunan dogru sayisi: "
          << bulunanDogru
          << endl;

    dosya << "CUDA oylama suresi: "
          << sure
          << " ms"
          << endl;


    dosya.close();


    // -----------------------------
    // Belleği temizle
    // -----------------------------

    cudaFree(gpuKenar);
    cudaFree(gpuOylama);
    cudaFree(gpuSin);
    cudaFree(gpuCos);

    cudaEventDestroy(baslangic);
    cudaEventDestroy(bitis);


    cout << "Sonuclar kaydedildi." << endl;


    return 0;
}
