#include <PubSubClient.h>
#include <WiFiNINA.h>
#include <SPI.h>
#include <Arduino.h>

// Wifi
const char* ssid = "FTTH";
const char* password = "9253045036@n";

// MQTT Broker Details
String device_id = "Device001";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_clientId = "clientId-gZwBY51kul";
const char* topic_publish = "MotionSensorData_nano_example";
const char* topic_subscribe = "MotionSensorData_nano_CommandRequest";

WiFiClient esp_client;
void callback(char* topic, byte* payload, unsigned int length);
PubSubClient mqtt_client(mqtt_server, mqtt_port, callback, esp_client);

// Data Sending Time
unsigned long CurrentMillis, PreviousMillis, DataSendingTime = (unsigned long) 5000;

// Time to check if its a wave
unsigned long CurrentMillis1, PreviousMillis1, CheckingTime = (unsigned long) 2000;

// Variable
byte lightStatus;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A0, INPUT);
  delay(5000);
  Serial.println("MADE BY NIKHIL");
  setup_wifi();
  mqtt_connect();
}

void loop() {
  float sensor_input = digitalRead(A0);
  bool check = false;
  bool termination = true;
  String sensor_data = "";

  // Checking if its a wave or pat
  if (sensor_input) {
    delay(8000);
    sensor_input = digitalRead(A0);
    if (sensor_input) 
    {
      sensor_data = "wave";
      check = true;
    }
    if (!sensor_input && !check) 
    {
      sensor_data = "pat";
      check = false;
    }
  } else sensor_data = "none";
  

  // Devices State Sync Request
  CurrentMillis = millis();
  if (CurrentMillis - PreviousMillis > DataSendingTime) {
    PreviousMillis = CurrentMillis;

    // Verify that data is being sent
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);

    // Publish Sensor Data
    mqtt_publish((char*)sensor_data.c_str());
  }

  if (!mqtt_client.loop())
    mqtt_connect();
}

void setup_wifi() {
  Serial.print("\nConnecting to ");
  Serial.println("\"" + String(ssid) + "\"");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
}

void mqtt_connect() {
  // Loop until we're connected
  while (!mqtt_client.connected()) {
    Serial.println("Attempting MQTT connection...");

    // Attempt to connect
    if (mqtt_client.connect(mqtt_clientId)) {
      Serial.println("MQTT Client Connected");
      mqtt_publish((char*)("Hi from " + device_id).c_str());
      // Subscribe
      mqtt_subscribe(topic_subscribe);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      // Wait for 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqtt_publish(char* data) {
  mqtt_connect();
  if (mqtt_client.publish(topic_publish, data))
    Serial.println("Publish \"" + String(data) + "\" ok");
  else
    Serial.println("Publish \"" + String(data) + "\" failed");
}

void mqtt_subscribe(const char* topic) {
  if (mqtt_client.subscribe(topic))
    Serial.println("Subscribe \"" + String(topic) + "\" ok");
  else
    Serial.println("Subscribe \"" + String(topic) + "\" failed");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String command;
  Serial.print("\n\nMessage arrived [");
  Serial.print(topic);
  Serial.println("] ");
  for (int i = 0; i < length; i++) {
    command += (char)payload[i];
  }
  if (command.length() > 0) {
    Serial.println("Command Receive is: " + command);
  }
}
