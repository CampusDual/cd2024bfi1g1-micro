#include <WiFi.h> 
#include <Wire.h>
#include <SparkFun_SHTC3.h>
#include <HTTPClient.h>
#define portTICK_PERIOD_MS ((TickType_t)(1000 / configTICK_RATE_HZ))
TickType_t xDelay = 20000 / portTICK_PERIOD_MS;
const char* ssid = "nombre de tu red";  
const char* password = "contraseña";
SHTC3 shtc3;
const String cloud_url = "URLservidor";

void setup() {
  Serial.begin(9600);
  while(!Serial){
  }
  Serial.println("Iniciando conexión Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando...");
  }

  Serial.println("Conectado a Wi-Fi");
  Serial.print("Direccion IP: ");
  Serial.println(WiFi.localIP());
  if(xTaskCreate( task_upload, "task_upload", 2048, NULL, 1, NULL) != pdPASS){
		Serial.println("Error en creacion tarea task_con");
		exit(-1);
	}
   if (shtc3.begin() != SHTC3_Status_Nominal) {
    Serial.println("Error al inicializar sensor.");
    while (1);
  }
  Serial.println("Sensor inicializado.");
}


void loop() {
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

String take_readings(){
if (shtc3.update() == SHTC3_Status_Nominal) {
    float temperature = shtc3.toDegC();
    float humidity = shtc3.toPercent();

    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humedad: ");
    Serial.print(humidity);
    Serial.println(" %");

    String json_data = "{\"t\": " + String(temperature, 2) + ", \"h\": " + String(humidity, 2) + "}";

  } else {
    Serial.println("Error al leer datos del sensor.");
  }
}

void task_upload(void *pvParameter) {
    while(1) {
        String json_data = take_readings();
        sendDataToCloud(json_data);
    	  vTaskDelay(xDelay);
    }
  }


