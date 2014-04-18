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
    setIdentity(KM.processNoiseCov, cv::Scalar::all(1e-4));
    setIdentity(KM.measurementNoiseCov, cv::Scalar::all(1e-1));
    setIdentity(KM.errorCovPost, cv::Scalar::all(.1));

    
    
    
    
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


        
        //use the mask to correct the scaledDepth image
        cv::bitwise_not(validMask, validMask);
        cv::bitwise_or(scaledDepth, 127, scaledDepth, validMask);
        
    
        //currentFrame = floor(capture.get(CV_CAP_PROP_POS_FRAMES));
       
    
        if(templateCaptured){
            //calculateSAD(&scaledDepth);
            //update kalman filter
            KM.predict();
            
            //measure the new position of face
            matchTemplate(&scaledDepth);
            
            //copy measurements into measurement Kalman filter matrix
            measurement(0) = templatePosition.x;
            measurement(1) = templatePosition.y;
            measurement(2) = normTemplatePosition.z;
            
            //compute the corrected values of measurement;
            cv::Mat estimated = KM.correct(measurement);
            
            //copy measurement back into normTemplatePostion
            
            templatePosition.x = estimated.at<float>(0, 0);
            templatePosition.y = estimated.at<float>(1, 0);
            normTemplatePosition.z = estimated.at<float>(2, 0);
            
            
            convertToNormalPositions();
            
        }else{
            
            //inefficient, change to move inline with report
            if(faceDetected){
                grabTemplate(&scaledDepth);
                drawTemplateSearchBox();
            }else{
                //detect face
                detectFace(&rgbData);
            }
        }
        //if(bestSAD>4000) faceDetected = false;
        ready = false;
        
    }
    

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
    
    cv::Point2f templateScale = {1, 1.5};
    
    if(faces.size()>0){
        
        templateSize.x = faces[0].width*templateScale.x;
        templateSize.y = faces[0].height*templateScale.y;
        
        templatePosition.x = faces[0].x-(0.5*(templateSize.x-faces[0].width));
        templatePosition.y = faces[0].y-(0.5*(templateSize.y-faces[0].height));
        
        faceDetected = true;
        std::cout << "Found face at X: " << templatePosition.x << " Y: " << templatePosition.y << std::endl;
        
    }
    
    //added but not compiled and checked
    grabTemplate(frame);
    
}

void TemplateCapture::grabTemplate(cv::Mat* source){
    std::cout << "Grabbing template at X:" << templatePosition.x << " Y: " << templatePosition.y << std::endl;
    
    //temp =  cv::Mat(rgbData, cv::Rect(templatePosition.x, templatePosition.y, templateSize.x, templateSize.y)).clone();
    temp =  cv::Mat(*source, cv::Rect(templatePosition.x, templatePosition.y, templateSize.x, templateSize.y)).clone();
    //cv::imshow("template", temp);
    
    templateCaptured = true;
    
    
    initTemplateDepth = getAverageDepth(rawDepth, templatePosition);
    
    convertToNormalPositions();
    
    
    KM.statePost.at<float>(3) = templatePosition.x;
    KM.statePost.at<float>(4) = templatePosition.y;
    KM.statePost.at<float>(5) = normTemplatePosition.z;
    
    //initialise world coordinates for the template
    templateSizeWorld.x = templateSize.x * normTemplatePosition.z / 594.0;
    templateSizeWorld.y = templateSize.y * normTemplatePosition.z / 591.0;
    
    std::cout << "init template world size : " << templateSizeWorld.x << std::endl;
    
    cv::imshow("Template", temp);
}

bool TemplateCapture::isOpened(){
    return capture.isOpened();
}

void TemplateCapture::matchTemplate(cv::Mat* source){
    
    //scale template according to current depth.
    cv::Size newSize;
    
    //convert template world corrdinates back into image coordinates
    newSize.width = templateSizeWorld.x / normTemplatePosition.z * 594.0;
    newSize.height = templateSizeWorld.y / normTemplatePosition.z * 591.0;
    
    
    
    cv::Mat resizeTemp;
    
    if(newSize.width>0 && newSize.height>0){
        resizeTemp.create(newSize.height+1, newSize.width+1, CV_32FC1);
        cv::resize(temp, resizeTemp, newSize);
    }else{
        resizeTemp = temp;
    }
    

    
    //cv::rectangle(rgbData, cv::Rect(templatePosition.x-newSize.width/4, templatePosition.y - newSize.height/4, newSize.width*1.5, newSize.height*1.5), cv::Scalar(0, 200, 100));
    

    
    cv::Mat result;
    try{
        result.create(source->rows-resizeTemp.rows+1, source->cols-resizeTemp.cols+1, CV_32FC1);
    
        cv::matchTemplate(*source, resizeTemp, result, CV_TM_SQDIFF_NORMED);
        cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );
    
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
    
        cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
    
        templatePosition = minLoc;
        std::cout << "Template-x: " << templatePosition.x << " Template-y: " << templatePosition.y << std::endl;

        //templatePosition.x += temp.cols*0.5;
        std::cout << "MinVal: " << minVal << std::endl;
    }catch(std::exception e){
        std::cout << "[Exception]" << e.what() << std::endl;
        
    }
    convertToNormalPositions();

    
    
    //cv::rectangle(rgbData, cv::Rect(minLoc.x, minLoc.y, 2, 2), color);
    cv::rectangle(rgbData, cv::Rect(templatePosition.x, templatePosition.y, temp.cols, temp.rows), color);
    cv::rectangle(rgbData, cv::Rect(templatePosition.x, templatePosition.y, newSize.width, newSize.height), cv::Scalar(255, 123, 7));
    
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
    normTemplatePosition.z = getAverageDepth(rawDepth, templatePosition);
    
    normTemplatePosition.x = (templatePosition.x - 339) * normTemplatePosition.z / 594.0;
    normTemplatePosition.y = (templatePosition.y - 242) * normTemplatePosition.z / 591.0;
    

    
    
}

float TemplateCapture::getAverageDepth(cv::Mat mat, cv::Point p){
    
    if(p.x + temp.cols > sourceSize.x) p.x = sourceSize.x - temp.cols;
    if(p.y + temp.rows > sourceSize.y) p.y = sourceSize.y - temp.rows;
    cv::Mat window = cv::Mat(mat, cv::Rect(p.x, p.y, temp.cols, temp.rows));
    
    float size = window.cols*window.rows;
    float oneOverSize = 1.0/size;
    
    cv::Scalar tmp = cv::sum(window);
    return tmp.mul(oneOverSize)[0];
    
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
