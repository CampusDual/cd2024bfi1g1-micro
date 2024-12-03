#include "nvs_flash.h"
#include "nvs.h"
#include "esp_system.h"
const char* password = "hello_world";
void setup() {
  Serial.begin(9600);
  delay(1000);

  // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
  //Open connection
    Serial.println("\n");
    Serial.println("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        Serial.println("Error (%s) opening NVS handle!\n");
    } else {
        Serial.println("Done\n");
    }
    delay(100);
  //Writing
    Serial.println("Updating password in NVS ... ");
     
    err = nvs_set_str(my_handle, "password", "hola");
    Serial.println((err != ESP_OK) ? "Failed!\n" : "Done\n");

  // Commit written value.
    Serial.println("Committing updates in NVS ... ");
    err = nvs_commit(my_handle);
    Serial.println((err != ESP_OK) ? "Failed!\n" : "Done\n");
  // Obtener el tamaño necesario para el valor de la clave "password"
    size_t required_size = 0;
    err = nvs_get_str(my_handle, "password", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
      Serial.println("Error al obtener el tamaño del string");
      return;
  }
  delay(2000);
  // Reservar memoria para almacenar la cadena
    char* pw = (char*) malloc(required_size);

  // Read
    Serial.println("Reading from memory");
    err = nvs_get_str(my_handle, "password", pw, &required_size);
    switch (err) {
      case ESP_OK:
        Serial.println("Done\n");
        Serial.print("Contraseña leída: ");
        Serial.println(pw);
        break;
        case ESP_ERR_NVS_NOT_FOUND:
          Serial.println("The value is not initialized yet!\n");
          break;
        default :
          Serial.println("Error reading");
        }
  // Close
    nvs_close(my_handle);
}

void loop() {
  // put your main code here, to run repeatedly:

}
