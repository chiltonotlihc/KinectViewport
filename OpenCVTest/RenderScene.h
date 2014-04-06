//
//  RenderScene.h
//  OpenGLScene
//
//  Created by Oliver Chilton on 24/03/2014.
//  Copyright (c) 2014 Oliver Chilton. All rights reserved.
//

#ifndef __OpenGLScene__RenderScene__
#define __OpenGLScene__RenderScene__

#include <iostream>
#include <string>
#include <vector>
#include <GLUT/GLUT.h>
#include <OpenGL/OpenGL.h>

#include "templateMatching.h"


class RenderScene{
private:
    
    static RenderScene *instance;
    void setInstance();
    
    
    std::string windowTitle;
    int WINDOW_HEIGHT;
    int WINDOW_WIDTH;
    float position;
    
    std::vector<float> eyeVector;
    std::vector<float> centerVector;
    std::vector<float> upVector;
    
    
    void renderLights();
    void setMatricies();
    
    TemplateCapture capture;
    
    
public:
    RenderScene();
    RenderScene(std::string title, int width, int height);
    virtual ~RenderScene();
    
    void initialize(int, char**);
    
    void displayScene();
    void runScene();

    
    void setEyePosition(float x, float y, float z);
    float getEyeX(){return eyeVector[0];};
    
    
    static void displayWrapper();
    static void runWrapper();
    
};



#endif /* defined(__OpenGLScene__RenderScene__) */
