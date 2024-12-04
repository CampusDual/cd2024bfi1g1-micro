#include <WiFiManager.h>

void setup(){
  WiFi.mode(WIFI_STA);

  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  WiFiManager wiFiManager;

  wiFiManager.resetSettings();

  bool res;
  res = wiFiManager.autoConnect("ConexionESP32", "12345678");

  if(!res){
    Serial.println("Conexion fallida");
  }else{
    Serial.print("Conectado a: "); 
    Serial.println(WiFi.SSID());
  } 
}

void loop(){

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
}