//
//  ofxTimeEvent.h
//  toposcan_4screen
//
//  Created by FURUDATE Ken on 2/21/15.
//
//

#ifndef toposcan_4screen_ofxTimeEvent_h
#define toposcan_4screen_ofxTimeEvent_h

#include "ofMain.h"

class ofxTimerEvent : ofThread{
	
	long long startTimeMillis;
	map<string, long long> keyframes;
	long long duration;
	
	long long lastUpdatedTime;
	int timeResolution;
	int repeatCount;
	
public:
	
	ofEvent<string> keyframe;
	
	void start(long long _duration=-1, long long startOffset=0, int _timeResolution=10){
		if(_duration >= 0){
			duration = _duration;
		}
		timeResolution = _timeResolution;
		startTimeMillis = ofGetElapsedTimeMillis()-startOffset;
		lastUpdatedTime = 0;
		repeatCount = 0;
		getPocoThread().setName("ofxTimerEventThread");
		startThread(true);
	}
	void stop(){
		stopThread();
	}
	void addKeyFrame(string name, long long time){
		keyframes.insert(make_pair(name, time));
	}
	void removeKeyFrame(string name){
		map<string, long long>::iterator it = keyframes.find(name);
		if(it != keyframes.end()){
			keyframes.erase(it);
		}
	}
	void setDuration(int _duration){
		duration = _duration;
	}
	
protected:
	void threadedFunction(){
		while(isThreadRunning()){
			if(lock()){
				long long now = (ofGetElapsedTimeMillis()-startTimeMillis);
				if(duration >= 0){
					int newRepeatCount = now/duration;
					if(repeatCount != newRepeatCount){
						repeatCount = newRepeatCount;
						lastUpdatedTime -= duration;
					}
					now %= duration;
				}
				for(auto & k : keyframes){
					if(k.second > lastUpdatedTime && k.second < now){
						string arg = k.first;
						ofNotifyEvent(keyframe, arg, this);
						break;
					}
				}
				lastUpdatedTime = now;
				unlock();
			}
			ofSleepMillis(timeResolution);
		}
	}
};

#endif
