#include <WiFi.h>
#include <Wire.h>
#include <SparkFun_SHTC3.h>
#include <HTTPClient.h>
#include <WiFiManager.h> // Incluir WiFiManager
#include <Preferences.h>
#include "nvs_flash.h"

#define portTICK_PERIOD_MS ((TickType_t)(1000 / configTICK_RATE_HZ))

SHTC3 shtc3;
String cloud_url; // Almacenar dinámicamente la URL
Preferences preferences;

TaskHandle_t taskHandleSensor;
TickType_t xDelay = 20000 / portTICK_PERIOD_MS;

WiFiManager wm; // Crear una instancia de WiFiManager



void setup() {
  Serial.begin(115200);
  Wire.begin();

  

  // Inicializar Preferences
  preferences.begin("settings", false);
  cloud_url = preferences.getString("cloud_url", "http://defaulturl.com");

  // Configurar un campo personalizado para la URL del servidor
  WiFiManagerParameter custom_cloud_url("cloud_url", "URL del servidor", cloud_url.c_str(), 128);

  wm.addParameter(&custom_cloud_url);

  // Iniciar WiFiManager
  if (!wm.autoConnect("ESP32_Config")) { // Nombre del punto de acceso
    Serial.println("Error de conexión o tiempo agotado. Reiniciando...");
    ESP.restart();
  }

  // Guardar la URL del servidor ingresada
  cloud_url = custom_cloud_url.getValue();
  preferences.putString("cloud_url", cloud_url);

  Serial.println("\nWiFi conectado.");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("URL del servidor: ");
  Serial.println(cloud_url);

  if (shtc3.begin() != SHTC3_Status_Nominal) {
    Serial.println("Error al inicializar el sensor.");
    while (1);
  }
  Serial.println("Sensor inicializado.");

  if (xTaskCreate(taskSensor, "TaskSensor", 2048, NULL, 1, &taskHandleSensor) != pdPASS) {
    Serial.println("Error al crear la tarea del sensor.");
    while (1);
  }
}

void loop() {
  // WiFiManager no requiere tareas en el bucle principal
}

String takeReadings() {
  if (shtc3.update() == SHTC3_Status_Nominal) {
    float temperature = shtc3.toDegC();
    float humidity = shtc3.toPercent();

    String mac_address = WiFi.macAddress();

    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humedad: ");
    Serial.print(humidity);
    Serial.println(" %");

    return "{ \"data\": { \"DEV_MAC\": \"" + mac_address + "\", \"ME_TEMP\": " + String(temperature, 2) + ", \"ME_HUMIDITY\": " + String(humidity, 2) + " } }";
  } else {
    Serial.println("Error al leer datos del sensor.");
    return "";
  }
}

void sendDataToCloud(String json_data) {
  if (json_data.isEmpty()) return;

  HTTPClient http;
  http.begin(cloud_url);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(json_data);
  if (httpResponseCode > 0) {
    Serial.print("Datos enviados. Código de respuesta: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error al enviar datos. Código: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void taskSensor(void* pvParameters) {
  while (1) {
    String json_data = takeReadings();
    sendDataToCloud(json_data);
    vTaskDelay(xDelay);
  }
}


