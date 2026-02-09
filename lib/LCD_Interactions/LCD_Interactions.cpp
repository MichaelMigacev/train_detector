#include "LCD_Interactions.h"
#include <Arduino.h>

void LCDInteractions::setupLCD()
{
    lcd.init();
    lcd.backlight();
    createCustomCharacters();
    lcd.clear();
}

void LCDInteractions::printLine(uint8_t row, String text)
{
    printWithCustomChars(row, text);
}

void LCDInteractions::clearLine(uint8_t row)
{
    lcd.setCursor(0, row);
    for (uint8_t i = 0; i < LCD_COLUMNS; i++)
    {
        lcd.print(" ");
    }
    lcd.setCursor(0, row);
}

void LCDInteractions::displayDepartures(const String departures[], uint8_t count)
{
    for (uint8_t i = 0; i < LCD_ROWS; i++)
    {
        clearLine(i);
        if (i < count)
        {
            printLine(i, departures[i]);
        }
    }
}

void LCDInteractions::printWithCustomChars(uint8_t row, String text)
{
    text = replaceUmlauts(text);

    lcd.setCursor(0, row);
    for (uint8_t i = 0; i < text.length(); i++)
    {
        if (text[i] >= 1 && text[i] <= 7)
        {
            lcd.write(byte(text[i])); // Write custom character
        }
        else
        {
            lcd.print(text[i]); // Write normal character
        }
    }
}

String LCDInteractions::replaceUmlauts(String input)
{
    input.replace("ä", "\x02"); // Replace ä with a
    input.replace("ö", "\x03"); // Replace ö with o
    input.replace("ü", "\x01"); // Replace ü with custom char 0 (smiley)
    input.replace("Ä", "\x06"); // Replace Ä with custom char 4 (A with dots)
    input.replace("Ö", "\x04"); // Replace Ö with custom char 4 (O with dots)
    input.replace("Ü", "\x07"); // Same smiley for uppercase
    input.replace("ß", "\x05"); // Replace ß with custom char 5 (szet)
    return input;
}

void LCDInteractions::createCustomCharacters()
{
    uint8_t u_umlaut[8] = {
        0b01010,
        0b00000,
        0b10001,
        0b10001,
        0b10001,
        0b10011,
        0b01101,
        0b00000};
    uint8_t a_umlaut[8] = {
        0b01010,
        0b00000,
        0b01110,
        0b00001,
        0b01111,
        0b10001,
        0b01111,
        0b00000};
    uint8_t o_umlaut[8] = {
        0b01010,
        0b00000,
        0b01110,
        0b10001,
        0b10001,
        0b10001,
        0b01110,
        0b00000};
    uint8_t o_umlaut_big[8] = {
        0b01010,
        0b01110,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b01110,
        0b00000};
    uint8_t szet[8] = {
        0b01100,
        0b10010,
        0b10010,
        0b11100,
        0b10010,
        0b10010,
        0b11100,
        0b10000};
    uint8_t a_umlaut_big[8] = {
        0b01010,
        0b01110,
        0b10001,
        0b10001,
        0b11111,
        0b10001,
        0b10001,
        0b00000};
    uint8_t u_umlaut_big[8] = {
        0b01010,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b01110,
        0b00000};

    lcd.createChar(1, u_umlaut);     // ü → 0
    lcd.createChar(2, a_umlaut);     // ä → 1
    lcd.createChar(3, o_umlaut);     // ö → 2
    lcd.createChar(4, o_umlaut_big); // ö → 2
    lcd.createChar(5, szet);         // ß → 5
    lcd.createChar(6, a_umlaut_big); // Ä → 4
    lcd.createChar(7, u_umlaut_big); // Ü → 1
}

void LCDInteractions::printLCDMessages(String lcdMessage1, String lcdMessage2, String lcdMessage3, String lcdMessage4)
{
    printLine(0, lcdMessage1);
    printLine(1, lcdMessage2);
    printLine(2, lcdMessage3);
    printLine(3, lcdMessage4);
}