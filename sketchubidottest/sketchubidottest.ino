/****************************************
 * Include Libraries
 ****************************************/
#include <PubSubClient.h>
#include <SPI.h>
#include <WiFi.h>

/****************************************
 * Define Your Wi-Fi crendetials
 ****************************************/
char ssid[] = "GONZALEZ"; //  Your network SSID (name)
char password[] = "42683089";    // Your network password (use for WPA, or use as key for WEP)

/****************************************
 * Define Constants
 ****************************************/
#define TOKEN "b8wApPq5HbTvWkgCPFV3DPjcbNhyRL" // Your Ubidots TOKEN
#define DEVICE_LABEL "edison" // Your device label
#define VARIABLE_LABEL_1 "sensor-1"// Your variable label (1)
#define VARIABLE_LABEL_2 "sensor-2"// Your variable label (2)
#define VAR_SUBSCRIBE "relay" // Your variable label for subscribing

#define MQTT_SERVER "things.ubidots.com"
#define sensor1 A0
#define sensor2 A1
#define RELAY 3 

char topic[150];

float sensorValue1;
float sensorValue2;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

/****************************************
 * Auxiliar Functions
 ****************************************/
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0]=='1'){
    digitalWrite(RELAY, HIGH);
    //digitalWrite(LED, HIGH);
  }
  else{
    digitalWrite(RELAY, LOW);
    //digitalWrite(LED, LOW);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("intel-edison", TOKEN,"")) {
      Serial.println("connected");
      client.subscribe("/v1.6/devices/"DEVICE_LABEL"/"VAR_SUBSCRIBE"/lv");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

/****************************************
 * Main Functions
 ****************************************/
void setup() {
    Serial.begin(115200);
    pinMode(sensor1, INPUT);
    pinMode(sensor2, INPUT);
    pinMode(RELAY, OUTPUT);
    delay(10);
    
     // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
  
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    client.setServer(MQTT_SERVER, 1883);
    client.setCallback(callback);
    sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);    
    client.subscribe("/v1.6/devices/"DEVICE_LABEL"/"VAR_SUBSCRIBE"/lv"); 

}

void loop() {
    if (!client.connected()) {
      reconnect();
      client.subscribe("/v1.6/devices/"DEVICE_LABEL"/"VAR_SUBSCRIBE"/lv"); 
    }
    char* payload = (char *) malloc(sizeof(char) * 100);
    sensorValue1 = analogRead(sensor1);
    sensorValue2 = analogRead(sensor2);
    
    sprintf(payload, "{\"%s\": %f}", VARIABLE_LABEL_1, sensorValue1);    
    client.publish(topic, payload);
    //Serial.println((topic, payload));
    Serial.print("Sensor Value 1: ");
    Serial.println(sensorValue1);
      
    sprintf(payload, "{\"%s\": %f}", VARIABLE_LABEL_2, sensorValue2);    
    client.publish(topic, payload);
    //Serial.println((topic, payload));    
    Serial.print("Sensor Value 2: ");
    Serial.println(sensorValue2);

    free(payload);
    client.loop();
    delay(5000);
}

