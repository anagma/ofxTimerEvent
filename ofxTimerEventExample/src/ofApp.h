#pragma once

#include "ofMain.h"
#include "ofxTimerEvent.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
	
		void mousePressed(int x, int y, int button);

	void onKeyframe(string & name);
	
	ofxTimerEvent<string> timer;
	
	string lastKeyframe;
};
