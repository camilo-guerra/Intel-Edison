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
#define DEVICE_LABEL "edison-ultrasonic" // Your device label
#define VARIABLE_LABEL_1 "ultrasonic"// Your variable label (2)
#define VAR_SUBSCRIBE "relay" // Your variable label for subscribing

#define MQTT_SERVER "things.ubidots.com"
#define RELAY 3 
#define PinTrig 7   //pin para generar la señal
#define PinEcho 8  //pin para el la lectura de la señal

char topic[150];

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
    pinMode(RELAY, OUTPUT);
    pinMode(PinTrig, OUTPUT); 
    pinMode(PinEcho , INPUT);
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
   
   float duracion;
   float distancia;
   digitalWrite(PinEcho, LOW);
   delayMicroseconds(2); //espera 2 microsegundos para seguir ejecutando el sketch
   digitalWrite(PinTrig, HIGH);

   delayMicroseconds(8); //espera 8 microsegundos para emular el los ciclos del sonic burst
   digitalWrite(PinTrig, LOW);
   duracion = pulseIn(PinEcho, HIGH); //PulseIn toma el tiempo que tarda entre pasar de LOW a HIGH o al contrario en un pin
   distancia = (duracion/2) / 29.1; //formula para generar al distancia en centimetros

 //condicionales para establecer limites
  if (distancia >= 300 || distancia <= 0){
    Serial.println("Fuera de Rango");
     sprintf(payload, "{\"%s\": %f}", VARIABLE_LABEL_1,"Fuera de Rango");    
    client.publish(topic, payload);
   //Serial.println((topic, payload));    
     }
  else {
    sprintf(payload, "{\"%s\": %f}", VARIABLE_LABEL_1,distancia);    
    client.publish(topic, payload);
    Serial.print("La distacia es de ");
    Serial.print(distancia);
    Serial.println(" cm");
  }
  
    free(payload);
    client.loop();
    delay(2000);
}

