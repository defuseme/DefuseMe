#ifndef __DEFUSE_ME__
#define __DEFUSE_ME__
#include <Arduino.h>


#define    CMD_ENUMERATE   0x80
#define    CMD_NEIGHBOUR   0x81
#define    CMD_SETTINGS    0x82
#define    CMD_GAMESTATUS  0x83
#define    CMD_RESET       0x84
#define    CMD_NONE        0xFF



class TaggedValue {
    friend class DefuseMeModule;
  public:
    /**
       TaggedValue constructor
       tag is the name of the value we are interested in (use the F() macro to create a const __FlashStrinHelper* )
    */
    TaggedValue(const __FlashStringHelper* tag);
    /**
       true if some value was received
    */
    boolean hasValue();

    virtual void parse(const char* in);
  private:

    /**
       the tag name
    */
    const __FlashStringHelper *tag;
    /**
       check if String starts with the tag
    */
    boolean check(const char* in);

    bool startsWith(const char *pre, const char *str)
    {
      size_t lenpre = strlen(pre),
             lenstr = strlen(str);
      return (lenstr < (lenpre + 1)) ? false : ((strncmp( pre, str, lenpre) == 0) && str[lenpre]==':');
    }
    bool startsWith_P(const __FlashStringHelper *pre, const char *str) {
      size_t lenpre = strlen_P((const char *)pre),
             lenstr = strlen(str);
      return (lenstr < (lenpre + 1)) ? false : ((strncmp_P(str, (const char *) pre, lenpre) == 0) && str[lenpre]==':');
    }
    boolean valset = false;

};

class IntTaggedValue: public TaggedValue {
  public:
    IntTaggedValue(const __FlashStringHelper* tag);
    /**
       get the read out value
    */
    int getValue();
    virtual void parse(const char* in);
   
  private:

    int value;
};

class SnoTaggedValue : public TaggedValue {
public:
	SnoTaggedValue(const __FlashStringHelper* tag) : TaggedValue(tag) {
		//strcpy(_sno, "012345678901234");
	};

	char getDigit(byte i) {
		return _sno[i];
	}
	char* getString() {
		return _sno;
	}
	virtual void parse(const char* in) {
		Serial.print("Sno parse: ");
		Serial.println(in);
		strcpy(_sno, strchr(in, ':') + 1);
		Serial.println(_sno);

		TaggedValue::parse(in);
	};

private:
	char _sno[32];   // sno is always 15 digits
};

/**
   description of a feature (for example name="LED",data="4" to tell your neighbours you have 4 leds)
*/
struct tag {
/*	tag(const  __FlashStringHelper* cname, char* cdata)
	{
		name = cname; data = cdata;
	};
 */
const  __FlashStringHelper* name;
  char* data;
};


/**
   State of the bomb
*/
struct GameState {
  /**
     time in ms
  */
  long int time;
  /**
     state of the bomb
     0 - deactivated
     1 - active
     2 - exploded
  */
  byte state;
  /**
     count of strikes - starts with 0
  */
  byte strikes;
};







class DefuseMeModule {
  private:
    typedef volatile struct {
      byte lastCMD = CMD_NONE;
      byte writePos = 0;
      byte nlc = 0;
      int readPos;
    } t_isr_data;

    volatile t_isr_data isr_data;

    volatile byte currentState = 0;
    GameState gameState[2];
    byte myState = 1;
    volatile boolean newGameStatus = false;


    volatile boolean neighbourLineWaiting = false;
    volatile char lineBuffers[2][80];
    volatile byte curLine = 0;
    struct cmdResponse {
      int length;
      char* buffer;
    };
    cmdResponse responses[5];

	byte demoMode = 0;
    unsigned long demoModeMillis = 0;
    unsigned long demoModeStartMillis = 0;
    byte demoModeStrikes =0;




  public:
    DefuseMeModule();
    /**
       initializes the module and waits for the game controller to reply with the configuration data for all modules and start the game
       tags: tags you are interested in
       len: length of tags

    */
    void waitForInit( TaggedValue *tags[], const int len, const  __FlashStringHelper* descr, const tag outtags[], const  int outtags_len);
    /*
       returns true if there is new GameState present since the call to getGameState().
       returns false otherwise.
    */
    boolean updateState();
    /*
       returns the last received GameState
    */
    GameState getGameState();
    /**
       sets own state:
       0- deactivated
       1- active
    */
    void setMyState(byte state);
    /**
       TRIGGER the module if it is not deactivated (send state = 2 on next update => one more strike)
    */
    void trigger();
	/**
	sets own state to armed/active
	*/
	void setArmed() { setMyState(1); };
	/**
	sets own state to disarmed/inactive
	*/
	void setDisarmed() { setMyState(0); };

	/**
	interrupthandler
	*/
	byte handler(byte c);

    void begin();

};
#endif
