/**
 **************************************************
 *
 * @file        basicReadings.ino
 *
 * @brief       Example showing how to read pressure and temperature data
 *              from the Soldered BMP585 Barometric Pressure Sensor
 *              breakout over I2C
 *
 * @link        solde.red/333189 : Soldered BMP585 Barometric Pressure Sensor breakout
 *
 * @authors     Soldered Electronics
 ***************************************************/
#include <BMP585-SOLDERED.h>

// Create an instance of the BMP585 sensor
Soldered_BMP585 pressureSensor;

void setup()
{
    // Initialize serial for output
    Serial.begin(115200);
    Serial.println("BMP585 Example - Basic Readings");

    // Initialize I2C bus
    Wire.begin();

    // Check if sensor is connected and initialize it.
    // Address is optional, defaults to 0x47
    while (pressureSensor.begin() != BMP5_OK)
    {
        Serial.println("Error: BMP585 not connected, check wiring and I2C address!");
        delay(1000);
    }

    Serial.println("BMP585 connected!");
}

void loop()
{
    // Get new measurements from the sensor. This must be called before
    // accessing the pressure and temperature data, otherwise it will never update
    if (pressureSensor.getSensorData() == BMP5_OK)
    {
        Serial.print("Pressure: ");
        Serial.print(pressureSensor.pressure, 2);
        Serial.print(" Pa\t");
        Serial.print("Temperature: ");
        Serial.print(pressureSensor.temperature, 2);
        Serial.println(" degC");
    }

    // Print 10x per second
    delay(100);
}
