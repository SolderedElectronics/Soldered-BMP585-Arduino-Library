/**
 **************************************************
 *
 * @file        dataReadyInterrupt.ino
 *
 * @brief       Example showing how to use the BMP585's physical interrupt
 *              pin to know when a new pressure/temperature reading is
 *              ready, instead of polling the interrupt status register
 *
 * @link        solde.red/333189 : Soldered BMP585 Barometric Pressure Sensor breakout
 *
 * @authors     Soldered Electronics
 ***************************************************/
#include <BMP585-SOLDERED.h>

// Create an instance of the BMP585 sensor
Soldered_BMP585 pressureSensor;

// Pin connected to the sensor's INT pin, change to match your wiring
int interruptPin = 2;

// Flag to know when an interrupt occurs
volatile bool interruptOccurred = false;

void bmp585InterruptHandler()
{
    interruptOccurred = true;
}

void setup()
{
    // Initialize serial for output
    Serial.begin(115200);
    Serial.println("BMP585 Example - Data Ready Interrupt");

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

    // Configure the interrupt pin as push/pull, active high, pulsed
    pressureSensor.configureInterrupt(BMP5_PULSED, BMP5_ACTIVE_HIGH, BMP5_INTR_PUSH_PULL, true);

    // Only assert the pin on a new data-ready reading
    pressureSensor.setInterruptSource(true);

    // Attach the interrupt handler to the microcontroller pin wired to INT
    attachInterrupt(digitalPinToInterrupt(interruptPin), bmp585InterruptHandler, RISING);
}

void loop()
{
    if (interruptOccurred)
    {
        interruptOccurred = false;

        if (pressureSensor.getSensorData() == BMP5_OK)
        {
            Serial.print("Pressure: ");
            Serial.print(pressureSensor.pressure, 2);
            Serial.print(" Pa\t");
            Serial.print("Temperature: ");
            Serial.print(pressureSensor.temperature, 2);
            Serial.println(" degC");
        }
    }
}
