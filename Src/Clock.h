#include "ExternalDependencies.h"

/*
	PLATFORM SPECIFIC. NOT RELATED TO ANY OTHER CODE AND CAN BE RE-WRITTEN AS LONG AS CORE FUNCTIONS PRODUCE THE SAME OUTPUT
*/

class Clock {
private:
	SYSTEMTIME currentTime, lastSavedTime;
	double currentFramerate;
	int millisecondsPerFrame;

public:
	Clock() {
		GetSystemTime(&currentTime);
		GetSystemTime(&lastSavedTime);
	}
	Clock(int f) {
		GetSystemTime(&currentTime);
		GetSystemTime(&lastSavedTime);
		setFramerate(f);
	}
	~Clock()
	{}

	bool timePassedSinceLastWait() {
		GetSystemTime(&currentTime);
		auto intTime = [](SYSTEMTIME t) {return static_cast<int>(t.wSecond * 1000 + t.wMilliseconds); };	// needs to account for all time, not just milli and sec

		if (intTime(currentTime) >= (intTime(lastSavedTime) + millisecondsPerFrame) || currentTime.wMinute != lastSavedTime.wMinute) {
			GetSystemTime(&lastSavedTime);
			return 1;
		}
		else {
			return 0;
		}
	}

	void setFramerate(int f) {
		currentFramerate = f;
		millisecondsPerFrame = floor(1000.0f / currentFramerate);
	}
	int getFramerate() {
		return currentFramerate;
	}
};