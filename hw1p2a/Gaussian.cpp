#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

// Function to create a Gaussian kernel
Mat createGaussianKernel(int filterSize, double sigma) {
    Mat kernel = Mat(filterSize, filterSize, CV_64F);
    double r, s = 2.0 * sigma * sigma;

    // sum is for normalization
    double sum = 0.0;

    // Generate kernel
    for (int x = -filterSize/2; x <= filterSize/2; x++) {
        for (int y = -filterSize/2; y <= filterSize/2; y++) {
            r = sqrt(x*x + y*y);
            kernel.at<double>(x + filterSize/2, y + filterSize/2) = (exp(-(r*r)/s)) / (M_PI * s);
            sum += kernel.at<double>(x + filterSize/2, y + filterSize/2);
        }
    }

    // Normalize the kernel
    for (int i = 0; i < filterSize; ++i)
        for (int j = 0; j < filterSize; ++j)
            kernel.at<double>(i, j) /= sum;

    return kernel;
}

void saveRawImage(const char* filename, const Mat& image) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        cerr << "Cannot open file for writing: " << filename << endl;
        return;
    }

    int width = image.cols;
    int height = image.rows;
    int bytesPerPixel = image.channels(); 
    cout << "width: " << width << endl;
    cout << "height: " << height << endl;
    cout << "bytesPerPixel: " << bytesPerPixel << endl;
    size_t totalBytes = width * height * bytesPerPixel;

    size_t written = fwrite(image.data, sizeof(unsigned char), totalBytes, file);

    if (written != totalBytes) {
        cerr << "Error writing the image data to file: " << filename << endl;
    } else {
        cout << "Image successfully saved as: " << filename << endl;
    }
    fclose(file);
}

// Function to apply Gaussian filter
void ApplyGaussianFilter(Mat &inputImage, Mat &outputImage, int filterSize, double sigma) {
    int pad = filterSize / 2;
    Mat paddedInput;
    Mat kernel = createGaussianKernel(filterSize, sigma);

    copyMakeBorder(inputImage, paddedInput, pad, pad, pad, pad, BORDER_REPLICATE);

    for (int i = pad; i < inputImage.rows + pad; ++i) {
        for (int j = pad; j < inputImage.cols + pad; ++j) {
            double sum = 0.0; 
            for (int fi = -pad; fi <= pad; ++fi) {
                for (int fj = -pad; fj <= pad; ++fj) {
                    sum += paddedInput.at<uchar>(i + fi, j + fj) * kernel.at<double>(fi + pad, fj + pad);
                }
            }
            outputImage.at<uchar>(i - pad, j - pad) = static_cast<uchar>(sum);
        }
    }
}
int main(int argc, char** argv) {
    // Check for proper syntax
    if (argc != 3) {
        cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
        cout << "usage: " << argv[0] << " input_image.raw output_image.jpg" << endl;
        return 0;
    }

    const int width = 768;
    const int height = 512;

    // Read image (filename specified by first argument) into Mat object
    Mat inputImage(height, width, CV_8UC1);
    FILE* file;
    if (!(file = fopen(argv[1], "rb"))) {
        cout << "Cannot open file: " << argv[1] <<endl;
        return 1;
    }
    fread(inputImage.data, sizeof(unsigned char), height * width, file);
    fclose(file);

    // Create output Mat object with the same size and type as inputImage
    Mat outputImage = Mat::zeros(inputImage.size(), inputImage.type());

    // Apply uniform filter to the input image
    ApplyGaussianFilter(inputImage, outputImage, 3, 7);

    // Display the original and filtered images
    namedWindow("Original Image", WINDOW_AUTOSIZE);
    imshow("Original Image", inputImage);

    namedWindow("Filtered Image", WINDOW_AUTOSIZE);
    imshow("Filtered Image", outputImage);
    imwrite("Gaussian_fixed_7x7.jpg", outputImage);

    waitKey(0); // Wait for a keystroke in the window

    string rawOutputFilename = string(argv[2]);
    saveRawImage(rawOutputFilename.c_str(), outputImage);
    cout << "Saved filtered image as RAW file: " << rawOutputFilename << endl;

    return 0;
}
