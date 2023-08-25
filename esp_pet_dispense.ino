#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

const char* ssid = "Robot 2.4G";
const char* password = "Rizvy136060";
const char* user_id = "0";
const char* authorizationToken = "your-api-key-190"; // Replace with your actual authorization token

const char* mqttServer = "4.213.73.200";
const int mqttPort = 1883;
const char* mqttUser = "on_esp32";
const char* mqttPassword = "espesp@90";

String mqttTopic_ = "user/" + String(user_id) + "/" + String(authorizationToken) + "/commands";

// Convert the String to const char*
const char* mqttTopic = mqttTopic_.c_str();

// String serverUrl = "https://ibrahimovi.tech/tasks/check/" + String(user_id);

// SSL fingerprint of the server's certificate
// const char* fingerprint = "85:A7:3D:F0:C3:82:8C:8D:CC:A4:BD:E2:19:10:5F:D2:9A:EB:6C:BC";

int a = D0;
int b = D1;
int c = D2;
bool dispense_now = false;

WiFiClientSecure secure_client;  // Use WiFiClientSecure for HTTPS
WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup() {
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);

  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);
  
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  connectToMQTT();
  // secure_client.setFingerprint(fingerprint);
  
  Serial.println("Connected to WiFi");
}

void connectToMQTT() {
    while (!mqttClient.connected()) {
        if (mqttClient.connect("ESP32Client", mqttUser, mqttPassword)) {
            Serial.println("Connected to MQTT broker");
            mqttClient.subscribe(mqttTopic);
        } else {
            Serial.print("Failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" Retrying in 5 seconds...");
            delay(5000);
        }
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.println("Message arrived on topic: " + String(topic));
    // Convert payload to string
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println("Payload: " + message);
    // Execute the action based on the received message
    if (message == "dispense_now") {
        digitalWrite(a, HIGH);
        digitalWrite(b, HIGH);
        digitalWrite(c, HIGH);
        Serial.println("dispense_now");
    }
}

void loop() {
  // std::unique_ptr<BearSSL::WiFiClientSecure> secure_client(new BearSSL::WiFiClientSecure);
  // secure_client->setFingerprint(fingerprint);
  // secure_client->setInsecure();

  if (!mqttClient.connected()) {
    connectToMQTT();
  }
  mqttClient.loop();

  if (dispense_now == true) {
    digitalWrite(a, HIGH);
    digitalWrite(b, HIGH);
    digitalWrite(c, HIGH);
    Serial.println("High");
  } else {
    digitalWrite(a, LOW);
    digitalWrite(b, LOW);
    digitalWrite(c, LOW);
    Serial.println("Low");
  }

  dispense_now = false;

  delay(10000);  // Delay for 30 seconds (30,000 milliseconds)
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Sending HTTP GET request...");
    
    HTTPClient http;
    http.begin(*secure_client, serverUrl);
    
    http.addHeader("Authorization", authorizationToken); // Add the authorization header
    
    int httpResponseCode = http.GET();
    if (httpResponseCode == HTTP_CODE_OK) {s
      String payload = http.getString();
      
      DynamicJsonDocument jsonDoc(1024);
      DeserializationError error = deserializeJson(jsonDoc, payload);
      
      if (!error) {
        bool server_dispense_now = jsonDoc["dispense_now"];
        if (server_dispense_now) {
          dispense_now = true;
          Serial.println("Dispensing now.");
        }
      } else {
        Serial.println("JSON parsing error");
      }
    } else {
      Serial.print("HTTP GET request failed, error code: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}
