#pragma once

#include "ofMain.h"
#include "ofxOscReceiver.h"

class ofApp : public ofBaseApp{

	public:
    ofApp(std::map<std::string, std::vector<float> > parameterMap);
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    protected:
        void createPanel();
        void createTexture();
    
        void drawShape();
   
        ofFbo fbo, trailFbo, blurFbo;
        ofShader shader;
        ofShader blurShader;
        ofCamera camera;
    
        ofVec2f offset[3];
        ofVec2f twistOffset;
        ofVec2f noiseLightOffset;
        ofVec3f cameraPos;
        ofMatrix4x4 modelMatrix;
        ofMatrix4x4 viewMatrix;
        ofMatrix4x4 projectionMatrix;
        ofMatrix4x4 MVP;
        ofVbo vbo;
        ofVbo panelVbo;
    
        ofFloatImage noiseImage, sphereImage, planeImage, tubeImage, ringImage;
    
        std::map<std::string, std::vector<float> > parameterMap; // osc
        ofxOscReceiver oscReceiver;
    
        template <typename T>
        T wrap(T target, T operand);
        void processOSC();
        void applyData();
        void avoidZero(float &value); 
};
