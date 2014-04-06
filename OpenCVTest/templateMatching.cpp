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
    
    templateSize.x = sourceSize.y/4;
    templateSize.y = sourceSize.x/4;
    
    
    faceDetected = false;
    templateCaptured = false;
    movingBox = false;
    bestSAD = 100000000;
    searchStep = 8;
    searchRadius = 0.25;
    
    recordingAccuracy = false;
    
    if(recordingAccuracy){
        recordingTruthPoints = false;
        truthPoints.push_back(cv::Point(0, 0));
    }
    
    ready = true;
    
    rgbPtr = &rgbData;
    
    
}

TemplateCapture::~TemplateCapture(){
    std::cout << "Destructor" << std::endl;
    
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
        rawDepth.convertTo(scaledDepth, CV_8UC1, 0.05f );
        //cv::waitKey(1);
    
        //currentFrame = floor(capture.get(CV_CAP_PROP_POS_FRAMES));
        if(bestSAD>4000) faceDetected = false;
    
        if(templateCaptured){
                calculateSAD(&rgbData);
        }else{
            if(faceDetected){
                grabTemplate(&rgbData);
                drawTemplateSearchBox();
            }else{
                //detect thread on seperate thread
                detectFace(&rgbData);
            }
        }
        ready = false;
        
    }
    

}

void TemplateCapture::showWindow(std::string name){
    cv::imshow(name, rgbData);
    
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


void TemplateCapture::detectFace(cv::Mat *frame){
    std::cout << "Detecting Face using VJ" << std::endl;
    
    std::vector<cv::Rect> faces;
    cv::Mat frame_gray;
    
    cvtColor(*frame, frame_gray, CV_BGR2GRAY);
    cv::equalizeHist(frame_gray, frame_gray);
    
    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0| CV_HAAR_SCALE_IMAGE, cv::Size(30, 30) );
    
    if(faces.size()>0){
        templatePosition.x = faces[0].x;
        templatePosition.y = faces[0].y;
    
        std::cout << "found face at X: " << templatePosition.x << " Y: " << templatePosition.y << std::endl;
    
        grabTemplate(frame);
    }

    
}

void TemplateCapture::grabTemplate(cv::Mat* source){
    std::cout << "grabbing template at:" << templatePosition.y << std::endl;
    
    //temp =  cv::Mat(rgbData, cv::Rect(templatePosition.x, templatePosition.y, templateSize.x, templateSize.y)).clone();
    temp =  cv::Mat(*source, cv::Rect(templatePosition.x, templatePosition.y, templateSize.x, templateSize.y)).clone();
    //cv::imshow("template", temp);
    
    templateCaptured = true;
    
}

bool TemplateCapture::isOpened(){
    return capture.isOpened();
}

cv::Scalar TemplateCapture::calculateSAD(cv::Mat* source){
    cv::Mat difference;
    cv::Mat searchArea;
    cv::Scalar SAD(10000000000000);
    cv::Scalar total;
    cv::Point bestPosition(0, 0);

    
    int limitX = sourceSize.x-templateSize.x;
    int limitY = sourceSize.y-templateSize.y;
    
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
    //std::cout << SAD[0] << std::endl;
    cv::rectangle(rgbData, cv::Rect(bestPosition.x, bestPosition.y, temp.cols, temp.rows), color);
    templatePosition = bestPosition;
    
    normTemplatePosition.x = -((float)templatePosition.x/(float)sourceSize.x)-1;
    normTemplatePosition.y = -((float)templatePosition.y/(float)sourceSize.y);
    //imshow("BestMatch", cv::Mat(scaledDepth, cv::Rect(bestPosition.x, bestPosition.y, temp.cols, temp.rows)));
    
    return SAD;
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
