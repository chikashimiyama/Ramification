#include "ofApp.h"
#include "const.h"

ofApp::ofApp(std::map<std::string, std::vector<float> > parameterMap):
parameterMap(parameterMap){}


void ofApp::createTexture(){
    noiseImage.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_GRAYSCALE );
    
    
    for(int i = 0; i < gNumPixels; i++){
        noiseImage.getPixels()[i] = ofRandom(255);
    }
    noiseImage.update();
    
}

//--------------------------------------------------------------
void ofApp::setup(){
    
    oscReceiver.setup(50000);

    
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
            pointVector.push_back(ofVec3f(i ,j,  0.0));
        }
    }
    
    vbo.setVertexData(&pointVector.front(), gNumPixels , GL_DYNAMIC_DRAW);
    
    modelMatrix.setTranslation(-gWindowHalfWidth, -gWindowHalfHeight,0);
    //modelMatrix.setRotate(ofQuaternion(90, ofVec3f(1.0, 0, 0 )));

    viewMatrix.makeLookAtViewMatrix(ofVec3f(0.0, 0, 1000), ofVec3f(0,0,0) , ofVec3f(0,1,0));
    projectionMatrix.makePerspectiveMatrix(60.0, 1.3333, 0.01, 10000);
    
}


void ofApp::processOSC(){
    while(oscReceiver.hasWaitingMessages()){
        
        // get message
        ofxOscMessage mes;
        oscReceiver.getNextMessage(&mes);
        
        // check address
        const std::string addr = mes.getAddress();
        std::map<std::string, std::vector<float> >::iterator it = parameterMap.find(addr);
        std::vector<float> dataVector;
        if(it == parameterMap.end()){
            ofLog(OF_LOG_ERROR) << "ofApp::processOSC " << addr << "no such parameter";
            continue;
        }else{
            if(mes.getNumArgs() != (*it).second.size()){
                ofLog(OF_LOG_ERROR) << "ofApp::processOSC " << addr << "number of args unmatched. received " << mes.getNumArgs() << " supposed to be " << (*it).second.size();
                continue;
            }
            
            for(int i = 0; i < mes.getNumArgs(); i++){
                ofxOscArgType tp = mes.getArgType(i);
                
                // make sure all data are float
                switch (tp) {
                    case OFXOSC_TYPE_FLOAT:{
                        dataVector.push_back(mes.getArgAsFloat(i));
                        break;
                    }
                    case OFXOSC_TYPE_INT32:{
                        dataVector.push_back(static_cast<float>(mes.getArgAsInt32(i)));
                        break;
                    }
                    default:{
                        ofLog(OF_LOG_ERROR) << "ofApp::update no such type";
                        break;
                    }
                }
            }
            (*it).second = dataVector;
        }
    }
}


void ofApp::applyData(){
    
    ofVec3f cp(parameterMap["/camera"][0],parameterMap["/camera"][1],parameterMap["/camera"][2]);
    
    viewMatrix.makeLookAtViewMatrix(cp, ofVec3f(0,0,0) , ofVec3f(0,1,0));
    
    avoidZero(parameterMap["/freqX"][0]);
    avoidZero(parameterMap["/freqY"][0]);
    
    for(int i = 0 ; i < 3 ; i++){
        offset[i].x += parameterMap["/speedX"][i]  / (gWindowWidth /  parameterMap["/freqX"][i]);
        offset[i].y += parameterMap["/speedY"][i]  / (gWindowHeight / parameterMap["/freqY"][i]);
    }
    // attention the order of calculation !!!
    MVP =  modelMatrix * viewMatrix * projectionMatrix;
    
}


void ofApp::avoidZero(float &value){
    
    if(value < 0.01f &&  value > 0.0f){
        value = 0.01f;
    }
    else if(value > -0.01f &&  value < 0.0f){
        value = -0.01f;
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    processOSC();
    applyData();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
    
    noiseImage.getTextureReference().bind();
    
    
    shader.begin();
    shader.setUniformMatrix4f("modelMatrix", modelMatrix);
    
    shader.setUniform1f("distanceFactor", parameterMap["/distanceFactor"][0]);
    shader.setUniform3fv("cameraPos", &parameterMap["/camera"].front());
    shader.setUniform2f("offset0", offset[0].x, offset[0].y);
    shader.setUniform2f("offset1", offset[1].x, offset[1].y);
    shader.setUniform2f("offset2", offset[2].x, offset[2].y);

    shader.setUniform2f("freq0",parameterMap["/freqX"][0], parameterMap["/freqY"][0]);
    shader.setUniform1f("amp0", parameterMap["/amp"][0]);
    shader.setUniform2f("freq1",parameterMap["/freqX"][1], parameterMap["/freqY"][1]);
    shader.setUniform1f("amp1", parameterMap["/amp"][1]);
    shader.setUniform2f("freq2",parameterMap["/freqX"][2], parameterMap["/freqY"][2]);
    shader.setUniform1f("amp2", parameterMap["/amp"][2]);
    shader.setUniformMatrix4f("MVP", MVP);
    vbo.draw(GL_POINTS, 0, gNumPixels);
    shader.end();
    
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
