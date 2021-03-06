//
//  templateMatching.cpp
//  OpenCVTest
//
//  Created by Oliver Chilton on 09/12/2013.
//  Copyright (c) 2013 Oliver Chilton. All rights reserved.
//

#include "templateMatching.h"


TemplateCapture::TemplateCapture(){
    std::cout << "TemplateCapture Constructor" << std::endl;
    
    capture = cv::VideoCapture( CV_CAP_OPENNI );
    //capture = cv::VideoCapture("/Users/chiltonotlihc/UniWork/FYP/XcodeProjects/OpenCVTest/TestMedia.mov");
    
    //Load the cascade clasifiers
    if( !face_cascade.load( face_cascade_name ) ){
        printf("--(!)Error loading\n");
        throw -1;
    };
    if( !eyes_cascade.load( eyes_cascade_name ) ){
        printf("--(!)Error loading\n");
        throw -1;
    };
    
    color = cv::Scalar(255, 255, 255);
    
    sourceSize.x = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    sourceSize.y = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    
    templateSize.x = sourceSize.y/3.5;
    templateSize.y = sourceSize.x/3.5;
    
    
    faceDetected = false;
    templateCaptured = false;
    movingBox = false;
    bestSAD = 100000000;
    searchStep = 8;
    searchRadius = 0.5;
    
    recordingAccuracy = false;
    
    if(recordingAccuracy){
        recordingTruthPoints = false;
        truthPoints.push_back(cv::Point(0, 0));
    }
    
    ready = true;
    
    rgbPtr = &rgbData;
    
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

    
    bootTime = 10;
    
    
}

TemplateCapture::~TemplateCapture(){
    std::cout << "Template Capture Destructor" << std::endl;
    
    if(recordingAccuracy){
        
        float accuracySum = 0;
        
        for(int i = 0; i<accuracyRecords.size(); i++){
            std::cout << accuracyRecords[i] << std::endl;
            accuracySum += accuracyRecords[i];
        }
    
        std::cout << "----------------------------- " << std::endl;
    
        std::cout << "Sum: " << accuracySum << std::endl;
    
        accuracySum = accuracySum/accuracyRecords.size();

        std::cout << "Count: " << accuracyRecords.size() << std::endl;
        std::cout << "averageAccuracy: " << accuracySum << std::endl;
    }
    
    
}

void TemplateCapture::run(){
    
    if(ready || !recordingTruthPoints){
        capture.grab();
    
        capture.retrieve(rgbData, CV_CAP_OPENNI_BGR_IMAGE);
        capture.retrieve(rawDepth, CV_CAP_OPENNI_DEPTH_MAP);
        capture.retrieve(validMask, CV_CAP_OPENNI_VALID_DEPTH_MASK);
        rawDepth.convertTo(scaledDepth, CV_8UC1, 0.05f );
        
        cv::blur(scaledDepth, scaledDepth, cv::Size(5, 5));
    
        
        //use the mask to correct the scaledDepth image
        //cv::bitwise_not(validMask, validMask);
        //cv::bitwise_or(scaledDepth, 127, scaledDepth, validMask);
        
    
        //currentFrame = floor(capture.get(CV_CAP_PROP_POS_FRAMES));
       
        if(bootTime-- > 0) return;
    
        if(templateCaptured){
            //calculateSAD(&scaledDepth);
            //update kalman filter
            KM.predict();
            
            //measure the new position of face
            matchTemplate(&scaledDepth);
            
            //copy measurements into measurement Kalman filter matrix
            measurement(0) = normTemplatePosition.x;
            measurement(1) = normTemplatePosition.y;
            measurement(2) = normTemplatePosition.z;
            
            
            
            //compute the corrected values of measurement;
            cv::Mat estimated = KM.correct(measurement);
            
            //copy measurement back into normTemplatePostion
            
            normTemplatePosition.x = estimated.at<float>(0, 0);
            normTemplatePosition.y = estimated.at<float>(1, 0);
            normTemplatePosition.z = estimated.at<float>(2, 0);
            drawFacePosition();
            
        }else{
            
            //inefficient, change to move inline with report
            if(faceDetected){
                grabTemplate(&scaledDepth);
            }else{
                //detect face
                detectFace(&rgbData);
            }
        }
        //if(bestSAD>4000) faceDetected = false;
        ready = false;
        
    }
    

}


void TemplateCapture::detectFace(cv::Mat *frame){
    std::cout << "Detecting Face using VJ" << std::endl;
    
    std::vector<cv::Rect> faces;
    cv::Mat frame_gray;
    
    cvtColor(*frame, frame_gray, CV_BGR2GRAY);
    cv::equalizeHist(frame_gray, frame_gray);
    
    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0| CV_HAAR_SCALE_IMAGE, cv::Size(30, 30) );
    
    cv::Point2f templateScale = {1.1, 1.6};
    
    if(faces.size()>0){
        
        faceCenter.x = faces[0].x;
        faceCenter.y = faces[0].y;
        
        templateSize.x = faces[0].width*templateScale.x;
        templateSize.y = faces[0].height*templateScale.y;
        
        templatePosition.x = faces[0].x-(0.5*(templateSize.x-faces[0].width));
        templatePosition.y = faces[0].y-(0.5*(templateSize.y-faces[0].height));
        
        faceDetected = true;
        std::cout << "Found face at X: " << templatePosition.x << " Y: " << templatePosition.y << std::endl;
        
    }
    
    
}

