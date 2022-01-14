#pragma once

#include "secrets.h"
#include "shared/serialWrapper.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

// selfmade, but there are alternatives:
// https://github.com/Bodmer/OpenWeather/ pulls in unwanted JSON decoder
// https://github.com/JHershey69/OpenWeatherOneCall won't work on all platforms

// use https://arduinojson.org/v6/assistant/ to get pre-generated code

// {"coord":{"lon":XX.XXXXX,"lat":XX.XXX},"weather":[{"id":800,"main":"Clear","description":"clear
// sky","icon":"01n"}],"base":"stations","main":{"temp":272.91,"feels_like":272.91,"temp_min":271.75,"temp_max":274.97,"pressure":1033,"humidity":90},"visibility":6000,"wind":{"speed":0.51,"deg":0},"clouds":{"all":0},"dt":1642181631,"sys":{"type":1,"id":1268,"country":"DE","sunrise":1642144040,"sunset":1642174937},"timezone":3600,"id":2923544,"name":"XXXX","cod":200}

namespace WeatherService {

struct OWMData {
    uint32_t timestamp = 0; // millis, not realtime
    uint16_t weather_id;    // could use description instead

    float tempFeelsLike;
    float temp;
    float temp_min;
    float temp_max;

    float windSpeed;

    // sunrise/sunset ignored for now
};

OWMData weatherInfo;

namespace {
const uint32_t UPDATE_DELAY = 30 * 60 * 1000;

const char OWM_URL[] = "https://api.openweathermap.org/data/2.5/weather?q=%s&APPID=%s";
char URL[sizeof(OWM_URL) + 50]; // dynamically build from original URL; increased size to fit key and location

void buildURL() { snprintf(URL, sizeof(URL), OWM_URL, OWM_LOCATION_KEY, OWM_API_KEY); }

// TODO rewrite module to not use String!
String httpGETRequest(const char* serverName) {
    HTTPClient http;
    http.begin(serverName);
    int httpResponseCode = http.GET();

    String payload = "{}";

    if (httpResponseCode > 0) {
        print(F("HTTP Response code: "));
        printlnRaw((uint16_t)httpResponseCode);
        payload = http.getString();
    } else {
        print(F("Error code: "));
        printlnRaw((uint16_t)httpResponseCode);
    }
    // Free resources
    http.end();

    return payload;
}

StaticJsonDocument<820> doc;
void updateWeather() {
    println(F("Checking for weather update..."));
    String reply = httpGETRequest(URL);
    printlnRaw(reply);

    // WARNING: the strings in the input will be duplicated in the JsonDocument.
    DeserializationError error = deserializeJson(doc, reply);
    if (error) {
        print(F("deserializeJson() failed: "));
        println(error.f_str());
        return;
    }

    print(F("Size of received JSON: "));
    printlnRaw((uint16_t)doc.memoryUsage());
    JsonObject obj = doc.as<JsonObject>();

    weatherInfo.weather_id = obj[F("weather")][0][F("speed")];

    const JsonObject main = doc["main"];
    weatherInfo.tempFeelsLike = (float)main[F("feels_like")] - 273.15;
    weatherInfo.temp = (float)main[F("temp")] - 273.15;
    weatherInfo.temp_min = (float)main[F("temp_min")] - 273.15;
    weatherInfo.temp_max = (float)main[F("temp_max")] - 273.15;

    weatherInfo.windSpeed = (float)obj[F("wind")][F("speed")];
    weatherInfo.timestamp = millis();

    print(F("Now at "));
    printRaw(weatherInfo.tempFeelsLike);
    println(F("°C"));
}
} // namespace

void setup() {
    buildURL();
    print(F("Asking for weather from "));
    printlnRaw(URL);
}

uint32_t nextUpdate = 0;
void loop() {
    uint32_t now = millis();
    if (now < nextUpdate) return;
    nextUpdate = now + UPDATE_DELAY;
    updateWeather();
}

} // namespace WeatherService