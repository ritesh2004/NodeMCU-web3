#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

#define SERVO_PIN 2
#define RED_PIN 14
#define GREEN_PIN 12
#define BLUE_PIN 13

Servo myServo;

const char* ssid = "Ritesh";
const char* password = "ritesh2004";

WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("[WSc] Disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("[WSc] Connected to server");
      webSocket.sendTXT("Hello from ESP8266");
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] Got data: %s\n", payload);
      String message = "";
      for (size_t i = 0; i < length; i++) {
        message += (char)payload[i];
      }
      onWebSocketMessage(message);  // Now it's a proper String
      break;
  }
}

void onWebSocketMessage(String message) {
  // Create a StaticJsonDocument
  StaticJsonDocument<200> doc;

  // Deserialize the JSON
  DeserializationError error = deserializeJson(doc, message);
  
  if (error) {
    Serial.print("JSON Parsing failed: ");
    Serial.println(error.c_str());
    return;
  }


  // Access the values
  int servo = doc["servo"];
  const String led = doc["led"];

  // Print values
  Serial.print("Servo: ");
  Serial.println(servo);
  
  Serial.print("LED: ");
  Serial.println(led);
  
  myServo.write(servo);
  
  if (led == "RED"){
    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, 255);
    analogWrite(BLUE_PIN, 255);
  }
  else if (led == "GREEN"){
    analogWrite(RED_PIN, 255);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 255);
  }
  else if (led == "BLUE"){
    analogWrite(RED_PIN, 255);
    analogWrite(GREEN_PIN, 255);
    analogWrite(BLUE_PIN, 0);
  }
  else {
    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 0);
  }
  // Example usage
  // setServoAngle(servo);
  // setLEDColor(led); // You'll define this based on your hardware
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.RSSI()); // Check signal strength


  webSocket.begin("192.168.1.4", 3000, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000); // 5s reconnect if disconnected

  myServo.attach(SERVO_PIN);
  myServo.write(0);
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  webSocket.loop();

  // analogWrite(RED_PIN, 255);
  // analogWrite(GREEN_PIN, 255);
  // analogWrite(BLUE_PIN, 0);
  // Serial.println("RED");
  // delay(2000);
}
