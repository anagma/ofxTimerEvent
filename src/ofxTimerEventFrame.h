//
//  ofxTimerEventFrame.h
//  voyageTest
//
//  Created by FURUDATE Ken on 6/28/15.
//
//

#ifndef voyageTest_ofxTimerEventFrame_h
#define voyageTest_ofxTimerEventFrame_h

#include "ofMain.h"

class ofxTimerEventFrame{
	
	long long startTimeMillis;
	multimap<string, long long> keyframes;
	long long duration;

	bool bRunning;
	
	long long lastUpdatedTime;
//	int timeResolution;
	int repeatCount;
	
	bool bLoop;
	bool bDurationSetManually;
	
public:
	
	ofEvent<string> keyframe;
	ofEvent<void> loop;
	ofEvent<void> finish;
	
	ofxTimerEventFrame(){
		bRunning = false;
		// default
//		timeResolution = OFX_TIMER_EVENT_TIME_RESOLUTION_DEFAULT;
		duration = -1;
		bLoop = false;
		startTimeMillis = -1;
		lastUpdatedTime = -1;
		repeatCount = 0;
		bDurationSetManually = false;
	}
	
	void start(long long startOffset=0){
		bRunning = true;
		if(keyframes.size() <= 0){
			ofLogError("ofxTimerEvent") << "keyframes is empty. stop";
		}
		
		
		if(! bDurationSetManually){
			duration = getFinalKeyframeTime();
			ofLogNotice("ofxTimerEvent") << "duration is not set. set it automatic" << duration;
		}
		
		long long elapsedTimeMillis = ofGetElapsedTimeMillis();
		
		startTimeMillis	= elapsedTimeMillis-startOffset;
		lastUpdatedTime	= elapsedTimeMillis-startTimeMillis;
		repeatCount		= lastUpdatedTime/duration;
		
//		if(! isThreadRunning()){
//			//			stopThread();
//			startThread(true);
//		}
//		//		ofSleepMillis(1000);
//		getPocoThread().setName("ofxTimerEventThread");
	}
	void seek(long long time){
		start(time);
	}
	void stop(){
		bRunning = false;
//		stopThread();
	}
	long long getNow(){
		return ofGetElapsedTimeMillis()-startTimeMillis;
	}
	void addKeyFrame(string name, long long time){
		keyframes.insert(make_pair(name, time));
		if(! bDurationSetManually){
			duration = getFinalKeyframeTime();
		}
	}
	void removeKeyFrame(string name){
		map<string, long long>::iterator it = keyframes.find(name);
		if(it != keyframes.end()){
			keyframes.erase(it);
			if(! bDurationSetManually){
				duration = getFinalKeyframeTime();
			}
		}
	}
	
	bool isRunning(){
		return bRunning;
//		return isThreadRunning();
	}
	
	void clear(){
		bRunning = false;
//		stopThread();
		keyframes.clear();
		if(! bDurationSetManually){
			duration = -1;
		}
		
	}
	
	// resolution
//	void setTimeResolution(int _res){
//		timeResolution = _res;
//	}
//	int getTimeResolution(){
//		return timeResolution;
//	}
	
	// duration
	void setDuration(long long _duration){
		duration = _duration;
		if(_duration >= 0){
			bDurationSetManually = true;
		}else{
			bDurationSetManually = false;
		}
	}
	long long getDuration(){
		if(bDurationSetManually){
			return duration;
		}
		
		// get last keyframe
		duration = getFinalKeyframeTime();
		return duration;
	}
	
	// loop
	void setLoop(bool _loop){
		ofLogVerbose("ofxTimerEventFrame") << "set loop to: " << _loop;
		bLoop = _loop;
		if(! bLoop){
			ofLogVerbose("ofxTimerEventFrame") << "start time truncate from: " << startTimeMillis;
			while(getNow() > duration){
				startTimeMillis += duration;
			}
			ofLogVerbose("ofxTimerEventFrame") << "start time truncated to: " << startTimeMillis;
			ofLogVerbose("ofxTimerEventFrame") << "now: " << getNow();
		}
	}
	bool isLoop(){
		return bLoop;
	}
	
	string getLastKeyframe(){
		if(! isRunning()){
			return "";
		}
		long long now = getNow();
		if(!bLoop && now > duration){
			return "";
		}
		now %= duration;
		pair<string, long long> nearest = make_pair("", -1);
		bool bFirst = true;
		for(auto & k : keyframes){
			if (k.second < now) {
				if(bFirst){
					bFirst = false;
					nearest = k;
				}else{
					if(now-k.second < now-nearest.second){
						nearest = k;
					}
				}
			}
		}
		return nearest.first;
	}
	
	
	
	void update(){
		if(bRunning){
			long long now = getNow();
			if(now > 0){
				if(bLoop){
					int newRepeatCount = now/duration;
					if(repeatCount != newRepeatCount){
						repeatCount = newRepeatCount;
						if(repeatCount > 0){
							
							// trigger all rest event
							for(auto & k : keyframes){
								if(k.second >= lastUpdatedTime && k.second <= duration){
									string arg = k.first;
									ofNotifyEvent(keyframe, arg, this);
								}
							}
							ofLogNotice("ofxTimerEventThread") << "Timer looped: " << repeatCount << ", " << now;
							ofNotifyEvent(loop, this);
						}
						lastUpdatedTime = 0;
					}
					now %= duration;
				}
				
				for(auto & k : keyframes){
					if(k.second >= lastUpdatedTime && k.second < now){
						string arg = k.first;
						ofNotifyEvent(keyframe, arg, this);
					}
				}
				
				lastUpdatedTime = now;
				
			}
			
			if(! bLoop && now > duration){
				ofLogNotice("ofxTimerEventThread") << "Reached to final keyframe. stop thread";
				ofNotifyEvent(finish, this);
				bRunning = false;
			}
		}
	}
	
protected:
	long long getFinalKeyframeTime(){
		long long time = 0;
		for(auto & k : keyframes){
			if(k.second >= time){
				time = k.second;
			}
		}
		return time;
	}
};

#endif