void TemplateCapture::grabTemplate(cv::Mat* source){
    std::cout << "Grabbing template at X:" << templatePosition.x << " Y: " << templatePosition.y << std::endl;
    

    temp =  cv::Mat(*source, cv::Rect(templatePosition.x, templatePosition.y, templateSize.x, templateSize.y)).clone();
    
    templateCaptured = true;
    
    newSize = templateSize;
    
    faceCenter = templatePosition + cv::Point(templateSize.x/2, templateSize.y/2);
    
    convertToNormalPositions();
    
    initTemplateDepth = normTemplatePosition.z;
    
    //std::cout << "initial depth: " << initTemplateDepth << std::endl;
    
    KM.statePost.at<float>(3) = normTemplatePosition.x;
    KM.statePost.at<float>(4) = normTemplatePosition.y;
    KM.statePost.at<float>(5) = normTemplatePosition.z;
    
    //initialise world coordinates for the template
    templateSizeWorld.x = templateSize.x * initTemplateDepth / 594.0;
    templateSizeWorld.y = templateSize.y * initTemplateDepth / 591.0;
    
    cv::imshow("Template", temp);
}

bool TemplateCapture::isOpened(){
    return capture.isOpened();
}

void TemplateCapture::matchTemplate(cv::Mat* source){

    
    //convert template world corrdinates back into image coordinates
    newSize.width = templateSizeWorld.x / normTemplatePosition.z * 594.0;
    newSize.height = templateSizeWorld.y / normTemplatePosition.z * 591.0;
    
    //std::cout << "New templateSize: " << newSize.width << std::endl;
    

    
    cv::Mat result;
    try{
        result.create(source->rows-temp.rows+1, source->cols-temp.cols+1, CV_32FC1);
    
        cv::matchTemplate(*source, temp, result, CV_TM_CCOEFF_NORMED);
        cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );
    
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
    
        cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
    
        templatePosition = maxLoc;
        //std::cout << "Template-x: " << templatePosition.x << " Template-y: " << templatePosition.y << std::endl;
        
    }catch(std::exception e){
        //std::cout << "[Exception]" << e.what() << std::endl;
        
    }
    
    faceCenter = templatePosition + cv::Point(templateSize.x/2, templateSize.y/2);
    convertToNormalPositions();

    
}

cv::Scalar TemplateCapture::calculateSAD(cv::Mat* source){
    cv::Mat difference;
    cv::Mat searchArea;
    cv::Scalar SAD(10000000000000);
    cv::Scalar total;
    cv::Point bestPosition(0, 0);

    
    int limitX = source->cols-templateSize.x;
    int limitY = source->rows-templateSize.y;
    
    //calculate search limits
    int startX = templatePosition.x-(searchRadius*templateSize.x);
    int startY = templatePosition.y-(searchRadius*templateSize.y);

    
    if(startX < 0) startX = 0;
    if(startY < 0) startY = 0;
    
    int endX = templatePosition.x+(searchRadius*templateSize.x);
    if(endX > limitX) endX = limitX;
    
    int endY = templatePosition.y+(searchRadius*templateSize.y);
    if(endY > limitY) endY = limitY;
    
    
    //cv::imshow("template", temp);
    
    for(int y = startY; y<endY; y+=searchStep){
        //cycle through rows
        for(int x = startX; x<endX; x+=searchStep){
            //cycle through columns
            //now compare with template and print the SAD
            
            //std::cout << "calculating sad at" << x << ", " << y << std::endl;
            
            //searchArea = cv::Mat(rgbData, cv::Rect(x, y, temp.cols, temp.rows));
            
            searchArea = cv::Mat(*source, cv::Rect(x, y, temp.cols, temp.rows));

            difference = cv::abs(searchArea-temp);
            
            total = cv::sum(difference);
            //cv::waitKey(1);
            //imshow("search", difference);

            //cv::rectangle(scaledDepth, cv::Rect(bestPosition.x, bestPosition.y, temp.cols, temp.rows), color);

            if(total[0]<SAD[0]) {
                SAD[0] = total[0];
                bestPosition.x = x;
                bestPosition.y = y;
                //std::cout << "smaller: " << SAD[0]  << "at position: " << bestPosition.x << ", " << bestPosition.y << std::endl;
            }
        
        }
        
    }
    
    //std::cout << bestPosition.x << ", " << bestPosition.y << std::endl;
    std::cout << SAD[0] << std::endl;
    cv::rectangle(rgbData, cv::Rect(bestPosition.x, bestPosition.y, temp.cols, temp.rows), color);
    templatePosition = bestPosition;
    
    //convert into coordinates for rendering
    convertToNormalPositions();
    
    
    
    return SAD;
}

