/*
  sen2lp.h - Library for converting sensor readings into Line Protocol that can be sent to InfluxDB
  Created by Juuso Solonen, November 16, 2024.
  Released into the public domain.
*/
#ifndef SEN2LP_H
#define SEN2LP_H

#include "Arduino.h"

enum FieldType {
    INT,
    FLT,
    STR,
    BOOL
};

struct Field {
    public:
        Field(const String& key, const String& value, FieldType fieldType);
        const String key;
        String value; 
        FieldType type;
        void formatValue(String& result);
};

class Sensor {
    public:
        Sensor(uint8_t vCount, const String& measurement, const String& tags, unsigned long time = 0); // vCount = amount of individual sensor readings
        ~Sensor();
        void setTimestamp(unsigned long newTimestamp); // Set optional timestamp for the line protocol
        Field* allocateData(uint8_t index, const String key, const String value, FieldType type); // Initialize inidivicual sensor
        void getFormattedFields(String& result);  // Method to get all Field data as a formatted string
        void getLineProtocol(String& result); // Get the sensor data as Line Protocol

    private:
        Field** data; // Array of sensor readings, or fields
        uint8_t size;
        const String measurement;
        const String tags; // Must be defined as comas separated list of <tagName>=<tagValue> e.g. "test1=value1,test2=value2"
        unsigned long timestamp;  // Store Unix timestamp for this field
};

#endif