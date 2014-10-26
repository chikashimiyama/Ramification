#include "ofApp.h"
#include "const.h"



void ofApp::createTexture(){
    noiseImage.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_GRAYSCALE );
    
    
    for(int i = 0; i < gNumPixels; i++){
        noiseImage.getPixels()[i] = ofRandom(255);
    }
    noiseImage.update();
    
}

//--------------------------------------------------------------
void ofApp::setup(){
    createTexture();
    
    ofBackground(ofColor::black);
    
    if(ofIsGLProgrammableRenderer()){
        ofLog() << "programrable renderer";
    }
    
    if(!shader.load("noise")){
        ofExit(1);
    }
    
    std::vector<ofVec3f> pointVector;
    for(int i = 0; i < gWindowWidth; i++){
        for(int j = 0; j < gWindowHeight; j++){
            pointVector.push_back(ofVec3f(i ,
                                          j,  0.0));
        }
    }
    
    vbo.setVertexData(&pointVector.front(), gNumPixels , GL_DYNAMIC_DRAW);

    camera.setPosition(ofVec3f(0,0, 1200));
    camera.lookAt(ofVec3f(0,0,0));
    camera.setFarClip(10000);
    camera.setNearClip(0.1);
    camera.setAspectRatio(1.3333);
    camera.setFov(60.0);
}


//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
    
    noiseImage.getTextureReference().bind();
    
    camera.begin();
    shader.begin();
    
    shader.setUniform3fv("cameraPos", camera.getPosition().getPtr());
    vbo.draw(GL_POINTS, 0, gNumPixels);
    shader.end();
    camera.end();
    noiseImage.getTextureReference().unbind();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
