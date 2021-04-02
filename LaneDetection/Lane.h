//
// Created by Administrator on 2021/3/30.
//

#ifndef LANEMARK3D_LANE_H
#define LANEMARK3D_LANE_H


#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <opencv2/opencv.hpp>

class Lane {
public:
    /**
     *  @brief Default constructor for Lane
     *        with ployorder,colour,polyCoeff,startCoordinate,status
     *        random values
     */
    Lane();

    /**
     *  @brief Default constructor for Lane
     *          with ployorder,colour,polyCoeff,startCoordinate,status
     *          random values
     *
     *  @param polyOrder is order of fitting polynomial
     *  @param color is the color of lane
     *  @param averagingCount number of values to average
     */
    Lane(int polyOrder, std::string color, int averagingCount);

    /**
     *  @brief Default destructor for Lane class
     */
    ~Lane();

    /*
     *  @brief Calculates the center x-coordinate averageof the first sliding
     *        window
     *
     *  @param coordinate is current x-center co-ordinate
     *
     *  @return averaged x-coordinate
     */
    int getStableCenter(int coordinate);

    /*
     *  @brief Sets the center coordinate of the first sliding window
     *
     *  @param point is current center co-ordinate
     *
     */
    void setStartCoordinate(cv::Point point);

    /*
     *  @brief Gets the center coordinate of the first sliding window
     *
     *  @return Center coordinate of the first sliding window(lane)
     */
    cv::Point getStartCoordinate();

    /*
     *  @brief Sets the status of lane
     *
     *  @param flag is the status of the lane if polynomial found
     */
    void setStatus(bool flag);

    /*
     *  @brief Gets the status of lane
     *
     *  @return status of the lane
     */
    bool getStatus();

    /*
     *  @brief Sets the polyOrder of lane
     *
     *  @param value is the polyorder of lane
     */
    void setPolyOrder(int value);

    /*
     *  @brief Gets the polyOrder of lane
     *
     *  @return value is the polyorder of lane
     */
    int getPolyOrder();

    /*
     *  @brief Sets the polynomial coeff of lane
     *
     *  @param coeff is a Mat(1x3) object containing coefficients
     */
    void setPolyCoeff(cv::Mat coeff);

    /*
     *  @brief Gets the polynomial coeff of lane
     *
     *  @return vector containing coefficients
     */
    std::vector<float> getPolyCoeff();

private:
    int polyOrder;  // declare integer for order of line.
    std::string colour;  // set RGB values for lane colour.
    std::vector<float> polyCoeff;  // Coefficients for equation
    cv::Point startCoordinates;  // Reference coordinates for line.

    // Average center to prevent jumps for entire run
    std::vector<int> averagingCenter;
    int averagingCount;
    int currentAveragingIndex;
    bool status;  // For status for program.
};


#endif //LANEMARK3D_LANE_H
