/******************************************************************************
MQTT_ESP32_Code.ino v.1.2.
Arduino code for subscribing to MQTT topic for recieving (or publish) messages
Date: 2/17/2021
Authors: 

Michael Guske, Oregon State University
Alex Wende, SparkFun Electronics

This code was created to use an ESP32 as an MQTT client. Specifically,
the ESP32 was purposed to rely published G-code commands to 
an Arduino Nano running GRBL on it. The ESP32, after sending a command, also transmits
the GRBL board's response back to the broker. The broker used for our project is 
able to the public for use.
******************************************************************************/

#include <WiFi.h>
#include <PubSubClient.h>

const char *WifiName =  "Enter Name Here";   // name of your WiFi network
const char *password =  "i.e. Password123"; // password of the WiFi network

const char *ID = "Examp";  // Name of our device, must be unique
const char *TOPIC = "ESP32"; // Topic to subcribe to
const char *reply_to = "resp";


WiFiClient wclient;
PubSubClient client(wclient); // Setup MQTT client

void respond(){
 delay(100);
 String temp;
 temp = Serial.readString(); //read response from grbl board
 char reply[temp.length()];
 temp.toCharArray(reply, temp.length()); //turn string into char array

 client.publish(reply_to, reply); //send back to broker

}
void callback(char* topic, byte* payload, unsigned int length){
 String response;
 if((char)payload[0] == ' '){
  return;
 }
 for (int i = 0; i < (length); i++) {
  response += (char)payload[i]; //read in MQTT published message
 }
 if(response == "end"){ // check list of condintions for special commands
  client.publish(reply_to,"Goodbye\\");
  exit(0);
 }
 else if(response == "Go"){
  client.publish(reply_to, "Beavs!\\");
 }
 else if(response == "PSMON") {
  Serial.print("M8\n"); //M8 rides pin A3 high
  respond();
 }

  else if(response == "PSMOFF") {
    Serial.print("M9\n"); //M9 rides pin A3 low
    respond();
  }
  else if(response == "M6") {
  Serial.print("M3S0\n"); 
  delay(2000); //delay time can be changed
  Serial.print("S255\n");
  respond(); 
  }
  else{
    Serial.print(response + '\n');
    respond();
  }
  
}

void setup_wifi(){
 
 WiFi.begin(WifiName, password); // Connect to network

 while (WiFi.status() != WL_CONNECTED){ // Wait for connection
  delay(1000);
 }
}

// Reconnect to client
void reconnect(){
 // Loop until we're reconnected
 while (!client.connected()){
 // Attempt to connect
  if(client.connect(ID)){
   client.subscribe(TOPIC);
  } 
  else{
  // Wait 5 seconds before retrying
   delay(5000);
  }
 }
}

void setup() {
  Serial.begin(115200); // Start serial communication
  delay(100);
  setup_wifi(); // Connect to network
  client.setServer("broker.hivemq.com", 1883); //public server used as broker
  client.setCallback(callback);// Initialize the callback routine
}

void loop() {
 if (!client.connected()){  // Reconnect if connection is lost
    reconnect();
 }

 client.loop();
}
