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
    frameCount = 0;
    previousTime = 0;
    position = 0;
}

RenderScene::RenderScene(std::string title, int width, int height){
    windowTitle = title;
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
    frameCount = 0;
    previousTime = 0;
    position = 0;
    
    //setup vectors
    eyeVector.push_back(0.0);
    eyeVector.push_back(0.0);
    eyeVector.push_back(-70.0);
    
    centerVector.push_back(0.0);
    centerVector.push_back(0.0);
    centerVector.push_back(0.0);
    
    upVector.push_back(0.0);
    upVector.push_back(1.0);
    upVector.push_back(0.0);
    
    
    hands.setCaptureObject(&capture);
    
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
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow(windowTitle.c_str());
    
    
    glutDisplayFunc(displayWrapper);
    
    glClearColor(1.0, 0.0, 0.0, 1.0);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    
    
    
    glutIdleFunc(runWrapper);
    
    glutPassiveMotionFunc(mouseCallback);
    
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
    
    setMatricies();
    
    
    drawSceneObjects();
    
    renderLights();
    
    glutSwapBuffers();
}

void RenderScene::drawSceneObjects(){
    
    
    GLfloat whiteDiffuseMaterialTea[] = {0.8, 0.8, 0.8, 1.0};
    GLfloat redSpecularMaterialTea[] = {1.0, 0.3, 0.3, 1.0};
    
    GLfloat whiteDiffuseMaterialArena[] = {0.4, 0.4, 0.4, 1.0};
    GLfloat whiteSpecularMaterialArena[] = {0.2, 0.2, 0.2, 1.0};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, whiteDiffuseMaterialTea);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, redSpecularMaterialTea);
    
    
    glutWireTeapot(10);
    
    glPushMatrix();
    
    glPushMatrix();
    glTranslatef(-10, -40, 100);
    glutSolidSphere(10, 20, 20);
    
    glTranslatef(20, 20, -10);
    glutSolidSphere(10, 20, 20);
    
    glTranslatef(20, 40, -50);
    glutSolidSphere(10, 20, 20);
    
    glTranslatef(-50, 20, 30);
    glutSolidSphere(10, 20, 20);
    
    glPopMatrix();
    
    glTranslatef(0, 0, 50);
    glScalef(1.0, 1.0, 4.0);
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, whiteDiffuseMaterialArena);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterialArena);
    
    glutSolidCube(100);
    
    
    glPopMatrix();
    
    
    
}

void RenderScene::runScene(){
    
    
    //capture.run();
    //eyeVector[0] = capture.getNormPositionX();
    //eyeVector[1] = capture.getNormPositionY();
    //eyeVector[2] = capture.getNormPositionZ();
    
    //eyeVector[0] = mouseX-WINDOW_WIDTH/2;
    
    eyeVector[0] = 25;
    eyeVector[1] = -5;
     
    std::cout << "Eye-x: " << eyeVector[0] << std::endl;
    std::cout << "Eye-y: " << eyeVector[1] << std::endl;
    std::cout << "Eye-z: " << eyeVector[2] << std::endl;
    
    //hands.update();
    //capture.showRGB("Input");
    //capture.showDepth("Depth");
    //capture.showMask("Mask");
    
    calculateFramerate();
    std::cout << "Framerate: " << fps << std::endl;
    
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
    
    GLfloat light0_position[] = { 10.0, 10.0, -80.0, 0.0 };
    GLfloat light1_position[] = { -8.0, -10.0, -60.0, 0.0 };
    GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat lmodel_ambient[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat ambient_light0[] = { 0.8, 0.6, 1.0, 0.5 };
    GLfloat ambient_light1[] = { 0.6, 1.0, 0.2, 0.5 };
    
    glLightfv( GL_LIGHT0, GL_POSITION, light0_position );
    glLightfv( GL_LIGHT1, GL_POSITION, light1_position );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, ambient_light0 );
    glLightfv( GL_LIGHT1, GL_DIFFUSE, ambient_light1 );
    
    
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 2.0);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.5);
    
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 2.0);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 1.0);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.5);
    
    //glLightfv( GL_LIGHT0, GL_DIFFUSE, white_light );
    //glLightfv( GL_LIGHT0, GL_SPECULAR, white_light );
    
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


void RenderScene::calculateFramerate(){
    
    frameCount++;
    
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    
    int timeInterval = currentTime - previousTime;
    
    if(timeInterval > 1000){
     
        fps = frameCount / (timeInterval / 1000.0f);
        
        previousTime = currentTime;
        
        frameCount = 0;
        
    }
}

void RenderScene::mouseCallback(int x, int y){
    
    instance->mouseX = x;
    instance->mouseY = y;
    
}


// wrappers for callbacks

void RenderScene::displayWrapper(){
    instance->displayScene();
    //instance->displayTestScene();
}

void RenderScene::runWrapper(){
    instance->runScene();
}






