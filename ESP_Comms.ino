/*
    Code to establish communication between Android app and the machine
  
    Code names:
      W - WiFi:
        0 - connecting
        1 - connected
      C - client:
        0 - disconnected
        1 - connected
      A - stop the process
      Z - run the process
      P - parameters
      J - jog to
      R - reset coordinate
      * - message
      # - replica of a message to Serial Monitor (no need to process it)
  
    Author: Askar Idrisov
*/

#include <ESP8266WiFi.h>

const char* AP_SSID = "YOUR SSID GOES HERE";
const char* AP_PASSWORD = "YOUR PASSWORD GOES HERE";
const int SERVER_PORT = 2048;

const byte STOP_PIN = 2;

boolean processRunning = false;

WiFiServer server(SERVER_PORT);

void setup() {
  pinMode(STOP_PIN, OUTPUT);
  Serial.begin(115200);

  WiFi.begin("DA", "iA10243546K");

  Serial.print("*W0\n");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  server.begin();
  
  Serial.print("** Connected to WiFi. IP: ");
  Serial.print(WiFi.localIP().toString());
  Serial.print('\n');
  
  Serial.print("*W1\n");
}

void loop() {
  WiFiClient client = server.accept();
  if (client) {
    Serial.print("*C1\n");
    client.print("** Connected to machine\n");
    
    while (client.connected()) {
      // Handle messages from the app
      if (client.available()) {
        String data = client.readStringUntil('\n');

        if (data.charAt(0) == '*') {
          switch (data.charAt(1)) {
            case 'A':
              digitalWrite(STOP_PIN, HIGH);
              delay(1000);
              digitalWrite(STOP_PIN, LOW);
              processRunning = false;
              break;

            case 'Z':
              Serial.print(data);
              Serial.print('\n');
              processRunning = true;
              break;
              
            default:
              if (processRunning) {
                client.print("** Error: process is running!\n");
              }
              else {
                Serial.print(data);
                Serial.print('\n');
              }
          }
        }
      }

      // Handle messages from Arduino
      if (Serial.available()) {
        client.print(Serial.readStringUntil('\n'));
        client.print('\n');
      }
    }

    client.stop();
    Serial.print("*C0\n");
  }
}
