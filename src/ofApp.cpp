#include "ofApp.h"
#include "const.h"

ofApp::ofApp(std::map<std::string, std::vector<float> > parameterMap):
parameterMap(parameterMap){
    planeImage.allocate(gNumGrid, gNumGrid, OF_IMAGE_COLOR);
    sphereImage.allocate(gNumGrid, gNumGrid, OF_IMAGE_COLOR);
    tubeImage.allocate(gNumGrid, gNumGrid, OF_IMAGE_COLOR);
    ringImage.allocate(gNumGrid, gNumGrid, OF_IMAGE_COLOR);
    
    for(int i = 0; i < gNumGrid; i++){
        for(int j = 0; j < gNumGrid; j++){
            
            float theta = (float)i / (float)(gNumGrid) * g2PI;
            float phi = (float)j / (float)(gNumGrid) * g2PI;
            float x = cos(theta) * sin(phi) * 0.5 + 0.5;
            float y = sin(theta) * sin(phi) * 0.5 + 0.5;
            float z = cos(phi) * 0.5 + 0.5;
            

            //plane
            planeImage.setColor(i, j, ofFloatColor( i / (float)(gNumGrid), j / (float)(gNumGrid), 0.5 ));
            
            //sphere
            sphereImage.setColor(i, j, ofFloatColor(x,y,z));
            
            //tube
            tubeImage.setColor(i, j,
                               ofFloatColor(
                                      cos( (float)i /(float)gNumGrid * g2PI - M_PI) * 0.5 + 0.5 ,
                                      sin( (float)j /(float)gNumGrid * g2PI - M_PI) * 0.5 + 0.5 ,
                                      sin( (float)i /(float)gNumGrid * g2PI - M_PI) *
                                            cos( (float)j /(float)gNumGrid * g2PI) * 0.5 + 0.5));
            
            //ring
            ringImage.setColor(i, j, ofFloatColor( cos(theta) * 0.5 + 0.5, sin(theta) * 0.5 + 0.5, 0.5 ));
        }
    }
    planeImage.update();
    sphereImage.update();
    tubeImage.update();
    ringImage.update();
}


