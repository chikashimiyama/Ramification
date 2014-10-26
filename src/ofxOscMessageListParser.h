//
//  ofxOscMessageListParser.h
//
//  Created by Chikashi Miyama on 12/10/14.
//
//

#ifndef ____ofxOscMessageListParser__
#define ____ofxOscMessageListParser__

#include "ofXML.h"
#include <tr1/unordered_map>

class ofxOscMessageListParser{
public:
    static ofxOscMessageListParser &getSingleton();
    bool parse(ofXml &messageListXML, std::map<std::string, std::vector<float> > &map);
private:
    ofxOscMessageListParser(); // cannot instantiate
};




#endif