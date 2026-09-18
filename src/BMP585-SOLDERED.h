/**
 **************************************************
 *
 * @file        BMP585-SOLDERED.h
 * @brief       Header file for the Soldered BMP585 Barometric Pressure
 *              Sensor breakout board, providing a simplified I2C-only
 *              library built on top of Bosch Sensortec's BMP5_SensorAPI.
 *
 *
 * @copyright   GNU General Public License v3.0
 * @authors     Soldered Electronics
 ***************************************************/

#ifndef __BMP585_SOLDERED__
#define __BMP585_SOLDERED__

#include "Arduino.h"
#include "Wire.h"
#include "libs/bmp5-api/bmp5.h"

class Soldered_BMP585
{
  public:
    /**
     * @brief                       Soldered BMP585 breakout constructor.
     */
    Soldered_BMP585()
    {
    }

    /**
     * @brief                       Initializes the sensor over I2C, applying
     *                              a sane default configuration (temperature
     *                              oversampling 64x, pressure oversampling 4x,
     *                              50 Hz output data rate, normal power mode).
     *                              The Soldered BMP585 breakout only breaks
     *                              out the I2C pins, so SPI is not supported
     *                              by this wrapper.
     *
     * @param uint8_t address       I2C address of the sensor, defaults to 0x47.
     * @param TwoWire &wirePort     I2C bus to use, defaults to Wire.
     *
     * @return                      BMP5_OK (0) on success, error code otherwise.
     */
    int8_t begin(uint8_t address = BMP5_I2C_ADDR_SEC, TwoWire &wirePort = Wire);

    /**
     * @brief                       Performs a soft reset of the sensor, resetting
     *                              all registers to their default values.
     *
     * @return                      BMP5_OK (0) on success, error code otherwise.
     */
    int8_t softReset();

    /**
     * @brief                       Sets the sensor's power mode.
     *
     * @param bmp5_powermode mode   One of BMP5_POWERMODE_STANDBY,
     *                              BMP5_POWERMODE_NORMAL, BMP5_POWERMODE_FORCED,
     *                              BMP5_POWERMODE_CONTINUOUS or
     *                              BMP5_POWERMODE_DEEP_STANDBY.
     *
     * @return                      BMP5_OK (0) on success, error code otherwise.
     */
    int8_t setMode(enum bmp5_powermode mode);

    /**
     * @brief                       Gets the sensor's current power mode.
     *
     * @param bmp5_powermode *mode  Pointer to store the current power mode.
     *
     * @return                      BMP5_OK (0) on success, error code otherwise.
     */
    int8_t getMode(enum bmp5_powermode *mode);

    /**
     * @brief                       Sets oversampling, output data rate and
     *                              pressure-enable configuration.
     *
     * @param uint8_t osrTemperature    Temperature oversampling, one of the
     *                                  BMP5_OVERSAMPLING_* macros.
     * @param uint8_t osrPressure       Pressure oversampling, one of the
     *                                  BMP5_OVERSAMPLING_* macros.
     * @param uint8_t odr               Output data rate, one of the
     *                                  BMP5_ODR_* macros.
     * @param bool pressureEnable       True to enable pressure measurement,
     *                                  false to measure temperature only.
     *                                  Defaults to true.
     *
     * @return                      BMP5_OK (0) on success, error code otherwise.
     */
    int8_t setOSRODRConfig(uint8_t osrTemperature, uint8_t osrPressure, uint8_t odr, bool pressureEnable = true);

    /**
     * @brief                       Gets the current oversampling, output data
     *                              rate and pressure-enable configuration.
     *
     * @param bmp5_osr_odr_press_config *config Pointer to store the configuration.
     *
     * @return                      BMP5_OK (0) on success, error code otherwise.
     */
    int8_t getOSRODRConfig(struct bmp5_osr_odr_press_config *config);

    /**
     * @brief                       Sets the IIR filter coefficient used for
     *                              temperature and pressure data.
     *
     * @param uint8_t iirTemperature    One of the BMP5_IIR_FILTER_* macros.
     * @param uint8_t iirPressure       One of the BMP5_IIR_FILTER_* macros.
     *
     * @return                      BMP5_OK (0) on success, error code otherwise.
     */
    int8_t setIIRConfig(uint8_t iirTemperature, uint8_t iirPressure);

    /**
     * @brief                       Gets the current IIR filter configuration.
     *
     * @param bmp5_iir_config *config   Pointer to store the configuration.
     *
     * @return                      BMP5_OK (0) on success, error code otherwise.
     */
    int8_t getIIRConfig(struct bmp5_iir_config *config);

    /**
     * @brief                       Reads the data-ready / FIFO / OOR interrupt
     *                              status. Useful for polling since this
     *                              breakout doesn't expose the sensor's
     *                              interrupt pin.
     *
     * @param uint8_t *status       Pointer to store the interrupt status, see
     *                              the BMP5_INT_ASSERTED_* macros in bmp5_defs.h.
     *
     * @return                      BMP5_OK (0) on success, error code otherwise.
     */
    int8_t getInterruptStatus(uint8_t *status);

    /**
     * @brief                       Reads pressure and temperature data and
     *                              stores them in the pressure and temperature
     *                              members.
     *
     * @return                      BMP5_OK (0) on success, error code otherwise.
     */
    int8_t getSensorData();

    /**
     * @brief                       Last pressure reading in Pa, updated by
     *                              getSensorData().
     */
    float pressure = 0;

    /**
     * @brief                       Last temperature reading in deg C, updated
     *                              by getSensorData().
     */
    float temperature = 0;

  private:
    struct bmp5_dev _dev;

    struct I2CContext
    {
        uint8_t address;
        TwoWire *wire;
    } _i2cContext;

    static BMP5_INTF_RET_TYPE i2cRead(uint8_t regAddr, uint8_t *readData, uint32_t len, void *intfPtr);
    static BMP5_INTF_RET_TYPE i2cWrite(uint8_t regAddr, const uint8_t *data, uint32_t len, void *intfPtr);
    static void delayUs(uint32_t period, void *intfPtr);
};

#endif
