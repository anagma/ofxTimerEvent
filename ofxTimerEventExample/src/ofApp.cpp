#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(0);
	ofSetVerticalSync(true);
	ofSetFrameRate(60);

	lastKeyframe = "";
	
	ofSetLogLevel(OF_LOG_VERBOSE);
	timer.addKeyFrame(0,	"timer0");
	timer.addKeyFrame(100,	"timer1");
	timer.addKeyFrame(1000, "timer2");
	timer.addKeyFrame(2000, "timer3");
	timer.setDuration(5000);
	timer.setLoop(true);
	timer.start();
	
	ofAddListener(timer.keyframe, this, &ofApp::onKeyframe);
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
	long long now = timer.getNow();

	ofSetColor(255);
	ofDrawBitmapString(ofToString(now), 100, 100);
	ofDrawBitmapString("last keyframe: "+lastKeyframe, 100, 150);
	ofDrawBitmapString("loop count: "+ ofToString(timer.getLoopCount()), 100, 200);
	
	float scaling = ofGetWidth() / (float)timer.getDuration() * 0.8;

	glPushMatrix();
	{
		glTranslated(ofGetWidth()*0.1, ofGetHeight()/2, 0);
		ofSetColor(255);
		ofDrawLine(0, 0, ofGetWidth()*0.8, 0);
		ofxTimerEventKeyframes<string> keyframes = timer.getKeyframes();
		for (auto & kv : keyframes) {
			ofDrawLine((float)kv.first*scaling, -20, (float)kv.first*scaling, 0);
			ofDrawBitmapString(kv.second, (float)kv.first*scaling+5, -5);
		}
		
		ofSetColor(255, 0, 0);
		ofDrawLine((float)now*scaling, -20, (float)now*scaling, 20);
	}
	glPopMatrix();
	
}

//--------------------------------------------------------------
void ofApp::onKeyframe(string & name){
	ofLogNotice() << "keyframe reached:" << name;
	lastKeyframe = name;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if(x > ofGetWidth()*0.1 && x < ofGetWidth()*0.8){
		float pct = ofMap(x, ofGetWidth()*0.1, ofGetWidth()*0.9, 0.f, 1.f);
		float now = pct*(float)timer.getDuration();
		timer.seek(now);
	}
}