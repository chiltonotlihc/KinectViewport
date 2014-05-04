//
//  HandDef.h
//  OpenCVTest
//
//  Created by Oliver Chilton on 04/05/2014.
//  Copyright (c) 2014 Oliver Chilton. All rights reserved.
//

#ifndef __OpenCVTest__HandDef__
#define __OpenCVTest__HandDef__

#include <iostream>
#include <vector>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/video/video.hpp>

class HandDef{
    
    cv::Point3f pos;
    bool open;
    
    cv::KalmanFilter KM;
    cv::Mat_<float> measurement;
    
public:
    HandDef();
    
    void updatePosition(cv::Point measurement);
    inline cv::Point3f getPosition(){return pos;};
    

    
    
    
};

#endif /* defined(__OpenCVTest__HandDef__) */
