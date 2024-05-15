#include <SoftwareSerial.h>
#include <TinyGPS.h>

#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>
WiFiClient wifiClient;

//clave y contraseña de internet
const char* ssid = "Totalplay-F5AA";
const char* password =  "F5AAC9F9NPqR7vHS";

// const char* ssid = "Wi-Fi IPN";
// const char* password =  "";

// const char* ssid = "Lesly";
// const char* password =  "123456789";

//variables a enviar
float LAT = 0;
float LON = 0;
int ID_DISPOSITIVO = 1;

//configuracion de puerto serial de software para comunicacion con el modulo GPS
TinyGPS gps;
SoftwareSerial ss;

void setup() 
{
  pinMode(2, OUTPUT);
  ss.begin(9600, SWSERIAL_8N1, 4, 5, false);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
    delay(500);
  
  if(!ss){
    while (1) { 
        digitalWrite(2, 1);
        delay(200);
    }
  }
}

void loop() 
{  
  digitalWrite(2, 0);
  if(WiFi.status()== WL_CONNECTED){
    
    digitalWrite(2, 0);
    //si hay informacion disponible
    if (newDataGPS()){
      
      //obtenemos y mostramos la posicion del GPS en el display
      getPositionGPS(LAT, LON);
 
      //creamos el URL con los datos reportados del GPS y el ID del brazalete
      HTTPClient http;
      String datos_a_enviar = "LAT=" + String(LAT, 8) + "&LON=" + String(LON, 8) + "&ID_DISPOSITIVO=" + ID_DISPOSITIVO;
      
      //Indicamos el destino
      http.begin(wifiClient, "http://chiquillep.000webhostapp.com/insertar.php");
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      if(http.POST(datos_a_enviar) == 200){
        digitalWrite(2, 0);
        delay(100);
        digitalWrite(2, 1);
        delay(100);
      }
      
      http.end();    
    }
    
    delay(2000); 
  }
}

/** Funciones GPS**/

//evalua si existen nuevos datos del GPS
bool newDataGPS()
{
  bool nuevoDato = false;
  //ejecuta el ciclo una vez por segundo
  for (unsigned long start = millis(); millis() - start < 1000;)
    //revisa si esta disponible el puerto serial del GPS
    while (ss.available())
      //si se leyo un dato en el puerto serial del GPS
      if (gps.encode(ss.read()))
        //hay un nuevo dato comunicado por el GPS
        nuevoDato = true;
  
  return nuevoDato;
}

//obtiene la posicion del GPS y se guardan en los argumentos de la funcion 
void getPositionGPS(float &flat, float &flon)
{
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
  
  //evalua si la posicion obtenida es valida, si no se podra por defecto la posicion Lat: 0.0, Lon: 0.0
  flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat;
  flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon;
}
