#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
const char* ssid = "nbz_iot";
const char* password =  "jj4dnakfg";
const char* mqttServer = "192.168.1.250";
const int mqttPort = 1883;
const char* mqttUser = "bruno";
const char* mqttPassword = "dnakfg";

WiFiClient espClient;
PubSubClient client(espClient);

ADC_MODE(ADC_VCC);

void setup()  
{
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("back_door", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  Serial.println("Reed Acionado");
  // Obtendo calculando e formatando a VCC
  
  
  float vdd = ESP.getVcc() / 766.0;
  String vdd_str = String(vdd);
  int vdd_crude = ESP.getVcc();
  String vdd_str_crude = String(vdd_crude);
   
  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"batt\":"; payload += vdd_str; payload += ",";
  payload += "\"stat\":"; payload += "\"Activated\""; payload += ",";
  payload += "\"batt_crude\":"; payload += vdd_str_crude;
  payload += "}";

  char attributes[100];
  payload.toCharArray( attributes, 100 );
  
  // Enviando dados
  Serial.println( "Alarme da Porta da Cozinha Acionado!" );
  
  //client.publish("alarm/backdoor/status", "Activated");
  client.publish("alarm/backdoor/status", attributes);
  
  Serial.println( attributes );
  
}

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}
 
void loop() {
  client.loop();
  
  Serial.println("Entrando no modo DeepSleep...");
  ESP.deepSleep(0);
}
