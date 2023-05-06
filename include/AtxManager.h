#ifndef _ATX_CONTROLLER_H
#define _ATX_CONTROLLER_H

#include <Arduino.h>

#define PIN_PS_ON 4
#define PIN_PWR_BTN 5
#define PIN_PWR_LED 6
#define PIN_SOFT_OFF 7
#define PIN_PWR_OK 8

#define PWR_OFF_DELAY 5000
#define BTN_INTERVAL 500
#define SOFT_OFF_THRESHOLD 1500
#define SOFT_OFF_DELAY 500

enum class SystemState : uint8_t {
	OFF = 0,
	INIT = 1,
	ON = 2
};

class AtxControllerClass {
public:
	AtxControllerClass();
	void begin();
	void loop();
	SystemState getState();
	void onPowerOn(void (*systemPowerOn)());
	void onPowerOff(void (*systemPowerOff)());
	void onPowerInit(void (*systemPowerInit)());
	bool buttonWasDown();
	void setButtonWasDown(bool wasDown);
	void enableSoftOffDetection();
	void disableSoftOffDetection();

private:
	void doSoftOffDetection();
	bool isSystemBooted();

	int _lastPowerOkState;
	bool _initialized;
	volatile int _timeDown;
	volatile bool _shutdownDetection;
	volatile SystemState _currentState;
	volatile SystemState _lastState;
	volatile bool _buttonWasDown;

	void (*systemPowerOn)();
	void (*systemPowerOff)();
	void (*systemPowerInit)();
};

extern AtxControllerClass AtxController;
#endif