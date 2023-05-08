#ifndef _ATX_CONTROLLER_H
#define _ATX_CONTROLLER_H

#include <Arduino.h>

// Pin definitions
#define PIN_PS_ON 4
#define PIN_PWR_BTN 5
#define PIN_PWR_LED 6
#define PIN_SOFT_OFF 7
#define PIN_PWR_OK 8

// Timing intervals
#define PWR_OFF_DELAY 5000
#define BTN_INTERVAL 500
#define SOFT_OFF_THRESHOLD 1500
#define SOFT_OFF_DELAY 500

/**
 * @brief Defines possible system states.
 */
enum class SystemState : uint8_t {
	/**
	 * @brief The system is currently turned off.
	 */
	OFF = 0,

	/**
	 * @brief The system is initializing.
	 */
	INIT = 1,

	/**
	 * @brief The system is on and initialized.
	 */
	ON = 2
};

/**
 * @brief ATX PSU management driver.
 */
class AtxControllerClass {
public:
	/**
	 * @brief Construct a new Atx Controller Class object.
	 */
	AtxControllerClass();

	/**
	 * @brief Initializes the driver.
	 */
	void begin();

	/**
	 * @brief Process PSU and soft-off events.
	 */
	void loop();

	/**
	 * @brief Get the current system state.
	 * 
	 * @return The current state.
	 */
	SystemState getState();

	/**
	 * @brief Attaches a handler callback for when the system turns on.
	 * 
	 * @param systemPowerOn The callback to execute when the system turns on.
	 */
	void onPowerOn(void (*systemPowerOn)());

	/**
	 * @brief Attaches a handler callback for when the system turns off.
	 * 
	 * @param systemPowerOff The callback to execute when the system turns off.
	 */
	void onPowerOff(void (*systemPowerOff)());

	/**
	 * @brief Attaches a handler callback for when the system starts initializing.
	 * 
	 * @param systemPowerInit The callback to execute on system init.
	 */
	void onPowerInit(void (*systemPowerInit)());

	/**
	 * @brief Checks to see if the power button was last known to be pressed down.
	 * 
	 * @return true if pressed.
	 * @return false if released.
	 */
	bool buttonWasDown();

	/**
	 * @brief Set the flag indicating the power button is pressed.
	 * 
	 * @param wasDown Set true if pressed, false if released.
	 */
	void setButtonWasDown(bool wasDown);

	/**
	 * @brief Enables soft-off detection.
	 */
	void enableSoftOffDetection();

	/**
	 * @brief Disables soft-off detection.
	 */
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

// Global driver instance
extern AtxControllerClass AtxController;
#endif