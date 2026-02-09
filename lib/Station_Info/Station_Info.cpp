#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "Station_Info.h"
#include "config.h"
#include "time.h"

DepartureList StationInfo::getSouthboundJourneys(String fromStation, String toStation)
{
    HTTPClient http;

    String url = "https://v6.bvg.transport.rest/journeys?";
    url += "from=" + fromStation;
    url += "&to=" + toStation;
    url += "&results=" + QUERY_SIZE; // Match array size

    http.begin(url);
    http.addHeader("Accept", "application/json");

    DepartureList result = {{}, 0}; // Initialize empty

    if (http.GET() == 200)
    {
        result = parseJourneys(http.getString());
    }

    http.end();
    return result;
}

DepartureList StationInfo::parseJourneys(String jsonResponse)
{
    DepartureList result = {{}, 0};

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonResponse);

    if (error)
    {
        Serial.print("Journey JSON parsing failed: ");
        Serial.println(error.c_str());
        return result;
    }

    // Get current time
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain current time");
        return result;
    }

    time_t now;
    time(&now);

    // Extract journeys array (different from departures!)
    JsonArray journeys = doc["journeys"].as<JsonArray>();
    Serial.println("Next journeys:");

    for (JsonObject journey : journeys)
    {
        if (result.count >= LCD_SIZE_ROWS)
            break; // Max 4 for LCD

        // Get the first leg of each journey
        JsonArray legs = journey["legs"].as<JsonArray>();
        if (legs.size() == 0)
            continue;

        JsonObject firstLeg = legs[0];

        String lineName = firstLeg["line"]["name"] | "Unknown";
        String direction = firstLeg["direction"] | "Unknown";
        const char *depTime = firstLeg["departure"];
        if (!depTime)
            continue;

        struct tm departure_tm = {0};
        sscanf(depTime, "%d-%d-%dT%d:%d:%d",
               &departure_tm.tm_year, &departure_tm.tm_mon, &departure_tm.tm_mday,
               &departure_tm.tm_hour, &departure_tm.tm_min, &departure_tm.tm_sec);

        departure_tm.tm_year -= 1900;
        departure_tm.tm_mon -= 1;
        departure_tm.tm_isdst = -1;

        time_t departure = mktime(&departure_tm);
        double diff_seconds = difftime(departure, now);
        int diff_minutes = (int)(diff_seconds / 60);

        if (diff_minutes >= 0)
        {
            // Create message and add to result
            result.departures[result.count] = makeJourneyMessage(lineName, direction, diff_minutes);
            result.count++;
        }
    }

    return result;
}

String StationInfo::makeJourneyMessage(String lineName, String direction, int diffMinutes)
{
    uint8_t lineLength = 3;
    uint8_t directionLength = 13;
    uint8_t timeLength = 2;
    String message = "";
    message += cutString(lineName, lineLength);
    message += " ";
    String cleanDirection = cleanStationName(direction);
    message += cutString(cleanDirection, directionLength);
    message += " ";
    String timeDisplay = formatTimeDisplay(diffMinutes);
    message += cutString(timeDisplay, timeLength);
    return message;
}

String StationInfo::cutString(String input, uint8_t maxLength)
{
    String lengthCheck = input;
    lengthCheck.replace("ü", "u");
    lengthCheck.replace("ä", "a");
    lengthCheck.replace("ö", "o");
    lengthCheck.replace("Ü", "U");
    lengthCheck.replace("A", "A");
    lengthCheck.replace("O", "O");
    lengthCheck.replace("ß", "s");
    if (lengthCheck.length() < maxLength)
    {
        while (lengthCheck.length() < maxLength)
        {
            lengthCheck += " ";
            input += " ";
        }
        return input;
    }
    else if (lengthCheck.length() > maxLength)
    {
        int displayCount = 0;
        String result = "";
        for (int i = 0; i < input.length() && displayCount < maxLength; i++)
        {
            // Check if current char starts a UTF-8 sequence (specifically umlaute)
            if (input[i] == 0xC3)
            {

                result += input[i];
                result += input[i + 1];
                i++; // Skip the second byte
            }
            else
            {
                result += input[i];
            }
            displayCount++;
        }
        return result;
    }
    return input;
}

String StationInfo::cleanStationName(String stationName)
{
    stationName = removeParentheses(stationName);
    uint8_t stationPrefixLong = 4;  // "S+U " is the longest prefix
    uint8_t stationPrefixShort = 2; // "S " or "U "

    if (stationName.startsWith("S+U "))
    {
        return stationName.substring(stationPrefixLong);
    }
    else if (stationName.startsWith("S ") || stationName.startsWith("U "))
    {
        return stationName.substring(stationPrefixShort);
    }
    return stationName;
}

String StationInfo::removeParentheses(String input)
{
    int start = input.indexOf('(');
    while (start != -1)
    {
        int end = input.indexOf(')', start);
        if (end != -1)
        {
            input.remove(start, end - start + 1);
        }
        else
        {
            break;
        }
        start = input.indexOf('(');
    }
    return input;
}

String StationInfo::formatTimeDisplay(int minutes)
{
    uint16_t maxDisplayMinutes = 99;
    uint8_t doubleDigitThreshold = 10;

    if (minutes == 0)
    {
        return "JZ";
    }
    else if (minutes < 0)
    {
        return "??";
    }
    else if (minutes > maxDisplayMinutes)
    {
        return "ZL";
    }
    else if (minutes < doubleDigitThreshold)
    {
        return " " + String(minutes);
    }
    else
    {
        return String(minutes);
    }
}