//=====================================================
// Class: SpreNeo
// The SpreNeo class is designed to control and manage
// LED lighting effects on a NeoPixel strip, offering a
// variety of visual patterns. The class provides an
// interface for initiating, stopping, and updating
// different LED effects. It encapsulates functions for
// two specific effects: the breathing effect
// (breatheEffect) and the shooting star effect
// (shootingStar). These effects are applied to
// NeoPixels, producing dynamic and visually appealing
// illumination.
// The class uses a structure (RGB_T) to represent RGB
// color values and another structure (LED_T) to store
// parameters related to the LED effects, such as the
// type of effect, time parameters, and color values.
// The LED effect types are defined as constants
// (LEDKIND_BRESS, LEDKIND_LEFTSTAR, and LEDKIND_RIGHTSTAR).
//-----------------------------------------------------
// date/author    : 2024/01/19 @chrmlinux03
// update/author  : 2024/01/19 @chrmlinux03
//=====================================================
/*  example

  #include <Spre.neo.hpp>

  SpreNeo neo;

  void setup(void) {
  neo.begin();
  neo.set(LEDKIND_RIGHTSTAR, 0, 0, 128, 60);
  neo.start();
  }

  void loop(void) {
  neo.update();
  }

*/

#ifndef __SPRE_NEO_HPP__
#define __SPRE_NEO_HPP__

#include <Arduino.h>
#include <SpresenseNeoPixel.h>

//------------------------------------------------------------
// NeoPixel Configuration
//------------------------------------------------------------
const uint16_t neoPin = 20;   // Pin number to which NeoPixels are connected
const uint16_t neoNum = 4;   // Number of NeoPixels in the strip
SpresenseNeoPixel<neoPin, neoNum> npx;  // NeoPixel instance

//------------------------------------------------------------
// LED Effect Types
//------------------------------------------------------------
enum {
  LEDKIND_BRESS = 0,        // Breathing effect
  LEDKIND_LEFTSTAR,         // Shooting star effect from left
  LEDKIND_RIGHTSTAR,         // Shooting star effect from right
  LEDKIND_SMOOTHLEFT,         // Shooting star effect from left
  LEDKIND_SMOOTHRIGHT,         // Shooting star effect from right
};

//------------------------------------------------------------
// RGB Color Structure
//------------------------------------------------------------
typedef struct {
  uint8_t r;   // Red component
  uint8_t g;   // Green component
  uint8_t b;   // Blue component
} RGB_T;

//------------------------------------------------------------
// LED Effect Parameters Structure
//------------------------------------------------------------
typedef struct {
  bool enable;    // Flag to enable or disable the LED effect
  int8_t kind;    // Type of LED effect (LEDKIND_BRESS, LEDKIND_LEFTSTAR, LEDKIND_RIGHTSTAR)
  int32_t tm;     // Time parameter for the LED effect
  RGB_T rgb;      // RGB color values for the LED effect
} LED_T;

//------------------------------------------------------------
// Volatile instance to store LED parameters
//------------------------------------------------------------
volatile LED_T _ledt;

class SpreNeo {
  public:
    //------------------------------------------------------------
    // Constructor: SpreNeo
    //   Input: None
    //   Output: None
    //   Description: Initializes a new instance of the SpreNeo class.
    //------------------------------------------------------------
    SpreNeo() {}

    //------------------------------------------------------------
    // Destructor: ~SpreNeo
    //   Input: None
    //   Output: None
    //   Description: Destroys an instance of the SpreNeo class.
    //------------------------------------------------------------
    ~SpreNeo() {}

    //-----------------------------------------------------
    // breatheEffect function
    //   Input: red, green, blue, msec
    //   Output: None
    //   Description: Implements the breathe effect on NeoPixels.
    //-----------------------------------------------------
    void breatheEffect(uint8_t red, uint8_t green, uint8_t blue, uint32_t msec) {
      static float phase = 0.0;
      static unsigned long preTm = 0;
      unsigned long tm = millis();
      if (tm - preTm >= msec) {
        preTm = tm;
        float brt = 0.5 + 0.5 * sin(phase);
        uint8_t r = (uint8_t)(brt * red);
        uint8_t g = (uint8_t)(brt * green);
        uint8_t b = (uint8_t)(brt * blue);

        for (int i = 0; i < neoNum; i++) {
          npx.set(i, r, g, b);
        }
        npx.show();
        phase += 0.1;
      }
    }

