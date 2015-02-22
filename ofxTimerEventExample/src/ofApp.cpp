#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_NOTICE);
	timer.addKeyFrame(TIMER_EVENT_1, TIMER_EVENT_1_TIME);
	timer.addKeyFrame(TIMER_EVENT_2, TIMER_EVENT_2_TIME);
	timer.start(TIMER_EVENT_LOOP_DURATION, -5000);
	
	ofAddListener(timer.keyframe, this, &ofApp::onKeyframe);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::onKeyframe(string & name){
	ofLogNotice() << "keyframe reached:" << name;
}