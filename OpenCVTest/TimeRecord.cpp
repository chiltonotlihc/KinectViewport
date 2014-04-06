//
//  TimeRecord.cpp
//  OpenCV-Template
//
//  Created by Oliver Chilton on 14/12/2013.
//  Copyright (c) 2013 Oliver Chilton. All rights reserved.
//

#include "TimeRecord.h"


TimeRecord::TimeRecord(int laps){
    std::cout << "Initialising TimeRecordObject" << std::endl;
    count = 0;
    maxLaps = laps;
    recording = false;
 
    startTime = -1;
    endTime = -1;
    
}

TimeRecord::~TimeRecord(){
    end();
    double average = calculateAverage();
    double total = calculateTotal();
    
    std::cout << "TimeRecord over." << std::endl;
    std::cout << "Total laps: " << count << ". " << std::endl;
    std::cout << "Total time: " << total << " SEC. " << std::endl;
    std::cout << "Average time: " << average << " SEC. " << std::endl;
    std::cout << "Framerate: " << 1.0/average << " FPS." << std::endl;
    
    lapTime.clear();
}

void TimeRecord::begin(){
    clock_t now = clock();
    std::cout << "Timer starting at: " << now << std::endl;
    
    startTime = now;
    lapTime.push_back(now);
    recording = true;
}

void TimeRecord::end(){
    clock_t now = clock();
    std::cout << "Timer ending at: " << now << std::endl;
    lapTime.push_back(now);
    endTime = now;
}
    
bool TimeRecord::addLap(){
    clock_t now = clock();
    count++;
    
    lapTime.push_back(now);
    
    return count>maxLaps;
}

double TimeRecord::calculateAverage(){
    double time = calculateTotal();
    
    return time/count;
    
    
}
double TimeRecord::calculateTotal(){
    return double(endTime - startTime)/CLOCKS_PER_SEC;
}



