#include <Wire.h>
#include <SparkFun_SHTC3.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_system.h"

SHTC3 shtc3;
const String cloud_url = "https://webhook.site/0abb401a-943c-4a56-a163-8bdb6421be67";
TaskHandle_t taskHandleSensor;

char ssid[50] = {0};
char password[50] = {0};

void initNVS() {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  nvs_handle_t my_handle;
  err = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (err != ESP_OK) return;

  size_t ssid_size = sizeof(ssid);
  size_t password_size = sizeof(password);
  if (nvs_get_str(my_handle, "ssid", ssid, &ssid_size) == ESP_OK &&
      nvs_get_str(my_handle, "password", password, &password_size) == ESP_OK) {
    Serial.print("Datos recuperados - SSID: ");
    Serial.println(ssid);
  } else {
    Serial.println("No se encontraron credenciales almacenadas.");
  }

  nvs_close(my_handle);
}

void saveCredentials(const char* ssid, const char* password) {
  nvs_handle_t my_handle;
  esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);

  if (err == ESP_OK) {
    nvs_set_str(my_handle, "ssid", ssid);
    nvs_set_str(my_handle, "password", password);
    nvs_commit(my_handle);
    Serial.println("Credenciales guardadas en NVS.");
    nvs_close(my_handle);
  }
}

void clearCredentials() {
  nvs_handle_t my_handle;
  esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);

  if (err == ESP_OK) {
    nvs_erase_key(my_handle, "ssid");
    nvs_erase_key(my_handle, "password");

    if (nvs_commit(my_handle) == ESP_OK) {
      Serial.println("Credenciales borradas exitosamente.");
    }
    nvs_close(my_handle);
  }
}

void initWiFi() {
  Serial.println("Introduce el nombre de la red WiFi (SSID):");
  while (Serial.available() == 0);
  Serial.readBytesUntil('\n', ssid, sizeof(ssid) - 1);

  Serial.println("Introduce la contraseña:");
  while (Serial.available() == 0);
  Serial.readBytesUntil('\n', password, sizeof(password) - 1);

  saveCredentials(ssid, password);

  WiFi.begin(ssid, password);
  Serial.println("Conectando a WiFi...");
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 30000) {
    delay(1000);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado a WiFi!");
  } else {
    Serial.println("\nNo se pudo conectar a WiFi.");
  }
}

void taskSensor(void* pvParameters) {
  while (1) {
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

      String json_data = "{\"m\": \"" + mac_address + "\", \"t\": " + String(temperature, 2) + ", \"h\": " + String(humidity, 2) + "}";
      sendDataToCloud(json_data);
    } else {
      Serial.println("Error al leer datos del sensor.");
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void sendDataToCloud(String json_data) {
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

void setup() {
  Serial.begin(115200);
  Wire.begin();

  initNVS();

  if (strlen(ssid) == 0 || strlen(password) == 0) {
    initWiFi();
  } else {
    WiFi.begin(ssid, password);
    Serial.println("Conectando a WiFi...");
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 30000) {
      delay(1000);
      Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConectado a WiFi!");
    } else {
      Serial.println("\nNo se pudo conectar a WiFi.");
    }
  }

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
    delay(1000);
}
