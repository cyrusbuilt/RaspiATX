#include "AtxManager.h"
#include "ButtonEvent.h"

AtxControllerClass::AtxControllerClass() {
	_initialized = false;
	_shutdownDetection = false;
}

void onPwrButtonDown(ButtonInformation *sender) {
	Serial.println(F("DEBUG: Power button down!"));
	AtxController.setButtonWasDown(true);
	delay(100);
}

void onPwrButtonUp(ButtonInformation *sender) {
	Serial.println(F("DEBUG: Power button up!"));
	if (AtxController.getState() == SystemState::OFF
		&& AtxController.buttonWasDown()) {
		Serial.println(F("DEBUG: Powering up!"));
		digitalWrite(PIN_PS_ON, LOW);
	}

	AtxController.setButtonWasDown(false);
}

void onPwrButtonHold(ButtonInformation *sender) {
	Serial.println(F("DEBUG: Button hold!"));
	if (AtxController.getState() != SystemState::OFF) {
		Serial.println(F("DEBUG: Dropping power!"));
		digitalWrite(PIN_PS_ON, HIGH);
	}
}

void AtxControllerClass::begin() {
	_currentState = SystemState::OFF;
	_lastState = SystemState::OFF;
	_lastPowerOkState = LOW;
	_buttonWasDown = false;

	pinMode(PIN_PS_ON, OUTPUT);
	digitalWrite(PIN_PS_ON, HIGH);

	pinMode(PIN_SOFT_OFF, INPUT);

	ButtonEvent.addButton(PIN_PWR_BTN, onPwrButtonDown, onPwrButtonUp, onPwrButtonHold, PWR_OFF_DELAY, NULL, BTN_INTERVAL);

	_initialized = true;
}

bool AtxControllerClass::isSystemBooted() {
	return (_currentState != SystemState::OFF && digitalRead(PIN_SOFT_OFF) == HIGH);
}

void AtxControllerClass::doSoftOffDetection() {
	if (!_shutdownDetection) {
		return;
	}

	bool systemBooted = isSystemBooted();
	if ((_currentState == SystemState::OFF && !systemBooted)
		|| (_currentState != SystemState::OFF && systemBooted)) {
		_timeDown = 0;
		return;
	}

	if (!systemBooted && _currentState != SystemState::OFF && _timeDown < SOFT_OFF_THRESHOLD) {
		delay(SOFT_OFF_DELAY);
		_timeDown += SOFT_OFF_DELAY;
		return;
	}

	if (!systemBooted && _currentState != SystemState::OFF && _timeDown >= SOFT_OFF_THRESHOLD) {
		Serial.println(F("DEBUG: *** Raspberry Pi no longer running! Powering off! ***"));
		digitalWrite(PIN_PS_ON, HIGH);
		disableSoftOffDetection();
		setButtonWasDown(false);
		_currentState = SystemState::OFF;
		if (systemPowerOff != NULL) {
			systemPowerOff();
		}
	}
}

void AtxControllerClass::loop() {
	if (!_initialized) {
		return;
	}

	ButtonEvent.loop();

	int pwrOkState = digitalRead(PIN_PWR_OK);
	if (digitalRead(PIN_PS_ON) == LOW && pwrOkState == LOW) {
		_currentState = SystemState::OFF;
	}

	if (pwrOkState != _lastPowerOkState) {
		Serial.print(F("DEBUG: PWR_OK state changed: "));
		Serial.println(pwrOkState);
		if (pwrOkState == HIGH && _currentState != SystemState::ON) {
			_currentState = SystemState::ON;
		}
		else if (pwrOkState == LOW && _currentState == SystemState::ON) {
			_currentState = SystemState::OFF;
		}
	}

	_lastPowerOkState = pwrOkState;
	if (_currentState != _lastState) {
		Serial.print(F("DEBUG: System state changed: "));
		Serial.println((uint8_t)_currentState);
		switch (_currentState) {
			case SystemState::ON:
				if (systemPowerOn != NULL) {
					systemPowerOn();
				}
				break;
			case SystemState::OFF:
				setButtonWasDown(false);
				disableSoftOffDetection();
				if (systemPowerOff != NULL) {
					systemPowerOff();
				}
				delay(1000);
				break;
			case SystemState::INIT:
				if (systemPowerInit != NULL) {
					systemPowerInit();
				}
				break;
			default:
				break;
		}
	}

	// Don't enable shutdown detection until we know the whole system is on.
	if (isSystemBooted()) {
		enableSoftOffDetection();
	}

	doSoftOffDetection();
	_lastState = _currentState;
}

SystemState AtxControllerClass::getState() {
	return _currentState;
}

void AtxControllerClass::onPowerOn(void (*systemPowerOn)()) {
	this->systemPowerOn = systemPowerOn;
}

void AtxControllerClass::onPowerOff(void (*systemPowerOff)()) {
	this->systemPowerOff = systemPowerOff;
}

void AtxControllerClass::onPowerInit(void (*systemPowerInit)()) {
	this->systemPowerInit = systemPowerInit;
}

bool AtxControllerClass::buttonWasDown() {
	return _buttonWasDown;
}

void AtxControllerClass::setButtonWasDown(bool wasDown) {
	_buttonWasDown = wasDown;
}

void AtxControllerClass::enableSoftOffDetection() {
	if (_shutdownDetection) {
		return;
	}
	_shutdownDetection = true;
	_timeDown = 0;
	Serial.println(F("DEBUG: Soft-off detection enabled"));
}

void AtxControllerClass::disableSoftOffDetection() {
	if (!_shutdownDetection) {
		return;
	}

	_shutdownDetection = false;
	_timeDown = 0;
	Serial.println(F("DEBUG: Soft-off detection disabled"));
}

AtxControllerClass AtxController;