    //-----------------------------------------------------
    // shootingStar function
    //   Input: red, green, blue, direc, waitTime
    //   Output: None
    //   Description: Implements the shooting star effect on NeoPixels.
    //-----------------------------------------------------
    void shootingStar(uint8_t red, uint8_t green, uint8_t blue, int direc, uint32_t waitTime) {
      static uint16_t startPos = (direc == 1) ? 0 : neoNum - 1;
      static unsigned long previousMillis = 0;
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= waitTime) {
        previousMillis = currentMillis;
        npx.clear();
        for (int i = -2; i <= 3; i++) {
          int pos = startPos + i * direc;
          if (pos >= 0 && pos < neoNum) {
            float distance = abs(startPos - pos);
            float brt = max(1.0 - 0.9 * distance, 0.0);
            uint8_t r = red;
            uint8_t g = green;
            uint8_t b = blue;
            r = (uint8_t)(r * brt);
            g = (uint8_t)(g * brt);
            b = (uint8_t)(b * brt);
            npx.set(pos, r, g, b);
          }
        }
        npx.show();
        startPos += direc;
        if (startPos >= neoNum || startPos < 0) {
          startPos = (direc == 1) ? 0 : neoNum - 1;
        }
      }
    }

    //-----------------------------------------------------
    // smoothStart function with virtual LEDs
    //   Input: red, green, blue, period, virtualLEDs, dir
    //   Output: None
    //   Description: Implements a smooth start effect on virtual LEDs using a sine wave.
    //-----------------------------------------------------
    void smoothStar(uint8_t red, uint8_t green, uint8_t blue, uint32_t period, int virtualLEDs, int dir) {
      static float phase = 0.0;
      static uint32_t previousMillis = 0;

      for (int i = 0; i < virtualLEDs; i++) {
        // Calculate brightness based on the sine wave with the specified period
        float brightness = 0.5 * sin(phase - dir * i * 2 * PI / virtualLEDs) + 0.5;

        uint8_t r, g, b;
        r = (uint8_t)(red * brightness);
        g = (uint8_t)(green * brightness);
        b = (uint8_t)(blue * brightness);

        // Map virtual LED to physical LED
        int physicalLED = map(i, 0, virtualLEDs - 1, 0, neoNum);
        npx.set(physicalLED, r, g, b);
      }

      if (millis() - previousMillis >= period) {
        // Change the phase of the sine wave every period
        phase += 0.05; // Adjust to an appropriate value

        npx.show();
        previousMillis = millis();
      }
    }
    
    //-----------------------------------------------------
    // begin function
    //   Input: None
    //   Output: None
    //   Description: Initializes the SpreNeo instance by stopping any ongoing effects and setting a default effect.
    //-----------------------------------------------------
    void begin() {
      npx.framerate(60);
      stop();
      set(LEDKIND_BRESS, 0, 0, 128, 60);
    }

    //-----------------------------------------------------
    // start function
    //   Input: None
    //   Output: None
    //   Description: Starts the LED effect.
    //-----------------------------------------------------
    void start() {
      _ledt.enable = true;
    }

    //-----------------------------------------------------
    // stop function
    //   Input: None
    //   Output: None
    //   Description: Stops the LED effect.
    //-----------------------------------------------------
    void stop() {
      _ledt.enable = false;
    }

    //-----------------------------------------------------
    // set function
    //   Input: kind, r, g, b, tm
    //   Output: None
    //   Description: Sets the LED effect parameters.
    //-----------------------------------------------------
    void set(uint8_t kind, uint8_t r, uint8_t g, uint8_t b, uint8_t tm) {
      _ledt.kind = kind;
      _ledt.rgb.r = r;
      _ledt.rgb.g = g;
      _ledt.rgb.b = b;
      _ledt.tm = tm;
    }

    //-----------------------------------------------------
    // update function
    //   Input: None
    //   Output: None
    //   Description: Updates the LED effect based on the set parameters.
    //-----------------------------------------------------
    void update(void) {
      if (_ledt.enable) {
        switch (_ledt.kind) {
          case LEDKIND_BRESS:
            breatheEffect(_ledt.rgb.r, _ledt.rgb.g, _ledt.rgb.b, _ledt.tm);
            break;
          case LEDKIND_LEFTSTAR:
            shootingStar(_ledt.rgb.r, _ledt.rgb.g, _ledt.rgb.b, 1, _ledt.tm);
            break;
          case LEDKIND_RIGHTSTAR:
            shootingStar(_ledt.rgb.r, _ledt.rgb.g, _ledt.rgb.b, -1, _ledt.tm);
            break;
          case LEDKIND_SMOOTHLEFT:
            smoothStar(_ledt.rgb.r, _ledt.rgb.g, _ledt.rgb.b, _ledt.tm, 24, 1);
            break;
          case LEDKIND_SMOOTHRIGHT:
            smoothStar(_ledt.rgb.r, _ledt.rgb.g, _ledt.rgb.b, _ledt.tm, 24, -1);
            break;
          default:
            break;
        }
      }
    }

  private:
};

#endif // __SPRE_NEO_HPP__