//
//  ShapeModel.cpp
//  OpenCVTest
//
//  Created by Oliver Chilton on 30/11/2013.
//  Copyright (c) 2013 Oliver Chilton. All rights reserved.
//

#include "ShapeModel.h"


/* default constructor */
ShapeModel::ShapeModel(){
    
}


ShapeModel::ShapeModel(std::string fileName): source(fileName) {
    std::ifstream file;
    std::string temp;
    float tempFloat;
    size_t pos;
    file.open(source, std::ios::in);
    
    /* Parse file into coordinate array */
    while(!file.eof()){
        std::vector<float> tempVec;
        std::getline(file, temp);
        
        while((pos = temp.find("||")) != std::string::npos){
            
            tempFloat = atof(temp.substr(0, pos).c_str());
            tempVec.push_back(tempFloat);
            temp.erase(0, pos+2);
        }
        tempFloat = atof(temp.substr(0, pos).c_str());
        tempVec.push_back(tempFloat);
        
        std::cout << "Added: ";
        for(int i = 0; i < tempVec.size(); i++){
            std::cout << tempVec[i] << ", ";
        }
        std::cout << std::endl;
        
        coordinates.push_back(tempVec);
    }
}


/* load file method */
void ShapeModel::loadFile(std::string fileName){
    
    if(coordinates.size() > 0){
        std::cout << "You have already loaded files, the currently loaded files will be appended" << std:: endl;
    }
    
    source = fileName;
    std::ifstream file;
    std::string temp;
    float tempFloat;
    size_t pos;
    std::vector<float> tempVec;
    file.open(source, std::ios::in);
    
    /* Parse file into coordinate array */
    while(!file.eof()){
        std::getline(file, temp);
        std::cout << temp << std::endl;
        
        while((pos = temp.find("||")) != std::string::npos){
            std::cout << "found: " << temp.substr(0, pos) << std::endl;
            tempFloat = atof(temp.substr(0, pos).c_str());
            tempVec.push_back(tempFloat);
            temp.erase(0, pos+2);
        }
        std::cout << "found: " << temp.substr(0, temp.length()) << std::endl;
        tempFloat = atof(temp.substr(0, pos).c_str());
        tempVec.push_back(tempFloat);
        
        coordinates.push_back(tempVec);
    }
}


ShapeModel::~ShapeModel(){
    
    std::cout << "Desructiing ShapeModel" << std::endl;
}