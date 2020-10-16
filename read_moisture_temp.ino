#include <DHT.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

//WiFi Setup 
const char* ssid="Your_SSID";
const char* pswd="Your_Password";
//MQTT Setup
const char* mqtt_server="Your_MQTT_Server";
#define mqtt_port 1883

//Initial value 
float humid=0.0;
float temp=0.0;
float moisture=0.0;

//Timer var
long now=millis();
long lastMeasure=0;
//Define Temperature Sensor & Soil Sensor
#define DHTPIN D7
#define DHTTYPE DHT11
const int soil_sensor=A0;

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wiFi(){
  delay(10);
  WiFi.begin(ssid, pswd);
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(ssid);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnect(){
  Serial.print("Attempting MQTT Connection...");
  if(client.connect("ESP8266Client")){
    Serial.println("Connected");
  }
  else{
    Serial.print("MQTT Connection failed, rc=");
    Serial.print(client.state());
    Serial.println("reconnect in 5 seconds");
    delay(5000);
  }
}


void setup() {
  Serial.begin(115200);
  Serial.setTimeout(500);
  setup_wiFi();
  client.setServer(mqtt_server, mqtt_port);
  dht.begin();
}

void loop() {
  if(!client.connected()){
    reconnect();
  }

  if(!client.loop())
    client.connect("ESPClient8266");

  now=millis();
  //check timer for 3 second
  if(now - lastMeasure >10000){
    humid=dht.readHumidity();
    temp=dht.readTemperature();
    moisture=(100.00-((analogRead(soil_sensor)/1023.00)*100.00));

    if(isnan(humid)||isnan(temp)||isnan(moisture)){
      Serial.println("Failed to read sensor");
      return;
    }
  //Computes temperature value in Celsius
  static char temperatureTemp[7];
  dtostrf(temp, 6, 2, temperatureTemp);

  static char humidityTemp[7];
  dtostrf(humid, 6, 2, humidityTemp);

  static char moistureTemp[7];
  dtostrf(moisture, 6, 2, moistureTemp);

  //Publish Temperature, Humid and Moisture values
  client.publish("/sensor1/temperature", temperatureTemp);
  client.publish("/sensor1/humidity", humidityTemp);
  client.publish("/sensor1/moisture", moistureTemp);
    
  Serial.print("Temperature : ");
  Serial.print(temp);
  Serial.println();
  Serial.print("Humidity : ");
  Serial.println(humid);
  Serial.print("Soil Moisture: ");
  Serial.println(moisture);
  Serial.println();

  delay(3000);
  }
}
