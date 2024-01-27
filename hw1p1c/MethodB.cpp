#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

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


// Custom function to equalize histogram
void equalizeHistCustom(Mat &src, Mat &dst) {
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };

    // Calculate histogram
    Mat hist;
    calcHist(&src, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);

    // Create Cumulative Distribution Function (CDF)
    Mat cdf;
    hist.copyTo(cdf);
    for (int i = 1; i < histSize; i++) {
        cdf.at<float>(i) += cdf.at<float>(i - 1);
    }

    // Normalize CDF
    cdf /= (src.rows * src.cols);
    cdf *= 255.0;

    // Apply equalization
    dst = src.clone();
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            int pixelVal = src.at<uchar>(y, x);
            int equalizedVal = static_cast<int>(cdf.at<float>(pixelVal));
            dst.at<uchar>(y, x) = saturate_cast<uchar>(equalizedVal);
        }
    }
}

int main(int argc, char *argv[]) {
    FILE *file;
    int BytesPerPixel = 3;
    int Width = 750;  // Image width
    int Height = 422; // Image height
    
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

    // Read image (filename specified by first argument) into image data matrix
    if (!(file=fopen(argv[1],"rb"))) {
        cout << "Cannot open file: " << argv[1] << endl;
        exit(1);
    }
    fread(Imagedata, sizeof(unsigned char), Width * Height * BytesPerPixel, file);
    fclose(file);

    Mat image(Height, Width, CV_8UC3, Imagedata); // Assuming 24-bit RGB color

    // Convert from RGB to YUV
    Mat imageYUV;
    cvtColor(image, imageYUV, COLOR_RGB2YUV);

    vector<Mat> channels(3);
    split(imageYUV, channels);

    // Apply custom histogram equalization to the Y channel
    Mat yChannelEqualized;
    equalizeHistCustom(channels[0], yChannelEqualized);

    // Combine Y channel back with U and V channels
    channels[0] = yChannelEqualized;
    Mat resultImage;
    merge(channels, imageYUV);

    // Convert back to RGB
    cvtColor(imageYUV, resultImage, COLOR_YUV2RGB);

    if (!(file = fopen(argv[2], "wb"))) {
        cout << "Cannot open file: " << argv[2] << endl;
        exit(1);
    }
    fwrite(resultImage.data, sizeof(unsigned char), Width * Height * BytesPerPixel, file);
    fclose(file);

    // Display and save results
    imshow("Original Image", image);
    imshow("Enhanced Image", resultImage);
    imwrite("MethodB.jpg", resultImage);

    waitKey(0);

    delete[] Imagedata;
    return 0;
}
