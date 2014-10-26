//
//  PureData.h
//  noize
//
//  Created by Chikashi Miyama on 19/10/14.
//
//

#ifndef __noize__PureData__
#define __noize__PureData__

#include <stdio.h>
#include "ofxPd.h"


class PureData : public pd::PdReceiver{


public:
    void setup(const int numOutChannels, const int numInChannels, const int sampleRate, const int ticksPerBuffer);

    // audio callbacks
    void audioReceived(float * input, int bufferSize, int nChannels);
    void audioRequested(float * output, int bufferSize, int nChannels);
    void receiveMessage(const std::string& dest, const std::string& msg, const pd::List& list);

protected:
    ofxPd pd;
};


#endif