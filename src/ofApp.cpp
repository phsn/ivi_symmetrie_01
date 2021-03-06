#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    receiver.setup(PORT);
    sender.setup(HOST, PORT);
    
    baseFBO.allocate( ofGetWidth(), ofGetHeight(), GL_RGB );
    kalaidoFBO.allocate( ofGetWidth(), ofGetHeight(), GL_RGB );
    
    squareSize = 2*ofGetHeight()/sqrt(3);
    
    kalaidoEnabled = true;
    fisheyeEnabled = true;
    
    kalaido.load( "mykaleido" );
    fisheye.load("fisheye");
    
    ofAddListener(beatSyncThread.tickEvent, this, &ofApp::onTick);
    ofAddListener(beatSyncThread.tick8Event, this, &ofApp::onTick8);
    ofAddListener(beatSyncThread.tick16Event, this, &ofApp::onTick16);
    ofAddListener(beatSyncThread.barEvent, this, &ofApp::onBar);
    ofAddListener(beatSyncThread.bpmChange, this, &ofApp::onBPMChange);
    
    beatSyncThread.start();
    
    gui.setup("ivi_symmetrie_01", "ivi_symmetrie_01", ofGetWidth()/2-100,ofGetHeight()-150);
    gui.add(numRepetitions.set("Repetitions", 10, 1, 100));
    gui.add(accumulation.set("Accumulation", 70.0, 0.0, 100.0));
    gui.add(fisheye_distortion.set("Distortion", 0.6, -1.0, 1.0));
    gui.add(fisheye_abberation.set("Chromatic Abberation", 70.0, 0.0, 200.0));

    
    centerObj.setup(ofVec3f(0,0,0), ofVec3f(0,0,0), ofVec3f(1,1,1));

}

