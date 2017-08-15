#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		

        ofVideoGrabber 		vidGrabber;

        ofxCvColorImage			colorImg_cam;
        ofxCvColorImage			colorImg;

        ofxCvGrayscaleImage 	grayImage;
        //ofxCvGrayscaleImage 	grayBg;
        ofxCvGrayscaleImage 	grayBg0;
        ofxCvGrayscaleImage 	grayBg1;
        ofxCvGrayscaleImage 	grayBg2;
        ofxCvGrayscaleImage 	grayBg3;
        ofxCvGrayscaleImage 	grayBg4;
        ofxCvGrayscaleImage 	grayBg5;
        ofxCvGrayscaleImage 	grayBg6;
        ofxCvGrayscaleImage 	grayBg7;
        ofxCvGrayscaleImage 	grayBg8;
		ofxCvGrayscaleImage 	grayBg9;
		ofxCvGrayscaleImage 	grayDiff;

        ofxCvContourFinder 	contourFinder;

		int 				threshold;
		bool				bLearnBakground;

        ofImage img;
        ofImage img_old;
        ofXml XML;
    


};

