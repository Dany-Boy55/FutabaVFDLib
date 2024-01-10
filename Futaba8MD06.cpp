/*
MIT License
 Futaba8MD06.cpp - Class provides basic funcionality to Futaba 5x7 matrix VFD modules
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

#include <Futaba8MD06.h>
#ifndef SPI
#include <SPI.h>
#endif

/*
Instantiate a VFD object:
    number_of_display_digits: number of 5x7 digits contained in the vfd module 8 by default
    chipSelect: digital IO pin for chip select (printed as CS or SS on some modules)
    enable: digital IO pin for enable ()
*/
Futaba8MD06::Futaba8MD06(uint8_t number_of_display_digits = 8, uint8_t chipSelect, uint8_t reset){
    /*  Display startup proceedure from datasheet:
        1- Set "display timing" aka specify the number of digits/characters
            1.1- optionally activate universal mode, not used here so far
        2- Set dimming
        3- Transfer data
        4- Light up display by changing "display off" to "normal mode"
        5- Repeat 3, the display will continuosly "show" once "normal mode" is enabled
    */
    Futaba8MD06::ssPin = chipSelect;
    Futaba8MD06::resetPin = reset;
    // number of digits in the display, starting at 0
    // it seems that only 8 and 16 digit displays exist, so cleanup to be safe
    if(number_of_display_digits < 9){
        Futaba8MD06::display_digits = 7;
    }else{
        Futaba8MD06::display_digits = 15;
    }
    pinMode(Futaba8MD06::ssPin, OUTPUT);
    pinMode(Futaba8MD06::resetPin, OUTPUT);
}
void Futaba8MD06::begin(){
    digitalWrite(Futaba8MD06::resetPin, HIGH);
    digitalWrite(Futaba8MD06::ssPin, HIGH);
    // Give the display a little time to initialize
    delay(10);
    // setup SPI comms for the vfd
    SPI.begin();
    // Max 0.5 MHz clock (from datasheet)
    SPI.beginTransaction(SPISettings(200000, LSBFIRST, SPI_MODE3));
    // send initialization data
    digitalWrite(Futaba8MD06::ssPin, LOW);
    SPI.transfer(DIGITS_DISPLAY_COMMAND);
    SPI.transfer(Futaba8MD06::display_digits);
    digitalWrite(Futaba8MD06::ssPin, HIGH);
    Futaba8MD06::setBrightness(128);
    Futaba8MD06::show();
}

void Futaba8MD06::disableStandby(){
    SPI.beginTransaction(SPISettings(200000, LSBFIRST, SPI_MODE3));
    digitalWrite(Futaba8MD06::ssPin, LOW);
    SPI.transfer(STANDBY_DISABLE_COMMAND);  
    digitalWrite(Futaba8MD06::ssPin, HIGH);
    //Futaba8MD06::sendRaw16bit(DISPLAY_BRIGHTNESS_COMMAND << 8 + brightness);
}

void Futaba8MD06::enableStandby(){
    SPI.beginTransaction(SPISettings(200000, LSBFIRST, SPI_MODE3));
    digitalWrite(Futaba8MD06::ssPin, LOW);
    SPI.transfer(STANDBY_ENABLE_COMMAND);  
    digitalWrite(Futaba8MD06::ssPin, HIGH);
    //Futaba8MD06::sendRaw16bit(DISPLAY_BRIGHTNESS_COMMAND << 8 + brightness);
}

void Futaba8MD06::show(){
    SPI.beginTransaction(SPISettings(200000, LSBFIRST, SPI_MODE3));
    digitalWrite(Futaba8MD06::ssPin, LOW);
    SPI.transfer(ENABLE_LIGHT_COMMAND);  
    digitalWrite(Futaba8MD06::ssPin, HIGH);
    //Futaba8MD06::sendRaw16bit(DISPLAY_BRIGHTNESS_COMMAND << 8 + brightness);
}

void Futaba8MD06::clearDisplay()
{
    for (size_t i = 0; i < Futaba8MD06::display_digits; i++)
    {
        sendASCIICharacter(i, CGROM_SYMBOL_ALL_SEGMENTS_OFF);
    }
    
}

void Futaba8MD06::setBrightness(uint8_t brightness){
    SPI.beginTransaction(SPISettings(200000, LSBFIRST, SPI_MODE3));
    digitalWrite(Futaba8MD06::ssPin, LOW);
    SPI.transfer(DISPLAY_BRIGHTNESS_COMMAND);
    SPI.transfer(brightness);    
    digitalWrite(Futaba8MD06::ssPin, HIGH);
    //Futaba8MD06::sendRaw16bit(DISPLAY_BRIGHTNESS_COMMAND << 8 + brightness);
}

void Futaba8MD06::sendASCIICharacter(uint8_t position,char character){

    SPI.beginTransaction(SPISettings(200000, LSBFIRST, SPI_MODE3));
    digitalWrite(Futaba8MD06::ssPin, LOW);
    SPI.transfer(ASCII_CHAR_START_COMMAND + position);
    SPI.transfer(character);    
    digitalWrite(Futaba8MD06::ssPin, HIGH);
    //Futaba8MD06::sendRaw16bit((ASCII_CHAR_START_COMMAND+position) << 8 + character);
}

/* Print loops back to position 0 when printable would overflow display size*/
/* If the incoming buffer sill fits within the display chars*/
/* If incoming buffer is much longer than display space, ignore trailing chars*/
size_t Futaba8MD06::write(const uint8_t *buffer, size_t size)
{
    if(Futaba8MD06::cursorPosition + size > Futaba8MD06::display_digits + 1u) Futaba8MD06::cursorPosition=0;
    size_t n = 0;
    while (size--) {
        if (write(*buffer++)) n++;
        else break;
    }
    return n;
}

/* If incoming char lands outside the display size, do nothing and return 0*/
size_t Futaba8MD06::write(uint8_t c)
{
    if(Futaba8MD06::cursorPosition <= Futaba8MD06::display_digits){
        Futaba8MD06::sendASCIICharacter(Futaba8MD06::cursorPosition++, c);
        return 1;
    }else{
        return 0;
    }
}