void TemplateCapture::convertToNormalPositions(){
    
    //TODO: change this to allow for the adaptive resizing of the template
    normTemplatePosition.z = -getAverageDepth(rawDepth, faceCenter)*0.1;
    
    normTemplatePosition.x = -(faceCenter.x - 339) * normTemplatePosition.z / 594.0;
    normTemplatePosition.y = (faceCenter.y - 242) * normTemplatePosition.z / 591.0;
    

    
    
}

float TemplateCapture::getAverageDepth(cv::Mat mat, cv::Point faceCenter){
    
    cv::Size windowSize = {20, 20};

    cv::Mat window = cv::Mat(mat, cv::Rect(faceCenter.x-windowSize.width, faceCenter.y-windowSize.height, windowSize.width, windowSize.height));
    
    float size = window.cols*window.rows;
    float oneOverSize = 1.0/size;
    
    cv::Scalar tmp = cv::sum(window);
    return tmp.mul(oneOverSize)[0];
    
}


void TemplateCapture::showRGB(std::string name){
    
    cv::imshow(name, rgbData);
    
}

void TemplateCapture::showDepth(std::string name){
    
    cv::imshow(name, scaledDepth);
    
}

void TemplateCapture::showMask(std::string name){
    
    cv::imshow(name, validMask);
    
}

void TemplateCapture::drawFacePosition(){
    
    faceCenter = templatePosition + cv::Point(templateSize.x/2, templateSize.y/2);
    
    //cv::rectangle(rgbData, cv::Rect(minLoc.x, minLoc.y, 2, 2), color);
    cv::rectangle(rgbData, cv::Rect(templatePosition.x, templatePosition.y, temp.cols, temp.rows), color);
    cv::ellipse(rgbData, faceCenter, cv::Size(10, 10), 0.0, 0.0, 360.0, cv::Scalar(100, 255, 60));
}

void TemplateCapture::drawTemplateSearchBox(){
    if(movingBox) {
        templatePosition = mouse+mouseOffset;
    }
    
    cv::Point otherCorner = templatePosition+templateSize;
    cv::rectangle(rgbData, templatePosition, otherCorner, color);
    cv::rectangle(scaledDepth, templatePosition, otherCorner, color);
    
}

void TemplateCapture::mouseMoved(int x, int y){
    //std::cout << "mouse moved: " << x << ", " << y << std::endl;
    if(!movingBox) return;
    
    mouse.x = x;
    mouse.y = y;
    
}

void TemplateCapture::mouseDown(int x, int y){
    std::cout << "mouse down: " << x << ", " << y << std::endl;
    mouse.x = x;
    mouse.y = y;
    if(!templateCaptured || false){
        if(x>templatePosition.x && x<templatePosition.x+templateSize.x && y>templatePosition.y && y<templatePosition.y+templateSize.y){
            //mouse was clicked inside box
            color = cv::Scalar(50, 255, 50);
            mouseOffset.x = templatePosition.x-x;
            mouseOffset.y = templatePosition.y-y;
            movingBox = true;
        }
        
        
    }
    
    if(recordingTruthPoints){
        std::cout << "----------------------------- " << std::endl;
        truthPoints.push_back(cv::Point(mouse.x-templateSize.x/2, mouse.y-templateSize.y/2));
        std::cout << "Recorded Truth Point: " << mouse.x-templateSize.x/2 << ", " << mouse.y-templateSize.y/2 << std::endl;
        ready = true;
        
        if(!templateCaptured){
            templatePosition.x = mouse.x-templateSize.x/2;
            templatePosition.y = mouse.y-templateSize.y/2;
            grabTemplate(&rgbData);
        }
        recordAccuracy();
        
    }
    
}

void TemplateCapture::mouseUp(){
    //std::cout << "mouse up" << std::endl;
    //mouse was released
    color = cv::Scalar(255, 255, 255);
    
    movingBox = false;
    
}

void TemplateCapture::setTruthPoints(std::vector<cv::Point> set){
    totalFrames = capture.get(CV_CAP_PROP_FRAME_COUNT);
    if(set.size() >= totalFrames) return;
    
    truthPoints = set;
    
    recordingAccuracy = true;
    
}

void TemplateCapture::recordAccuracy(){
    if(!recordingAccuracy) {
        std::cout << "Accuracy Record not initialised" << std::endl << "Please execute function: setTruthPoints" << std::endl;
        return;
    }
    if(truthPoints.size() == 0) return;
    double acc;
    
    std::cout << sqrtf(pow(0, 2));
    
    acc = sqrtf(pow(templatePosition.x-truthPoints[currentFrame-1].x, 2)+pow(templatePosition.y-truthPoints[currentFrame-1].y, 2));
    std::cout << "Frame: " << currentFrame-1 << std::endl;
    std::cout << "TemplatePosition: " << templatePosition << std::endl;
    std::cout << "TruthPosition: " << truthPoints[currentFrame-1] << std::endl;
    std::cout << "Matching Accuracy: " << acc << std::endl;
    accuracyRecords.push_back(acc);
    

}
