#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

// Function to calculate and plot histogram
Mat plotHistogram(Mat& src) {
    // Establish the number of bins
    int histSize = 256;
    // Set the ranges (for B,G,R)
    float range[] = { 0, 256 };
    const float* histRange = { range };
    bool uniform = true, accumulate = false;
    Mat hist;
    // Compute the histograms
    calcHist(&src, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);

    // Draw the histograms
    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);
    Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0));
    // Normalize the result to [0, histImage.rows]
    normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    for (int i = 1; i < histSize; i++) {
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
             Point(bin_w * i, hist_h - cvRound(hist.at<float>(i))),
             Scalar(255), 2, 8, 0);
    }
    return histImage;
}


int main(int argc, char *argv[]) {
// Define file pointer and variables
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

    // Define the transfer function parameters
    float a = 1.5; // scale factor
    float b = 0;   // offset

    Mat image(Height, Width, CV_8UC1, Imagedata); /

    Mat histBefore = plotHistogram(image);
    imshow("Histogram Before", histBefore);
    imwrite("histogram_beforeB.jpg", histBefore);

    Mat enhancedImage;
    equalizeHist(image, enhancedImage);

    Mat histAfter = plotHistogram(enhancedImage);
    imshow("Histogram After", histAfter);
    imwrite("histogram_afterB.jpg", histAfter); 

    imshow("Original Image", image);
    imshow("Enhanced Image", enhancedImage);
    imwrite("enhanced_imageB.jpg", enhancedImage);

    waitKey(0);

    delete[] Imagedata;
    return 0;
}
