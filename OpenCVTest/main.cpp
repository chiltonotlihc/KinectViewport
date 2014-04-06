#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include "templateMatching.h"
#include "TimeRecord.h"

using namespace std;
using namespace cv;

void MouseCallBackFunc(int event, int x, int y, int flags, void* userdata);

string window_name = "Capture - Face detection";



int main(){
    
    TemplateCapture capture;
    //TimeRecord recorder(496);
    
    bool pause = false;
    
    namedWindow(window_name, 1);
    //namedWindow("depth", 1);
    
    setMouseCallback(window_name, MouseCallBackFunc, &capture);
    //recorder.begin();
    for(;;){
        capture.run();
        //if(recorder.addLap()) break;
        
        if(!capture.isOpened()) break;
        
        imshow(window_name, capture.getRgbData());
        imshow("depth", capture.getDepthData());
        
        if(waitKey(2) == 'c') break;
        if(waitKey(2) == 's') capture.grabTemplate(capture.rgbPtr);
        if(waitKey(10) == 'p') pause = true;
        while(pause){
           if(waitKey(2) == 'p') pause = false;
        }
        
    }
    //recorder.end();
     
    
    return 0;
    
}

void MouseCallBackFunc(int event, int x, int y, int flags, void* userdata){
    
    TemplateCapture* tC = reinterpret_cast<TemplateCapture*>(userdata);
    
    if(event == EVENT_LBUTTONDOWN) tC->mouseDown(x, y);
    if(event == EVENT_LBUTTONUP) tC->mouseUp();
    if(event == EVENT_MOUSEMOVE) tC->mouseMoved(x,y);
};
































