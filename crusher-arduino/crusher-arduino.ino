#include <OneButton.h>      //https://github.com/mathertel/OneButton
#include <AccelStepper.h>   //https://www.airspayce.com/mikem/arduino/AccelStepper/index.html
#include "FastLED.h"        //https://github.com/FastLED/FastLED
#include "LedEffects.h"


// PIN DEFINITIONS
#define BUTTON 11
#define TOP_STOP_SENSOR 10
#define BOTTOM_STOP_SENSOR 9

#define MOTOR_STEP 8
#define MOTOR_DIRECTION 7

#define LED_PIN 12
#define NUM_LEDS 1


// MOTOR DEFINITIONS

#define MAX_SPEED 1850
#define SPEED 1800          // This is the maximum speed the motor is capable of
#define RESET_OFFSET 3000   //Moving the crusher a bit away from the sensor

#define BRIGHTNESS 255      // Default LED brightness

CRGB leds[NUM_LEDS];
LedEffects ledEffects(leds, NUM_LEDS);

AccelStepper stepper(AccelStepper::DRIVER, MOTOR_STEP, MOTOR_DIRECTION);

bool buttonPressed, buttonLongPressed, topSensorTriggered, bottomSensorTriggered;

enum State {
  RESET,
  MOVE_TO_IDLE,
  IDLE,
  CRUSHING,
  STOP
};

OneButton topSensor = OneButton(
  TOP_STOP_SENSOR,  // Input pin for the button
  true,             // Button is active LOW
  true              // Enable internal pull-up resistor
);

OneButton bottomSensor = OneButton(
  BOTTOM_STOP_SENSOR,  // Input pin for the button
  true,                // Button is active LOW
  true                 // Enable internal pull-up resistor
);

OneButton btn = OneButton(
  BUTTON,  // Input pin for the button
  true,    // Button is active high
  true     // Disable internal pull-up resistor
);

/**
* Read boolean value and reset it after reading. This is needed to combine OneButton and StateMachine
*/
bool sensorTriggered(bool &sensorTriggered) {
  bool wasTriggered = sensorTriggered;
  sensorTriggered = false;
  return wasTriggered;
}

// Wrapper functions for each sensor, we need this "abstraction" to make it work with the state machine implementation
bool checkTopSensorTriggered() {
  return sensorTriggered(topSensorTriggered);
}

bool checkBottomSensorTriggered() {
  return sensorTriggered(bottomSensorTriggered);
}

bool checkButtonPressed() {
  return sensorTriggered(buttonPressed);
}

bool checkButtonLongPressed() {
  return sensorTriggered(buttonLongPressed);
}

State currentState = RESET;
State previousState = RESET;

bool executeOnce = true;

long endPosition = 485000;  //

// Function to change state, ensuring the transition only happens once
void changeState(State newState) {
  if (newState != currentState) {
    previousState = currentState;
    currentState = newState;
    executeOnce = true;  // Reset executeOnce flag when entering a new state
    Serial.print("Entering state: ");
    Serial.println(currentState);  // Debugging output
  }
}

