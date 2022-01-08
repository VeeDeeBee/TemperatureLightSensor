#include <Arduino.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Credentials.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>

DHT dht(DHTPIN, DHTTYPE); 

// ESP8266WebServer server(80);

WiFiClient espClient;
PubSubClient mqttClient(espClient);

int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value

String ip;
bool online;
int count;

String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ; 
}

void mqttReconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("outdoorSensorClient")) {
      Serial.println("connected");  
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// void handle_temperature() {
//   Serial.println();
//   Serial.println("Requesting temperature");

//   float temp = dht.readTemperature();
 
//   Serial.print("Temperature = ");
//   Serial.print(temp);
//   server.send(200, "text/html", String(temp)); 
// }

// void handle_humidity() {
//   Serial.println();
//   Serial.println("Requesting temperature");

//   float hum = dht.readHumidity();
 
//   Serial.print("Humidity = ");
//   Serial.print(hum);
//   server.send(200, "text/html", String(hum)); 
// }

// void handle_brightness() {
//   Serial.println();
//   Serial.println("Requesting brightness");

//   int brightness = analogRead(A0);
 
//   Serial.print("Brightness = ");
//   Serial.print(brightness);
//   server.send(200, "text/html", String(brightness)); 
// }

void setup()
{
 // Serial.begin(9600);
  
 // Serial.println("Connecting to ");
 // Serial.println(ssid);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
 //   Serial.print(".");
    delay(500);
  }

 // Serial.println();

  count = 0;

  if (WiFi.status() == WL_CONNECTED) {
  //  Serial.println("Connected");
    ip = IpAddress2String(WiFi.localIP());
  //  Serial.println("IP address: ");
  //  Serial.println(ip);
  //  Serial.println("Starting up...");
    online = true;

    if (!MDNS.begin("outdoorsensor")) 
    {  
      true;          
    //  Serial.println("Error setting up MDNS responder!");
    }
    else 
    {
      true;
      //Serial.println("mDNS responder started");
    }
  }

  dht.begin();

  // server.on("/temperature", handle_temperature);
  // server.on("/humidity", handle_humidity);
  // server.on("/brightness", handle_brightness);

  // server.begin();
  // Serial.println("HTTP server started");

  mqttClient.setServer(mqtt_broker, mqtt_port);

}


void loop()
{

  MDNS.update();

  Serial.println("Awake");

  // if (count == 7200) {
  //   int i = 0;
  //   while (i < 60 && WiFi.status() != WL_CONNECTED) {
  //     delay(500);
  //     Serial.print(".");
  //     i++;
  //   }
  //   count = 0;
  // } else {
  //   count++;
  // }
  // server.handleClient();

  if (!mqttClient.connected()) {
    mqttReconnect();
  }

  // if(!mqttClient.loop())
  // {
  //   mqttClient.connect("ESP8266Client");
  // }
    // Publishes Temperature and Humidity values

  static char temp[7];
  dtostrf(dht.readHumidity(),6,2,temp);
  mqttClient.publish("outside/humidity", temp);

  dtostrf(dht.readTemperature(),6,2,temp);
  mqttClient.publish("outside/temperature", temp);

  dtostrf(analogRead(A0),6,2,temp);
  mqttClient.publish("outside/brightness", temp);

  Serial.println("published!");

  delay(500);

  Serial.println("Going to sleep");

 // ESP.deepSleep(5e6); 

  ESP.deepSleep(300e6); 

}