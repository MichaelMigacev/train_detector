#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

static String QUERY_SIZE = "8";              // How many journeys to query (should be >= LCD lines)
static constexpr uint8_t LCD_SIZE_ROWS = 4U; // 4 rows standard
struct DepartureList
{
    String departures[LCD_SIZE_ROWS]; // Fixed size for LCD lines
    int count;                        // How many departures are actually filled
};

class StationInfo
{
public:
    DepartureList getSouthboundJourneys(String fromStation, String toStation);
    // String cutString(String input, int maxLength);
    // String makeJourneyMessage(String lineName, String direction, int diffMinutes);

private:
    DepartureList parseJourneys(String jsonResponse);
    String makeJourneyMessage(String lineName, String direction, int diffMinutes);
    String cutString(String input, uint8_t maxLength);
    String cleanStationName(String stationName);
    String removeParentheses(String input);
    String formatTimeDisplay(int minutes);
};