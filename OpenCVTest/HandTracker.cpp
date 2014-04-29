//
//  HandTracker.cpp
//  OpenCVTest
//
//  Created by Oliver Chilton on 18/04/2014.
//  Copyright (c) 2014 Oliver Chilton. All rights reserved.
//

#include "HandTracker.h"


HandTracker::HandTracker(){
    std::cout << "Constructing Hand Tracking Object" << std::endl;
    
    tempCap = NULL;

    
    
}

HandTracker::HandTracker(TemplateCapture* ipCap){
    std::cout << "Constructing Hand Tracking Object" << std::endl;
    
    tempCap = ipCap;
    mThreshold = 70.0f;
    
    
}

HandTracker::~HandTracker(){
    std::cout << "Destructing Hand Tracking Object" << std::endl;
    
}

void HandTracker::setCaptureObject(TemplateCapture* ipCap){
    
    tempCap = ipCap;
    
}


void HandTracker::update(){
    
    
    cv::Mat input = tempCap->getDepthData();
    
    cv::Mat outputImage = tempCap->getRgbData();
    cv::Mat validMask = tempCap->getMaskData();
    cv::bitwise_not(validMask, validMask);
    cv::bitwise_or(input, 127, input, validMask);
    
    //set threshold value from current face distance
    mThreshold = -tempCap->getNormPositionZ();
    
    
    std::cout << "Threshold: " << mThreshold << std::endl;
    
    
    cv::Mat blurred, thresholded, thresholdedHands;
    
    //blur the input depth image
    cv::blur(input, blurred, cv::Size(10, 10));
    
    //threshold the blurred image into two seperate matricies
    cv::threshold( blurred, thresholded, mThreshold, 255, CV_THRESH_BINARY_INV);
    cv::threshold( blurred, thresholdedHands, mThreshold-50, 255, CV_THRESH_BINARY_INV);
    
    imshow("Blurred", blurred);
    imshow("Thresholded", thresholded);
    imshow("ThresholdedHands", thresholdedHands);
    
    
    findContours(thresholdedHands);
    
    showContours(tempCap->getRgbData());

    
    
    
}

void HandTracker::findContours(cv::Mat input){
    
    //2D vector of points to store contours
    std::vector<std::vector<cv::Point> > tempContours;
    
    //iterators for filling the contour vectors
    std::vector<std::vector<cv::Point> >::iterator newContour;

    
    cv::findContours(input, tempContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    
    newContour = tempContours.begin();
    contours.clear();
    
    while(newContour != tempContours.end()){
        
        //add another vector element
        std::cout << cv::contourArea(*newContour) << std::endl;
        if(cv::contourArea(*newContour) > 1000){
            contours.push_back(ContourDef(*newContour));
        }
        
        newContour++;
    }


}

void HandTracker::showContours(cv::Mat outputImage){
    
    for(std::vector<ContourDef>::iterator c = contours.begin(); c<contours.end(); c++){
        c->showContourLines(outputImage);
    }
    
}















