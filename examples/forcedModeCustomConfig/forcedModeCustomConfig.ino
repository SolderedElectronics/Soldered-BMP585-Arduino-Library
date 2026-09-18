/**
 **************************************************
 *
 * @file        forcedModeCustomConfig.ino
 *
 * @brief       Example showing how to configure oversampling/IIR filtering
 *              and take single-shot readings in forced power mode with the
 *              Soldered BMP585 Barometric Pressure Sensor breakout.
 *              Forced mode triggers one measurement then returns the sensor
 *              to standby automatically, useful for low-power applications
 *              where you only need occasional readings.
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
    Serial.println("BMP585 Example - Forced Mode & Custom Config");

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

    // Lower the oversampling from the library default since forced mode is
    // typically used for infrequent, quick readings. ODR is ignored in
    // forced mode, but still needs to be set to a valid value
    pressureSensor.setOSRODRConfig(BMP5_OVERSAMPLING_1X, BMP5_OVERSAMPLING_1X, BMP5_ODR_50_HZ);

    // Bypass the IIR filter since forced mode readings aren't continuous
    // enough for the filter to settle
    pressureSensor.setIIRConfig(BMP5_IIR_FILTER_BYPASS, BMP5_IIR_FILTER_BYPASS);

    // Trigger the first measurement
    pressureSensor.setMode(BMP5_POWERMODE_FORCED);
}

void loop()
{
    // Wait for the measurement to be ready
    uint8_t interruptStatus = 0;
    pressureSensor.getInterruptStatus(&interruptStatus);

    if (interruptStatus & BMP5_INT_ASSERTED_DRDY)
    {
        if (pressureSensor.getSensorData() == BMP5_OK)
        {
            Serial.print("Pressure: ");
            Serial.print(pressureSensor.pressure, 2);
            Serial.print(" Pa\t");
            Serial.print("Temperature: ");
            Serial.print(pressureSensor.temperature, 2);
            Serial.println(" degC");
        }

        // Trigger the next measurement
        pressureSensor.setMode(BMP5_POWERMODE_FORCED);

        // Only take a reading once per second
        delay(1000);
    }
}
