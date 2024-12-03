#include <Wire.h>
#include <SparkFun_SHTC3.h>
#include <WiFi.h>
#include <HTTPClient.h>

SHTC3 shtc3;

const char* ssid = "nombreRED";
const char* password = "CONTRASEÑA";

const String cloud_url = "URLservidor";

void setup() {
  Serial.begin(9600);
  Wire.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a Wi-Fi...");
  }
  Serial.println("Conectado a Wi-Fi!");

  if (shtc3.begin() != SHTC3_Status_Nominal) {
    Serial.println("Error al inicializar.");
    while (1);
  }
  Serial.println("Sensor inicializado.");
}

void loop() {
  if (shtc3.update() == SHTC3_Status_Nominal) {
    float temperature = shtc3.toDegC();
    float humidity = shtc3.toPercent();

    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humedad: ");
    Serial.print(humidity);
    Serial.println(" %");

    String json_data = "{\"temperature\": " + String(temperature, 2) + ", \"humidity\": " + String(humidity, 2) + "}";

    sendDataToCloud(json_data);

  } else {
    Serial.println("Error al leer datos del sensor.");
  }

  delay(2000);
}

void sendDataToCloud(String json_data) {
  HTTPClient http;

  http.begin(cloud_url);

  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(json_data);

  if (httpResponseCode > 0) {
    Serial.print("Datos enviados. Codigo de respuesta: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error al enviar los datos. Codigo de error: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}
