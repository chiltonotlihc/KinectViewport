#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include "templateMatching.h"
#include "TimeRecord.h"
#include "RenderScene.h"
#include "oc_utilities.h"

using namespace std;
using namespace cv;




int main(int argc, char** argv){
    
    
    
    RenderScene scene("3D Scene", 640, 480);
    
    scene.initialize(argc, argv);
    

    
    
    
    return 0;
    
    
}
































