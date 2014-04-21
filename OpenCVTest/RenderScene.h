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
#include "HandTracker.h"


class RenderScene{
private:
    
    static RenderScene *instance;
    void setInstance();
    
    
    std::string windowTitle;
    int WINDOW_HEIGHT;
    int WINDOW_WIDTH;
    int frameCount;
    int previousTime;
    float fps;
    float position;
    
    std::vector<float> eyeVector;
    std::vector<float> centerVector;
    std::vector<float> upVector;
    
    void calculateFramerate();
    
    void renderLights();
    void setMatricies();
    
    TemplateCapture capture;
    HandTracker hands;
    
    int mouseX, mouseY;
    
    
    
public:
    RenderScene();
    RenderScene(std::string title, int width, int height);
    virtual ~RenderScene();
    
    void initialize(int, char**);
    
    void displayScene();
    void displayTestScene();
    void drawSceneObjects();
    void runScene();
    
    
    void setEyePosition(float x, float y, float z);
    float getEyeX(){return eyeVector[0];};
    
    
    static void displayWrapper();
    static void runWrapper();
    static void mouseCallback(int x, int y);
    
};



#endif /* defined(__OpenGLScene__RenderScene__) */
