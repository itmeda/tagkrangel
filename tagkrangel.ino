#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

const char* ssid = "xxx";
const char* password = "xxx";

const int ledPin = 21;

// URL for the departure information
const char* URL2 = "https://transport.integration.sl.se/v1/sites/9500/departures";

HTTPClient http;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  Serial.print("CONNECTED to SSID: ");
  Serial.println(ssid);

  // setup pin 5 as a digital output pin
  pinMode (ledPin, OUTPUT);
}

void loop() {

  // Fetch departure information
  fetchDepartureInfo();

  delay(300000);

}

void fetchDepartureInfo() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(URL2);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      
      // Parse JSON payload
      DynamicJsonDocument doc(16000);
      DeserializationError error = deserializeJson(doc, payload);

      const char* message = "";


      if (!error) {
        JsonArray departures = doc["departures"];
        for (JsonObject departure : departures) {
          const char* mode = departure["line"]["transport_mode"];

          if (strcmp(mode, "TRAIN") == 0) {

            const char* line = departure["line"]["designation"];
            const char* destination = departure["destination"];
            const char* expected = departure["expected"];
            Serial.print(line);
            Serial.print(" ");
            Serial.print(destination);
            Serial.print(" ");
            Serial.println(expected);
            if (departure["deviations"].size() > 0) {

              message = departure["deviations"][0]["message"];

              Serial.println(message);
            }
          }
        }
        
        size_t length = strlen(message);

        if (length == 0) {
          Serial.println("The string is empty or has zero length.");
          digitalWrite (ledPin, LOW);  // turn off the LED
        } else {
          Serial.print("The length of the string is: ");
          Serial.println(length);
          digitalWrite (ledPin, HIGH);  // turn on the LED
        }          
        
      } else {
        Serial.print("JSON deserialization failed: ");
        Serial.println(error.c_str());
      }
    } else {
      Serial.print("HTTP GET request failed, error: ");
      Serial.println(http.errorToString(httpCode));
    }
    http.end();
  } else {
    Serial.println("Not connected to WiFi");
    loop1();
  }
}

void loop1() {
  digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
