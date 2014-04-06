//
//  TimeRecord.h
//  OpenCV-Template
//
//  Created by Oliver Chilton on 14/12/2013.
//  Copyright (c) 2013 Oliver Chilton. All rights reserved.
//

#ifndef __OpenCV_Template__TimeRecord__
#define __OpenCV_Template__TimeRecord__

#include <iostream>
#include <vector>
#include <ctime>

class TimeRecord{
private:
    clock_t startTime;
    std::vector<clock_t> lapTime;
    clock_t endTime;
    float count;
    float maxLaps;
    double calculateAverage();
    double calculateTotal();
    
    bool recording;
    
public:
    TimeRecord(int laps);
    virtual ~TimeRecord();
    
    
    void begin();
    void end();
    //void reset();
    
    bool addLap();
    
};

#endif /* defined(__OpenCV_Template__TimeRecord__) */
