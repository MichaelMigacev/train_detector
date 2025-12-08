#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

struct DepartureList
{
    String departures[4]; // Fixed size for 4 LCD lines
    int count;            // How many departures are actually filled
};

class StationInfo
{
public:
    DepartureList getSouthboundJourneys(String fromStation, String toStation);
    String cutString(String input, int maxLength);
    String makeJourneyMessage(String lineName, String direction, int diffMinutes);

private:
    DepartureList parseJourneys(String jsonResponse);
    // String makeJourneyMessage(String lineName, String direction, int diffMinutes);
    // String cutString(String input, int maxLength);
    String cleanStationName(String stationName);
    String removeParentheses(String input);
    String formatTimeDisplay(int minutes);
};