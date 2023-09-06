#include <iostream>
#include <FreeImage.h>

int main(int argc, char* argv[]) {
    // 初始化FreeImage库
    FreeImage_Initialise();

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " input.ppm output.bmp" << std::endl;
        return 1;
    }

    const char* inputPpmFilename = argv[1];
    const char* outputBmpFilename = argv[2];

    // 尝试打开P3格式的.ppm文件
    FIBITMAP* ppmImage = FreeImage_Load(FIF_PPM, inputPpmFilename, 0);

    if (ppmImage) {
        // 保存为BMP格式
        FreeImage_Save(FIF_BMP, ppmImage, outputBmpFilename, 0);
        
        // 释放图像内存
        FreeImage_Unload(ppmImage);

        std::cout << "Conversion successful. BMP file saved as " << outputBmpFilename << std::endl;
    } else {
        std::cerr << "Error: Unable to open or convert the input PPM file." << std::endl;
    }

    // 清理FreeImage库资源
    FreeImage_DeInitialise();

    return 0;
}
