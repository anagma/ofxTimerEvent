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

#define OFX_TIMER_EVENT_TIME_RESOLUTION_DEFAULT 5

class ofxTimerEvent : ofThread{
	
	long long startTimeMillis;
	map<string, long long> keyframes;
	long long duration;
	
	long long lastUpdatedTime;
	int timeResolution;
	int repeatCount;
	
	bool bLoop;
	bool bDurationSetManually;
	
public:
	
	ofEvent<string> keyframe;
	ofEvent<void> loop;
	ofEvent<void> finish;
	
	ofxTimerEvent(){
		// default
		timeResolution = OFX_TIMER_EVENT_TIME_RESOLUTION_DEFAULT;
		duration = -1;
		bLoop = false;
		startTimeMillis = -1;
		lastUpdatedTime = -1;
		repeatCount = 0;
		bDurationSetManually = false;
	}
	
	void start(long long startOffset=0){
		if(keyframes.size() <= 0){
			ofLogError("ofxTimerEvent") << "keyframes is empty. stop";
		}
		
		
		if(! bDurationSetManually){
			duration = getFinalKeyframeTime();
		}
		
		long long elapsedTimeMillis = ofGetElapsedTimeMillis();
		
		startTimeMillis	= elapsedTimeMillis-startOffset;
		lastUpdatedTime	= elapsedTimeMillis-startTimeMillis;
		repeatCount		= lastUpdatedTime/duration;

		if(! isThreadRunning()){
//			stopThread();
			startThread(true);
		}
//		ofSleepMillis(1000);
		getPocoThread().setName("ofxTimerEventThread");
	}
	void seek(long long time){
		start(time);
	}
	void stop(){
		stopThread();
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
		return isThreadRunning();
	}
	
	void clear(){
		stopThread();
		keyframes.clear();
		if(! bDurationSetManually){
			duration = -1;
		}
		
	}
	
	// resolution
	void setTimeResolution(int _res){
		timeResolution = _res;
	}
	int getTimeResolution(){
		return timeResolution;
	}
	
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
		bLoop = _loop;
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
	
	
protected:
	void threadedFunction(){
		while(isThreadRunning()){
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
				break;
			}
			ofSleepMillis(timeResolution);
		}
	}
	
	long long getFinalKeyframeTime(){
		long long time = 0;
		for(auto & k : keyframes){
			if(k.second >= duration){
				time = k.second;
			}
		}
		return time;
	}
};

#endif
