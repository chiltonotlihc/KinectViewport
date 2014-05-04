//
//  ContourDef.cpp
//  OpenCVTest
//
//  Created by Oliver Chilton on 18/04/2014.
//  Copyright (c) 2014 Oliver Chilton. All rights reserved.
//

#include "ContourDef.h"


ContourDef::ContourDef(){
    longestHullsIdx = new int[2];
    longestHullsLength = new double[2];
    
    huMoments = new double[8];
    
}

ContourDef::ContourDef(std::vector<cv::Point> initPoints){
    longestHullsIdx = new int[2];
    longestHullsLength = new double[2];
    
    huMoments = new double[8];
    
    contourPoints = initPoints;
    
    analyse();
    //segmentHand();
}

ContourDef::~ContourDef(){
    //delete longestHullsIdx;
    //delete longestHullsLength;
    
    //delete[] huMoments;
    
}

void ContourDef::analyse(){
    
    if(contourPoints.size() <= 100) return;
    
    //
    //  Detect Hull Points in the Contour
    //
    
        //Define sizes of the hullPoints vector
        std::vector<int> hullIdx;
        
    
        //get indecies of the hull points in the contour
        cv::convexHull(contourPoints, hullIdx, true);
        hullPoints.clear();
    
    
        //Clean up Hull points when too close together
        std::vector<int>::iterator hIdx = hullIdx.begin()+1;
        cv::Point tempPoint1, tempPoint2;
        for(; hIdx != hullIdx.end(); hIdx++){
            tempPoint1 = contourPoints[*(hIdx-1)];
            tempPoint2 = contourPoints[*hIdx];
            
            double distance = std::abs(cv::norm(tempPoint1 - tempPoint2));
            
            //if distance between points is small, remove first point
            if(distance < 5.0 && false){
                hullIdx.erase(hIdx-1);
                hIdx--;
            }else{
                //else, push point onto hullPoints vector
                hullPoints.push_back(tempPoint1);
                
            }
        
        }
    
        hullPoints.push_back(*hullPoints.begin());
    
    
    
    //
    //  Detect Defects in the Contour
    //
    


        //Detect the convexity defects
        cv::convexityDefects(contourPoints, hullIdx, defectStructs);
        

        //Loop through list of defects and populate the defectPoints array
        defectPoints.clear();
    
        cv::Point defectAvg = {0, 0};
    cv::Point tempPoint;
        for(std::vector<cv::Vec4i>::iterator d = defectStructs.begin(); d<defectStructs.end(); d++){
            
            //check that the defect point is greater than some distance
            if((*d)[3]/265.0 < 20) continue;
            
            
            
            //get the index of the point in the contour
            int farIdx = (*d)[2];
            tempPoint = cv::Point(contourPoints[farIdx]);
            defectPoints.push_back(tempPoint);
            defectAvg+=tempPoint;
            
            
        }
    
    defectAvg = defectAvg * (1.0/defectPoints.size());

    
    
    isOpenHand = (defectPoints.size() >= 4);
    
    if(isOpenHand){
        
        handCenter = cv::Point(defectAvg);
        
    }else{
        
        cv::Scalar avg = cv::mean(contourPoints);
        handCenter.x = avg[0];
        handCenter.y = avg[1];
    }
    
    
    
    
    contourMoments = cv::moments(contourPoints, false);
    cv::HuMoments(contourMoments, huMoments);
    
    /*
    //std::cout << "Hu Moments:";
    for(int i = 0 ; i<8; i++){
        
        std::cout << "\t" << huMoments[i];
    }
    std::cout << std::endl;
     */
    
}

void ContourDef::segmentHand(){

    if(hullPoints.size()< 2) return;
    
    longestHullsLength[0] = -1;
    longestHullsLength[1] = -1;
    double length;
    //loop thought the hull points
    int counter = 0;
    for(std::vector<cv::Point>::iterator h = hullPoints.begin()+1; h<hullPoints.end(); h++){
        
        //find absolute length
        
        length = std::abs(cv::norm(*h - *(h-1)));
        
        //put the longest lengths into the array
        if(length > longestHullsLength[0]){
            longestHullsLength[0] = length;
            longestHullsIdx[0] = counter;
        }else if(length > longestHullsLength[1]){
            longestHullsLength[1] = length;
            longestHullsIdx[1] = counter;
        }
        
        counter++;
    }
    
    
    
}

double ContourDef::huDistance(double* moments){
    
    double total = 0;
    double sum;
    
    try{
        
        for(int i = 0; i<8; i++){
            sum = moments[i] - huMoments[i];
            total += sum*sum;
        }
        
        total = std::sqrt(total);
        
    }catch(std::exception e){
        std::cerr << "[EXCEPTION] Array out of bounds" << std::endl;
        return -1;
    }
    
    return total;
    
    
}

void ContourDef::showAll(cv::Mat output){
    showContourLines(output);
    showDefectPoints(output);
    showHullLines(output);
    showHullPoints(output);
    
    cv::circle(output, handCenter, 10, cv::Scalar(255, 255, 100));
    
}

void ContourDef::showContourFill(cv::Mat output){
    cv::Scalar color;
    if(isOpenHand) {
        color = cv::Scalar(255, 255, 0);
    }else color = cv::Scalar(255, 0, 255);
    
    std::vector<std::vector<cv::Point>> contours;
    contours.push_back(contourPoints);
    
    cv::drawContours(output, contours, 0, color, -1);
    
    
}

void ContourDef::showContourLines(cv::Mat output){
    cv::Scalar color;
    if(isOpenHand) {
        color = cv::Scalar(255, 255, 0);
    }else color = cv::Scalar(255, 0, 255);
    
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
    int counter = 0;
    for(std::vector<cv::Point>::iterator h = hullPoints.begin()+1; h<hullPoints.end(); h++, counter++){
        
        if(counter == longestHullsIdx[0] || counter == longestHullsIdx[1]){
            cv::line(output, *(h-1), *h, cv::Scalar(0, 255, 127));
        }else
            cv::line(output, *(h-1), *h, color);
        
    }
    
    
}

void ContourDef::showHullPoints(cv::Mat output){
    
    cv::Scalar color(0, 127, 255);
    
    for(std::vector<cv::Point>::iterator h = hullPoints.begin(); h<hullPoints.end(); h++){
        
        cv::circle(output, *h, 10, color);
        
    }
    
    
}




