#include <ESP8266WiFi.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define GPS_TX_PIN 4
#define GPS_RX_PIN 5

const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";

SoftwareSerial gpsSerial(GPS_TX_PIN, GPS_RX_PIN);
TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
}

void loop() {
  // Read GPS data
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        double latitude = gps.location.lat();
        double longitude = gps.location.lng();
        // Send location data to server
        sendDataToServer(latitude, longitude);
      }
    }
  }
  
  delay(1000); // Adjust delay as needed to control GPS update frequency
}

void sendDataToServer(double latitude, double longitude) {
  // Create HTTP client object
  WiFiClient client;
  
  // Server URL
  const char* server = "yourserver.com";
  
  // Construct the URL with latitude and longitude parameters
  String url = "/update?lat=" + String(latitude, 6) + "&lon=" + String(longitude, 6);
  
  // Connect to server
  if (client.connect(server, 80)) {
    // Send HTTP GET request
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + server + "\r\n" +
                 "Connection: close\r\n\r\n");
    Serial.println("Request sent");
    
    // Wait for response
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  } else {
    Serial.println("Failed to connect to server");
  }
  
  client.stop(); // Close connection
}
