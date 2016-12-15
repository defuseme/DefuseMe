#include "pins_arduino.h"
#include "DefuseMe.h"
#include "DebounceInput.h"
#include "PushButton.h"
#include "StateMachine.h"

class AirplaneSwitchesSM : public StateMachine
{
  public:
    enum EState : byte {
      S_SelfTest = 0, // Make sure that all LEDs work and all button are down.
      S_Init,         // Start Game
      S_WaitOn,       // Wait for active button
      S_Button,       // A button has been activated
      S_WaitOff,      // Wait until all buttons to be off again
      S_Boom,         // Ups..
    };

  protected:
    byte ledstate;  // state displayed by the LEDs
    byte button;    // which button has been activated
    byte strikes;   // how many strikes have happend

    virtual byte DoProcessInternal();

  private:
    byte nextstate_0[8] = {6, 10, 7, 1, 0, 10, 2, 1}; // 10 is leads to a trigger but is not used as new ledstate
    byte nextstate_1[8] = {3, 4, 10, 6, 10, 7, 10, 5};
    byte nextstate_2[8] = {10, 0, 5, 10, 2, 3, 4, 10};

    bool running = false;

    short c_boom = 0;
};

#define PIN_SWITCH_LEFT 2
#define PIN_SWITCH_CENTER 3
#define PIN_SWITCH_RIGHT 4
#define PIN_LED_LEFT 5
#define PIN_LED_CENTER 6
#define PIN_LED_RIGHT 7
#define PIN_LED_ARMED 0

#define LED_ON 0
#define LED_OFF 1

DefuseMeModule module;
AirplaneSwitchesSM engine;
PushButton leftSwitch(PIN_SWITCH_LEFT);
PushButton centerSwitch(PIN_SWITCH_CENTER);
PushButton rightSwitch(PIN_SWITCH_RIGHT);

///////////////////////////////////////////////////////////////////////////////

void setup (void)
{
  Serial.begin (115200);
  Serial.println(F("startstart"));

  // init "Armed LED" - pin 0 (RX) can not be uses in combination with serial
  pinMode(PIN_LED_ARMED, OUTPUT);
  digitalWrite(PIN_LED_ARMED, LED_ON);

  pinMode(PIN_LED_LEFT, OUTPUT);
  pinMode(PIN_LED_CENTER, OUTPUT);
  pinMode(PIN_LED_RIGHT, OUTPUT);

  pinMode(PIN_SWITCH_LEFT, INPUT_PULLUP);
  pinMode(PIN_SWITCH_CENTER, INPUT_PULLUP);
  pinMode(PIN_SWITCH_RIGHT, INPUT_PULLUP);

  //The Values we want to send out to our neighbours
  tag *ourtags = new tag[3];
  ourtags[0] = {.name = F("ACTIVE"), .data = "true"}; //active module =>user interaction possible
  ourtags[1] = {.name = F("BUTTON"), .data = "3"};    //1 button
  ourtags[2] = {.name = F("LED"), .data = "3"};       //3 leds (without armed LED)

  /*
    //creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
    module.waitForInit(interestingTags, 2, F("ID:2355\n"
                                  "VERSION:0\n"
                                  "URL:https://example.com/\n"
                                  "AUTHOR:Petschge\n"
                                  "DESC:3 safety switches and 3 LEDs\n"
                                  "REPO:https://github.com/me/awesome-module.git\n"),
                       ourtags, 3);


    if (button.hasValue()) {
      Serial.print(F("BUTTON was set. Value: "));
      Serial.println(  button.getValue());
    } else {
      Serial.println(F("BUTTON was not set!"));
    }

    //those are not needed anymore
    delete ourtags;
  */
}

///////////////////////////////////////////////////////////////////////////////

void loop (void)
{
  if (module.updateState())
  {
    // Demo...
    long countdown = module.getGameState().time;
    byte state = module.getGameState().state;
    byte strikes = module.getGameState().strikes;

    engine.SetGameState(module.getGameState());
  }

  engine.DoProcess();
}

///////////////////////////////////////////////////////////////////////////////

