#include "ofMain.h"
#include "ofApp.h"
#include "ofxOscMessageListParser.h"

//========================================================================
int main( ){
    ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
    ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context

    ofXml xml;
    if(!xml.loadFromBuffer(ofBufferFromFile(ofToDataPath("OSCMessages.xml")))){
        ofLog() << "unable to XML file. fatel error.";
        std::terminate();
    }
    
    std::map<std::string, std::vector<float> > map;
    if(!ofxOscMessageListParser::getSingleton().parse(xml, map)){
        ofLog() << "unable to interpret XML. fatel error.";
        std::terminate();
    }
    
	ofRunApp(new ofApp(map));

}

