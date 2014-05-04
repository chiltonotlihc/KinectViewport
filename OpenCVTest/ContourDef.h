//
//  ContourDef.h
//  OpenCVTest
//
//  Created by Oliver Chilton on 18/04/2014.
//  Copyright (c) 2014 Oliver Chilton. All rights reserved.
//

#ifndef __OpenCVTest__ContourDef__
#define __OpenCVTest__ContourDef__

#include <iostream>
#include <vector>
#include <exception>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/video/video.hpp>

class ContourDef{
private:
    std::vector<cv::Point> contourPoints;
    std::vector<cv::Point> hullPoints;
    std::vector<cv::Point> defectPoints;
    
    std::vector<cv::Vec4i> defectStructs;
    
    cv::Moments contourMoments;
    double *huMoments;
    
    int* longestHullsIdx;
    double* longestHullsLength;
    
    void analyse();
    void segmentHand();
    
    cv::Point handCenter;
    
    
public:
    ContourDef();
    ContourDef(std::vector<cv::Point> initPoints);
    ~ContourDef();
    
    void update(std::vector<cv::Point> newPoints);
    
    double huDistance(double* moments);

    
    void showHullPoints(cv::Mat output);
    void showHullLines(cv::Mat output);
    void showDefectPoints(cv::Mat output);
    void showContourLines(cv::Mat output);
    void showContourFill(cv::Mat output);
    
    void showAll(cv::Mat output);
    
    std::vector<cv::Point> getHullPoints();
    std::vector<cv::Point> getDefectPoints();
    
    inline double* getHuMoments(){return huMoments;};
    
    
    bool isOpenHand;
    
};


#endif /* defined(__OpenCVTest__ContourDef__) */
