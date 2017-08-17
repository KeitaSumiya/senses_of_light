#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    vidGrabber.setVerbose(true);
    vidGrabber.setDeviceID(0);
    vidGrabber.initGrabber(cam_coe*320,cam_coe*240);
    cam_w = vidGrabber.getWidth();
    cam_h = vidGrabber.getHeight();
    printf("cam_w %d   cam_h %d \n", cam_w, cam_h );
    
    ofSetFrameRate(5);
    mySerial.setup("/dev/cu.usbmodem1421",9600);
    
    target_whole_w = cam_coe*target_whole_base;
    target_whole_h = cam_coe*target_whole_base;
    if (target_whole_w > cam_w - target_whole_x){
        target_whole_w = cam_w - target_whole_x;
    }
    if (target_whole_h > cam_h - target_whole_y){
        target_whole_h = cam_h - target_whole_y;
    }
    target_w = target_whole_w/num_w;
    target_h = target_whole_h/num_h;
    if (isReadSetting){
        XML.load("mySettings.xml");
        for (int w=0; w<num_w; w++){
            for (int h=0; h<num_h; h++){
                int val_id = w*num_w+h;
                target_xs[val_id] = XML.getValue<int>("//X"+ofToString(val_id));
                target_ys[val_id] = XML.getValue<int>("//Y"+ofToString(val_id));
                target_ws[val_id] = XML.getValue<int>("//W"+ofToString(val_id));
                target_hs[val_id] = XML.getValue<int>("//H"+ofToString(val_id));
            }
        }
    } else {
        for (int w=0; w<num_w; w++){
            for (int h=0; h<num_h; h++){
                int val_id = w*num_w+h;
                target_xs[val_id] = target_whole_x + w*target_w;
                target_ys[val_id] = target_whole_y + h*target_h;
                target_ws[val_id] = target_w;
                target_hs[val_id] = target_h;
            }
        }
    }


    colorImg_cam.allocate(cam_w,cam_h);

	//bLearnBakground = true;
	//threshold = 112;

}

//--------------------------------------------------------------
void ofApp::update(){
	ofBackground(100,100,100);

    bNewFrame = false;
    vidGrabber.update();
    bNewFrame = vidGrabber.isFrameNew();

}

