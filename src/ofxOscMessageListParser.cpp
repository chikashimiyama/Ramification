//
//  ofxOscMessageListParser.cpp
//  noize
//
//  Created by Chikashi Miyama on 12/10/14.
//
//

#include "ofxOscMessageListParser.h"

ofxOscMessageListParser::ofxOscMessageListParser(){}

ofxOscMessageListParser &ofxOscMessageListParser::getSingleton(){
    static ofxOscMessageListParser singleton;
    return singleton;
}

bool ofxOscMessageListParser::parse(ofXml &messageListXML, std::map<std::string, std::vector<float> > &map){
    
    messageListXML.setToChild(0);
    if(!(messageListXML.getName() == "messageType")){
        return false;
    }
    do{
        
        std::string address = messageListXML.getValue("address");
        int args = messageListXML.getIntValue("args");
        std::vector<float> init;

        if(args == 1){
            float value = messageListXML.getFloatValue("default");
            init.push_back(value);
        }
        else if(args > 1){
            
            for(int i = 0; i < args; i++){
                std::string query = "default[@index=" + ofToString(i) + "]";
                float value = messageListXML.getFloatValue(query);
                init.push_back(value);
            }
        }
        
        ofLog() << address << "  " << ofToString(init);
        map.insert(std::pair<std::string, std::vector<float> >(address, init));
    }while(messageListXML.setToSibling());
    

    
}