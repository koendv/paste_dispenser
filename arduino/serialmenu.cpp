#include <Arduino.h>
#include "settings.h"
#include "tb6612.h"


  /* 
   *  Menu. To access the menu, connect to the serial port.
   */

namespace settings {
  static const int8_t kNoSelection = -1;
  static const int8_t MAX_MENU = 10;
  static enum {KEYBD_CHAR, KEYBD_NUMBER, KEYBD_STRING, KEYBD_ESC, KEYBD_ESC_BRACKET, KEYBD_ESC_O} keybd_state = KEYBD_CHAR;
  long last_time_keybd_millis = 0;
  static const uint16_t kKeybdTimeout = 2 * 1000; /* 2 seconds timeout for multi-character keycodes */
  static const uint8_t KEY_UP = 251; /* keycodes for arrow keys */
  static const uint8_t KEY_DOWN = 252;
  static const uint8_t KEY_LEFT = 253;
  static const uint8_t KEY_RIGHT = 254;

  void serialPrintProgmem(const char * progmemStr);
  void printHelp();
  void printMenu();
   
  /* menu state */
  int8_t selectedMenuItem = kNoSelection;

  /* 
   *  Our menu is an array of menuItems. A menuItem is either a variable or a function. 
   *  If the user selects a variable, prompt user to enter the new value.
   *  If the user selects a a function, execute the function.
   */

  struct menuItem {
    char key;           // hotkey 
    uint16_t* var;      // menu item variable. NULL if menu item is a function.
    const char* name;   // human readable name
    const char* units;  // "seconds". "steps", etc.
    uint16_t low;       // low bound on var
    uint16_t high;      // high bound on var
    const char* help;   // terse yet informative help text
    void (*callback)() ;// menu item function. NULL if menu item is a variable.     
  };

  /* menu definition */
  // XXX fixme. Help text should be terse yet informative.
  // Menu text strings, stored in flash. 
  const static char forwardSpeedHelp[] PROGMEM = "speed when forward button pressed.";
  const static char forwardStepsHelp[] PROGMEM = "number of steps to move when forward button pressed. if 0 keep\r\n moving until forward button is released.";
  const static char backwardSpeedHelp[] PROGMEM = "speed when backward button pressed.";
  const static char pullbackDelayHelp[] PROGMEM = "time to wait after forward motion before pulling back.";
  const static char pullbackStepsHelp[] PROGMEM = "number of steps to pull back. if 0 don't pull back.";
  const static char microStepsHelp[] PROGMEM = "microsteps per full step. valid values 1, 2, 4, 8, 16, 32, 64.";
  const static char profileNumberHelp[] PROGMEM = "active profile";
  const static char readConfigHelp[] PROGMEM = "revert to saved";
  const static char writeConfigHelp[] PROGMEM = "save to non-volatile memory";
  const static char helpHelp[] PROGMEM = "this";

  const static char forwardSpeedName[] PROGMEM = "Forward speed";
  const static char forwardStepsName[] PROGMEM = "Forward steps";
  const static char backwardSpeedName[] PROGMEM = "Backward speed";
  const static char pullbackDelayName[] PROGMEM = "Pullback delay";
  const static char pullbackStepsName[] PROGMEM = "Pullback steps";
  const static char microStepsName[] PROGMEM = "Microstepping";
  const static char profileNumberName[] PROGMEM = "Profile number";
  const static char readConfigName[] PROGMEM = "Restore profile";
  const static char writeConfigName[] PROGMEM = "Save profile";
  const static char helpName[] PROGMEM = "Help";

  const static char stepsPerSecUnits[] PROGMEM = "steps/sec";
  const static char stepsUnits[] PROGMEM = "steps";
  const static char millisecUnits[] PROGMEM = "millisec";
  const static char noUnits[] PROGMEM = "";

