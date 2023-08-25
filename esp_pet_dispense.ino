#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#include <ESP8266WebServer.h>
#include <EEPROM.h>

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

int a = D0;
int b = D1;
int c = D2;
bool dispense_now = false;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

bool configSaved = false;
int config_mode_led = D6;
int wifi_connected_led = D7;
int button_pin = D8;
int adcValue = 0;
int apStarted = 0;
ESP8266WebServer server(80);

void handleRoot() {
  if (!configSaved) {
    String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=2.0' /><style>body {background-color: #946ceb}</style></head><body>";
    html += "<form method='post' action='/save'>";
    html += "SSID: <input type='text' name='ssid'><br>";
    html += "Password: <input type='text' name='password'><br>";
    html += "<input type='submit' value='Save'>";
    html += "</form></body></html>";
    server.send(200, "text/html", html);
  } else {
    server.send(200, "text/html", "Configuration saved. Restart device.");
  }
}

void handleSave() {
  String newSSID = server.arg("ssid");
  String newPassword = server.arg("password");

  digitalWrite(config_mode_led, LOW);

  // Save to EEPROM
  EEPROM.begin(512);
  writeString(0, newSSID);
  writeString(32, newPassword);
  EEPROM.commit();
  EEPROM.end();

  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
  delay(1000); // Give time to redirect
  ESP.restart();
}

void setup() {
  Serial.begin(9600);

  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);

  pinMode(config_mode_led, OUTPUT);
  pinMode(wifi_connected_led, OUTPUT);

  pinMode(button_pin, INPUT);

  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);
  
  // Check if config is saved in EEPROM
  EEPROM.begin(512);
  String savedSSID = readString(0);
  String savedPassword = readString(32);
  EEPROM.end();
  
  if (savedSSID.length() > 0) {
    configSaved = true;
    Serial.println("Connecting to saved WiFi...");
    Serial.println(savedSSID);
    Serial.println(savedPassword);
    WiFi.begin(savedSSID.c_str(), savedPassword.c_str());

    int retries = 0;

    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
      retries++;

      if(retries > 9){
        break;
      }
    }
    
    if(retries > 9){
       Serial.println("Failed to connect to WiFi");
    } else {
      digitalWrite(wifi_connected_led, HIGH);
      Serial.println("Connected to WiFi");
    }
  } 
  else {
    Serial.println("No saved WiFi configuration found.");
  }
  
  // connectToMQTT();
  // Serial.println("Connected to WiFi");
}

void start_ap_mode(){
  if(apStarted == 0){

    for (int i = 0 ; i < EEPROM.length() ; i++) {
      EEPROM.write(i, 0);
    }

    apStarted = 1;
    configSaved = false;
    digitalWrite(wifi_connected_led, LOW);
    digitalWrite(config_mode_led, HIGH);
    WiFi.mode(WIFI_AP);
    WiFi.softAP("Pet-Feeder", "12345678"); // Set your AP name and password
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    server.on("/", handleRoot);
    server.on("/save", handleSave);
    server.begin();
  }
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
        delay(10000);
    }
}

void loop() {
  adcValue = digitalRead(button_pin);

  if(adcValue == 1){
    Serial.println("(adcValue == 1");
    apStarted = 0;
    start_ap_mode();
  }

  if(!configSaved){
    server.handleClient();
  } else {
    if (WiFi.status() == WL_CONNECTED) {
      if (!mqttClient.connected()) {
        connectToMQTT();
      }
      mqttClient.loop();

      digitalWrite(a, LOW);
      digitalWrite(b, LOW);
      digitalWrite(c, LOW);
      Serial.println("Low");
      delay(500);

    } else {
      Serial.println("Wifi not Connected...");
    }
  }
  
}

//EEPROM
void writeString(char index,String data)
{
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++)
  {
    EEPROM.write(index+i,data[i]);
  }
  EEPROM.write(index+_size,'\0');   //Add termination null character for String Data
  EEPROM.commit();
}


String readString(char index)
{
  int i;
  char data[100]; //Max 100 Bytes
  int len=0;
  unsigned char k;
  k=EEPROM.read(index);
  while(k != '\0' && len<500)   //Read until null character
  {    
    k=EEPROM.read(index+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}
