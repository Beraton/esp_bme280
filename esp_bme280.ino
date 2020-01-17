#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

const char* ssid = "ppi";
const char* password = "intel+tech";

const char* mqttServer = "10.3.1.109";
const int mqttPort = 1883;
const char* clientID = "ESP8266";
const char* mqttTopic = "esp/test";

WiFiClient espClient;
PubSubClient client(mqttServer, mqttPort, espClient);
Adafruit_BME280 bme;

void connect() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  unsigned long wifiConnectStart = millis();

  while (WiFi.status() != WL_CONNECTED) {
    if (WiFi.status() == WL_CONNECT_FAILED) {
      Serial.println("Failed to connect to WiFi");
      delay(10000);
    }
    delay(500);
    Serial.println("...");
    
    //Try to connect to wifi only for 5 seconds
    if (millis() - wifiConnectStart > 15000) {
      Serial.println("Failed to connect to WiFi");
      return;
    }
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //Initialize bme sensor
  bool status;

  status = bme.begin(0X76);

  if (!status) {
    Serial.println("Could not find a valid BME280 Sensor");
  }

  Serial.println("Device ready to use!");
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);

  //Waiting for Serial to initialize
  while (!Serial) { }

  Serial.println("Device started");

  connect();

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect(clientID)) {
      Serial.println("connected");  
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  String temp = String(bme.readTemperature());
  String hum = String(bme.readHumidity());
  String pres = String(bme.readPressure() / 100.0F);
  temp.remove(4);
  hum.remove(4);
  pres.remove(6);
  String msg = temp+","+hum+","+pres;
 
  Serial.println(msg);
  client.publish(mqttTopic, (char*) msg.c_str());
  

  delay(2000);
  ESP.deepSleep(20e6);
}

void loop() {
  client.loop();
}
