#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "LfRect.hpp"

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
    
    ofImage                 selectedColorOFImg;
    ofImage                 selectedBipolarizedOFImg;
    ofxCvColorImage			selectedColorImg;
    ofxCvGrayscaleImage 	selectedBipolarizedImg;

    vector<ofxCvColorImage>	selectedColorImgs;
    vector<ofxCvColorImage>	colorImgs;
    vector<ofxCvGrayscaleImage>	bipolarizedImgs;
    
    vector<ofImage> imgs;
    ofImage img;
    ofXml XML;
    
    static const int num_w = 3;
    static const int num_h = 3;
    static const int num_lf = num_w * num_h;
    bool isValids[num_lf];
    const int cam_coe = 6;
    //const int target_whole_base = 200;
    const int target_whole_size = 1080;
    
    int cam_w;
    int cam_h;
    int target_whole_x = 420;
    int target_whole_y = 0;
    int target_whole_w;
    int target_whole_h;
    int target_xs[num_lf];
    int target_ys[num_lf];
    int target_ws[num_lf];
    int target_hs[num_lf];
    int target_x;
    int target_y;
    int target_w;
    int target_h;
    int draw_xs[num_lf];
    int draw_ys[num_lf];
    int LfRectBrightnesss[num_lf];
    bool bNewFrame = false;
    bool isPressed = false;
    
    ofSerial mySerial;
    
    bool isBig = false;
    bool isSmall = false;
    int target_id = 0;
    bool isReadSetting = false;

    vector<int> pxCounts;
    
    int threshold_bNum; // threshold for the number of 'b'rien shrimp
    int threshold_mNum = 4; // threshold for the number of 'm'in state(min state means a darkest lightface)
    vector<int> minRectNum; // vector of lightface IDs for min state. This also count the number of min state.
    
    //LF simulator
    LfRect lfRect[9];
    int bStatus[9];
    int draw_simul_x = 1800;

    //lightface rect setup
    int size = 150;
    int gap = 3;
    
    int draw_whole_x = 50;
    int draw_whole_y = 50;

};

