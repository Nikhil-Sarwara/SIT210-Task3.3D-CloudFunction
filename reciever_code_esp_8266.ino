#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Arduino.h>

// Wifi
const char* ssid = "FTTH";
const char* password = "9253045036@n";

// MQTT Broker Details
String device_id = "Device002";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_user = "Device002";
const char* mqtt_password = "Device002";
const char* mqtt_clientId = "clientId-gZwBY51kvl";
const char* topic_subscribe = "MotionSensorData_nano_example";

WiFiClient esp_client;
void callback(char* topic, byte* payload, unsigned int length);
PubSubClient mqtt_client(mqtt_server, mqtt_port, callback, esp_client);

void setup()
{
  Serial.begin(115200);
  pinMode(14, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(4, OUTPUT);
  delay(5000);
  Serial.println("Welcome to HIVE MQ BROKER");
  setup_wifi();
  mqtt_connect();
}

void loop()
{
  if(!mqtt_client.loop())
  mqtt_connect();
}

void setup_wifi()
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println("\"" + String(ssid) + "\"");

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqtt_connect()
{
  // Loop until we're connected
  while(!mqtt_client.connected())
  {
    Serial.println("Attempting MQTT connection...");

    // Attempt to connect
    // 
    if (mqtt_client.connect(mqtt_clientId))
    {
      Serial.println("MQTT Client Connected");
      // Subscribe
      mqtt_client.subscribe(topic_subscribe);
    } else
    {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      // Wait for 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqtt_subscribe(const char * topic)
{
  if(mqtt_client.subscribe(topic))
  Serial.println("Subscribe \"" + String(topic) + "\" ok");
  else 
  Serial.println("Subscribe \"" + String(topic) + "\" failed");
}

void callback(char* topic, byte* payload, unsigned int length)
{
  String data;
  for (int i = 0; i < length; i++)
  {
    data += (char)payload[i];
  }
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  Serial.println(data);

  if(data == "wave")
  {
    wave_blink();
  }
  if(data == "pat")
  {
    pat_blink();
  }
}

void wave_blink()
{
  for(int i = 0; i < 3; i++)
  {
    digitalWrite(14, HIGH);
    delay(500);
    digitalWrite(14, LOW);
    delay(500);
  }
}

void pat_blink()
{
  for(int i = 0; i < 2; i++)
  {
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(4, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(4, LOW);
    delay(100);
  }
}