  // callback for checking and setting microsteps
  void microStepsCallback() {
    // check value of microsteps is a power of two
    if ((microSteps != 1) && (microSteps != 2) && (microSteps != 4) && (microSteps != 8) && (microSteps != 16) && (microSteps != 32)  && (microSteps != 64)) microSteps = 2; 
    // set microsteps in tb6612 driver
    tb6612::setMicrosteps(microSteps);
  }
 
  // menu itself.
 
  const PROGMEM static struct menuItem consoleMenu[MAX_MENU] 
  { {'f', &forwardSpeed, forwardSpeedName, stepsPerSecUnits, 0, kMaxSpeed, forwardSpeedHelp, NULL},
    {'s', &forwardSteps, forwardStepsName, stepsUnits, 0, kMaxSteps, forwardStepsHelp , NULL},
    {'b', &backwardSpeed, backwardSpeedName, stepsPerSecUnits, 1, kMaxSpeed, backwardSpeedHelp, NULL},
    {'d', &pullbackDelay, pullbackDelayName, millisecUnits, 0, kMaxPullbackDelay, pullbackDelayHelp, NULL},
    {'p', &pullbackSteps, pullbackStepsName, stepsUnits, 0, kMaxSteps, pullbackStepsHelp, NULL},
    {'m', &microSteps, microStepsName, noUnits, 1, kMaxMicrosteps, microStepsHelp, microStepsCallback},
    {'n', &profileNumber, profileNumberName, noUnits, 0, kMaxProfile, profileNumberHelp, NULL},
    {'r', NULL, readConfigName, NULL, 0, 0, readConfigHelp, readConfig},
    {'w', NULL, writeConfigName, NULL, 0, 0, writeConfigHelp, writeConfig},
    {'h', NULL, helpName, NULL, 0, 0, helpHelp, printHelp} 
  };

   /* 
    * Small character buffer with character echo and backspace.
    */
  static const uint8_t bufsiz = 6; // 5 characters is enough for parsing uint16_t. 
  char buf[bufsiz];
  uint8_t ptr = 0;

  // Clear character buffer
  void clearbuf() {
    for (ptr = 0; ptr < bufsiz; ptr++) buf[ptr] = '\0';
    ptr = 0;
    return;
  }

  /* 
   * readchar() 
   * 
   * Read one character, with echo and backspace. Non-blocking. 
   * 
   * Handle mapping vt100 escape codes of arrows and numeric keypad to characters.
   *
  ^M enter
  ^[^[ escape
  ^[A up
  ^[B down
  ^[C right
  ^[D left
  ^[OA up
  ^[OB down
  ^[OC right
  ^[OD left
  ^[OM enter
  ^[Oj *
  ^[Ok +
  ^[Ol ,
  ^[Om -
  ^[On .
  ^[Oo /
  ^[Op 0
  ^[Oq 1
  ^[Or 2
  ^[Os 3
  ^[Ot 4
  ^[Ou 5
  ^[Ov 6
  ^[Ow 7
  ^[Ox 8
  ^[Oy 9

  We have four keyboard states:
    KEYB_CHAR: normal character entry
    KEYB_ESC: Escape received. Waiting for next character. If next character is [, go to KEYB_ESC_BRACKET. If next character is O, go to KEYB_ESC_O.
    KEYB_ESC_BRACKET: Escape [ received. Waiting for next character. 
    KEYB_ESC_O: Escape O received. Waiting for next character.       
 */

