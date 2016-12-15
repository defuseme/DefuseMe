#include "DefuseMe.h"
#ifdef ESP8266
#include "SPISlave.h"
#endif
//char (*curHandler)(char);
DefuseMeModule* curModule;



#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)


// SPI interrupt routine
ISR (SPI_STC_vect)
{
  SPDR = curModule->handler(SPDR);
}
#elif ESP8266
//isr is intialised on SPI init
#else
#error "ISR not defined for this processor"
#endif
DefuseMeModule::DefuseMeModule(): responses {
  { 0, 0}, //CMD_ENUMERATE
  { 0, 0}, //CMD_NEIGHBOUR
  { 0, 0}, //CMD_SETTINGS
  { 0, 0} //CMD_GAMESTATUS
} {

  randomSeed(seed^analogRead(A0)^analogRead(A0)^analogRead(A1)^analogRead(A2));
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  // turn on SPI in slave mode
  SPCR |= _BV(SPE);
  // turn on interrupts
  SPCR |= _BV(SPIE);
  SPCR |= _BV(CPHA);
#else
#error "SPI init not defined for this processor"
#endif
  curModule = this;
}
boolean DefuseMeModule::updateState() {
  if(!demoMode){
    return newGameStatus;
  }
  else{
    if(demoModeMillis-millis()>20){
        demoModeMillis = millis();
        if((millis() - demoModeStartMillis)<30000){
          gameState[currentState].time = 30000 - (millis() - demoModeStartMillis);
          gameState[currentState].state = 1;
          gameState[currentState].strikes = demoModeStrikes;
        }else{
          gameState[currentState].time = 0;
          gameState[currentState].state = 2;
          gameState[currentState].strikes = demoModeStrikes;
        }
        return true;
    }else{
      return false;
    }
  }
}


GameState DefuseMeModule::getGameState() {
  newGameStatus = false;
  return gameState[(currentState + 1) % 2];
}
void DefuseMeModule::setMyState(byte state) {
  myState = state;
}

void DefuseMeModule::trigger() {
  if (myState != 0) {
    myState = 2;
    demoModeStrikes++;
  }
}

void DefuseMeModule::waitForInit( TaggedValue *tags[], const int len, const  __FlashStringHelper* descr, const tag outtags[], const  int outtags_len) {
  String out = "";
  out += descr;
  out += F("REGISTER:");
  for (int i = 0; i < len; i++) {
    out += tags[i]->tag;
    if (i < (len - 1) ) {
      out += ',';
    }
  }
  out += '\n';
  out += '\n';
  for (int i = 0; i < outtags_len; i++) {
    out +=  outtags[i].name;
    out += ':';
    out += outtags[i].data;
    out += '\n';
  }
  responses[0].length = out.length();
  responses[0].buffer = (char *) out.c_str();
  Serial.println(responses[0].buffer);
  long int startmillis=millis();
  do {
    if (neighbourLineWaiting) {
      for (int i = 0; i < len; i++) {
        byte waitingLine = (curLine + 1) % 2;
        if ( tags[i]->check((char*)lineBuffers[waitingLine])) {
          tags[i]->parse((char*)lineBuffers[waitingLine]);
        }
      }
      neighbourLineWaiting = false;
    }
    if(millis()-startmillis>5000){
        Serial.println(F("No gameState update received for 5sec => starting Demo Mode"));
        responses[0] = {0, 0};
        out="";
        demoMode=true;
        demoModeStartMillis = millis();
        demoModeMillis = millis();
        gameState[currentState].time = 30000;
        gameState[currentState].state = 0;
        gameState[currentState].strikes = 0;

        return;
    }
  } while (!updateState()); //we received a game state => init finished
  Serial.println(F("waitForInit: OK"));
  responses[0] = {0, 0};
  out = "";
}


/**
   handler for the SPI receive interrupt
   the returned data is send out on the next SPI-byte
*/

byte DefuseMeModule::handler(byte c) {
  if(isr_data.lastCMD!=CMD_RESET){
    switch (c) {
      case CMD_RESET:
      case CMD_ENUMERATE:
      case CMD_NEIGHBOUR:
      case CMD_SETTINGS:
      case CMD_GAMESTATUS:
        // reset everything on command and begin sending response
        isr_data.lastCMD = c;
        isr_data.readPos = 0;
        isr_data.writePos = 0;
        isr_data.nlc = 0;
        return responses[isr_data.lastCMD - 0x80].length;
        break;
      case 0://0=>send data in the buffer
        return responses[isr_data.lastCMD - 0x80].buffer[isr_data.readPos++];
        break;
      case '\n':
        lineBuffers[curLine][isr_data.writePos] = 0;
        if (isr_data.lastCMD == CMD_NEIGHBOUR) {
          neighbourLineWaiting = true;
        } else if (isr_data.lastCMD == CMD_GAMESTATUS) {
          switch (isr_data.nlc++) {
            case 0:
              gameState[currentState].time = atol((char*)lineBuffers[curLine]);
              break;
            case 1:
              gameState[currentState].strikes = atoi((char*)lineBuffers[curLine]);
              break;
            case 2:
              gameState[currentState].state = atoi((char*)lineBuffers[curLine]);
              newGameStatus = true;
              currentState = (currentState + 1) % 2;
              if (myState == 2) {//the trigger state is only send once
                myState = 1;
                return '2';
              }
              break;
          }
        }
        isr_data.writePos = 0;
        curLine = (curLine + 1) % 2;
        break;
      default://no command and not 0 or '\n' => write it in the buffer
        lineBuffers[curLine][isr_data.writePos++] = c;
        break;
    }
  }else{
    seed = (seed<<8) | ((short)c);
    if(isr_data.writePos++==2){
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        SPDR = 0;
        asm volatile ("jmp 0");
#else
#error "reset not defined/(needed?) for this processor"
#endif
    }
  }
  return ('0' + myState);//actualy only needed on CMD_GAMESTATUS...
}





boolean TaggedValue::hasValue() {
  return valset;
};

boolean TaggedValue::check(const char* in) {
  return startsWith_P(tag, in);
};


void TaggedValue::parse(const char* in) {
  valset = true;
};

TaggedValue::TaggedValue(const  __FlashStringHelper* tag): tag(tag) {};


IntTaggedValue::IntTaggedValue(const  __FlashStringHelper* tag): TaggedValue(tag) {};
int IntTaggedValue::getValue() {
  return value;
}


void IntTaggedValue::parse(const char* in) {
  value = atoi(strchr (in, ':' ) + 1);
  TaggedValue::parse(in);
};























