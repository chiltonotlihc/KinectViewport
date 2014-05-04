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
    averageHu = new double[8];
    for(int i = 0; i<8; i++){
        averageHu[i] = 0.0;
        
    }
    averageCount = 0;
    
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
    
    cv::Size s = tempCap->getSourceSize();
    
    lMask = cv::Mat::zeros(s, CV_8U);
    lMask(cv::Rect(0, 0, s.width/2, s.height)) = 255;

    
}


void HandTracker::update(){
    
    cv::Mat input = tempCap->getDepthData();

    cv::Mat outputImage = tempCap->getRgbData();
    cv::Mat validMask = tempCap->getMaskData();

    cv::Size s = tempCap->getSourceSize();
    
    cv::bitwise_not(validMask, validMask);
    cv::bitwise_or(input, 127, input, validMask);
    
    //set threshold value from current face distance
    mThreshold = -tempCap->getNormPositionZ()*0.5;
    
    cv::blur(input, input, cv::Size(7, 7));
    
    
    //std::cout << "Threshold: " << mThreshold << std::endl;
    
    cv::Mat thresholded, thresholdedHands, thresholdedHandsLeft, thresholdedHandsRight;

    
    //threshold the blurred image into two seperate matricies
    cv::threshold( input, thresholded, mThreshold, 255, CV_THRESH_BINARY_INV);
    cv::threshold( input, thresholdedHands, mThreshold*0.95, 255, CV_THRESH_TRUNC);
    
    //split thresholded hands into to images
    cv::Mat rMask;
    cv::bitwise_not(lMask, rMask);
    
    thresholdedHandsLeft = cv::Mat(thresholdedHands, cv::Rect(0, 0, s.width/2, s.height)).clone();
    thresholdedHandsRight  = cv::Mat(thresholdedHands, cv::Rect(s.width/2, 0, s.width/2, s.height)).clone();
    
    
    cv::normalize(thresholdedHandsLeft, thresholdedHandsLeft, 0, 255, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(thresholdedHandsRight, thresholdedHandsRight, 0, 255, cv::NORM_MINMAX, -1, cv::Mat() );
    
    cv::threshold(thresholdedHandsLeft, thresholdedHandsLeft, 180, 255, CV_THRESH_BINARY_INV);
    cv::threshold(thresholdedHandsRight, thresholdedHandsRight, 180, 255, CV_THRESH_BINARY_INV);
    

    imshow("InputDepth", input);

    imshow("ThresholdedHandsLeft", thresholdedHandsLeft);
    imshow("ThresholdedHandsRight", thresholdedHandsRight);
    //imshow("ThresholdedHands", thresholdedHands);
    
    

    
    
    findContours(thresholdedHandsLeft, &leftContours, cv::Point(0, 0));
    findContours(thresholdedHandsRight, &rightContours, cv::Point(s.width/2, 0));
    

    //runningAverageHu(averageHu, &averageCount);
    
    
    showContours(tempCap->getRgbData(), &leftContours);
    showContours(tempCap->getRgbData(), &rightContours);
    
    
    
    
}

void HandTracker::findContours(cv::Mat input, std::vector<ContourDef>* contourVector, cv::Point offset){
    
    //2D vector of points to store contours
    std::vector<std::vector<cv::Point> > tempContours;
    
    //iterators for filling the contour vectors
    std::vector<std::vector<cv::Point> >::iterator newContour;

    
    cv::findContours(input, tempContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, offset);
    
    newContour = tempContours.begin();
    contourVector->clear();
    
    while(newContour != tempContours.end()){
        
        //add another vector element
        //std::cout << cv::contourArea(*newContour) << std::endl;
        if(cv::contourArea(*newContour) > 8000){
            contourVector->push_back(ContourDef(*newContour));
            //std::cout << "Added Contour: " << cv::contourArea(*newContour) << std::endl;
        }
        
        newContour++;
    }


}

void HandTracker::runningAverageHu(double* average, int* count){
    
    for(std::vector<ContourDef>::iterator c = leftContours.begin(); c<leftContours.end(); c++){
        
        double* moments = c->getHuMoments();
        
        
        for(int i = 0; i<8; i++){
            average[i] *= *count;
            average[i] += moments[i];
            average[i] /= *count+1;
            
            std::cout << average[i] << ", ";
        }
        std::cout << std::endl;
        

    }
    *count++;
    
    
}


void HandTracker::showContours(cv::Mat outputImage, std::vector<ContourDef> *which){
    
    for(std::vector<ContourDef>::iterator c = which->begin(); c<which->end(); c++){
        c->showAll(outputImage);
        //c->showContourFill(outputImage);
        //c->showHullLines(outputImage);
        //c->showContourLines(outputImage);
    }

    
    
}















