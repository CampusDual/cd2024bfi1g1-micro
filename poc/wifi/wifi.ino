#include <WiFi.h>

const char* ssid = "nombreRED";  
const char* password = "CONTRASEÑA";

void setup() {
  Serial.begin(9600);
  
  Serial.println("Iniciando conexión Wi-Fi...");
  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando...");
  }

  Serial.println("Conectado a Wi-Fi");
}

void loop() {
}