  void readchar() {
    char ch;
    // read character
    if (!Serial.available()) return;
    ch = Serial.read();
    
    // Check for timeout so we don't get stuck in a state.
    long now_millis = millis();
    if ((millis() - last_time_keybd_millis > kKeybdTimeout) && ((keybd_state == KEYBD_ESC) ||  (keybd_state == KEYBD_ESC_BRACKET) || (keybd_state == KEYBD_ESC_O)))
      keybd_state = KEYBD_CHAR; // timeout if two seconds after Escape, Escape [ or Escape [ O we still haven't seen a character. 
    last_time_keybd_millis = now_millis;

    // ansi vt100 terminals have different character codes for numeric keypad digits and keyboard digits. 
    // this code:
    // maps ansi escape codes for numeric keypad 0..9 to characters 0..9
    // maps arrow keys to key codes KEY_UP, KEY_DOWN, KEY_LEFT and KEY_RIGHT
    
    switch (keybd_state) {
      case KEYBD_CHAR:
        if (ch == '\e') { // Esc - change state to "Escape received".
          keybd_state = KEYBD_ESC; 
          return;
        }
        break;
        
      case KEYBD_ESC:
        if (ch ==  '[') { // Esc [ - change state to "Escape [ received".
          keybd_state = KEYBD_ESC_BRACKET;
          return;
        } else if (ch == 'O') { // Esc O - change state to "Escape O received"
          keybd_state = KEYBD_ESC_O;
          return;
        }
        // Hitting the "Escape" key twice in quick succession is like saying "get me out, now".
        else if (ch == '\e') { // two consecutive escapes - map to a single escape, now.
          break;
        } else { // discard char
          keybd_state = KEYBD_CHAR;
          return;
        }
        break;
        
      case KEYBD_ESC_BRACKET:
       if (ch == 'O') {
          keybd_state = KEYBD_ESC_O; // Esc [ O - change state to "Escape [ O received".
          return;
        };
        keybd_state = KEYBD_CHAR;
        if (ch == 'A') ch = KEY_UP; // up arrow
        else if (ch == 'B') ch = KEY_DOWN; // down arrow
        else if (ch == 'C') ch = KEY_RIGHT; // right arrow
        else if (ch == 'D') ch = KEY_LEFT; // left arrow
        else return; // discard
        break;
        
      case KEYBD_ESC_O:
        keybd_state = KEYBD_CHAR;
        if (ch == 'A') ch = KEY_UP; // up arrow
        else if (ch == 'B') ch = KEY_DOWN; // down arrow
        else if (ch == 'C') ch = KEY_RIGHT; // right arrow
        else if (ch == 'D') ch = KEY_LEFT; // left arrow
        else if (ch == 'M') ch = '\n'; // enter
        else if ((ch >= 'j') && (ch <= 'y')) {
          char lookuptable[] = "*+,-./0123456789";
          ch = lookuptable[ch - 'j'];
        } else return; // discard character
        break;
          
      default: 
        keybd_state = KEYBD_CHAR;
        return;
    }
    
 /*  
    // debug code
    switch (keybd_state) {
      case KEYBD_CHAR:
        Serial.println("KEYBD_CHAR");
        break;
      case KEYBD_ESC:
        Serial.println("KEYBD_ESC");
        break;
      case KEYBD_ESC_BRACKET:
        Serial.println("KEYBD_ESC_BRACKET");
        break;
      case KEYBD_ESC_O:
        Serial.println("KEYBD_ESC_O");
        break;
      default:
        Serial.println("KEYBD_ERR");
        break;
    }
 */   
    // At this point we (assume we) have a valid character, with all keypad escape codes and arrows mapped.
    // Now we can do the menu handling.
    // XXX have not implemented menu navigation using arrow keys yet.

    // Handle number entry
    if (selectedMenuItem >= 0) {
      // Handle end of number entry
      if ((ch == '\r') || (ch == '\n')) {
        Serial.println();
        if (ptr > 0) { // if buffer not empty
          // convert buffer to valid number
          int newval = atoi(buf);
          // retrieve menu item from flash
          menuItem thisItem;
          memcpy_P(&thisItem, &consoleMenu[selectedMenuItem], sizeof(thisItem));
          // number value within bounds
          newval = constrain(newval, (int)thisItem.low, (int)thisItem.high);
          // assign new value to selected variable
          if ((selectedMenuItem < MAX_MENU) && (thisItem.var != NULL))
            *thisItem.var = newval;
          // if there is a callback execute it
          if (thisItem.callback != NULL) thisItem.callback();
        }
        clearbuf();
        selectedMenuItem = kNoSelection;
        printMenu();
        return;          
      }
      // handle backspace
      if (ch == '\b') { // backspace
        if (ptr > 0) {
          buf[--ptr] = '\0';
          // erase backspaced character
          Serial.print('\b');
          Serial.print(' ');
          Serial.print('\b');
          return;
        } else {
          Serial.print('\a'); // bell
          return;
        }
      } else { // character isn't backspace
        if (!isDigit(ch)) return; // discard weirdness
        if (ptr < bufsiz-1) { // read character in buffer
          Serial.print(ch);
          buf[ptr++] = ch;
          return;
        } 
        else { // buffer full
          Serial.print('\a'); // bell
          return;
        }
      }
    }
    
    // Handle menu item hotkeys
    for (int i = 0; i < MAX_MENU; i++) {
      // retrieve menu item from flash
      menuItem thisItem;
      memcpy_P(&thisItem, &consoleMenu[i], sizeof(thisItem));
      // is this the menu item the user wants?
      if (thisItem.key != ch) continue;
      /* we have a match */
      if (selectedMenuItem >= 0) {
        // discard previous selection
        Serial.println(); 
        selectedMenuItem = kNoSelection; 
      }
      // if it's a variable print a prompt and wait for input
      if (thisItem.var != NULL) {
        // print prompt
        serialPrintProgmem(thisItem.name);
        Serial.print(F(" ["));
        Serial.print(thisItem.low);
        Serial.print(F(".."));
        Serial.print(thisItem.high);
        Serial.print(F("]? "));
        // set up buffer and wait for input
        clearbuf();
        selectedMenuItem = i;
        return;
      }
      // if it's a callback execute it
      if ((thisItem.var == NULL) && (thisItem.callback != NULL)) {
        thisItem.callback();
        serialPrintProgmem(thisItem.name);
        Serial.println(F(" done"));
        return;
      }
    }

    // discard remaining characters and print menu
    printMenu();
    return;
  }
  