// Main function to handle states and execute logic
void handleState() {
  switch (currentState) {
    case RESET:
      if (executeOnce) {
        // Code to run when entering RESET state
        Serial.println("# Resetting");
        stepper.setSpeed(SPEED);
        stepper.moveTo(-endPosition);
        executeOnce = false;  // Ensure this block runs only once

        Serial.print("Current Position: ");
        Serial.println(stepper.currentPosition());  // Check current position

        Serial.print("Distance to go Position: ");
        Serial.println(stepper.distanceToGo());  // Check current position
      }
      // Handle pulse effect
      ledEffects.pulse(CRGB::Blue, 1500);

      // Move the motor to the idle position
      if (stepper.distanceToGo() != 0) {
        stepper.run();
      }

      // Transition to MOVE_TO_IDLE if the top sensor is triggered
      if (checkTopSensorTriggered()) {
        stepper.stop();
        Serial.print("Reached top sensor, current position before resetting it: ");
        Serial.println(stepper.currentPosition());
        stepper.setCurrentPosition(0);
        changeState(MOVE_TO_IDLE);
      }

      // Transition to STOP if the button is pressed
      if (checkButtonPressed()) {
        changeState(STOP);
      }
      break;

    case MOVE_TO_IDLE:
      if (executeOnce) {
        Serial.println("# Moving to idling position");
        stepper.setSpeed(SPEED);
        stepper.setCurrentPosition(0);
        stepper.moveTo(RESET_OFFSET);

        executeOnce = false;  // Ensure this block runs only once

        Serial.print("Current Position: ");
        Serial.println(stepper.currentPosition());  // Check current position

        Serial.print("Distance to go Position: ");
        Serial.println(stepper.distanceToGo());  // Check current position
      }
      // Run pulse effect
      ledEffects.pulse(CRGB::Blue, 500);

      // Move the motor to the idle position
      if (stepper.distanceToGo() != 0) {
        stepper.run();
      } else {
        // Transition to IDLE when the motor reaches the idle position
        Serial.println("Reached the idle position.");
        changeState(IDLE);
      }
      break;

    case IDLE:
      if (executeOnce) {
        Serial.println("# Idling");
        stepper.stop();       // Stop motor when entering idle
        executeOnce = false;  // Ensure this block runs only once
      }
      ledEffects.rainbow(20);

      // Transition to CRUSHING state if button is pressed
      if (checkButtonPressed()) {
        changeState(CRUSHING);
      }
      break;

    case CRUSHING:
      if (executeOnce) {
        Serial.println("# Crushing");
        stepper.setSpeed(SPEED);
        stepper.moveTo(endPosition);
        executeOnce = false;  // Ensure this block runs only once

        Serial.print("Current Position: ");
        Serial.println(stepper.currentPosition());  // Check current position

        Serial.print("Distance to go Position: ");
        Serial.println(stepper.distanceToGo());  // Check current position
      }
      ledEffects.pulse(CRGB::Orange, 1500);

      // Move the motor to the idle position
      if (stepper.distanceToGo() != 0) {
        stepper.run();
      } else {
        Serial.print("Did not reach bottom sensor but exhausted travel path");
        stepper.stop();
        changeState(RESET);
      }

      // Transition to RESET if bottom sensor is triggered
      if (checkBottomSensorTriggered()) {
        Serial.print("Reached bottom sensor, current position before reset: ");
        Serial.println(stepper.currentPosition());
        endPosition = stepper.currentPosition();  //update the end position now that we know it for sure
        stepper.stop();
        changeState(RESET);
      }

      // Transition to STOP if button is pressed
      if (checkButtonPressed()) {
        stepper.stop();
        changeState(STOP);
      }
      break;

    case STOP:
      if (executeOnce) {
        Serial.println("# Stopped, waiting for button actions");
        stepper.stop();       // Stop motor when entering STOP state
        executeOnce = false;  // Ensure this block runs only once
      }
      ledEffects.pulse(CRGB::Green, 1000);

      // Transition to RESET if button is pressed
      if (checkButtonPressed()) {
        stepper.setSpeed(SPEED);
        stepper.moveTo(-endPosition);
        changeState(RESET);
      }

      // Transition to CRUSHING if long press detected
      if (checkButtonLongPressed()) {
        stepper.setSpeed(SPEED);
        stepper.moveTo(endPosition);
        changeState(CRUSHING);
      }
      break;
  }
}

void setup() {

  Serial.begin(9600);
  Serial.println("# Startup");

  Serial.println("Configuring LED");
  FastLED.addLeds<WS2811, LED_PIN, RGB>(leds, NUM_LEDS);

  Serial.println("Configuring Stepper");
  stepper.setMaxSpeed(MAX_SPEED);  // Max speed (steps/second)
  stepper.setAcceleration(1000);   // Acceleration (steps/second^2)
  stepper.setCurrentPosition(0);

  Serial.println("Configuring Button and Sensors");
  btn.attachClick([]() {
    Serial.println("Clicked!");
    buttonPressed = true;
  });
  btn.attachLongPressStart([]() {
    Serial.println("Long Press Start!");
    buttonLongPressed = true;
  });
  topSensor.attachPress([]() {
    Serial.println("Top sensor triggered!");
    topSensorTriggered = true;
  });
  bottomSensor.attachPress([]() {
    Serial.println("Bottom sensor triggered!");
    bottomSensorTriggered = true;
  });
  changeState(RESET);
}


void loop() {
  btn.tick();
  bottomSensor.tick();
  topSensor.tick();
  
  handleState();

  // Update the LEDs as needed (reduce FastLED.show() calls for performance)
  static unsigned long lastLedUpdate = 0;
  const unsigned long ledUpdateInterval = 50;  // Update every 50ms
  unsigned long currentMillis = millis();
  if (currentMillis - lastLedUpdate >= ledUpdateInterval) {
    lastLedUpdate = currentMillis;
    FastLED.show();
  }
}
