#ifndef LfRect_hpp
#define LfRect_hpp
#include "ofMain.h"
#include <stdio.h>
#endif /* LfRect_hpp */

class LfRect{
private:
    int size;
    ofVec2f pos;
    int gradationSpeed;
    
public:
    LfRect();
    void setup(ofVec2f _pos, int _size, int _gradationSpeed);
    void setStatus(int _status);
    void update();
    void draw();
    void updateGradationSpeed(int val);
    int getBrightness();
    int status;
    int brightness;
    string statusStr;
};
