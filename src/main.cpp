#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // 读取图像
    cv::Mat image = cv::imread("data/ranjinle.jpg");
    
    if (image.empty()) {
        std::cout << "无法读取图像文件！" << std::endl;
        return -1;
    }
    
    std::cout << "图像尺寸: " << image.cols << "x" << image.rows << std::endl;
    std::cout << "通道数: " << image.channels() << std::endl;
    
    // 显示图像
    cv::imshow("读取的图像", image);
    std::cout << "按任意键继续..." << std::endl;
    
    // 保存图像
    bool saved = cv::imwrite("output_image.jpg", image);
    if (saved) {
        std::cout << "图像已保存为 output_image.jpg" << std::endl;
    }
    
    cv::waitKey(0);
    return 0;
}
