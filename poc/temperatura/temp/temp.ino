#include "SparkFun_SHTC3.h" 


SHTC3 mySHTC3; 


void setup() {
  Serial.begin(9600); 
  Wire.begin(); // Inicia la comunicación I2C


  // Inicializa el sensor SHTC3
  if (mySHTC3.begin() != SHTC3_Status_Nominal) {
    Serial.println("Error al inicializar el sensor SHTC3.");
    while (1);
  }
}


void loop() {


  // Actualiza las lecturas del sensor
  if (mySHTC3.update() == SHTC3_Status_Nominal) {


    // Obtiene y muestra la temperatura en grados Celsius
    float temperatura = mySHTC3.toDegC();
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" °C");


    // Obtiene y muestra la humedad relativa en porcentaje
    float humedad = mySHTC3.toPercent();
    Serial.print("Humedad: ");
    Serial.print(humedad);
    Serial.println(" %");


  } else {
    Serial.println("Error al obtener datos del sensor.");
  }


  delay(1000); 
  
}