#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

void RGBtoYUV(const Mat &src, Mat &dst) {
    dst = Mat::zeros(src.size(), src.type());
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            Vec3b rgb = src.at<Vec3b>(y, x);
            float R = rgb[2];
            float G = rgb[1];
            float B = rgb[0];
            uchar Y = saturate_cast<uchar>(0.257 * R + 0.504 * G + 0.098 * B + 16);
            uchar U = saturate_cast<uchar>(-0.148 * R - 0.291 * G + 0.439 * B + 128);
            uchar V = saturate_cast<uchar>(0.439 * R - 0.368 * G - 0.071 * B + 128);
            dst.at<Vec3b>(y, x) = Vec3b(Y, U, V);
        }
    }
}

void YUVtoRGB(const Mat &src, Mat &dst) {
    dst = Mat::zeros(src.size(), src.type());
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            Vec3b yuv = src.at<Vec3b>(y, x);
            float Y = yuv[0];
            float U = yuv[1];
            float V = yuv[2];
            uchar R = saturate_cast<uchar>(1.164 * (Y - 16) + 1.596 * (V - 128));
            uchar G = saturate_cast<uchar>(1.164 * (Y - 16) - 0.813 * (V - 128) - 0.391 * (U - 128));
            uchar B = saturate_cast<uchar>(1.164 * (Y - 16) + 2.018 * (U - 128));
            dst.at<Vec3b>(y, x) = Vec3b(R, G, B);
        }
    }
}
int main(int argc, char *argv[]) {
    FILE *file;
    int BytesPerPixel = 3;
    int Width = 750;  
    int Height = 422;
    
    // Check for proper syntax
    if (argc < 3){
        cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
        cout << "program_name input_image.raw output_image.raw [BytesPerPixel = 1]" << endl;
        return 0;
    }
    
    // Check if image is grayscale or color
    if (argc < 3){
        cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
        cout << "program_name input_image.raw output_image.raw" << endl;
        return 0;
    }
    
    // Allocate image data array
    unsigned char* Imagedata = new unsigned char[Width * Height * BytesPerPixel];

    if (!(file=fopen(argv[1],"rb"))) {
        cout << "Cannot open file: " << argv[1] << endl;
        exit(1);
    }
    fread(Imagedata, sizeof(unsigned char), Width * Height * BytesPerPixel, file);
    fclose(file);

    Mat image(Height, Width, CV_8UC3, Imagedata); // Assuming 24-bit RGB color

    // Convert from RGB to YUV
    Mat imageYUV;
    RGBtoYUV(image, imageYUV);

    vector<Mat> channels(3);
    split(imageYUV, channels);

    // Apply CLAHE to the Y channel
    Ptr<CLAHE> clahe = createCLAHE();
    clahe->setClipLimit(2.0); 
    clahe->setTilesGridSize(Size(8, 8)); 
    Mat yChannelCLAHE;
    clahe->apply(channels[0], yChannelCLAHE);

    // Combine Y channel back with U and V channels
    channels[0] = yChannelCLAHE;
    Mat resultImage;
    merge(channels, imageYUV);

    // Convert back to RGB
    YUVtoRGB(imageYUV, resultImage);

    // Display and save results
    imshow("Original Image", image);
    imshow("Enhanced Image with CLAHE", resultImage);
    imwrite("Modified_CLAHE.jpg", resultImage);

    waitKey(0);

    delete[] Imagedata;
    return 0;
}
