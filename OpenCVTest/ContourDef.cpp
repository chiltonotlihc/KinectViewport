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
    
    if(contourPoints.size() <= 3) return;
    
    //
    //  Detect Hull Points in the Contour
    //
    
        //Define sizes of the hullPoints vector
        hullPoints = std::vector<cv::Point>( contourPoints.size() );
        std::vector<int> hullIdx;
        
        //get coordinates of the hull points
        cv::convexHull(contourPoints, hullPoints, false);
        
        //get indecies of the hull points in the contour
        cv::convexHull(contourPoints, hullIdx);
    
    
    //
    //  Detect Defects in the Contour
    //
    

        //Detect the convexity defects
        cv::convexityDefects(contourPoints, hullIdx, defectStructs);
        

        //Loop through list of defects and populate the defectPoints array
        defectPoints.clear();
    
    
        for(std::vector<cv::Vec4i>::iterator d = defectStructs.begin(); d<defectStructs.end(); d++){
            
            int farIdx = (*d)[2];
            

            defectPoints.push_back(cv::Point(contourPoints[farIdx]));

            
        }
    
}

void ContourDef::showAll(cv::Mat output){
    showContourLines(output);
    showDefectPoints(output);
    showHullLines(output);
    showHullPoints(output);
    
}

void ContourDef::showContourLines(cv::Mat output){
    
    cv::Scalar color(255, 0, 0);
    
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




