//
//  ContourDef.cpp
//  OpenCVTest
//
//  Created by Oliver Chilton on 18/04/2014.
//  Copyright (c) 2014 Oliver Chilton. All rights reserved.
//

#include "ContourDef.h"


ContourDef::ContourDef(){
    
    
}

ContourDef::ContourDef(std::vector<cv::Point> initPoints){
    
    contourPoints = initPoints;
    
    analyse();
}

ContourDef::~ContourDef(){

    
}

void ContourDef::analyse(){
    
    if(contourPoints.size() <= 100) return;
    
    //
    //  Detect Hull Points in the Contour
    //
    
        //Define sizes of the hullPoints vector
        std::vector<int> hullIdx;
        
    
        //get indecies of the hull points in the contour
        cv::convexHull(contourPoints, hullIdx);
        hullPoints.clear();
    
    
        //Clean up Hull points when too close together
        std::vector<int>::iterator hIdx = hullIdx.begin()+1;
        cv::Point tempPoint1, tempPoint2;
        for(; hIdx != hullIdx.end(); hIdx++){
            tempPoint1 = contourPoints[*(hIdx-1)];
            tempPoint2 = contourPoints[*hIdx];
            
            double distance = norm(tempPoint1 - tempPoint2);
            
            //if distance between points is small, remove first point
            if(distance < 5.0){
                hullIdx.erase(hIdx-1);
                hIdx--;
            }else{
                //else, push point onto hullPoints vector
                hullPoints.push_back(tempPoint2);
                
            }
        
        }
    
    
    
    //
    //  Detect Defects in the Contour
    //
    


        //Detect the convexity defects
        cv::convexityDefects(contourPoints, hullIdx, defectStructs);
        

        //Loop through list of defects and populate the defectPoints array
        defectPoints.clear();
    
    
        for(std::vector<cv::Vec4i>::iterator d = defectStructs.begin(); d<defectStructs.end(); d++){
            
            //check that the defect point is greater than some distance
            if((*d)[3]/265.0 < 20) continue;
            
            
            
            //get the index of the point in the contour
            int farIdx = (*d)[2];
            defectPoints.push_back(cv::Point(contourPoints[farIdx]));

            
        }
    
    isOpenHand = (defectPoints.size() >= 4);
    
    
}

void ContourDef::showAll(cv::Mat output){
    showContourLines(output);
    showDefectPoints(output);
    showHullLines(output);
    showHullPoints(output);
    
}

void ContourDef::showContourLines(cv::Mat output){
    cv::Scalar color;
    if(isOpenHand) {
        color = cv::Scalar(0, 255, 0);
    }else color = cv::Scalar(255, 0, 0);
    
    for(std::vector<cv::Point>::iterator c = contourPoints.begin()+1; c<contourPoints.end(); c++){
        
        cv::line(output, *(c-1), *c, color);
        
    }
    
    
}


void ContourDef::showDefectPoints(cv::Mat output){
    
    cv::Scalar color(255, 127, 0);
    
    for(std::vector<cv::Point>::iterator d = defectPoints.begin(); d<defectPoints.end(); d++){
        
        cv::circle(output, *d, 10, color);
        
    }
    
    
}

void ContourDef::showHullLines(cv::Mat output){
    
    cv::Scalar color(255, 0, 127);
    
    for(std::vector<cv::Point>::iterator h = hullPoints.begin()+1; h<hullPoints.end(); h++){
        
        cv::line(output, *(h-1), *h, color);
        
    }
    
    
}

void ContourDef::showHullPoints(cv::Mat output){
    
    cv::Scalar color(0, 127, 255);
    
    for(std::vector<cv::Point>::iterator h = hullPoints.begin(); h<hullPoints.end(); h++){
        
        cv::circle(output, *h, 10, color);
        
    }
    
    
}




