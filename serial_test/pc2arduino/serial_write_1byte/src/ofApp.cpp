#include "ofApp.h"

ofSerial mySerial;
unsigned char myByte = 123;
bool isPressed = false;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);

    mySerial.setup("/dev/cu.usbmodem1421",9600);
    //cout << mySerial.isInitialized() << endl;
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (isPressed) {
        bool byteWasWritten = mySerial.writeByte(myByte);
        if ( !byteWasWritten )
            printf("byte was not written to serial port \n");
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key=='g'){
        cout << mySerial.isInitialized() << endl;
        isPressed = true;
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
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
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
