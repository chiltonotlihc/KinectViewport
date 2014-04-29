//
//  templateMatching.h
//  OpenCVTest
//
//  Created by Oliver Chilton on 09/12/2013.
//  Copyright (c) 2013 Oliver Chilton. All rights reserved.
//

#ifndef __OpenCVTest__templateMatching__
#define __OpenCVTest__templateMatching__

#include <iostream>
#include <vector>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/video/video.hpp>


class TemplateCapture{
private:
    cv::VideoCapture capture;
    cv::Mat rawDepth;
    cv::Mat scaledDepth;
    cv::Mat rgbData;
    cv::Mat validMask;
    cv::Mat validDepth;
    cv::Mat temp;
    float initTemplateDepth;
    int currentFrame;
    int totalFrames;
    
    int bootTime;
    
    cv::Point templatePosition;
    cv::Point faceCenter;
    cv::Point3_<float> normTemplatePosition;
    cv::Point sourceSize;
    cv::Point templateSize;
    cv::Size newSize;
    cv::Point templateSizeWorld;
    cv::Point mouseOffset;
    cv::Point mouse;
    cv::Scalar color;
    
    bool faceDetected;
    bool templateCaptured;
    bool ready;
    bool movingBox;
    
    void convertToNormalPositions();
    void drawTemplateSearchBox();
    void detectFace(cv::Mat* frame);
    float getAverageDepth(cv::Mat mat, cv::Point faceCenter);
    
    
    float bestSAD;
    int searchStep;
    float searchRadius;
    
    //Variables for accuracy testing
    bool recordingAccuracy;
    bool recordingTruthPoints;
    std::vector<cv::Point> truthPoints;
    std::vector<float> accuracyRecords;

    //Viola Jones Variables
    cv::String face_cascade_name = "haarcascade_frontalface_alt.xml";
    cv::String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
    cv::CascadeClassifier face_cascade;
    cv::CascadeClassifier eyes_cascade;
    
    //KalmanFilter
    cv::KalmanFilter KM;
    cv::Mat_<float> measurement;

    
public:
    TemplateCapture();
    ~TemplateCapture();
    void run();
    
    inline cv::Mat getRawDepth(){return rawDepth;};
    inline cv::Mat getDepthData(){return scaledDepth;};
    inline cv::Mat getValidDepth(){return validDepth;};
    inline cv::Mat getRgbData(){return rgbData;};
    inline cv::Mat getMaskData(){return validMask;};
    
    void showRGB(std::string name);
    void showDepth(std::string name);
    void showMask(std::string name);
    
    
    cv::Scalar calculateSAD(cv::Mat* source);
    void matchTemplate(cv::Mat* source);
    float getNormPositionX(){return normTemplatePosition.x;};
    float getNormPositionY(){return normTemplatePosition.y;};
    float getNormPositionZ(){return normTemplatePosition.z;};
    void grabTemplate(cv::Mat* source);
    void mouseDown(int x, int y);
    void mouseUp();
    void mouseMoved(int x, int y);
    bool isOpened();
    cv::Mat* rgbPtr;
    
    
    //Methods for accuracy Testing
    void setTruthPoints(std::vector<cv::Point> set);
    void recordAccuracy();
    
    
};





#endif /* defined(__OpenCVTest__templateMatching__) */