void ofApp::createTexture(){

    noiseImage.allocate(gNumGrid, gNumGrid, OF_IMAGE_GRAYSCALE );
    
    for(int i = 0; i < gNumPixels; i++){
        noiseImage.getPixels()[i] = ofRandom(0.0, 1.0);
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
    for(int i = 0; i < gNumGrid; i++){
        for(int j = 0; j < gNumGrid; j++){
            pointVector.push_back(ofVec3f(i ,j,  0.0));
        }
    }
    
    vbo.setVertexData(&pointVector.front(), gNumPixels , GL_DYNAMIC_DRAW);
    


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

template <typename T>
T ofApp::wrap(T target, T operand){
    if(operand == 0)
        return 0;
    while(target < 0)
        target += operand;
    while(target > operand)
        target -= operand;
    return target;
}

void ofApp::applyData(){
    
    modelMatrix.makeIdentityMatrix();
    modelMatrix.rotate(90, 1, 0, 0);
    modelMatrix.scale(parameterMap["/scaleX"][0], parameterMap["/scaleY"][0], 1.0);
    

    
    avoidZero(parameterMap["/freqX"][0]);
    avoidZero(parameterMap["/freqY"][0]);
    
    for(int i = 0 ; i < 3 ; i++){
        float rotation = parameterMap["/rotate"][i];
        float speedX = parameterMap["/speedX"][i] * cos(rotation) - parameterMap["/speedX"][i] * sin(rotation);
        float speedY = parameterMap["/speedY"][i] * sin(rotation) + parameterMap["/speedY"][i] * cos(rotation);
        offset[i].x += speedX/ (gNumGrid /  parameterMap["/freqX"][i]);
        offset[i].y += speedY/ (gNumGrid / parameterMap["/freqY"][i]);
        
        offset[i].x = wrap(offset[i].x, (float)gNumGrid);
        offset[i].y = wrap(offset[i].y, (float)gNumGrid);

    }
    
    twistOffset.x += parameterMap["/twistSpeedX"][0];
    twistOffset.y += parameterMap["/twistSpeedY"][0];
    twistOffset.x = wrap(twistOffset.x, (float)g2PI);
    twistOffset.y = wrap(twistOffset.y, (float)g2PI);
    
    noiseLightOffset.x += parameterMap["/noiseLightSpeedX"][0];
    noiseLightOffset.y += parameterMap["/noiseLightSpeedY"][0];
    noiseLightOffset.x = wrap(noiseLightOffset.x, (float)gNumGrid);
    noiseLightOffset.y = wrap(noiseLightOffset.y, (float)gNumGrid);

    
    float elevation = parameterMap["/camera/elevation"][0];
    float distance = parameterMap["/camera/distance"][0];
    float azimuth = parameterMap["/camera/azimuth"][0];
    cameraPos.y = distance * sin(elevation) * cos(azimuth);
    cameraPos.x = distance * sin(elevation) * sin(azimuth);
    cameraPos.z = distance * cos(elevation);
    
    viewMatrix.makeLookAtViewMatrix(cameraPos, ofVec3f(0,0,0) , ofVec3f(0,1,0));
    
    // attention the order of calculation !!!
    MVP =  modelMatrix * viewMatrix * projectionMatrix;

    ofLog() << cameraPos;
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
    ofBackground(0);

    ofEnableDepthTest();

    shader.begin();
    shader.setUniform2f("randomCoord", ofRandom(0, gNumGrid), ofRandom(gNumGrid));
    shader.setUniformTexture("noiseTexture", noiseImage.getTextureReference(), 1);
    shader.setUniformTexture("sphereTexture", sphereImage.getTextureReference(), 2);
    shader.setUniformTexture("planeTexture", planeImage.getTextureReference(), 3);
    shader.setUniformTexture("tubeTexture", tubeImage.getTextureReference(), 4);
    shader.setUniformTexture("ringTexture", ringImage.getTextureReference(), 5);

    shader.setUniformMatrix4f("modelMatrix", modelMatrix);
    shader.setUniform1f("noiseFactor", parameterMap["/noiseFactor"][0]);
    shader.setUniform1f("distanceFactor", parameterMap["/distanceFactor"][0]);
    shader.setUniform1f("noiseToColorFactor", parameterMap["/noiseToColorFactor"][0]);

    shader.setUniform3f("cameraPos", cameraPos.x , cameraPos.y, cameraPos.z);
    shader.setUniform2f("offset0", offset[0].x, offset[0].y);
    shader.setUniform2f("offset1", offset[1].x, offset[1].y);
    shader.setUniform2f("offset2", offset[2].x, offset[2].y);
    
    shader.setUniform1f("twistFactor", parameterMap["/twistFactor"][0]);
    shader.setUniform2f("twistFreq", parameterMap["/twistFreqX"][0], parameterMap["/twistFreqY"][0]);
    shader.setUniform2f("twistOffset", parameterMap["/twistOffsetX"][0], parameterMap["/twistOffsetY"][0]);

    shader.setUniform1f("sphereAmp", parameterMap["/sphereAmp"][0]);
    shader.setUniform1f("tubeAmp", parameterMap["/tubeAmp"][0]);
    shader.setUniform1f("planeAmp", parameterMap["/planeAmp"][0]);
    shader.setUniform1f("ringAmp", parameterMap["/ringAmp"][0]);
    
    shader.setUniform2f("noiseLightFreq", parameterMap["/noiseLightFreqX"][0], parameterMap["/noiseLightFreqY"][0]);
    shader.setUniform1f("noiseLightAmp", parameterMap["/noiseLightAmp"][0]);
    shader.setUniform2f("noiseLightOffset", noiseLightOffset.x, noiseLightOffset.y);
    
    shader.setUniform2f("freq0",parameterMap["/freqX"][0], parameterMap["/freqY"][0]);
    shader.setUniform1f("amp0", parameterMap["/amp"][0]);
    shader.setUniform2f("freq1",parameterMap["/freqX"][1], parameterMap["/freqY"][1]);
    shader.setUniform1f("amp1", parameterMap["/amp"][1]);
    shader.setUniform2f("freq2",parameterMap["/freqX"][2], parameterMap["/freqY"][2]);
    shader.setUniform1f("amp2", parameterMap["/amp"][2]);
    shader.setUniformMatrix4f("MVP", MVP);
    vbo.draw(GL_POINTS, 0, gNumPixels);
    shader.end();
    ofDisableDepthTest();
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
