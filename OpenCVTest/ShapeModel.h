//
//  ShapeModel.h
//  OpenCVTest
//
//  Created by Oliver Chilton on 30/11/2013.
//  Copyright (c) 2013 Oliver Chilton. All rights reserved.
//

#ifndef __OpenCVTest__ShapeModel__
#define __OpenCVTest__ShapeModel__

#include "ShapeModel.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>


class ShapeModel{
public:
    ShapeModel(std::string fileName);
    ShapeModel();
    ~ShapeModel();
    
    void loadFile(std::string fileName);
    
    std::vector<float> getCoordinates(int i);
    
    
    
private:
    std::vector<std::vector<float>> coordinates;
    std::string source;
    
};

#endif /* defined(__OpenCVTest__ShapeModel__) */
