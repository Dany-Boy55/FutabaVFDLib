/*
MIT License
 Futaba8MD06.h - Class provides basic funcionality to Futaba 5x7 matrix VFD modules
 Copyright (c) 2024 Daniel Rivera  All right reserved.
 
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 
 Created 2 Jan 2024 by Daniel Rivera
 */
#ifndef FUTABA8MD06
#define FUTABA8MD06

#include <Arduino.h>
#include <Print.h>
#include <Printable.h>

// Commands taken from datasheet
#define DIGITS_DISPLAY_COMMAND      0b11100000  //0XE0
#define DISPLAY_BRIGHTNESS_COMMAND  0b11100100  //0XE4
#define ENABLE_LIGHT_COMMAND        0b11101000  //0XE7
#define STANDBY_ENABLE_COMMAND      0B11101101  //Whatever in hex lol
#define STANDBY_DISABLE_COMMAND     0B11101100
#define ASCII_CHAR_START_COMMAND    0B00100000

// CGROM (symbols stored in the VFD controller ROM) 
// definitions from datasheet table CGROM (standard ascii is built in)
// All symbols below have been tested on the 8 digit version of the VFD
#define CGROM_SYMBOL_ARROW_UP           8
#define CGROM_SYMBOL_ARROW_DOWN         9
#define CGROM_SYMBOL_ARROW_LEFT         10
#define CGROM_SYMBOL_ARROW_RIGHT        11
#define CGROM_SYMBOL_ALL_SEGMENTS_OFF   16
#define CGROM_SYMBOL_ONE_VERTICAL       17
#define CGROM_SYMBOL_TWO_VERTICAL       18
#define CGROM_SYMBOL_THREE_VERTICAL     19
#define CGROM_SYMBOL_FOUR_VERTICAL      20
#define CGROM_SYMBOL_ALL_SEGMENTS_ON    21
#define CGROM_SYMBOL_SIX_HORIZONTAL     22
#define CGROM_SYMBOL_FIVE_HORIZONTAL    23
#define CGROM_SYMBOL_FOUR_HORIZONTAL    24
#define CGROM_SYMBOL_THREE_HORIZONTAL   25
#define CGROM_SYMBOL_TWO_HORIZONTAL     26
#define CGROM_SYMBOL_ONE_HORIZONTAL     27


// TODO: implement more flexible options for print functions, custom characters, potentially improve performance
class Futaba8MD06 
: public Print // Done: override the arduino print code and make this class inherit its methods
{
private:
    // Digital IO pins required additionally to the SPI pins
    uint8_t ssPin;
    uint8_t resetPin;
    // Internal members to keep track of the VFD status and such
    uint8_t display_digits;
    uint8_t cursorPosition;
public:
    Futaba8MD06(uint8_t number_of_display_digits = 8, uint8_t chipSelect = 2, uint8_t enable = 3);
    void begin();
    void disableStandby(); // Todo: unify into a single method with args?
    void enableStandby();
    void show();            // TODO: clarify the use of the "enable_light" command
    void clearDisplay();
    void setBrightness(uint8_t brightness);
    void sendASCIICharacter(uint8_t position, char character);
    // Override method inherited from the Print class
    size_t write(uint8_t c);
    // Override method inherited from the Print class
    size_t write(const uint8_t *buffer, size_t size);
    // TODO: implementation lmao this does nothing
    void sendCustomChar(char &character);
};
#endif