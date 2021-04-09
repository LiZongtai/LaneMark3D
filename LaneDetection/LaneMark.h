//
// Created by Administrator on 2021/3/31.
//

#ifndef LANEMARK3D_LANEMARK_H
#define LANEMARK3D_LANEMARK_H

#include <vector>
#include <opencv2/opencv.hpp>

class LaneMark {
public:
    LaneMark(int laneNum);
    bool update(std::vector<cv::Point2f> imgPoints);
    void setCamMatrix(cv::Mat);
    void setDistCoeff(cv::Mat);
    void setProjectMatrix();
    cv::Mat getViewMatrix();
    cv::Mat getProjectMatrix();

private:
    const unsigned int SCR_WIDTH = 1280;
    const unsigned int SCR_HEIGHT = 720;
    cv::Vec3d rvec, tvec;
    cv::Mat camMatrix, distCoeffs;
    cv::Mat viewMatrix = cv::Mat::zeros(4, 4, CV_32F);
    cv::Mat projMatrix= cv::Mat::zeros(4, 4, CV_32F);
    std::vector<cv::Point3f> worldPoints = {};
    std::vector<cv::Point2f> imgPoints = {};
    void culViewMatrix();
    void culProjectionMatrix();
};


#endif //LANEMARK3D_LANEMARK_H
