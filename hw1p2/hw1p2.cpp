#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

void plotHistogram(const unsigned char imageData[][256][1], int size, int BytesPerPixel);

int main(int argc, char *argv[]) {
    // Define file pointer and variables
    FILE *file;
    int BytesPerPixel;
    int Size = 256;
    
    // Check for proper syntax
    if (argc < 3){
        cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
        cout << "program_name input_image.raw output_image.raw [BytesPerPixel = 1] [Size = 256]" << endl;
        return 0;
    }
    
    // Check if image is grayscale or color
    if (argc < 4){
        BytesPerPixel = 1; // default is grey image
    } 
    else {
        BytesPerPixel = atoi(argv[3]);
        // Check if size is specified
        if (argc >= 5){
            Size = atoi(argv[4]);
        }
    }
    
    // Allocate image data array
    unsigned char* Imagedata = new unsigned char[Size * Size * BytesPerPixel];

    // Read image (filename specified by first argument) into image data matrix
    if (!(file=fopen(argv[1],"rb"))) {
        cout << "Cannot open file: " << argv[1] <<endl;
        exit(1);
    }
    fread(Imagedata, sizeof(unsigned char), Size * Size * BytesPerPixel, file);
    fclose(file);

    // Plot histogram
    plotHistogram(Imagedata, Size, BytesPerPixel);

    delete[] Imagedata;
    return 0;
}


void plotHistogram(unsigned char* imageData, int size, int BytesPerPixel) {
    int hist[256] = {0};

    // Calculate histogram
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            for (int k = 0; k < BytesPerPixel; k++) {
                int index = (i * size + j) * BytesPerPixel + k;
                hist[imageData[index]]++;
            }
        }
    }

    // Create histogram image
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound((double)hist_w / 256);
    Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0));

    // Normalize the histogram
    int max = hist[0];
    for (int i = 1; i < 256; i++) {
        if (max < hist[i]) {
            max = hist[i];
        }
    }
    for (int i = 0; i < 256; i++) {
        hist[i] = ((double)hist[i] / max) * histImage.rows;
    }
    // Draw each bin of histogram
    for (int i = 0; i < 256; i++) {
        cv::line(histImage, cv::Point(bin_w * i, hist_h),
                 cv::Point(bin_w * i, hist_h - hist[i]),
                 cv::Scalar(255), 1, 8, 0);
    }

    // Display histogram
    cv::imshow("Histogram", histImage);
    cv::waitKey(0);
}