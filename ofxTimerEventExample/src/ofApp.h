#pragma once

#include "ofMain.h"
#include "ofxTimerEvent.h"

#define TIMER_EVENT_1 "timer1"
#define TIMER_EVENT_2 "timer2"

#define TIMER_EVENT_1_TIME 1000
#define TIMER_EVENT_2_TIME 3000

#define TIMER_EVENT_LOOP_DURATION 5000

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

	void onKeyframe(string & name);
	
	ofxTimerEvent timer;
};
