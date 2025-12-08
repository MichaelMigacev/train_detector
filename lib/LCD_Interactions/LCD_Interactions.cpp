#include "LCD_Interactions.h"
#include <Arduino.h>

void LCDInteractions::setupLCD()
{
    lcd.init();
    lcd.backlight();
    createCustomCharacters();
    lcd.clear();
}

void LCDInteractions::printLine(int row, String text)
{
    printWithCustomChars(row, text);
}

void LCDInteractions::clearLine(int row)
{
    lcd.setCursor(0, row);
    for (int i = 0; i < 20; i++)
    {
        lcd.print(" ");
    }
    lcd.setCursor(0, row);
}

void LCDInteractions::displayDepartures(const String departures[], int count)
{
    for (int i = 0; i < 4; i++)
    {
        clearLine(i);
        if (i < count)
        {
            printLine(i, departures[i]);
        }
    }
}

void LCDInteractions::printWithCustomChars(int row, String text)
{
    text = replaceUmlauts(text);

    lcd.setCursor(0, row);
    for (int i = 0; i < text.length(); i++)
    {
        if (text[i] >= 1 && text[i] <= 7)
        {                             // Check for custom chars 0-3
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
    // Example custom character (a simple smiley face)
    uint8_t u_umlaut[8] = {
        0b01010,
        0b00000,
        0b10001,
        0b10001,
        0b10001,
        0b10011,
        0b01101,
        0b00000};
    // Char 2: a with dots for ä
    uint8_t a_umlaut[8] = {
        0b01010,
        0b00000,
        0b01110,
        0b00001,
        0b01111,
        0b10001,
        0b01111,
        0b00000};

    // Char 3: o with dots for ö
    uint8_t o_umlaut[8] = {
        0b01010,
        0b00000,
        0b01110,
        0b10001,
        0b10001,
        0b10001,
        0b01110,
        0b00000};

    // Char 4: o with dots for Ö
    uint8_t o_umlaut_big[8] = {
        0b01010,
        0b01110,
        0b10001,
        0b10001,
        0b10001,
        0b10001,
        0b01110,
        0b00000};

    // Char 5: Sharp s for ß
    uint8_t szet[8] = {
        0b01100,
        0b10010,
        0b10010,
        0b11100,
        0b10010,
        0b10010,
        0b11100,
        0b10000};

    // Char 6: a with dots for Ä
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
    lcd.createChar(5, szet);
    lcd.createChar(6, a_umlaut_big); // Ä → 4
    lcd.createChar(7, u_umlaut_big); // Ü → 1
}