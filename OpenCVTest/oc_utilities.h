//
//  oc_utilities.h
//  OpenCVTest
//
//  Created by Oliver Chilton on 08/04/2014.
//  Copyright (c) 2014 Oliver Chilton. All rights reserved.
//

#ifndef __OpenCVTest__oc_utilities__
#define __OpenCVTest__oc_utilities__

#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"



class ImageSnap{
private:
    cv::VideoCapture cap;
    std::string path;
    
    cv::Mat rgb;
    cv::Mat ir, irshow;
    cv::Mat* sbs;
    
public:
    ImageSnap(std::string iPath);
    ~ImageSnap();
    
    //void snap();
    void run();
    void show();
    
    
};


#endif /* defined(__OpenCVTest__oc_utilities__) */
