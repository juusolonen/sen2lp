/*
  sen2lp.h - Library for converting sensor readings into Line Protocol that can be sent to InfluxDB
  Created by Juuso Solonen, November 16, 2024.
  Released into the public domain.
*/
#include "Arduino.h"
#include "sen2lp.h"

Field::Field(const char* key, const char* value, FieldType fieldType) {
    this->key = key;
    strncpy(this->value, value, sizeof(this->value) - 1);
    this->value[sizeof(this->value) - 1] = '\0';  // Null-terminate
    this->type = fieldType;
}

void Field::formatValue(char* result) {
    switch (this->type) {
        case INT:{
            float floatVal = atof(this->value);
            int intVal = (int)floatVal;
            char intChar[10];  // buffer to hold the converted integer as char*
            sprintf(intChar, "%d", intVal);  // Format integer as string
            strcat(result, intChar);
            strcat(result, "i");
            break;}
        case STR:{
            strcat(result, "\"");
            strcat(result, this->value);
            strcat(result, "\"");
            break;}
        case BOOL:{
            if (strcmp(this->value , "true") == 0 || strcmp(this->value, "false") == 0 || strcmp(this->value, "T") == 0 || strcmp(this->value, "F") == 0 
            || strcmp(this->value , "True") == 0 || strcmp(this->value, "False") == 0 || strcmp(this->value, "t") == 0 || strcmp(this->value, "f") == 0
            || strcmp(this->value , "TRUE") == 0 || strcmp(this->value, "FALSE") == 0) {
                strcat(result, this->value);
            } else {
                int val = atoi(this->value);
                strcat(result, val > 0 ? "true" : "false");
            }
            break;}
        default:{
            float fVal = atof(this->value);
            char fChar[10];  // buffer to hold the converted float as char*
            sprintf(fChar, "%.2f", fVal);  // Format integer as string
            strcat(result, fChar);
            break;}
    }
}


    Sensor::Sensor(uint8_t vCount, const char* measurement, const char* tags, unsigned long time) {
        this->measurement = measurement;
        this->tags = tags;
        this->timestamp = time;
        size = vCount;
        data = new Field*[size];

        for (uint8_t i = 0; i < size; ++i) {
            data[i] = nullptr; 
        }
    }

    Sensor::~Sensor() {
        for (uint8_t i = 0; i < size; ++i) {
            delete data[i];
        }
        delete[] data;
    }

    void Sensor::setTimestamp(unsigned long newTimestamp) {
        this->timestamp = newTimestamp;
    }

    Field* Sensor::allocateData(uint8_t index, const char* key, const char* value, FieldType type) {
        if (index < size) {
            if (data[index] == nullptr) {
                data[index] = new Field(key, value, type); 
                return data[index];
            } else {
                Serial.println("Data already allocated at index");
            }
        } else {
            Serial.println("Index out of bounds.");
        }
        return nullptr;
    }


    void Sensor::getFormattedFields(char* result) {
        result[0] = '\0'; 
        for (uint8_t i = 0; i < size; ++i) {
            if (data[i] != nullptr) {
                strcat(result, data[i]->key);
                strcat(result, "=");
                strcat(result, data[i]->value);
                if (i < size - 1) {
                    strcat(result, ",");
                }
            }
        }
    }

    void Sensor::getLineProtocol(char* result) {
        result[0] = '\0';
        strcat(result, measurement);
        strcat(result, ",");
        strcat(result, tags);

        for (uint8_t i = 0; i < size; ++i) {
            if (data[i] != nullptr) {
                if (i == 0) {
                    strcat(result, " ");
                }
                strcat(result, data[i]->key);
                strcat(result, "=");
                if (data[i]->type != FieldType::FLT) {
                   data[i]->formatValue(result);
                }
                else { // Hack-ish here, for some reason did not work inside formatValue
                  strcat(result, data[i]->value);
                }
                if (i < size - 1) {
                    strcat(result, ",");
                }
            }
        }

        if (this->timestamp > 0) {
          char timestampStr[11];
          snprintf(timestampStr, sizeof(timestampStr), "%lu", this->timestamp); 
          strcat(result, " ");
          strcat(result, timestampStr);
        }
    }
