//
// Created by Administrator on 2021/3/31.
//

#include "LaneMark.h"

LaneMark::LaneMark(int laneNum) {
    float d = 3.0f * 8.0f / (float) laneNum;
    worldPoints = {};
    worldPoints.emplace_back(cv::Point3f(-0.6f, 1.5f, -1.0f));
    worldPoints.emplace_back(cv::Point3f(0.6f, 1.5f, -1.0f));
    worldPoints.emplace_back(cv::Point3f(0.6f, 1.5f, -1.0f - d));
    worldPoints.emplace_back(cv::Point3f(-0.6f, 1.5f, -1.0f - d));
}

//std::vector<cv::Point2f> LaneMark::getPoints2f(std::vector<cv::Point3f> input_pt3d) {
//    std::vector<cv::Point2f> result_pt2d = {};
//    cv::projectPoints(input_pt3d, Rvec, Tvec, camMatrix, distCoeff, result_pt2d);
//    return result_pt2d;
//}


bool LaneMark::update(std::vector<cv::Point2f> p) {
    if (p.size() != 4) {
        return false;
    } else {
        imgPoints = p;
    }
    try {
        cv::solvePnP(worldPoints, imgPoints, camMatrix, distCoeffs, rvec, tvec, false, cv::SOLVEPNP_P3P);
//        raux.convertTo(Rvec, CV_32F);   // 旋转向量
//        taux.convertTo(Tvec, CV_32F);   // 平移向量
        culViewMatrix();
        //culProjectionMatrix();
    } catch (int e) {
        return false;
    }
    return true;
}

void LaneMark::culViewMatrix() {
    cv::Mat viewMatrixf = cv::Mat::zeros(4, 4, CV_32F);
    cv::Mat rot;
    Rodrigues(rvec, rot);
    for (unsigned int row = 0; row < 3; ++row) {
        for (unsigned int col = 0; col < 3; ++col) {
            viewMatrixf.at<float>(row, col) = (float) rot.at<double>(row, col);
        }
        viewMatrixf.at<float>(row, 3) = (float) tvec[row];
    }
    viewMatrixf.at<float>(3, 3) = 1.0f;

    cv::Mat cvToGl = cv::Mat::zeros(4, 4, CV_32F);
    cvToGl.at<float>(0, 0) = 1.0f;
    cvToGl.at<float>(1, 1) = -1.0f; // Invert the y axis
    cvToGl.at<float>(2, 2) = -1.0f; // invert the z axis
    cvToGl.at<float>(3, 3) = 1.0f;
    viewMatrixf = cvToGl * viewMatrixf;
    cv::transpose(viewMatrixf, viewMatrixf);

    viewMatrix = viewMatrixf;
}

cv::Mat LaneMark::getViewMatrix() {
    return viewMatrix;
}

cv::Mat LaneMark::getProjectMatrix() {
    return projMatrix;
}

void LaneMark::setCamMatrix(cv::Mat matrix) {
    camMatrix = matrix;
}

void LaneMark::setDistCoeff(cv::Mat matrix) {
    distCoeffs = matrix;
}

void LaneMark::culProjectionMatrix() {
    float nearp = 0.01f;
    float farp = 1000.0f;

    float f_x = camMatrix.at<double>(0, 0);
    float f_y = camMatrix.at<double>(1, 1);

    float c_x = camMatrix.at<double>(0, 2);
    float c_y = camMatrix.at<double>(1, 2);

    projMatrix.at<float>(0, 0) = 2 * f_x / (float) SCR_WIDTH;
    projMatrix.at<float>(1, 1) = 2 * f_y / (float) SCR_HEIGHT;

    projMatrix.at<float>(2, 0) = 1.0f - 2 * c_x / (float) SCR_WIDTH;
    projMatrix.at<float>(2, 1) = 2 * c_y / (float) SCR_HEIGHT - 1.0f;
    projMatrix.at<float>(2, 2) = -(farp + nearp) / (farp - nearp);
    projMatrix.at<float>(2, 3) = -1.0f;

    projMatrix.at<float>(3, 2) = -2.0f * farp * nearp / (farp - nearp);
}

void LaneMark::setProjectMatrix() {
    culProjectionMatrix();
}