  /*
   * print a string stored in progmem (flash).
   */
   void serialPrintProgmem(const char * progmemStr) {
    char ch;
    while(ch = pgm_read_byte(progmemStr)){
      Serial.print(ch);
      progmemStr++;
    }
    return;
   }

   /*
   * Menu routines proper
   */
   
   void printHelp() {
    // print firmware version. helpful for support.
    Serial.println();
    Serial.print(F("Firmware compiled "));
    Serial.println(F(__DATE__));
    Serial.println();
    
    for (int i = 0; i < MAX_MENU; i++) {
      // retrieve menu item from flash
      menuItem thisItem;
      memcpy_P(&thisItem, &consoleMenu[i], sizeof(thisItem));
      // print menu line
      Serial.print(thisItem.key);
      Serial.print(' ');
      serialPrintProgmem(thisItem.name);
      Serial.print(F(": "));
      serialPrintProgmem(thisItem.help);
      Serial.println();
    }
    Serial.println();
    Serial.print(F("Select using one of: "));
    for (int i = 0; i < MAX_MENU; i++) {
      // retrieve menu item from flash
      menuItem thisItem;
      memcpy_P(&thisItem, &consoleMenu[i], sizeof(thisItem));
      // print hotkey
      Serial.print(thisItem.key);
      Serial.print(' ');
    }
    Serial.println();
    Serial.println(F("Enter new value using 0-9, backspace, enter."));

    return;  
  }

  void printMenu() {
    Serial.println();
    for (int i = 0; i < MAX_MENU; i++) {
      // retrieve menu item from flash
      menuItem thisItem;
      memcpy_P(&thisItem, &consoleMenu[i], sizeof(thisItem));
      // print hotkey
      Serial.print('[');
      Serial.print(thisItem.key);
      Serial.print(']');
      Serial.print(' ');
      // menu item: print name, value and units if it's a variable, just name if it's a function.
      serialPrintProgmem(thisItem.name);
      Serial.print(' ');
      if (thisItem.var != 0) {
        Serial.print(*thisItem.var); // variable value
        Serial.print(' ');
        serialPrintProgmem(thisItem.units);
      }
      Serial.println();
    }
  }    
} // namespace settings
// not truncated
