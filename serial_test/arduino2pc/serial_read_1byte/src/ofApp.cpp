#include "ofApp.h"

ofSerial mySerial;
int myByte = 0;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    mySerial.setup("/dev/cu.usbmodem1411",9600);
}

//--------------------------------------------------------------
void ofApp::update(){
    myByte = mySerial.readByte();
}

//--------------------------------------------------------------
void ofApp::draw(){
    if ( myByte == OF_SERIAL_NO_DATA )
        printf("no data was read \n");
    else if ( myByte == OF_SERIAL_ERROR )
        printf("an error occurred \n");
    else
        printf("myByte is %d \n", myByte);    
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
