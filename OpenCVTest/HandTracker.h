//
//  HandTracker.h
//  OpenCVTest
//
//  Created by Oliver Chilton on 18/04/2014.
//  Copyright (c) 2014 Oliver Chilton. All rights reserved.
//

#ifndef __OpenCVTest__HandTracker__
#define __OpenCVTest__HandTracker__

#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/video/video.hpp>

#include "templateMatching.h"
#include "ContourDef.h"
#include "HandDef.h"

#define HT_LEFT 0
#define HT_RIGHT 1
#define HT_BOTH -1


class HandTracker{

private:
    float mThreshold;
    
    TemplateCapture* tempCap;
    
    std::vector<ContourDef> leftContours;
    std::vector<ContourDef> rightContours;
    
    HandDef leftHand;
    HandDef rightHand;
    
    
    
    double* averageHu;
    int averageCount;
    
    cv::Mat lMask;
    
    
    void processImage();
    void findContours(cv::Mat input, std::vector<ContourDef>* contourVector, cv::Point offset);
    void findHands();

    void runningAverageHu(double* average, int* count);
    
    
public:
    HandTracker();
    HandTracker(TemplateCapture *ipCap);
    ~HandTracker();
    
    void setCaptureObject(TemplateCapture *ipCap);
    
    void update();
    
    void showContours(cv::Mat outputImage, std::vector<ContourDef>* contourVector);
    
    
    
};



#endif /* defined(__OpenCVTest__HandTracker__) */
