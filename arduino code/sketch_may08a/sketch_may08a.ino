#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include "arduino_secrets.h" 

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
char serverAddress[] = "green-farm-iot.herokuapp.com";  // server address || localhost
int port = 80;

int status = WL_IDLE_STATUS;
WiFiClient wifiClient;
HttpClient httpClient = HttpClient(wifiClient, serverAddress, port);


#include "DHT.h"                //Humidity and moisture
#define DHTPIN 2                //  (change acdcording your connection)
#define DHTTYPE DHT11 

DHT dht(DHTPIN, DHTTYPE);


#include <MQUnifiedsensor.h>
#define placa "Arduino MKR"
#define Voltage_Resolution 5
#define pin A2                  //  Analog input 0 of your arduino
#define type "MQ-135"           //  MQ135
#define ADC_Bit_Resolution 10   //  For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6  //RS / R0 = 3.6 ppm  

MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);


float soil_moisture;
float temp;
float co2;
float humidity;
float lux;

void setup()
{
  Serial.begin(9600);
  
  pinMode(A3, INPUT); // Light
  pinMode(A1, INPUT); // Soil Moisture
  pinMode(A2, INPUT); // CO2
 // pinMode(10, OUTPUT);// Water Pump
 // pinMode(2, OUTPUT); // Servo Motor
  pinMode(2, INPUT);  // Temp and humidity

  isWifi();
  dht.begin();
  

  //Set math model to calculate the PPM concentration and the value of constants
  MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
  
  MQ135.setA(102.2); MQ135.setB(-2.473); // Configure the equation to to calculate NH4 concentration
  MQ135.init();

  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ135.update(); // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
  /*****************************  MQ CAlibration ********************************************/ 
  MQ135.serialDebug(true);
}

void loop() {
  delay(5000);
  tempCheck();
  humidityCheck();
  soilMoistureCheck();
  lightDensityCheck();
  ppmCheck();

   printData();
  postRequest(String(temp), String(humidity), String(soil_moisture), String(lux), String(co2));
  readResponse();
}
void printData()
{
  Serial.println("");
  Serial.println("*********************************************");
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print("\nHumidity: ");
  Serial.print(humidity);
  Serial.print("\nCO2: ");
  Serial.print(co2);
  Serial.print("\nLIGHT: ");
  Serial.print(lux);
  Serial.print("\nSOIL_MOISTURE: ");
  Serial.print(soil_moisture);
  Serial.print("\n*********************************************");
  Serial.println("");
}
float lightDensityCheck()
{
  float light = analogRead(A0);
  lux = light;
  return light;
}
float tempCheck()
{
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(t) || isnan(f))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return 0;
  }
  temp = t;
  return t;
}
float humidityCheck()
{
  float h = dht.readHumidity();
  humidity = h;
  return h;
}

float soilMoistureCheck()
{
  soil_moisture = 100 - ( (analogRead(A1)/1023.00) * 100 );
  return soil_moisture;
}

float ppmCheck()
{
  MQ135.update(); // Update data, the arduino will read the voltage from the analog pin
  MQ135.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup
  MQ135.serialDebug(); // Will print the table on the serial port

  co2 = MQ135.readSensor() + 400;

  //Serial.println("CO2 ");
  //Serial.print(co2);
  
  return MQ135.readSensor() + 400;
}

void postRequest(String temp, String humidity, String soil_moisture, String lux, String co2)
{
  Serial.println("making POST request");
  
  String contentType = "application/json";
  String postData = "{\"temp\":" + String(temp) + ", \"humidity\":"+String(humidity)+", \"soilMoisture\":"+String(soil_moisture)+", \"light\":"+String(lux)+", \"ppm\":"+String(co2)+"}";
  httpClient.post("/myPost", contentType, postData);
}

void readResponse()
{
  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void isWifi()
{
   // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) 
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) 
  {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    Serial.println("Connected to WiFi");
    printWifiStatus();
  }
}
