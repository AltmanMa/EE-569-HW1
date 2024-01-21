// This sample code reads in image data from a RAW image file and 
// writes it into another file

// NOTE:	The code assumes that the image is of size 256 x 256 and is in the
//			RAW format. You will need to make corresponding changes to
//			accommodate images of different sizes and/or types

#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;
bool isRed(int i, int j) {
    return i % 2 != 0 && j % 2 == 0;
}

bool isBlue(int i, int j) {
    return i % 2 == 0 && j % 2 != 0;
}

bool isGreen(int i, int j) {
    return !isRed(i, j) && !isBlue(i, j);
}

int main(int argc, char *argv[])

{
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
	unsigned char Imagedata[Size][Size][BytesPerPixel];
	unsigned char*** RGBImagedata = new unsigned char**[Size];
	for (int i = 0; i < Size; ++i) {
    	RGBImagedata[i] = new unsigned char*[Size];
    	for (int j = 0; j < Size; ++j) {
        	RGBImagedata[i][j] = new unsigned char[3];
    	}	
	}

	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size*Size*BytesPerPixel, file);
	fclose(file);


	for (int i = 0; i < Size; ++i) {
    	for (int j = 0; j < Size; ++j) {
        	if (isRed(i, j)) {
				RGBImagedata[i][j][0] = Imagedata[i][j][0];
				float blue = 0.0;
				float green = 0.0;
				int countblue = 0;
				int countgreen = 0;
				if(i>0){
					green += Imagedata[i-1][j][0];
					countgreen++;
					if(j>0){
						blue += Imagedata[i-1][j-1][0];
						countblue++;
					}
					if(j<Size-1){
						blue += Imagedata[i-1][j+1][0];
						countblue++;
					}
				}
				if(i<Size-1){
					green += Imagedata[i+1][j][0];
					countgreen++;
					if(j>0){
						blue += Imagedata[i+1][j-1][0];
						countblue++;
					}
					if(j<Size-1){
						blue += Imagedata[i+1][j+1][0];
						countblue++;
					}
				}
				if(j>0){
					green += Imagedata[i][j-1][0];
					countgreen++;
				}
				if(j<Size-1){
					green += Imagedata[i][j+1][0];
					countgreen++;
				}
				if (countgreen != 0){
					RGBImagedata[i][j][1] = green/countgreen;
					}
				if (countblue != 0) {
					RGBImagedata[i][j][2] = blue/countblue;
					}

            // Interpolate missing green and blue values for a red pixel
            // Be careful at the image boundaries
        	} else if (isBlue(i, j)) {
				RGBImagedata[i][j][2] = Imagedata[i][j][0];
				float red = 0.0;
				float green = 0.0;
				int countred = 0;
				int countgreen = 0;
				if(i>0){
					green += Imagedata[i-1][j][0];
					countgreen++;
					if(j>0){
						red += Imagedata[i-1][j-1][0];
						countred++;
					}
					if(j<Size-1){
						red += Imagedata[i-1][j+1][0];
						countred++;
					}
				}
				if(i<Size-1){
					green += Imagedata[i+1][j][0];
					countgreen++;
					if(j>0){
						red += Imagedata[i+1][j-1][0];
						countred++;
					}
					if(j<Size-1){
						red += Imagedata[i+1][j+1][0];
						countred++;
					}
				}
				if(j>0){
					green += Imagedata[i][j-1][0];
					countgreen++;
				}
				if(j<Size-1){
					green += Imagedata[i][j+1][0];
					countgreen++;
				}
				if (countgreen != 0) {
					RGBImagedata[i][j][1] = green/countgreen;
					}
				if (countred != 0) {
					RGBImagedata[i][j][0] = red/countred;
					}
            	// Interpolate missing green and red values for a blue pixel
            	// Be careful at the image boundaries
        	} else {
				RGBImagedata[i][j][1] = Imagedata[i][j][0];
				float red = 0.0;
				float blue = 0.0;
				int countred = 0;
				int countblue = 0;
				if(i>0){
					red+= Imagedata[i-1][j][0];
					countred++;
				}
				if(i<Size-1){
					red += Imagedata[i+1][j][0];
					countred++;
				}
				if(j>0){
					blue += Imagedata[i][j-1][0];
					countblue++;
				}
				if(j<Size-1){
					blue += Imagedata[i][j+1][0];
					countblue++;
				}
				if (countblue != 0) {
					RGBImagedata[i][j][2] = blue/countblue;
					}
				if (countred != 0) {
					RGBImagedata[i][j][0] = red/countred;
				}
            	// Interpolate missing red and blue values for a green pixel
            	// Be careful at the image boundaries
            	// Remember to differentiate between green pixels in red rows and blue rows
        	}
    	}
	}

	// Write image data (filename specified by second argument) from image data matrix

	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	for (int i = 0; i < Size; ++i) {
    	for (int j = 0; j < Size; ++j) {
        	fwrite(RGBImagedata[i][j], sizeof(unsigned char), 3, file);
    	}
	}

	fclose(file);
	for (int i = 0; i < Size; ++i) {
    	for (int j = 0; j < Size; ++j) {
        	delete[] RGBImagedata[i][j];
    	}
    	delete[] RGBImagedata[i];
	}
	delete[] RGBImagedata;

	return 0;
}
