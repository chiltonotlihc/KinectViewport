//
//  oc_utilities.cpp
//  OpenCVTest
//
//  Created by Oliver Chilton on 08/04/2014.
//  Copyright (c) 2014 Oliver Chilton. All rights reserved.
//

#include "oc_utilities.h"


SaveVideo::SaveVideo(std::string fileName){
    frameCount = 0;
    outputName = fileName;
    capture = cv::VideoCapture(CV_CAP_OPENNI);
    
    cv::Size s = cv::Size((int) capture.get(CV_CAP_PROP_FRAME_WIDTH),
                      (int) capture.get(CV_CAP_PROP_FRAME_HEIGHT));
    
    //int ex = static_cast<int>(capture.get(CV_CAP_PROP_FOURCC));
    writerRGB.open(outputName+"rgb.avi", -1, capture.get(CV_CAP_PROP_FPS), s, true);
    writerRawDepth.open(outputName+"rawDepth.avi", -1, capture.get(CV_CAP_PROP_FPS), s, true);
    writerValidMask.open(outputName+"validMask.avi", -1, capture.get(CV_CAP_PROP_FPS), s, true);
    
    if (!writerRGB.isOpened())
    {
        std::cout  << "Could not open the output RGB video for write"<< std::endl;
        throw -1;
    }
    
    if (!writerRawDepth.isOpened())
    {
        std::cout  << "Could not open the output RawDepth video for write"<< std::endl;
        throw -1;
    }
    
    if (!writerValidMask.isOpened())
    {
        std::cout  << "Could not open the output ValidMask video for write"<< std::endl;
        throw -1;
    }
    
    
    storage.open(outputName+"rgb.xml", cv::FileStorage::WRITE);
    
    
    
}

SaveVideo::~SaveVideo(){
    std::cout << "Finishing saving." << std::endl;
    
}

void SaveVideo::run(){
    std::cout << "Saving frame: " << frameCount << std::endl;
    
    capture.grab();
    
    capture.retrieve(rgb, CV_CAP_OPENNI_BGR_IMAGE);
    capture.retrieve(rawDepth, CV_CAP_OPENNI_DEPTH_MAP);
    capture.retrieve(validMask, CV_CAP_OPENNI_VALID_DEPTH_MASK);
    
    storage << '2';
    //writerRawDepth << rawDepth;
    //writerValidMask << validMask;
    
    imshow("output", rgb);
    frameCount++;
    
}



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
