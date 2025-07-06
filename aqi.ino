#include <HardwareSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "ESPAsyncWebServer.h"
#include <TinyGPSPlus.h>

// SDS011 on Serial1 (RX = GPIO22, TX = GPIO21)
HardwareSerial SDS_Serial(1);
// GPS on Serial2 (RX = GPIO16, TX = GPIO17)
HardwareSerial GPS_Serial(2);
TinyGPSPlus gps;

AsyncWebServer server(80);

float pm25 = 0;
float pm10 = 0;
double latitude = 0.0;
double longitude = 0.0;

const char* ssid = "Arpu";
const char* password = "arpu$sv29e_52k4";

void setup() {
  Serial.begin(115200);
  delay(1000);

  // SDS011 Serial
  SDS_Serial.begin(9600, SERIAL_8N1, 22, 21);
  Serial.println("SDS011 sensor started...");

  // GPS Serial
  GPS_Serial.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("GPS module started...");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected. IP: ");
  Serial.println(WiFi.localIP());

  // Webserver route
  server.on("/pmdata", HTTP_GET, [](AsyncWebServerRequest *request){
    String response = "{\"pm25\":" + String(pm25, 1) +
                      ",\"pm10\":" + String(pm10, 1) +
                      ",\"lat\":" + String(latitude, 6) +
                      ",\"lng\":" + String(longitude, 6) + "}";

    Serial.print("Client requested data: ");
    Serial.println(response);

    AsyncWebServerResponse *res = request->beginResponse(200, "application/json", response);
    res->addHeader("Access-Control-Allow-Origin", "*");
    request->send(res);
  });

  server.begin();
}

void loop() {
  // Read SDS011 Data
  if (SDS_Serial.available() >= 10) {
    if (SDS_Serial.read() == 0xAA && SDS_Serial.read() == 0xC0) {
      uint8_t data[6];
      SDS_Serial.readBytes(data, 6);
      SDS_Serial.read(); // checksum
      SDS_Serial.read(); // tail 0xAB

      pm25 = (data[0] + (data[1] << 8)) / 10.0;
      pm10 = (data[2] + (data[3] << 8)) / 10.0;

      Serial.println("---- SDS011 Data ----");
      Serial.print("PM2.5: "); Serial.print(pm25); Serial.println(" µg/m³");
      Serial.print("PM10 : "); Serial.print(pm10); Serial.println(" µg/m³");
    }
  }

  // Read GPS Data
  while (GPS_Serial.available()) {
    char c = GPS_Serial.read();
    gps.encode(c);
  }

  if (gps.location.isUpdated()) {
    latitude = gps.location.lat();
    longitude = gps.location.lng();

    Serial.println("---- GPS Data ----");
    Serial.print("Latitude : "); Serial.println(latitude, 6);
    Serial.print("Longitude: "); Serial.println(longitude, 6);
  }

  delay(1000);
}
