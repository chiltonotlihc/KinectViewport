//
//  RenderScene.cpp
//  OpenGLScene
//
//  Created by Oliver Chilton on 24/03/2014.
//  Copyright (c) 2014 Oliver Chilton. All rights reserved.
//

#include "RenderScene.h"


RenderScene *RenderScene::instance = NULL;


RenderScene::RenderScene(){
    windowTitle = "Window1";
    WINDOW_WIDTH = 200;
    WINDOW_HEIGHT = 200;
    position = 0;
}

RenderScene::RenderScene(std::string title, int width, int height){
    windowTitle = title;
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
    position = 0;
    
    //setup vectors
    eyeVector.push_back(0.0);
    eyeVector.push_back(0.0);
    eyeVector.push_back(10.0);
    
    centerVector.push_back(0.0);
    centerVector.push_back(2.0);
    centerVector.push_back(0.0);
    
    upVector.push_back(0.0);
    upVector.push_back(1.0);
    upVector.push_back(0.0);
    
    
    
    
}

RenderScene::~RenderScene(){
    
    
}


void RenderScene::setEyePosition(float x, float y, float z){
    
    eyeVector[0] = x;
    eyeVector[1] = y;
    eyeVector[2] = z;
    
}


void RenderScene::initialize(int argc, char**argv){
    //set global RenderScene variable to this instance
    setInstance();
    
    
    
    //setup glut environment
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow(windowTitle.c_str());
    
    
    glutDisplayFunc(displayWrapper);
    
    glClearColor(1.0, 0.0, 0.0, 1.0);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    
    
    
    glutIdleFunc(runWrapper);
    
    glutMainLoop();
}


void RenderScene::setInstance(){
    instance = this;
}

/* Display function,
 *  Render Scene here....
 */
void RenderScene::displayScene(){
    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    renderLights();
    setMatricies();
    
    
    glPushMatrix();

    
    //glTranslatef(eyeVector[1], eyeVector[2], -500);
        glutSolidTeapot(35);
    glPopMatrix();
    
    glutSwapBuffers();
}

void RenderScene::runScene(){
    
    
    capture.run();
    eyeVector[0] = capture.getNormPositionX();
    eyeVector[1] = capture.getNormPositionY();
    eyeVector[2] = capture.getNormPositionZ();
     
    std::cout << "Eye-x: " << eyeVector[0] << std::endl;
    std::cout << "Eye-y: " << eyeVector[1] << std::endl;
    std::cout << "Eye-z: " << eyeVector[2] << std::endl;
    
    //hands.update(&capture);
    capture.showRGB("Input");
    capture.showDepth("Depth");
    //capture.showMask("Mask");
    
    glutPostRedisplay();
}



void RenderScene::setMatricies(){
    
    int width = glutGet( GLUT_WINDOW_WIDTH );
    int height = glutGet( GLUT_WINDOW_HEIGHT );
    float aspectRatio = width/height;
    glViewport( 0, 0, width, height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 60, aspectRatio, 1, 500.0 );
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluLookAt(eyeVector[0], eyeVector[1], eyeVector[2],
              centerVector[0], centerVector[1], centerVector[2],
              upVector[0], upVector[1], upVector[2]);

}

void RenderScene::renderLights(){
    
    GLfloat light1_position[] = { 0.0, 1.0, 1.0, 0.0 };
    GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat lmodel_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
    GLfloat ambient_light[] = { 0.8, 0.8, 0.8, 1.0 };
    
    glLightfv( GL_LIGHT0, GL_POSITION, light1_position );
    glLightfv( GL_LIGHT0, GL_AMBIENT, ambient_light );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, white_light );
    glLightfv( GL_LIGHT0, GL_SPECULAR, white_light );
    
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, lmodel_ambient );
    
    
}

void RenderScene::displayTestScene(){

    renderLights();
    setMatricies();
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glBegin(GL_POLYGON);
    glVertex2i(100,100);
    glVertex2i(100,300);
    glVertex2i(400,300);
    glVertex2i(600,150);
    glVertex2i(400,100);
    glEnd();
    glFlush();
    
    
}




// wrappers for callbacks
void RenderScene::displayWrapper(){
    instance->displayScene();
    //instance->displayTestScene();
}

void RenderScene::runWrapper(){
    instance->runScene();
}






