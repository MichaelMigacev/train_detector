#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "Station_Info.h"
#include "config.h"
#include "time.h"

DepartureList StationInfo::getSouthboundJourneys(const char *fromStation, const char *toStation)
{
    HTTPClient http;

    String url = "https://v6.bvg.transport.rest/journeys?";
    url += "from=" + String(fromStation);
    url += "&to=" + String(toStation);
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

        // Extract journey data (different structure!)
        String lineName = firstLeg["line"]["name"] | "Unknown";
        String direction = firstLeg["direction"] | "Unknown";
        const char *depTime = firstLeg["plannedDeparture"] | firstLeg["departure"];
        if (!depTime)
            continue;

        // Time calculation (same as before)
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
            Serial.print("Journey " + lineName + " to " + direction + ": ");
            if (diff_minutes == 0)
                Serial.println("NOW");
            else if (diff_minutes == 1)
                Serial.println("1 min");
            else
                Serial.printf("%d mins\n", diff_minutes);
        }
    }

    return result;
}

String StationInfo::makeJourneyMessage(String lineName, String direction, int diffMinutes)
{
    String message = "";
    message += cutString(lineName, 3);
    message += " ";
    message += cutString(direction, 13);
    message += " ";
    message += cutString(String(diffMinutes), 2);
    return message;
}

String StationInfo::cutString(String input, int maxLength)
{
    if (input.length() < maxLength)
    {
        while (input.length() < maxLength)
        {
            input += " ";
        }
        return input;
    }
    else if (input.length() > maxLength)
    {
        input = input.substring(0, maxLength);
    }
    return input;
}
