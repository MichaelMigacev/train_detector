#pragma once
#include <LiquidCrystal_I2C.h>

class LCDInteractions
{
public:
    void setupLCD();
    void printLine(int row, String text);
    void clearLine(int row);
    void displayDepartures(const String departures[], int count);

private:
    LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);
    void createCustomCharacters();
    String replaceUmlauts(String input);
    void printWithCustomChars(int row, String text);
};