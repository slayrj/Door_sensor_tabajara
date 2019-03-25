#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
const char* ssid = "ssid";
const char* password =  "ssid_password";
const char* mqttServer = "192.168.1.250";
const int mqttPort = 1883;
const char* mqttUser = "user_mqtt";
const char* mqttPassword = "passw_mqtt";

#define vermelho 5

WiFiClient espClient;
PubSubClient client(espClient);

ADC_MODE(ADC_VCC);

void setup()  
{
  Serial.begin(115200);
  pinMode(vermelho, OUTPUT);
  digitalWrite(vermelho, LOW);
  
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
  uint16_t v = ESP.getVcc();
  float_t v_cal = ((float)v/865.0f);
  char v_str[10];
  char voltuncal[10];
  dtostrf(v_cal, 5, 2, v_str);
  dtostrf(ESP.getVcc(), 5, 0, voltuncal);
  // sprintf(v_str,"%s V", v_str);
  Serial.print("Voltagem sem formatacao: ");
  Serial.println(ESP.getVcc());
  Serial.println(v_str);
  
  // Enviando dados
  Serial.println( "Alarme da Porta foi acionado" );
  
  client.publish("alarm/backdoor/status", "Activated");
  client.publish("alarm/backdoor/status/batt", v_str);
  client.publish("alarm/backdoor/status/realbatt", voltuncal);

  if ( ESP.getVcc() < 2595 ){
    digitalWrite(vermelho, HIGH);
    Serial.println("Bateria Fraca!!");
    }
  else {
    digitalWrite(vermelho, LOW);
    Serial.println("Bateria Ok!!");
    }
  delay(500);
  digitalWrite(vermelho, LOW);
  
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
