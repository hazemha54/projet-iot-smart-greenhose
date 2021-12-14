#include "DHT.h"
#include "PubSubClient.h" // Connect and publish to the MQTT broker

// Code for the ESP32
#include "WiFi.h" // Enables the ESP32 to connect to the local network (via WiFi)
#define DHTPIN 19  // Pin connected to the DHT sensor
#include <ESP32Servo.h>
//-----------------------------------
Servo myservo;



#define DHTTYPE DHT11  // DHT11 or DHT22
DHT dht(DHTPIN, DHTTYPE);

// WiFi
const char* ssid = "Infinix HOT 8";                 // Your personal network SSID
const char* wifi_password = "54101071"; // Your personal network password

// MQTT
const char* mqtt_server = "10.0.2.15";  // IP of the MQTT broker
const char* humidity_topic = "iot/humidity";
const char* temperature_topic = "iot/temperature";
const char* hsol_topic = "iot/hsol" ;
const char* mqtt_username = "hazem"; // MQTT username
const char* mqtt_password = "hazem"; // MQTT password
const char* clientID = "iot"; // MQTT client ID

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient); 


// Custom function to connet to the MQTT broker via WiFi
void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}
//--------------------------------------------------------------------------

int PinAnalogiqueHumidite=2;       //Broche Analogique de mesure d'humidité
int PinNumeriqueHumidite=4;        //Broche Numérique mesure de l'humidité
int pump =18;    //LED témoin de seuilde  sécheresse
int angle = 0;         // the current angle of servo motor
int pos = 0; //srvo

int hsol;  //Humidite su sol, mesure analogique
int secheresse;  //0 ou 1 si seuil atteint

//-----------------------------------------------------------------------------



void setup() {
  Serial.begin(115200);
  pinMode(pump, OUTPUT);  //pomp
  delay(100);
  Serial.println(F("DHTxx test!"));
  dht.begin();
      pinMode(PinAnalogiqueHumidite, INPUT);       // analogique
    pinMode(PinNumeriqueHumidite, INPUT); //dig
  
  
}

void loop() {
  connect_MQTT();
  Serial.setTimeout(2000);
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
   if (isnan(h) || isnan(t) ) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hsol = analogRead(PinAnalogiqueHumidite); // Lit la tension analogique
  secheresse = analogRead(PinNumeriqueHumidite);
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C");
  Serial.println("hsolhumudity= "); // afficher la mesure
  Serial.print(hsol);
  Serial.println(secheresse);  //0 ou 1 si le seuil est dépassé
  

  // MQTT can only transmit strings
  String hs="Hum: "+String((float)h)+" % ";
  String ts="Temp: "+String((float)t)+" C ";
  String husol="husol: "+String((float)hsol)+"%";

  // PUBLISH to the MQTT Broker (topic = Temperature, defined at the beginning)
  if (client.publish(temperature_topic, String(t).c_str())) {
    Serial.println("Temperature sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Temperature failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(temperature_topic, String(t).c_str());
  }

  // PUBLISH to the MQTT Broker (topic = Humidity, defined at the beginning)
  if (client.publish(humidity_topic, String(h).c_str())) {
    Serial.println("Humidity sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Humidity failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(humidity_topic, String(h).c_str());
  }
  // PUBLISH to the MQTT Broker (topic = Humidity, defined at the beginning)
  if (client.publish(hsol_topic, String(hsol).c_str())) {
    Serial.println("Humidity sol sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Humidity failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(hsol_topic, String(hsol).c_str());
  }
  client.disconnect();  // disconnect from the MQTT broker
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  if (hsol =0){
    digitalWrite(pump,HIGH);
    delay(5000);
    digitalWrite(pump,LOW);
    Serial.println("pump on") ;
    } 
     else  {
    digitalWrite(pump,LOW);
    Serial.println("pump off") ;
        
      }
  
  delay(5000);       
}