//--------------------------------------------------------------
void ofApp::draw(){
    int draw_whole_x = 50;
    int draw_whole_y = 50;
    ofSetHexColor(0xffffff);
    colorImg_cam.draw(draw_whole_x, draw_whole_y);
    
    if (bNewFrame){
        
        // dicide(crop) colorImages for each lightface
        colorImgs.clear();
        imgs.clear();
        for (int w=0; w<num_w; w++){
            for (int h=0; h<num_h; h++){
                int val_id = w*num_w+h;
                int target_x = target_xs[val_id];
                int target_y = target_ys[val_id];
                int target_w = target_ws[val_id];
                int target_h = target_hs[val_id];
                colorImg.allocate(target_w,target_h);
                //grayImage.allocate(target_w,target_h);

                img.setFromPixels(vidGrabber.getPixels());
                img.crop(target_x, target_y, target_w, target_h);
                imgs.push_back(img);
                colorImg.setFromPixels(img.getPixels());
                //grayImage = colorImg;
                colorImgs.push_back(colorImg);
            }
        }

        // select particular hue(= selected hue)
        // count number of pixels for selected hue
        pxCounts.clear();
        selectedColorImgs.clear();
        bipolarizedImgs.clear();
        for (int val_id=0; val_id<val_size; val_id++){
            int pxCount = 0;
            int target_x = target_xs[val_id];
            int target_y = target_ys[val_id];
            int target_w = target_ws[val_id];
            int target_h = target_hs[val_id];
        
            selectedColorOFImg.allocate(target_w, target_h, OF_IMAGE_COLOR);
            selectedBipolarizedOFImg.allocate(target_w, target_h, OF_IMAGE_GRAYSCALE);
            selectedColorOFImg.setColor(ofColor::black);
            selectedBipolarizedOFImg.setColor(ofColor::black);

            //Red: 0 (wrapped round from 255)
            //Orange: 25
            //Yellow: 42
            //Green: 85
            //Blue: 170
            //Purple: 205
            //Red: 255 (wraps round to 0)
            int hueMin = 37;
            int hueMax = hueMin + 10;
            for (int img_w=0; img_w<target_w; img_w++){
                for (int img_h=0; img_h<target_h; img_h++){
                    float hue =        imgs[val_id].getColor(img_w, img_h).getHue();
                    float saturation = imgs[val_id].getColor(img_w, img_h).getSaturation();
                    float brightness = imgs[val_id].getColor(img_w, img_h).getBrightness();
                    float bipolarizedHue = 0;
                    float selectedHue = 0;
                    float selectedSaturation = 0;
                    float selectedBrightness = 0;
                    if(hueMax <= 255){
                        if(hue >= hueMin){
                            if(hue <= hueMax){
                                pxCount++;
                                bipolarizedHue = 255;
                                selectedHue = hue;
                                selectedSaturation = saturation;
                                selectedBrightness = brightness;
                            }
                        }
                    }else{
                        if(hue >= hueMin){
                            pxCount++;
                            bipolarizedHue = 255;
                            selectedHue = hue;
                            selectedSaturation = saturation;
                            selectedBrightness = brightness;
                        }else if(hue <= hueMax-256){
                            pxCount++;
                            bipolarizedHue = 255;
                            selectedHue = hue;
                            selectedSaturation = saturation;
                            selectedBrightness = brightness;
                        }
                    }
                    
                    selectedColorOFImg.setColor(img_w%target_w, img_h%target_h, ofColor::fromHsb(selectedHue, selectedSaturation, selectedBrightness) );
                    selectedBipolarizedOFImg.setColor(img_w%target_w, img_h%target_h, bipolarizedHue );
                }
            }
            
            pxCounts.push_back(pxCount);
            selectedColorImg.setFromPixels(selectedColorOFImg.getPixels());
            selectedBipolarizedImg.setFromPixels(selectedBipolarizedOFImg.getPixels());
            selectedColorImgs.push_back(selectedColorImg);
            bipolarizedImgs.push_back(selectedBipolarizedImg);

            //selectedColorImg.draw(draw_whole_x + target_xs[val_id], draw_whole_y + target_ys[val_id]);
        }
        
        // draw pixels for selected hue
        for (int val_id=0; val_id<val_size; val_id++){
            ofSetHexColor(0xffffff);
            draw_xs[val_id] = draw_whole_x + target_xs[val_id];
            draw_ys[val_id] = draw_whole_y + target_ys[val_id];
            //bipolarizedImgs[val_id].draw(draw_xs[val_id], draw_ys[val_id]);
            selectedColorImgs[val_id].draw(draw_xs[val_id], draw_ys[val_id]);
        }
        
        // calculate the brightness of lightface(=LfRectBrightness)
        for (int val_id=0; val_id<val_size; val_id++){
            int pxCount_max = target_ws[val_id]*target_hs[val_id];
            int pxCount_min = 0;
            int LfRectBrightness = 255*pxCounts[val_id]/(pxCount_max-pxCount_min);
            LfRectBrightnesss[val_id] = LfRectBrightness;
            printf("id %d   pxCount %d   LfRectBrightness %d \n", val_id, pxCounts[val_id], LfRectBrightness);
        }
        
        // draw values(pxCount, LfRectBrightness)
        for (int val_id=0; val_id<val_size; val_id++){
            ofSetHexColor(0xffffff);
            stringstream reportStr1;
            reportStr1 << pxCounts[val_id] << "_" << LfRectBrightnesss[val_id];
            ofDrawBitmapString(reportStr1.str(), draw_xs[val_id], draw_ys[val_id]+10);
        }

        // draw settings
        stringstream reportStr;
        //reportStr << "bg subtraction and blob detection" << endl
        //<< "press ' ' to capture bg" << endl
        //<< "threshold " << threshold << " (press: +/-)" << endl
        reportStr << "fps: " << ofGetFrameRate();
        ofDrawBitmapString(reportStr.str(), draw_whole_x, draw_whole_y+cam_h+50);
        
        // send(write) values to Arduino
        if (isPressed) {
            for (int i=0; i<val_size; i++){
                isValids[i] = false;
                int high = (LfRectBrightnesss[i] >> 7) & 127;
                int low  = LfRectBrightnesss[i] & 127;
                bool byteWasWritten1 = mySerial.writeByte(128+i);
                bool byteWasWritten2 = mySerial.writeByte(high);
                bool byteWasWritten3 = mySerial.writeByte(low);
                if ( byteWasWritten1 && byteWasWritten2 && byteWasWritten3 ) {
                    printf("           write LfRectBrightness %d \n", LfRectBrightnesss[i]);
                    isValids[i] = true;
                } else {
                    printf("an error occurred \n");
                }
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key){
        case 'g':
            isPressed = true;
            break;
		//case ' ':
			//bLearnBakground = true;
			//break;
		//case '+':
			//threshold ++;
			//if (threshold > 255) threshold = 255;
			//break;
		//case '-':
			//threshold --;
			//if (threshold < 0) threshold = 0;
			//break;
        case 'q':
            isBig = true;
            isSmall = false;
            break;
        case 'a':
            isBig = false;
            isSmall = false;
            break;
        case 'z':
            isBig = false;
            isSmall = true;
            break;
        case '0':
            target_id = 0;
            break;
        case '1':
            target_id = 1;
            break;
        case '2':
            target_id = 2;
            break;
        case '3':
            target_id = 3;
            break;
        case '4':
            target_id = 4;
            break;
        case '5':
            target_id = 5;
            break;
        case '6':
            target_id = 6;
            break;
        case '7':
            target_id = 7;
            break;
        case '8':
            target_id = 8;
            break;
	}

    if (isBig){
        if (key == OF_KEY_RIGHT){
            if( target_xs[target_id] + target_ws[target_id] < cam_w ){
                target_ws[target_id] += 1;
            }
        } else if (key == OF_KEY_LEFT){
            if( target_ws[target_id] < cam_w ){
                if( target_xs[target_id] > 0 ){
                    target_xs[target_id] -= 1;
                    target_ws[target_id] += 1;
                }
            }
        } else if (key == OF_KEY_DOWN){
            if( target_ys[target_id] + target_hs[target_id] < cam_h ){
                target_hs[target_id] += 1;
            }
        } else if (key == OF_KEY_UP){
            if( target_hs[target_id] < cam_h ){
                if( target_ys[target_id] > 0 ){
                    target_ys[target_id] -= 1;
                    target_hs[target_id] += 1;
                }
            }
        }
    } else if (isSmall) {
        if (key == OF_KEY_RIGHT){
            if( target_ws[target_id] > 1 ){
                target_xs[target_id] += 1;
                target_ws[target_id] -= 1;
            }
        } else if (key == OF_KEY_LEFT){
            if( target_ws[target_id] > 1 ){
                target_ws[target_id] -= 1;
            }
        } else if (key == OF_KEY_DOWN){
            if( target_hs[target_id] > 1 ){
                target_ys[target_id] += 1;
                target_hs[target_id] -= 1;
            }
        } else if (key == OF_KEY_UP){
            if( target_hs[target_id] > 1 ){
                target_hs[target_id] -= 1;
            }
        }
    } else {
        if (key == OF_KEY_RIGHT){
            if( target_xs[target_id] + target_ws[target_id] < cam_w ){
                target_xs[target_id] += 1;
            }
        } else if (key == OF_KEY_LEFT){
            if( target_xs[target_id] > 0 ){
                target_xs[target_id] -= 1;
            }
        } else if (key == OF_KEY_DOWN){
            if( target_ys[target_id] + target_hs[target_id] < cam_h ){
                target_ys[target_id] += 1;
            }
        } else if (key == OF_KEY_UP){
            if( target_ys[target_id] > 0 ){
                target_ys[target_id] -= 1;
            }
        }
    }

    if (key=='S'){
        XML.clear();
        XML.addChild("XYWH");
        XML.setTo("//XYWH"); // set back to the root node
        for (int w=0; w<num_w; w++){
            for (int h=0; h<num_h; h++){
                int val_id = w*num_w+h;
                XML.addValue("X"+ofToString(val_id), ofToString(target_xs[val_id]));
                XML.addValue("Y"+ofToString(val_id), ofToString(target_ys[val_id]));
                XML.addValue("W"+ofToString(val_id), ofToString(target_ws[val_id]));
                XML.addValue("H"+ofToString(val_id), ofToString(target_hs[val_id]));
            }
        }
        XML.save("mySettings.xml");
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
