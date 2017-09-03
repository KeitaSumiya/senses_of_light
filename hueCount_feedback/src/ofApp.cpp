#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    vidGrabber.setVerbose(true);
    vidGrabber.setDeviceID(0);
    vidGrabber.initGrabber(cam_coe*320,cam_coe*240);
    cam_w = vidGrabber.getWidth();
    cam_h = vidGrabber.getHeight();
    colorImg_cam.allocate(cam_w,cam_h);
    printf("cam_w %d   cam_h %d \n", cam_w, cam_h );
    
    ofSetFrameRate(1);
    mySerial.setup("/dev/cu.usbmodem1421",9600);
    
    // decide position of lf-rectangle in image from webcam
    //target_whole_w = cam_coe*target_whole_base;
    //target_whole_h = cam_coe*target_whole_base;
    target_whole_w = target_whole_size;
    target_whole_h = target_whole_size;
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
                int id_lf = w*num_w + h;
                target_xs[id_lf] = XML.getValue<int>("//X"+ofToString(id_lf));
                target_ys[id_lf] = XML.getValue<int>("//Y"+ofToString(id_lf));
                target_ws[id_lf] = XML.getValue<int>("//W"+ofToString(id_lf));
                target_hs[id_lf] = XML.getValue<int>("//H"+ofToString(id_lf));
            }
        }
    } else {
        for (int w=0; w<num_w; w++){
            for (int h=0; h<num_h; h++){
                int id_lf = w*num_w + h;
                target_xs[id_lf] = target_whole_x + w*target_w;
                target_ys[id_lf] = target_whole_y + h*target_h;
                target_ws[id_lf] = target_w;
                target_hs[id_lf] = target_h;
            }
        }
    }

    
    for(int w=0; w<num_w; w++){
        for(int h=0; h<num_h; h++){
            int x = draw_simul_x + (size+gap)*w;
            int y = draw_whole_y + (size+gap)*h + gap*5;
            lfRect[w*3+h].setup(ofVec2f(x,y), size, 10);
        }
    }

    //LF default status setup
    for(int id_lf=0; id_lf<num_lf; id_lf++){
        lfRect[id_lf].status = 0;
        if(lfRect[id_lf].status == 0){
            bStatus[id_lf] = 3;
        }
    }
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
    ofSetHexColor(0xffffff);
    colorImg_cam.draw(draw_whole_x, draw_whole_y);
    
    if (bNewFrame){
        
        // decide(crop) colorImages for each lightface
        colorImgs.clear();
        imgs.clear();
        for (int w=0; w<num_w; w++){
            for (int h=0; h<num_h; h++){
                int id_lf = w*num_w + h;
                int target_x = target_xs[id_lf];
                int target_y = target_ys[id_lf];
                int target_w = target_ws[id_lf];
                int target_h = target_hs[id_lf];
                colorImg.allocate(target_w,target_h);

                img.setFromPixels(vidGrabber.getPixels());
                img.crop(target_x, target_y, target_w, target_h);
                imgs.push_back(img);
                colorImg.setFromPixels(img.getPixels());
                colorImgs.push_back(colorImg);
            }
        }

        // select particular hue(= selected hue)
        // count number of pixels for selected hue
        pxCounts.clear();
        selectedColorImgs.clear();
        bipolarizedImgs.clear();
        for (int id_lf=0; id_lf<num_lf; id_lf++){
            int pxCount = 0;
            int target_x = target_xs[id_lf];
            int target_y = target_ys[id_lf];
            int target_w = target_ws[id_lf];
            int target_h = target_hs[id_lf];
        
            selectedColorOFImg.allocate(target_w, target_h, OF_IMAGE_COLOR);
            selectedBipolarizedOFImg.allocate(target_w, target_h, OF_IMAGE_GRAYSCALE);
            selectedColorOFImg.setColor(ofColor::black);
            selectedBipolarizedOFImg.setColor(ofColor::black);

            //  0: Red(wrapped round from 255)
            // 25: Orange
            // 42: Yellow
            // 85: Green
            //170: Blue
            //205: Purple
            //255: Red (wraps round to 0)
            int hueMin = 0;
            int hueMax = hueMin + 255;
            for (int img_w=0; img_w<target_w; img_w++){
                for (int img_h=0; img_h<target_h; img_h++){
                    float hue =        imgs[id_lf].getColor(img_w, img_h).getHue();
                    float saturation = imgs[id_lf].getColor(img_w, img_h).getSaturation();
                    float brightness = imgs[id_lf].getColor(img_w, img_h).getBrightness();
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
        }
        
        // draw pixels for selected hue
        for (int id_lf=0; id_lf<num_lf; id_lf++){
            ofSetHexColor(0xffffff);
            draw_xs[id_lf] = draw_whole_x + target_xs[id_lf];
            draw_ys[id_lf] = draw_whole_y + target_ys[id_lf];
            //bipolarizedImgs[id_lf].draw(draw_xs[id_lf], draw_ys[id_lf]);
            selectedColorImgs[id_lf].draw(draw_xs[id_lf], draw_ys[id_lf]);
        }
        
        //LF rect control flow
        threshold_bNum = 5;
        
        ofSetColor(0, 255, 0);
        int siml_x = draw_simul_x         - gap;
        int siml_y = draw_whole_y + gap*5 - gap;
        int siml_w = (size+gap)*num_w     + gap;
        int siml_h = (size+gap)*num_h     + gap;
        ofDrawRectangle(siml_x, siml_y, siml_w, siml_h);

        ofSetHexColor(0xffffff);

        for(int id_lf=0; id_lf<num_lf; id_lf++){
            printf("%d \n", lfRect[id_lf].status);
            if(lfRect[id_lf].status==3){
                if(pxCounts[id_lf] >= threshold_bNum){
                    lfRect[id_lf].status = 1;
                }
            }
            
            else if(lfRect[id_lf].status==0 && lfRect[id_lf].status!=bStatus[id_lf]){
                minRectNum.push_back(id_lf);
            }
            bStatus[id_lf] = lfRect[id_lf].status;
        }
        
        if(minRectNum.size() >= threshold_mNum){
            lfRect[minRectNum[0]].status = 2;
            minRectNum.erase(minRectNum.begin());
        }
        
        //LF rect update
        for(int id_lf=0; id_lf<num_lf; id_lf++){
            lfRect[id_lf].update();
        }
        
        // get the brightness of lightface
        for (int id_lf=0; id_lf<num_lf; id_lf++){
            LfRectBrightnesss[id_lf] = lfRect[id_lf].getBrightness();
            printf("id %d   pxCount %d   lfBrightness %d \n", id_lf, pxCounts[id_lf], LfRectBrightnesss[id_lf]);
        }
        
        
        // draw values(pxCount, LfRectBrightness)
        for (int id_lf=0; id_lf<num_lf; id_lf++){
            ofSetHexColor(0xffffff);
            stringstream reportStr1;
            reportStr1 << pxCounts[id_lf] << "_" << LfRectBrightnesss[id_lf];
            ofDrawBitmapString(reportStr1.str(), draw_xs[id_lf], draw_ys[id_lf]+10);
        }

        // draw settings
        stringstream reportStr;
        reportStr << "fps: " << ofGetFrameRate();
        ofDrawBitmapString(reportStr.str(), draw_whole_x, draw_whole_y+cam_h+50);
        
        // send(write) values to Arduino
        if (isPressed) {
            for (int id_lf=0; id_lf<num_lf; id_lf++){
                isValids[id_lf] = false;
                int high = (LfRectBrightnesss[id_lf] >> 7) & 127;
                int low  = LfRectBrightnesss[id_lf] & 127;
                bool byteWasWritten1 = mySerial.writeByte(128+id_lf);
                bool byteWasWritten2 = mySerial.writeByte(high);
                bool byteWasWritten3 = mySerial.writeByte(low);
                if ( byteWasWritten1 && byteWasWritten2 && byteWasWritten3 ) {
                    printf("           write lfBrightness %d \n", LfRectBrightnesss[id_lf]);
                    isValids[id_lf] = true;
                } else {
                    printf("an error occurred \n");
                }
            }
        }
    }

    //LF rect draw
    for(int id_lf=0; id_lf<num_lf; id_lf++){
        lfRect[id_lf].draw();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key){
        case 'g':
            isPressed = true;
            break;
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
                int id_lf = w*num_w+h;
                XML.addValue("X"+ofToString(id_lf), ofToString(target_xs[id_lf]));
                XML.addValue("Y"+ofToString(id_lf), ofToString(target_ys[id_lf]));
                XML.addValue("W"+ofToString(id_lf), ofToString(target_ws[id_lf]));
                XML.addValue("H"+ofToString(id_lf), ofToString(target_hs[id_lf]));
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
