//
//  oc_utilities.cpp
//  OpenCVTest
//
//  Created by Oliver Chilton on 08/04/2014.
//  Copyright (c) 2014 Oliver Chilton. All rights reserved.
//

#include "oc_utilities.h"


ImageSnap::ImageSnap(std::string ipPath){
    
    cap = cv::VideoCapture( CV_CAP_OPENNI );
    path = ipPath;
    
    float cols = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    float rows = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    
    sbs = new cv::Mat(rows, cols*2, CV_8UC3);
}

ImageSnap::~ImageSnap(){
    std::cout << "Destructing ImageSnap Object." << std::endl;
    
}

void ImageSnap::run(){
    
    
    
    if(cap.isOpened()){
        
        
        cap.grab();
        
        cap.retrieve(rgb, CV_CAP_OPENNI_BGR_IMAGE);
        cap.retrieve(ir, CV_CAP_OPENNI_VALID_DEPTH_MASK);

        
        
        //cv::hconcat(rgb, ir, sbs);
        cv::Mat left(*sbs, cv::Rect(0, 0, rgb.cols, rgb.rows));
        rgb.copyTo(left);
        cv::Mat right(*sbs, cv::Rect(rgb.cols, 0, rgb.cols, rgb.rows));
        ir.copyTo(right);
    }
    
    
}

void ImageSnap::show(){
    
    cv::imshow("Output", *sbs);
    cv::imshow("ir", ir);
}