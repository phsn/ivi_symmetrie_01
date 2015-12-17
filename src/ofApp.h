#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"

#include "iviObj.h"
#include "beatThread.h"

#define PORT 8000
#define HOST "192.168.178.73"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
    
		void draw();
        void drawKalaido();
        void drawBase();
    
        void handleOSC(ofxOscMessage m);
    
        void onTick16(ofVec2f & tObj);
        void onTick8(ofVec2f & tObj);
        void onTick(ofVec2f & tObj);
        void onBar(ofVec2f & bObj);
        void onBPMChange(ofVec2f & tObj);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    ofxPanel gui;
    ofParameter<int> numRepetitions;
    ofParameter<float> accumulation;
    ofParameter<float> fisheye_distortion;
    ofParameter<float> fisheye_abberation;

    
    iviObj centerObj;
    ofColor centerColor;
    
    ofxOscReceiver receiver;
    ofxOscSender sender;
    
    beatThread beatSyncThread;
    
    ofFbo baseFBO;
    ofFbo kalaidoFBO;
    
    bool kalaidoEnabled;
    bool fisheyeEnabled;
    
    ofShader kalaido;
    ofShader fisheye;
    
    int squareSize;
		
};
