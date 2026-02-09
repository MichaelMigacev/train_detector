#pragma once
#include <LiquidCrystal_I2C.h>

static constexpr uint8_t LCD_COLUMNS = 20U; // 20 columns standard for many LCDs
static constexpr uint8_t LCD_ROWS = 4U;     // 4 rows standard

class LCDInteractions
{
public:
    void setupLCD();
    void printLine(uint8_t row, String text);
    void clearLine(uint8_t row);
    void displayDepartures(const String departures[], uint8_t count);
    void printLCDMessages(String lcdMessage1, String lcdMessage2, String lcdMessage3, String lcdMessage4);

private:
    LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, LCD_COLUMNS, LCD_ROWS);
    void createCustomCharacters();
    String replaceUmlauts(String input);
    void printWithCustomChars(uint8_t row, String text);
};