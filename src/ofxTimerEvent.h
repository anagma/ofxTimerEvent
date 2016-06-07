//
//  ofxTimeEvent.h
//  toposcan_4screen
//
//  Created by FURUDATE Ken on 2/21/15.
//
//

#ifndef ofxTimeEvent_h
#define ofxTimeEvent_h

#include "ofMain.h"

#define OFX_TIMER_EVENT_TIME_RESOLUTION_DEFAULT 1

template<class EventArg = string>
using ofxTimerEventKeyframes = multimap<long long, EventArg>;

template<class EventArg = string>
using ofxTimerEventKeyframesEntry = typename ofxTimerEventKeyframes<EventArg>::value_type;

template<class EventArg = string>
using ofxTimerEventKeyframesIterator = typename ofxTimerEventKeyframes<EventArg>::iterator;


template<class EventArg = string>
class ofxTimerEvent : ofThread{
	
public:
	
	ofEvent<EventArg> keyframe;
	ofEvent<void> loop;
	ofEvent<void> finish;

	ofxTimerEvent(){
		// default
		timeResolution = OFX_TIMER_EVENT_TIME_RESOLUTION_DEFAULT;
		duration = -1;
		bLoop = false;
		startTimeMillis = -1;
		lastUpdatedTime = -1;
		bDurationSetManually = false;
	}
	~ofxTimerEvent(){
		stop();
		waitForThread();
	}
	
	void start(long long startOffset=0){
		ofLogVerbose("ofxTimerEvent") << "start timer from: " << startOffset << endl;
		if(keyframes.size() <= 0){
			ofLogError("ofxTimerEvent") << "keyframes is empty. stop";
		}
		long long elapsedTimeMillis = ofGetElapsedTimeMillis();
		
		startTimeMillis	= elapsedTimeMillis-startOffset;
		lastUpdatedTime	= startOffset;

		if(! isThreadRunning()){
			startThread();
		}
	}
	void seek(long long time){
		start(time);
	}
	void stop(){
		stopThread();
	}
	long long getNow(){
		if(isRunning()){
			return (ofGetElapsedTimeMillis()-startTimeMillis)%getDuration();
		}
		return 0;
	}
	int getLoopCount(){
		if(isRunning()){
			return floor((ofGetElapsedTimeMillis()-startTimeMillis)/getDuration());
		}
		return 0;
	}
	void addKeyFrame(long long time, EventArg & arg){
		keyframes.insert(make_pair(time, arg));
	}
	void addKeyFrame(long long time, const EventArg & arg){
		keyframes.insert(make_pair(time, arg));
	}
	void removeKeyFrame(long long timeFrom, long long timeTo){
		auto itLow = keyframes.lower_bound(timeFrom);
		auto itUp  = keyframes.upper_bound(timeTo);
		keyframes.erase(itLow, itUp);
	}
	
	void clear(){
		stopThread();
		keyframes.clear();
		if(! bDurationSetManually){
			duration = -1;
		}
	}

	
	bool isRunning(){
		return isThreadRunning();
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
		if(_duration > 0){
			duration = _duration;
			bDurationSetManually = true;
		}else{
			duration = -1;
			bDurationSetManually = false;
		}
	}
	long long getDuration(){
		if(bDurationSetManually){
			return duration;
		}
		
		// get last keyframe
		return getFinalKeyframe()->first;;
	}
	void setDurationAuto(){
		setDuration(-1);
	}
	
	// loop
	void setLoop(bool _loop){
		bLoop = _loop;
	}
	bool isLoop(){
		return bLoop;
	}
	
	EventArg * getLastKeyframe(){
		if(! isRunning()){
			return NULL;
		}
		long long now = getNow();
		
		auto nearest = keyframes.upper_bound(now);
		if(nearest == keyframes.begin()){
			if(bLoop && getLoopCount() > 0){
				return getFinalKeyframe()->second;
			}else{
				return NULL;
			}
		}
		--nearest;
		return nearest->second;
	}
	
	EventArg * getNextKeyframe(){
		if(! isRunning()){
			return NULL;
		}
		long long now = getNow();
		
		auto nearest = keyframes.upper_bound(now);
		if(nearest == keyframes.end()){
			if(bLoop){
				return keyframes.begin()->second;
			}else{
				return NULL;
			}
		}
		return nearest->second;
	}
	
	ofxTimerEventKeyframes<EventArg> & getKeyframes(){
		return keyframes;
	}
	
protected:
	
	long long startTimeMillis;
	ofxTimerEventKeyframes<EventArg> keyframes;

	long long duration;
	
	long long lastUpdatedTime;
	int timeResolution;
	
	bool bLoop;
	bool bDurationSetManually;

	void threadedFunction(){
		
		while(isThreadRunning()){
			long long now = getNow();
			if(now > 0){
				if(bLoop && now < lastUpdatedTime){
					// loop!!
					ofLogNotice("ofxTimerEventThread") << "Timer looped";
					ofNotifyEvent(loop, this);

					// trigger rest events
					for(auto it = keyframes.upper_bound(lastUpdatedTime); it != keyframes.end(); it++){
						triggetEvent(it->second);
					}
					lastUpdatedTime = -1;
				}
				
				auto itLow = keyframes.upper_bound(lastUpdatedTime);
				auto itUp = keyframes.upper_bound(now);
				
				if(itLow != keyframes.end()){
					for(auto it=itLow; it!=itUp; it++){
						triggetEvent(it->second);
					}
				}
				
				
				lastUpdatedTime = now;
				
			}
			
			if(! bLoop && now > getDuration()){
				ofLogNotice("ofxTimerEventThread") << "Reached to final keyframe. stop thread";
				ofNotifyEvent(finish, this);
				break;
			}
			ofSleepMillis(timeResolution);
		}
	}
	
	ofxTimerEventKeyframesEntry<EventArg> * getFinalKeyframe(){
		if(keyframes.empty()){
			return NULL;
		}
		ofxTimerEventKeyframesEntry<EventArg> & entry = (*keyframes.rbegin());
		return & entry;
	}
	
	void triggetEvent(EventArg & arg){
		ofNotifyEvent(keyframe, arg, this);
	}
};

#endif
