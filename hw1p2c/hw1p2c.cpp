#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;
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
int main(int argc, char *argv[]) {
    // Check for proper syntax
    if (argc < 5) {
        cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
        cout << "usage: " << argv[0] << " input_image.raw output_image.raw width height" << endl;
        return -1;
    }

    const int Width = atoi(argv[3]); 
    const int Height = atoi(argv[4]); 
    
    unsigned char* Imagedata = new unsigned char[Width * Height];

    // Read image (filename specified by first argument) into image data matrix
    FILE *file;
    if (!(file = fopen(argv[1], "rb"))) {
        cout << "Cannot open file: " << argv[1] << endl;
        delete[] Imagedata;
        return -1;
    }
    fread(Imagedata, sizeof(unsigned char), Width * Height, file);
    fclose(file);

    // Initialize Mat object from array
    Mat image(Height, Width, CV_8UC1, Imagedata);
    Mat filteredImage;

    // NLM Filter parameters
    float h = 10; // Luminance filter strength
    float hForColorComponents = 10; // Color filter strength (if using a color image)
    int templateWindowSize = 7; // Odd number (3, 5, 7...)
    int searchWindowSize = 21; // Odd number (21, 31, 41...)

    // Apply Non-Local Means Denoising
    fastNlMeansDenoising(image, filteredImage, h, templateWindowSize, searchWindowSize);

    // Save the filtered image to a raw file
    if (!(file = fopen(argv[2], "wb"))) {
        cout << "Cannot open file: " << argv[2] << endl;
        delete[] Imagedata;
        return -1;
    }
    fwrite(filteredImage.data, sizeof(unsigned char), Width * Height, file);
    fclose(file);
    cout << "Filtered image saved to " << argv[2] << endl;

    // Display the original and filtered images
    namedWindow("Original Image", WINDOW_AUTOSIZE);
    imshow("Original Image", image);
    namedWindow("Filtered Image", WINDOW_AUTOSIZE);
    imshow("Filtered Image", filteredImage);

    // Optionally, save the filtered image to a JPG file
    imwrite("NLM_filtered.jpg", filteredImage);

    string rawOutputFilename = string(argv[2]);
    saveRawImage(rawOutputFilename.c_str(), filteredImage);
    cout << "Saved filtered image as RAW file: " << rawOutputFilename << endl;

    waitKey(0);

    // Cleanup
    delete[] Imagedata;

    return 0;
}
