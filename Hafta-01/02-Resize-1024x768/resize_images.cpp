%%writefile images.cpp

#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main()
{
    std::string folderPath = "/content/images";
    std::string outputPath = "/content/output";

    for (const auto& entry : fs::directory_iterator(folderPath))
    {
        if (!entry.is_regular_file())
            continue;

        std::string extension = entry.path().extension().string();

        if (extension == ".jpg" ||
            extension == ".jpeg" ||
            extension == ".png" ||
            extension == ".bmp")
        {
            std::cout << "Resim bulundu: "
                      << entry.path().filename()
                      << std::endl;

            cv::Mat image = cv::imread(entry.path().string());

            if (image.empty())
            {
                std::cout << "Resim okunamadi!" << std::endl;
                continue;
            }

            std::cout << "Orijinal boyut: "
                      << image.cols << " x "
                      << image.rows << std::endl;

            cv::Mat resizedImage;

            cv::resize(
                image,
                resizedImage,
                cv::Size(1024, 768)
            );

            std::cout << "Yeni boyut: "
                      << resizedImage.cols << " x "
                      << resizedImage.rows << std::endl;

            std::string savePath =
                outputPath + "/" +
                entry.path().filename().string();

            cv::imwrite(savePath, resizedImage);

            std::cout << "Kaydedildi: "
                      << savePath << std::endl;

            std::cout << "------------------------"
                      << std::endl;
        }
    }

    return 0;
}
