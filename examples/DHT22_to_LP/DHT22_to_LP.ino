//Libraries
#include <DHT.h>
#include <sen2lp.h>


//Constants
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);  // Initialize DHT sensor for normal 16mhz Arduino

const char* measurement = "test_weather";
const char* tags = "room=test_room";

// Variables
float hum;  // Stores humidity value
float temp; // Stores temperature value
Field* sensorHum = nullptr;
Field* sensorTemp = nullptr;
Sensor* sensor = nullptr;

void setup() {
    Serial.begin(9600);
    dht.begin();

    unsigned long customTimestamp = 1609459200;  // Example Unix timestamp
    sensor = new Sensor(2, measurement, tags, customTimestamp);  // Create a Sensor object with space for 2 individual sensor readings

    // Allocate data for the struct at index 0 and 1
    // NOTE: the library expects the 'value' to be in string form
    sensorHum = sensor->allocateData(0, "humidity", "0", FieldType::INT);
    sensorTemp = sensor->allocateData(1, "temperature", "0", FieldType::FLT);
}

void loop() {
    // Read data and store it to variables hum and temp
    hum = dht.readHumidity();
    temp = dht.readTemperature();

    sprintf(sensorHum->value, "%.2f", hum);
    sprintf(sensorTemp->value, "%.2f", temp);

    // Prepare a buffer to hold the formatted string (Line Protocol)
    char result[100];  // Ensure this is large enough to hold the final result

    sensor->getLineProtocol(result);  // Get line protocol formatted data
    Serial.println(result);  // Print result to serial monitor

    delay(2000);  // Delay 2 sec.
}
