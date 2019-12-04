#pragma once

#include "ofMain.h"

#define PROJECTION_SCREEN_OFFSET 1920
#define PROJECTION_SCREEN_WIDTH 1280
#define PROJECTION_SCREEN_HEIGHT 800
#define CAM_WIDTH 640
#define CAM_HEIGHT 480
#define BUFFER_SIZE 25

class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    ofVideoGrabber vidGrabber;
    ofPixels pixCaptured;
    ofTexture videoTexture;

    // float sumH[BUFFER_SIZE][CAM_WIDTH][CAM_HEIGHT];
    // float sumS[BUFFER_SIZE][CAM_WIDTH][CAM_HEIGHT];
    float sumB[BUFFER_SIZE][CAM_WIDTH][CAM_HEIGHT];
    int bufferIdx;
    bool ready;

    int captureRadius;
    float scale;

    ofImage texts[3];
    int appear;
    int appearStep;
    int textIndex;
};
