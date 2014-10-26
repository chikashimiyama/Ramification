//
//  PureData.cpp
//  noize
//
//  Created by Chikashi Miyama on 19/10/14.
//
//

#include "ofMain.h"
#include "const.h"
#include "PureData.h"

void PureData::setup(const int numOutChannels, const int numInChannels, const int sampleRate, const int ticksPerBuffer){

    if(!pd.init(numOutChannels, numInChannels, sampleRate, ticksPerBuffer)) {
        OF_EXIT_APP(1);
    }

    pd.subscribe("toOF"); // necessary?
    pd.addReceiver(*this); // necessary?
    pd.receive(*this, "toOF");
    
    ofLog() << "PureData dsp started.";

    pd::Patch patch = pd.openPatch(pdFile);
    ofLog() << patch;
    
    if(gOscEnabled){
        pd.start();
    }
}

void PureData::audioReceived(float * input, int bufferSize, int nChannels){
    pd.audioIn(input, bufferSize, nChannels);
}

void PureData::audioRequested(float * output, int bufferSize, int nChannels){
    pd.audioOut(output, bufferSize, nChannels);
}

void PureData::receiveMessage(const std::string& dest, const std::string& msg, const pd::List& list){
    ofLog() << "message received from pd";
    ofLog() << dest << " " << msg;
}