//--------------------------------------------------------------
void ofApp::update(){
    // check for waiting messages
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        handleOSC(m);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetBackgroundAuto(true);

    kalaidoFBO.begin();
    drawKalaido();
    kalaidoFBO.end();
    ofSetColor( 255 );
    
    if ( fisheyeEnabled ) {
        fisheye.begin();
        fisheye.setUniform2f( "iResolution", ofGetWidth(), ofGetHeight()*1.77);
        fisheye.setUniform2f( "iMouse", ofGetWidth()/2.0+fisheye_distortion*ofGetWidth()/2.0, ofGetMouseY());
        fisheye.setUniform1f( "chromAbb", fisheye_abberation);

    }
    
    kalaidoFBO.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    
    if ( fisheyeEnabled ) {
        fisheye.end();
    }
    
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::drawKalaido(){
    baseFBO.begin();
    drawBase();
    baseFBO.end();
    ofSetColor( 255 );
    
    if ( kalaidoEnabled ) {
        kalaido.begin();
        kalaido.setUniform1i( "numRepetitions", numRepetitions);
        kalaido.setUniform2f( "screenSize", ofGetWidth(), ofGetHeight());
    }
    
    baseFBO.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    
    if ( kalaidoEnabled ) {
        kalaido.end();
    }

}

//--------------------------------------------------------------
void ofApp::drawBase(){
    ofSetColor(255,255,255,(1.0f-accumulation/100.0f)*255.0f);
    ofRect(0,0,ofGetWidth(),ofGetHeight());
    if ( kalaidoEnabled ) {
        //ofBackground(255,255,255,120);
        
    } else {
        //ofSetColor(0,0,0,100);
        //ofBackground(0,0,0,40);
    }
    
    //ofSetColor(255,255,255,120);
    //ofTriangle(ofGetWidth()/2, 0, ofGetWidth()/2+squareSize/2, ofGetHeight(),ofGetWidth()/2-squareSize/2, ofGetHeight());
    
    ofSetColor(centerColor);
    
    centerObj.update(ofVec3f(ofGetMouseX(), ofGetMouseY()-50, 0), ofVec3f(0,0,180*ofGetMouseX()/float(ofGetWidth())), ofVec3f(1,4,1));
    
    centerObj.draw();
    /*
    ofTranslate(ofGetWidth()*0.6,0);
    ofRotateZ(90*ofGetMouseX()/float(ofGetWidth()));
    ofTranslate(-ofGetWidth()*0.6,0);
    ofSetColor(0, 0, 0);
    //ofEllipse(ofGetMouseX(),ofGetMouseY(),200,200);
    ofRect(0,ofGetMouseY()-50,ofGetWidth(),100);*/
}

//--------------------------------------------------------------
void ofApp::onBar(ofVec2f & bObj){
    centerColor = ofColor(0,0,0);
}

//--------------------------------------------------------------
void ofApp::onTick(ofVec2f & tObj){
    centerColor = ofColor(0,0,40);
    ofxOscMessage m;
    m.setAddress("/BeatState/x");
    m.addFloatArg((tObj.y)/3.0f);
    sender.sendMessage(m);
}

//--------------------------------------------------------------
void ofApp::onTick8(ofVec2f & tObj){
    centerColor = ofColor(0,80,0);

}

//--------------------------------------------------------------
void ofApp::onTick16(ofVec2f & tObj){
    //centerColor = ofColor(150,0,0);

    //cout << "TICK " << tObj.y << endl;
}

//--------------------------------------------------------------
void ofApp::onBPMChange(ofVec2f & tObj){
    ofxOscMessage m;
    m.setAddress("/BPM_Slider/x");
    m.addFloatArg(tObj.x/300.0f);
    sender.sendMessage(m);
}

//--------------------------------------------------------------
void ofApp::handleOSC(ofxOscMessage m) {
    
    if(m.getAddress() == "/Sync/x" && m.getArgAsFloat(0) == 1) {
        beatSyncThread.syncBeat();
    }
    if(m.getAddress() == "/Tap/x" && m.getArgAsFloat(0) == 1) {
        beatSyncThread.tap();
        //cout << "TAP || BPM = " << beatSyncThread.getBPM() << endl;
    }
    if(m.getAddress() == "/clearTaps/x" && m.getArgAsFloat(0) == 1) {
        beatSyncThread.clearTaps();
    }
    if(m.getAddress() == "/BPM_Slider/x") {
        beatSyncThread.setBPM(int(m.getArgAsFloat(0)*300.0f));
        //cout << "BPM ADJUST || BPM = " << beatSyncThread.getBPM() << endl;
    }
    if(m.getAddress() == "/BPM_Mult/x") {
        
        if(   int(m.getArgAsFloat(0)) == 0
           && int(m.getArgAsFloat(1)) == 0
           && int(m.getArgAsFloat(2)) == 0
           && int(m.getArgAsFloat(3)) == 0) {
            
            beatSyncThread.resetMult();
            
        } else {
            if(int(m.getArgAsFloat(0)) == 1) {
                beatSyncThread.multBPM(0.25);
            } else if(int(m.getArgAsFloat(1)) == 1) {
                beatSyncThread.multBPM(0.5);
            } else if(int(m.getArgAsFloat(2)) == 1) {
                beatSyncThread.multBPM(2);
            } else if(int(m.getArgAsFloat(3)) == 1) {
                beatSyncThread.multBPM(4);
            }
        };
        
        //cout << "BPM ADJUST || BPM = " << beatSyncThread.getBPM() << endl;
        
    }
    
    if(m.getAddress() == "/BPM_MultBar/x") {
        
        float multbarVal = (1.0f-m.getArgAsFloat(0))*6.0f+1.0f;
        //cout << "MULTBAR " << multbarVal << endl;
        if(int(multbarVal) == 1) {
            
            beatSyncThread.resetMult();
            
        } else {
            
            beatSyncThread.multBPM(multbarVal);
            
        };
        
        //cout << "BPM ADJUST || BPM = " << beatSyncThread.getBPM() << endl;
        
    }
    
    if(m.getAddress() == "/BPM_MultBar_Zero/x") {
        
        float multbarVal = (1.0f-m.getArgAsFloat(0));
        //cout << "MULTBAR " << multbarVal << endl;
        if(int(multbarVal) == 1) {
            
            beatSyncThread.resetMult();
            
        } else {
            
            beatSyncThread.multBPM(multbarVal);
            
        };
        
        //cout << "BPM ADJUST || BPM = " << beatSyncThread.getBPM() << endl;
        
    }
}



//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == 'k' ) {
        kalaidoEnabled = !kalaidoEnabled;
    } else if(key == 'f') {
        fisheyeEnabled = !fisheyeEnabled;
    } else if(key == 'b') {
        centerObj.blur = !centerObj.blur;
    }

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
