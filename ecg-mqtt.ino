/*

     Developer : Srijan Singh
 
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

int outputpin= A0;

// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "Nokia 5.1 Plus";
const char* password = "12345678";

//temp
float tempC;
int reading;
int tempPin = 0;


// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "broker.emqx.io";

// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient espClient22;
PubSubClient client(espClient22);


// Lamp - LED - GPIO 4 = D2 on ESP-12E NodeMCU board
const int led = LED_BUILTIN;


//Connect your NodeMCU to your router
void setup_wifi() {
  delay(10);
  
  Serial.println();
 
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - NodeMCU IP address: ");
  Serial.println(WiFi.localIP());
}

// This functions is executed when some device publishes a message to a topic that your NodeMCU is subscribed to

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageInfo;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageInfo += (char)message[i];
  }
  Serial.println();


  // If a message is received on the topic room/lamp, you check if the message is either on or off. Turns the lamp GPIO according to the message
  if(topic=="temp_parul" and messageInfo == "show"){
      int analogValue = analogRead(outputpin);
      float millivolts = (analogValue/1024.0) * 3300; //3300 is the voltage provided by NodeMCU
      float celsius = (millivolts/10)-6;
      Serial.print("in DegreeC=   ");
      int c = int(celsius);
      Serial.println(c);
      
      //---------- Here is the calculation for Fahrenheit ----------//
      
      float fahrenheit = ((celsius * 9)/5 + 32);
      Serial.print(" in Farenheit=   ");
      
      int f = (int) fahrenheit;
      Serial.println(f);

      char cels_data[10];
      char farn_data[10];

      sprintf (cels_data, "%d", c);
      sprintf (farn_data, "%d", f);

       client.publish("temp_parul", cels_data);
       client.publish("temp_parul", farn_data);
    }
  
  Serial.println();
}


// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    
    if (client.connect("ESP8266Client22")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("temp_parul");
     
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// The setup function sets your ESP GPIOs to Outputs, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually controls the LEDs
void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

// For this project, you don't need to change anything in the loop function. Basically it ensures that the NodeMCU is connected to MQTT broker
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client22");

  }