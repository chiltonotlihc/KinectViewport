//
//  HandDef.cpp
//  OpenCVTest
//
//  Created by Oliver Chilton on 04/05/2014.
//  Copyright (c) 2014 Oliver Chilton. All rights reserved.
//

#include "HandDef.h"

HandDef::HandDef(){
    
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    
    open = true;
    
    //initialise Kalman filter object
    KM.init(6, 3, 0);
    KM.transitionMatrix = cv::Mat::eye(6, 6, CV_32F);
    KM.transitionMatrix.at<float>(0, 3) = 1.0;
    KM.transitionMatrix.at<float>(1, 4) = 1.0;
    KM.transitionMatrix.at<float>(2, 5) = 1.0;
    KM.statePost.at<float>(3) = 0;
    KM.statePost.at<float>(4) = 0;
    KM.statePost.at<float>(5) = 0;
    
    // intialise measurement matrix for Kalman filter
    measurement.create(3, 1);
    measurement.setTo(cv::Scalar(0));
    
    cv::setIdentity(KM.measurementMatrix);
    cv::setIdentity(KM.processNoiseCov, cv::Scalar::all(5e-4));
    cv::setIdentity(KM.measurementNoiseCov, cv::Scalar::all(1e-1));
    cv::setIdentity(KM.errorCovPost, cv::Scalar::all(.1));
    
    
}


void HandDef::updatePosition(cv::Point3f measured){
    
    KM.predict();
    
    
    //copy measurements into measurement Kalman filter matrix
    measurement(0) = measured.x;
    measurement(1) = measured.y;
    measurement(2) = measured.z;
    
    
    
    //compute the corrected values of measurement;
    cv::Mat estimated = KM.correct(measurement);
    
    //copy measurement back into position
    pos.x = estimated.at<float>(0, 0);
    pos.y = estimated.at<float>(1, 0);
    pos.z = estimated.at<float>(2, 0);

    
}

