#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

// Function to plot histogram
void plotHistogram(unsigned char* imageData, int width, int height, int BytesPerPixel, const string &histogramPath);

int main(int argc, char *argv[]) {
    // Define file pointer and variables
    cv::Mat img;
    FILE *file;
    int BytesPerPixel;
    int Width = 596;  // Image width
    int Height = 340; // Image height
    

    // Check for proper syntax
    if (argc < 3){
        cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
        cout << "program_name input_image.raw output_image.raw [BytesPerPixel = 1]" << endl;
        return 0;
    }
    
    // Check if image is grayscale or color
    if (argc < 4){
        BytesPerPixel = 1; // Default is grey image
    } else {
        BytesPerPixel = atoi(argv[3]);
    }
    
    // Allocate image data array
    unsigned char* Imagedata = new unsigned char[Width * Height * BytesPerPixel];

    // Read image (filename specified by first argument) into image data matrix
    if (!(file=fopen(argv[1],"rb"))) {
        cout << "Cannot open file: " << argv[1] << endl;
        exit(1);
    }
    fread(Imagedata, sizeof(unsigned char), Width * Height * BytesPerPixel, file);
    fclose(file);

    string histogramPath = (argc >= 5) ? argv[4] : "histogram.png";

    // Plot histogram
    plotHistogram(Imagedata, Width, Height, BytesPerPixel, histogramPath);

    delete[] Imagedata;
    return 0;
}

void plotHistogram(unsigned char* imageData, int width, int height, int BytesPerPixel, const string &histogramPath) {
    int hist[256] = {0};

    // Calculate histogram
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < BytesPerPixel; k++) {
                int index = (i * width + j) * BytesPerPixel + k;
                hist[imageData[index]]++;
            }
        }
    }

    // Create histogram image
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound((double)hist_w / 256);
    Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0));

    // Normalize the histogram
    int max = *max_element(hist, hist + 256);
    for (int i = 0; i < 256; i++) {
        hist[i] = ((double)hist[i] / max) * histImage.rows;
    }

    // Draw each bin of histogram
    for (int i = 0; i < 256; i++) {
        cv::line(histImage, cv::Point(bin_w * i, hist_h),
                 cv::Point(bin_w * i, hist_h - hist[i]),
                 cv::Scalar(255), 1, 8, 0);
    }

    if (!histImage.empty()) {
        cv::imshow("Histogram", histImage);
        cv::waitKey(0);
    } else {
        cout << "Error: Cannot display histogram image." << endl;
    }

    cv::imwrite(histogramPath, histImage);

    // Display histogram
    cv::imshow("Histogram", histImage);
    cv::waitKey(0);
}
