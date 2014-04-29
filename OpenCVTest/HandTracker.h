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


class HandTracker{

private:
    float mThreshold;
    
    TemplateCapture* tempCap;
    
    std::vector<ContourDef> contours;
    
    
    
    
    void processImage();
    void findContours(cv::Mat input);

    
    
public:
    HandTracker();
    HandTracker(TemplateCapture *ipCap);
    ~HandTracker();
    
    void setCaptureObject(TemplateCapture *ipCap);
    
    void update();
    
    void showContours(cv::Mat outputImage);
    
    
    
};



#endif /* defined(__OpenCVTest__HandTracker__) */
