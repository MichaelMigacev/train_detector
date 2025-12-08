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
    url += "&results=4"; // Match array size

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

    DynamicJsonDocument doc(8192);
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
        if (result.count >= 4)
            break; // Max 4 for LCD

        // Get the first leg of each journey
        JsonArray legs = journey["legs"].as<JsonArray>();
        if (legs.size() == 0)
            continue;

        JsonObject firstLeg = legs[0];

        String lineName = firstLeg["line"]["name"] | "Unknown";
        String direction = firstLeg["direction"] | "Unknown";
        const char *depTime = firstLeg["plannedDeparture"] | firstLeg["departure"];
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

            // Debug print
            /* Serial.print("Journey " + lineName + " to " + direction + ": ");
            if (diff_minutes == 0)
                Serial.println("NOW");
            else if (diff_minutes == 1)
                Serial.println("1 min");
            else
                Serial.printf("%d mins\n", diff_minutes); */
        }
    }

    return result;
}

String StationInfo::makeJourneyMessage(String lineName, String direction, int diffMinutes)
{
    String message = "";
    message += cutString(lineName, 3);
    message += " ";
    String cleanDirection = cleanStationName(direction);
    message += cutString(cleanDirection, 13);
    message += " ";
    String timeDisplay = formatTimeDisplay(diffMinutes);
    message += cutString(timeDisplay, 2);
    return message;
}

String StationInfo::cutString(String input, int maxLength)
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

    if (stationName.startsWith("S+U "))
    {
        return stationName.substring(4);
    }
    else if (stationName.startsWith("S ") || stationName.startsWith("U "))
    {
        return stationName.substring(2);
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
    if (minutes == 0)
    {
        return "JZ";
    }
    else if (minutes < 0)
    {
        return "??";
    }
    else if (minutes >= 100)
    {
        return "ZL";
    }
    else if (minutes < 10)
    {
        return " " + String(minutes);
    }
    else
    {
        return String(minutes);
    }
}