byte AirplaneSwitchesSM::DoProcessInternal() {
  if(running && (c_boom == 0)) {
    // Update LEDs
    digitalWrite(PIN_LED_LEFT, !!!(ledstate & 4));
    digitalWrite(PIN_LED_CENTER, !!!(ledstate & 2));
    digitalWrite(PIN_LED_RIGHT, !!!(ledstate & 1));

    // Update armed state
    if(ledstate == strikes) {
      // disarmed
      module.setMyState(0);
      digitalWrite(PIN_LED_ARMED, LED_OFF);
    } else {
      // armed
      module.setMyState(1);
      digitalWrite(PIN_LED_ARMED, LED_ON);
    }
  }
  
  switch (_eState) {
    // S_SelfTest -- test all LED and make sure the buttons are down
    default:
    case S_SelfTest:
      Serial.println(F("S_SelfTest "));
      WatchDog(0);   // reset watchdog
      leftSwitch.DoReset();   // reset pressed and released flags in button class
      centerSwitch.DoReset();
      rightSwitch.DoReset();
      
      digitalWrite(PIN_LED_LEFT, LED_ON);
      digitalWrite(PIN_LED_CENTER, LED_ON);
      digitalWrite(PIN_LED_RIGHT, LED_ON);

      leftSwitch.DoProcess();
      centerSwitch.DoProcess();
      rightSwitch.DoProcess();
      static byte counter = 0;
      if(counter++ < 100) {
        return S_SelfTest;
      }
      return S_Init;
    
    
    // S_Init - lets go
    case S_Init:
      Serial.println(F("S_init "));
      // armed
      module.setMyState(1);
      digitalWrite(PIN_LED_ARMED, LED_ON);

      // set initial state
      // FIXME: Use random seed of the global module to seed RNG!
      ledstate = 4+random(4);
      Serial.println(ledstate);

      running = true;
      
      if(!leftSwitch.IsDown() && !centerSwitch.IsDown() && !rightSwitch.IsDown() ) {
        return S_WaitOn;
      }

      // If one switch is activated, we keep redoing the init
      // That makes the LEDs flicker randomly and should
      // alert the user that something is wrong
      DelayNext(100);
      return S_Init;


    // S_WaitOn -- Wait that the user activates a switch
    case S_WaitOn:
      //Serial.println(F("S_WaitOn "));
      if (leftSwitch.IsDown()) {
        Serial.println(F("Button Left "));
        button = 2;
        DelayNext(10);
        return S_Button;
      } else if(centerSwitch.IsDown()) {
        Serial.println(F("Button Center "));
        button = 1;
        DelayNext(10);
        return S_Button;
      } else if(rightSwitch.IsDown()) {
        Serial.println(F("Button Right "));
        button = 0;
        DelayNext(10);
        return S_Button;
      }
      //Serial.println(F("No Button "));
      //DelayNext(10);
      return S_WaitOn;


    // S_WaitOff -- Wait until all switches are off again
    case S_WaitOff:
      //Serial.println(F("S_WaitOff "));
      // reset boom counter
      c_boom = 0;
      if(!leftSwitch.IsDown() && !centerSwitch.IsDown() && !rightSwitch.IsDown()) {
        return S_WaitOn;
      }
      //DelayNext(100);
      return S_WaitOff;


    // S_Button -- The interesting part. the user decided to flip a switch
    case S_Button:
      Serial.println(F("S_Button "));
      if(button == 2) { // The left button
        if(centerSwitch.IsDown() || rightSwitch.IsDown()) { // Two switches on, or too quick flipping is an error
          return S_Boom;
        } else if (leftSwitch.IsDown()) { // Switch is still on
          byte newstate = nextstate_2[ledstate];
          Serial.println(newstate);
          if(newstate == 10) {
            return S_Boom;
          }
          ledstate = newstate;
          return S_WaitOff;
        } else {
          return S_WaitOn;
        }
      } else if(button == 1) { // The center button
        if(leftSwitch.IsDown() || rightSwitch.IsDown()) { // Two switches on, or too quick flipping is an error
          return S_Boom;
        } else if (centerSwitch.IsDown()) { // Switch is still on
          byte newstate = nextstate_1[ledstate];
          Serial.println(newstate);
          if(newstate == 10) {
            return S_Boom;
          }
          ledstate = newstate;
          return S_WaitOff;          
        } else {
          return S_WaitOn;
        }
      } else if(button == 0) { // The right button
        if(leftSwitch.IsDown() || centerSwitch.IsDown()) { // Two switches on, or too quick flipping is an error
          return S_Boom;
        } else if (rightSwitch.IsDown()) { // Switch is still on
          byte newstate = nextstate_0[ledstate];
          Serial.println(newstate);
          if(newstate == 10) {
            return S_Boom;
          }
          ledstate = newstate;
          return S_WaitOff;
        } else {
          return S_WaitOn;
        }
      }

    // S_Boom -- That was the wrong decision
    case S_Boom:
      if(c_boom == 0) {
        Serial.println(F("S_Boom "));
        module.trigger();
      } else if(c_boom > 50) {
        return S_WaitOff;
      } else if (c_boom %2) {
        digitalWrite(PIN_LED_LEFT, LED_ON);
        digitalWrite(PIN_LED_CENTER, LED_ON);
        digitalWrite(PIN_LED_RIGHT, LED_ON);
        DelayNext(25);
      } else {
        digitalWrite(PIN_LED_LEFT, LED_OFF);
        digitalWrite(PIN_LED_CENTER, LED_OFF);
        digitalWrite(PIN_LED_RIGHT, LED_OFF);
        DelayNext(25);
      }
      c_boom++;
      return S_Boom;
  }
  return _eState;   // no state change - repeat actual state
}



