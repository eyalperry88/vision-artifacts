#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

  //ofSetWindowPosition(PROJECTION_SCREEN_OFFSET, 0); // second screen - projector

  ofHideCursor();

	ofBackground(0,0,0);
  ofEnableSmoothing();

  ofSetRectMode(OF_RECTMODE_CENTER);

	ofSetFrameRate(60); // if vertical sync is off, we can go a bit fast... this caps the framerate at 60fps.

  //get back a list of devices.
  vector<ofVideoDevice> devices = vidGrabber.listDevices();

  for(size_t i = 0; i < devices.size(); i++){
      if(devices[i].bAvailable){
          //log the device
          ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
      }else{
          //log the device and note it as unavailable
          ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
      }
  }

  vidGrabber.setDeviceID(0);
  vidGrabber.setDesiredFrameRate(30);
  vidGrabber.initGrabber(CAM_WIDTH, CAM_HEIGHT);

  pixCaptured.allocate(CAM_WIDTH, CAM_HEIGHT, OF_PIXELS_RGB);
  for (size_t k = 0; k < BUFFER_SIZE; k++){
    for(size_t i = 0; i < CAM_WIDTH; i++){
      for(size_t j = 0; j < CAM_HEIGHT; j++){
        // sumH[k][i][j] = 0.0;
        // sumS[k][i][j] = 0.0;
        sumB[k][i][j] = 0.0;
      }
    }
  }

  bufferIdx = 0;
  ready = false;
  videoTexture.allocate(pixCaptured);
  ofSetVerticalSync(false);

  captureRadius = 200;
  scale = 2.4;

  texts[0].load("cast.png");
  texts[1].load("a.png");
  texts[2].load("shadow.png");
  textIndex = 0;
  appear = 0;
  appearStep = 2;
    // 
    //
    // int centerX = CAM_WIDTH / 2;
    // int centerY = CAM_HEIGHT / 2;
    // for(int i = 0; i < CAM_WIDTH; i++){
    //   for(int j = 0; j < CAM_HEIGHT; j++){
    //     if (pow(i - centerX, 2) + pow(j - centerY, 2) > pow(captureRadius, 2)) {
    //       pixCaptured.setColor(i, j, ofColor(0, 0, 0));
    //     } else {
    //       pixCaptured.setColor(i, j, ofColor(200, 200, 200));
    //     }
    //   }
    // }
    // videoTexture.loadData(pixCaptured);

}

//--------------------------------------------------------------
void ofApp::update(){
  vidGrabber.update();

  appear += appearStep;
  if (appear > 255) {
    appear = 0;
    textIndex = (textIndex + 1) % 3;
  }

  if(vidGrabber.isFrameNew()){
    ofPixels & pixels = vidGrabber.getPixels();
    for(size_t i = 0; i < CAM_WIDTH; i++){
      for(size_t j = 0; j < CAM_HEIGHT; j++){
        ofColor c = pixels.getColor(CAM_WIDTH - i -1 , CAM_HEIGHT - j - 1);
        // sumH[bufferIdx][i][j] = c.getHue();
        // sumS[bufferIdx][i][j] = c.getSaturation();
        sumB[bufferIdx][i][j] = c.getBrightness();
      }
    }
    bufferIdx++;
    if (bufferIdx >= BUFFER_SIZE) {
      bufferIdx = 0;
      ready = true;
    }
  }

  if (ready) {
    float avgH = 0;
    float avgS = 0;
    float avgB = 0;
    for(size_t i = 0; i < CAM_WIDTH; i++){
      for(size_t j = 0; j < CAM_HEIGHT; j++){
        for(size_t k = 0; k < BUFFER_SIZE; k++){
          // avgH += sumH[k][i][j];
          // avgS += sumS[k][i][j];
          avgB += sumB[k][i][j];
        }
      }
    }
    // avgH /= (CAM_WIDTH * CAM_HEIGHT * BUFFER_SIZE);
    // avgS /= (CAM_WIDTH * CAM_HEIGHT * BUFFER_SIZE);
    avgB /= (CAM_WIDTH * CAM_HEIGHT * BUFFER_SIZE);
    //ofLogNotice() << "avg" << avgH << ", " << avgS << ", " << avgB;

    int centerX = CAM_WIDTH / 2;
    int centerY = CAM_HEIGHT / 2;

    for(int i = 0; i < CAM_WIDTH; i++){
      for(int j = 0; j < CAM_HEIGHT; j++){
        if (pow(i - centerX, 2) + pow(j - centerY, 2) > pow(captureRadius, 2)) {
          pixCaptured.setColor(i, j, ofColor(0, 0, 0));
        } else {
          float h = 0;
          float s = 0;
          float b = 0;
          for(int k = 0; k < BUFFER_SIZE; k++){
            // h += sumH[k][i][j];
            // s += sumS[k][i][j];
            b += sumB[k][i][j];
          }
          //ofLogNotice() << "raw" << h << ", " << s << ", " << b;
          //ofLogNotice() << "norm" << h / BUFFER_SIZE << ", " << s  / BUFFER_SIZE << ", " << b / BUFFER_SIZE;

          // h /= BUFFER_SIZE;
          // s /= BUFFER_SIZE;
          b /= BUFFER_SIZE;

          h = 0;
          s = 0;

          b = 127 - (b - avgB);
          //b = 255;
          b *= 0.66;

          ofColor newC = ofColor::fromHsb(h, s, b);
          pixCaptured.setColor(i, j, newC);
        }
      }
    }
    //load the inverted pixels
    videoTexture.loadData(pixCaptured);
  }
}

//--------------------------------------------------------------
void ofApp::draw(){

  ofPushMatrix();
  ofSetHexColor(0xffffff);
  ofTranslate(CAM_WIDTH, CAM_HEIGHT);
  ofRotateDeg(60.0);
  ofScale(scale, scale);
  videoTexture.draw(0, 0, CAM_WIDTH, CAM_HEIGHT);
  ofScale(1.0, 1.0);
  ofPopMatrix();


  ofPushMatrix();
  ofTranslate(50, 480);
  ofRotateDeg(90.0);
  ofEnableAlphaBlending();
  ofSetColor(255, 255, 255, max(0, 255 - 2 * appear));
  texts[(textIndex + 2) % 3].draw(0, 0);
  ofSetColor(255, 255, 255, appear);
  texts[textIndex].draw(0, 0);
  ofDisableAlphaBlending();
  ofPopMatrix();
}


//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
  if (key == OF_KEY_RIGHT) {
    scale += 0.1;
  } else if (key == OF_KEY_LEFT) {
    if (scale > 1.0) {
      scale -= 0.1;
    }
  } else if (key == OF_KEY_UP) {
    if (2 * captureRadius < CAM_HEIGHT) {
      captureRadius++;
    }
  } else if (key == OF_KEY_DOWN) {
    if (captureRadius > 0) {
      captureRadius--;
    }
  }
  ofLogNotice() << "radius: " << captureRadius << " , scale: " << scale;
}

//--------------------------------------------------------------
void ofApp::keyReleased  (int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
