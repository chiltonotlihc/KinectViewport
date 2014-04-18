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
#include <OpenGL/OpenGL.h>

#include "templateMatching.h"


class HandTracker{

private:
    float mThreshold, mBlobMin, mBlobMax;
    float mKinectTilt;
    
    cv::Mat mDepthTexture, mDepthSurface;
    
    cv::Vec3f mTargetPosition;
    
public:
    HandTracker();
    ~HandTracker();
    
    void update(TemplateCapture* tempCap);
    
    
    
};



#endif /* defined(__OpenCVTest__HandTracker__) */
