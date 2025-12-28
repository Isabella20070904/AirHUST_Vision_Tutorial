#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>

// 解析注释行获取图像信息
bool parseImageInfo(const std::string& line, int& width, int& height, int& channels, int& type) {
    std::regex pattern(R"(宽度=(\d+),\s*高度=(\d+),\s*通道数=(\d+),\s*类型=CV_8UC(\d))");
    std::smatch matches;
    
    if (std::regex_search(line, matches, pattern)) {
        if (matches.size() == 5) {
            width = std::stoi(matches[1].str());
            height = std::stoi(matches[2].str());
            channels = std::stoi(matches[3].str());
            int c = std::stoi(matches[4].str());
            
            if (c == 1) {
                type = CV_8UC1;
            } else if (c == 3) {
                type = CV_8UC3;
            } else {
                std::cerr << "不支持的通道数: " << c << std::endl;
                return false;
            }
            return true;
        }
    }
    return false;
}

// 从文本文件恢复图像
cv::Mat restoreImage(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return cv::Mat();
    }
    
    // 读取第一行（注释）
    std::string info_line;
    std::getline(file, info_line);
    
    int width = 0, height = 0, channels = 0, type = 0;
    if (!parseImageInfo(info_line, width, height, channels, type)) {
        std::cerr << "无法解析图像信息: " << info_line << std::endl;
        return cv::Mat();
    }
    
    std::cout << "恢复图像: " << filename << std::endl;
    std::cout << "  尺寸: " << width << "x" << height << std::endl;
    std::cout << "  通道: " << channels << std::endl;
    std::cout << "  类型: CV_8UC" << (channels == 1 ? "1" : "3") << std::endl;
    
    // 创建图像矩阵
    cv::Mat image(height, width, type);
    
    std::string line;
    int row = 0;
    
    while (row < height && std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        int col = 0;
        
        if (channels == 1) {
    // 单通道图像（灰度或二值）
    while (col < width && std::getline(ss, token, ',')) {
        int value = std::stoi(token);
        // 判断是否为二值图像（0/1），如果是则转换为0/255
        if (value == 0 || value == 1) {
            image.at<uchar>(row, col) = value > 0 ? 255 : 0;
        } else {
            image.at<uchar>(row, col) = static_cast<uchar>(value);
        }
        col++;
    }
} else if (channels == 3) {
            // 三通道图像（彩色）
            while (col < width && std::getline(ss, token, ',')) {
                // 读取BGR三个值
                int b = std::stoi(token);
                
                if (!std::getline(ss, token, ',')) break;
                int g = std::stoi(token);
                
                if (!std::getline(ss, token, ',')) break;
                int r = std::stoi(token);
                
                cv::Vec3b& pixel = image.at<cv::Vec3b>(row, col);
                pixel[0] = static_cast<uchar>(b);  // B
                pixel[1] = static_cast<uchar>(g);  // G
                pixel[2] = static_cast<uchar>(r);  // R
                
                col++;
            }
        }
        row++;
    }
    
    file.close();
    
    if (row != height) {
        std::cerr << "警告: 读取的行数(" << row << ")与高度(" << height << ")不匹配" << std::endl;
    }
    
    return image;
}

int main() {
    std::cout << "=== 从像素文本文件恢复图像 ===" << std::endl;
    
    // 恢复三个图像
    std::vector<std::string> files = {
        "data/pic1.txt",
        "data/pic2.txt", 
        "data/pic3.txt"
    };
    
    std::vector<cv::Mat> images;
    std::vector<std::string> names = {"彩色图像", "二值图像", "灰度图像"};
    
    for (size_t i = 0; i < files.size(); i++) {
        cv::Mat img = restoreImage(files[i]);
        if (!img.empty()) {
            images.push_back(img);
            
            // 保存恢复的图像
            std::string output_name = "restored_pic" + std::to_string(i+1) + ".png";
            cv::imwrite(output_name, img);
            std::cout << "  已保存为: " << output_name << std::endl;
            
            // 显示图像
            cv::imshow(names[i], img);
        } else {
            std::cerr << "恢复图像失败: " << files[i] << std::endl;
        }
    }
    
    if (!images.empty()) {
        std::cout << "\n所有图像已恢复并显示。按任意键继续..." << std::endl;
        cv::waitKey(0);
    } else {
        std::cerr << "没有成功恢复任何图像。" << std::endl;
    }
    
    return 0;
}
