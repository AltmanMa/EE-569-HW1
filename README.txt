# EE569 Homework Assignment #1
## Date: January 28, 2024
## Name: Altman Ma
## ID: 9993-1136-72
## Email: gujiama@usc.edu 

### Compiled on MACOS with g++
**Define compiler and linker**

- For .cpp files contained in folder "hw1p1a":
    - Compile `hw1p1a.cpp` with the following command:
        ```bash
        g++ hw1p1a.cpp -o [name of program]
        ```
    - Run the program:
        ```bash
        ./[name of program] ../images/House.raw HouseRGB.raw
        ```

- For all other .cpp files in all folders:
    - Compile with:
        ```bash
        g++ -std=c++11 [name of file].cpp -o [name of program] `pkg-config --cflags --libs opencv4`
        ```

- For programs compiled from .cpp files in folders "hw1p1b", "hw1p1c", "hw1p2a" (except "PSNR.cpp" in "hw1p2a"):
    - Run with:
        ```bash
        ./[name of program] [name of original image].raw [name of output image].raw
        ```

- For programs compiled from "PSNR.cpp" in "hw1p2a" and all other .cpp files in "hw1p2b", "hw1p2c":
    - Run with:
        ```bash
        ./[name of program] [name of original image].raw [name of output image].raw width height
        ```

Please replace `[name of file]` and `[name of program]`with actual names. 
Also  `[name of original image]`、`[name of output image]`、`width` and `height` should be replaced by real file name and size。

Especally, file "PSNR.cpp" exists in all folders in problem 2. It is used for calculate the PSNR value between original image and the processed image, and print the value out.