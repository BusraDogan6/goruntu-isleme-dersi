
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;


// Histogram resmi oluşturan fonksiyon
void histogramCiz(int histogram[256], string dosyaAdi)
{
    int histGenislik = 512;
    int histYukseklik = 400;

    Mat histResmi(
        histYukseklik,
        histGenislik,
        CV_8UC3,
        Scalar(255, 255, 255)
    );

    // Histogramdaki en büyük frekansı bul
    int maxFrekans = 0;

    for (int i = 0; i < 256; i++)
    {
        if (histogram[i] > maxFrekans)
            maxFrekans = histogram[i];
    }

    // Histogramı çiz
    for (int i = 0; i < 256; i++)
    {
        int yukseklik =
            (int)(((double)histogram[i] / maxFrekans)
            * (histYukseklik - 20));

        line(
            histResmi,
            Point(i * 2, histYukseklik),
            Point(i * 2, histYukseklik - yukseklik),
            Scalar(0, 0, 0),
            1
        );
    }

    imwrite(dosyaAdi, histResmi);
}


int main()
{
    // -------------------------------------------------
    // 1. RESMİ TEK KANALLI OLARAK OKU
    // -------------------------------------------------

    Mat resim = imread(
        "/content/input_image",
        IMREAD_GRAYSCALE
    );

    if (resim.empty())
    {
        cout << "Resim okunamadi!" << endl;
        return -1;
    }

    cout << "Resim boyutu: "
         << resim.cols << " x "
         << resim.rows << endl;


    // -------------------------------------------------
    // 2. HISTOGRAM, MIN VE MAX
    // -------------------------------------------------

    int histogram[256] = {0};

    int minimum = 255;
    int maximum = 0;


    // Bütün resmi piksel piksel dolaşıyoruz
    for (int y = 0; y < resim.rows; y++)
    {
        for (int x = 0; x < resim.cols; x++)
        {
            uchar piksel = resim.at<uchar>(y, x);


            // Histogram
            histogram[piksel]++;


            // Minimum değer
            if (piksel < minimum)
            {
                minimum = piksel;
            }


            // Maksimum değer
            if (piksel > maximum)
            {
                maximum = piksel;
            }
        }
    }


    cout << "Minimum piksel degeri: "
         << minimum << endl;

    cout << "Maximum piksel degeri: "
         << maximum << endl;


    // Orijinal histogramı kaydet
    histogramCiz(
        histogram,
        "/content/original_histogram.png"
    );


    // -------------------------------------------------
    // 3. LINEAR CONTRAST STRETCHING
    // -------------------------------------------------

    Mat yeniResim = resim.clone();


    if (maximum != minimum)
    {
        for (int y = 0; y < resim.rows; y++)
        {
            for (int x = 0; x < resim.cols; x++)
            {
                uchar eskiPiksel =
                    resim.at<uchar>(y, x);


                // Doğrusal kontrast germe formülü

                double yeniDeger =
                    ((double)(eskiPiksel - minimum)
                    / (maximum - minimum))
                    * 255.0;


                yeniResim.at<uchar>(y, x) =
                    saturate_cast<uchar>(yeniDeger);
            }
        }
    }


    // -------------------------------------------------
    // 4. YENİ HISTOGRAM
    // -------------------------------------------------

    int yeniHistogram[256] = {0};


    for (int y = 0; y < yeniResim.rows; y++)
    {
        for (int x = 0; x < yeniResim.cols; x++)
        {
            uchar piksel =
                yeniResim.at<uchar>(y, x);

            yeniHistogram[piksel]++;
        }
    }


    histogramCiz(
        yeniHistogram,
        "/content/stretched_histogram.png"
    );


    // -------------------------------------------------
    // 5. SONUCU KAYDET
    // -------------------------------------------------

    imwrite(
        "/content/contrast_stretched.png",
        yeniResim
    );


    cout << endl;
    cout << "Contrast stretching tamamlandi."
         << endl;

    cout << "Yeni minimum = 0" << endl;
    cout << "Yeni maximum = 255" << endl;


    return 0;
}
