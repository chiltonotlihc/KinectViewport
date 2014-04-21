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
    mThreshold = 50.0f;

    
    
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


void HandTracker::newUpdate(){
    
    
    cv::Mat input = tempCap->getDepthData();
    
    cv::Mat outputImage = tempCap->getRgbData();
    
    //set threshold value from current face distance
    //mThreshold = tempCap->getNormPositionZ()*0.1;
    
    //mThreshold += 20;
    
    std::cout << "Threshold: " << mThreshold << std::endl;
    
    
    cv::Mat blurred, thresholded, thresholdedHands;
    
    //blur the input depth image
    cv::blur(input, blurred, cv::Size(10, 10));
    
    //threshold the blurred image into two seperate matricies
    cv::threshold( blurred, thresholded, mThreshold, 255, CV_THRESH_BINARY_INV);
    cv::threshold( blurred, thresholdedHands, mThreshold-20, 255, CV_THRESH_BINARY_INV);
    
    imshow("Blurred", blurred);
    imshow("Thresholded", thresholded);
    imshow("ThresholdedHands", thresholdedHands);
    
    
    findContours(thresholded);
    
    
    showContours();

    
    
    
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
        contours.push_back(ContourDef(*newContour));
        
        newContour++;
    }


}

void HandTracker::showContours(){
    
    for(std::vector<ContourDef>::iterator c = contours.begin(); c<contours.end(); c++){
        c->showContourLines(tempCap->getRgbData());
    }
    
}


void HandTracker::update(){
    
    
    cv::Mat input = tempCap->getDepthData();
    
    cv::Mat outputImage;// = tempCap->getRgbData();
    
    
    //set threshold value from current face distance
    //mThreshold = tempCap->getNormPositionZ()*0.1;

    //mThreshold += 20;
    
    std::cout << "Threshold: " << mThreshold << std::endl;
    
    
    cv::Mat blurred, thresholded, thresholdedHands;
    
    //blur the input depth image
    cv::blur(input, blurred, cv::Size(10, 10));
    
    //threshold the blurred image into two seperate matricies
    cv::threshold( blurred, thresholded, mThreshold, 255, CV_THRESH_BINARY_INV);
    cv::threshold( blurred, outputImage, mThreshold, 255, CV_THRESH_BINARY_INV);
    cv::threshold( blurred, thresholdedHands, mThreshold-20, 255, CV_THRESH_BINARY_INV);
    
    cv::cvtColor(outputImage, outputImage, CV_GRAY2RGB);
    
    imshow("Blurred", blurred);
    //imshow("Thresholded", thresholded);
    //imshow("ThresholdedHands", thresholdedHands);
    
    
    
    //2D vector of points to store contours
    std::vector<std::vector<cv::Point> > contours;
    
    
    //find the image contours
    cv::findContours(thresholded, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    

    
    
    //loop through the stored contours
    for(std::vector<std::vector< cv::Point >>::iterator it = contours.begin(); it<contours.end() ; it++){
        
        
        std::vector<cv::Point> pts = *it;
        
        //protect againts small contours values of contours
        if(pts.size() <= 3) continue;
        
        cv::Mat pointsMatrix = cv::Mat(pts);
        
        //define green
        cv::Scalar colorContour(0, 255, 0);
        cv::Scalar colorHull(255, 0, 0);
        cv::Scalar colorDefect(0, 255, 255);
        
        
        for(std::vector<cv::Point>::iterator itp=pts.begin()+1; itp<pts.end(); itp++){
            
            cv::line(outputImage, *(itp-1), *itp, colorContour, 1);
            
            
        }
        
        
     
        // detect some convex hulls
        std::vector<cv::Point> hull( contours.size() );
        std::vector<int> hullIdx;
        
        //get coordinates of the hull points
        cv::convexHull(pts, hull, false);
        
        //get indecies of the hull points in the contour
        cv::convexHull(pts, hullIdx);
        
        for(std::vector<cv::Point>::iterator itp=hull.begin(); itp<hull.end(); itp++){
            
            cv::ellipse(outputImage, *itp, cv::Size(10, 10), 0.0, 0.0, 360.0 ,colorHull, 1);
            if(itp!= hull.begin()){
                
                cv::line(outputImage, *(itp-1), *itp, colorHull, 1);
                
            }
            
            
        }
        

        
        //detect defects
        std::vector<cv::Vec4i> defects;
        
        cv::convexityDefects(pts, hullIdx, defects);
        
        //vector of the farthest points
        std::vector<cv::Point> farDefects;
        for(std::vector<cv::Vec4i>::iterator d = defects.begin(); d<defects.end(); d++){
            
            int farIdx = (*d)[2];
            cv::Point farPoint = pts[farIdx];
            farDefects.push_back(farPoint);
            //std::cout << "Drawing defect at: " << farPoint.x << "x" << farPoint.y << std::endl;
            cv::ellipse(outputImage, farPoint, cv::Size(10, 10), 0.0, 0.0, 360.0 ,colorDefect, 3);
            
        }
        
        
        
       
    }

    imshow("output", outputImage);
}














