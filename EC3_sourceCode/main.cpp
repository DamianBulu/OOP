#include <iostream>
#include "Image.h"
#include "BrightnessContrastAdjustment.h"
#include "GammaCorrection.h"
#include "ImageConvolution.h"
#include "Drawing.h"
#include <filesystem>

// Scaling function for convolution
float scaleForBlur(float val) { return val; }
float scaleForSobel(float val) { return (val + 4 * 255) / (8 * 255) * 255; }

int main() {
    // Create images directory if it doesn't exist
    std::filesystem::create_directory("images");

    try {
        // Load the image
        Image img;
        if (!img.load("balloons.ascii.pgm")) {
            std::cerr << "Failed to load image!" << std::endl;
            return 1;
        }

        // Remove the gradient filling - we want to keep the original image data
        // (Your original code was overwriting the loaded image with a gradient)

        // Save original
        if (!img.save("images/original1.ascii.pgm")) {
            std::cerr << "Failed to save original image!" << std::endl;
        }

        // 1. Adjust brightness and contrast
        BrightnessContrastAdjustment brightnessAdj(1.5, 30);
        Image resultBrightness;
        brightnessAdj.process(img, resultBrightness);
        if (!resultBrightness.save("images/brightness_adjusted1.ascii.pgm")) {
            std::cerr << "Failed to save brightness adjusted image!" << std::endl;
        }

        // 2. Apply gamma correction
        GammaCorrection gammaCorr(0.5);
        Image resultGamma;
        gammaCorr.process(img, resultGamma);
        if (!resultGamma.save("images/gamma_corrected1.ascii.pgm")) {
            std::cerr << "Failed to save gamma corrected image!" << std::endl;
        }

        // 3. Apply convolution (blur)
        float blurKernel[3][3] = {
            {1 / 9.0f, 1 / 9.0f, 1 / 9.0f},
            {1 / 9.0f, 1 / 9.0f, 1 / 9.0f},
            {1 / 9.0f, 1 / 9.0f, 1 / 9.0f}
        };

        float** kernel = new float* [3];
        for (int i = 0; i < 3; i++) {
            kernel[i] = new float[3];
            for (int j = 0; j < 3; j++) {
                kernel[i][j] = blurKernel[i][j];
            }
        }

        ImageConvolution blur(kernel, 3, 3, scaleForBlur);
        Image resultBlur;
        blur.process(img, resultBlur);
        if (!resultBlur.save("images/blurred1.ascii.pgm")) {
            std::cerr << "Failed to save blurred image!" << std::endl;
        }

        // Clean up kernel memory
        for (int i = 0; i < 3; ++i) {
            delete[] kernel[i];
        }
        delete[] kernel;

        // 4. Draw shapes on a COPY of the original image
        Image imgWithShapes = img; // Make a copy
        Drawing::drawCircle(imgWithShapes, Point(400, 300), 100, 255);
        Drawing::drawRectangle(imgWithShapes, Point(100, 100), Point(300, 200), 200);
        if (!imgWithShapes.save("images/with_shapes1.ascii.pgm")) {
            std::cerr << "Failed to save image with shapes!" << std::endl;
        }

        // 5. Extract ROI
        Image roi;
        if (img.getROI(roi, 200, 150, 400, 300)) {
            if (!roi.save("images/roi1.ascii.pgm")) {
                std::cerr << "Failed to save ROI image!" << std::endl;
            }
        }
        else {
            std::cerr << "Failed to extract ROI!" << std::endl;
        }

        std::cout << "Image processing completed successfully!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}