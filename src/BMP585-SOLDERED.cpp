/**
 **************************************************
 *
 * @file        BMP585-SOLDERED.cpp
 * @brief       Source file for the Soldered BMP585 Barometric Pressure
 *              Sensor breakout board.
 *
 *
 * @copyright   GNU General Public License v3.0
 * @authors     Soldered Electronics
 ***************************************************/

#include "BMP585-SOLDERED.h"

int8_t Soldered_BMP585::begin(uint8_t address, TwoWire &wirePort)
{
    _i2cContext.address = address;
    _i2cContext.wire = &wirePort;

    _dev.intf_ptr = &_i2cContext;
    _dev.intf = BMP5_I2C_INTF;
    _dev.read = i2cRead;
    _dev.write = i2cWrite;
    _dev.delay_us = delayUs;

    int8_t result = bmp5_soft_reset(&_dev);
    if (result != BMP5_OK)
    {
        return result;
    }

    result = bmp5_init(&_dev);
    if (result != BMP5_OK)
    {
        return result;
    }

    result = setOSRODRConfig(BMP5_OVERSAMPLING_64X, BMP5_OVERSAMPLING_4X, BMP5_ODR_50_HZ, true);
    if (result != BMP5_OK)
    {
        return result;
    }

    return setMode(BMP5_POWERMODE_NORMAL);
}

int8_t Soldered_BMP585::softReset()
{
    return bmp5_soft_reset(&_dev);
}

int8_t Soldered_BMP585::setMode(enum bmp5_powermode mode)
{
    return bmp5_set_power_mode(mode, &_dev);
}

int8_t Soldered_BMP585::getMode(enum bmp5_powermode *mode)
{
    return bmp5_get_power_mode(mode, &_dev);
}

int8_t Soldered_BMP585::setOSRODRConfig(uint8_t osrTemperature, uint8_t osrPressure, uint8_t odr, bool pressureEnable)
{
    struct bmp5_osr_odr_press_config config = {0};

    int8_t result = bmp5_get_osr_odr_press_config(&config, &_dev);
    if (result != BMP5_OK)
    {
        return result;
    }

    config.osr_t = osrTemperature;
    config.osr_p = osrPressure;
    config.odr = odr;
    config.press_en = pressureEnable ? BMP5_ENABLE : BMP5_DISABLE;

    return bmp5_set_osr_odr_press_config(&config, &_dev);
}

int8_t Soldered_BMP585::getOSRODRConfig(struct bmp5_osr_odr_press_config *config)
{
    return bmp5_get_osr_odr_press_config(config, &_dev);
}

int8_t Soldered_BMP585::setIIRConfig(uint8_t iirTemperature, uint8_t iirPressure)
{
    struct bmp5_iir_config config = {0};
    config.set_iir_t = iirTemperature;
    config.set_iir_p = iirPressure;
    config.shdw_set_iir_t = BMP5_ENABLE;
    config.shdw_set_iir_p = BMP5_ENABLE;

    return bmp5_set_iir_config(&config, &_dev);
}

int8_t Soldered_BMP585::getIIRConfig(struct bmp5_iir_config *config)
{
    return bmp5_get_iir_config(config, &_dev);
}

int8_t Soldered_BMP585::getInterruptStatus(uint8_t *status)
{
    return bmp5_get_interrupt_status(status, &_dev);
}

int8_t Soldered_BMP585::configureInterrupt(enum bmp5_intr_mode mode, enum bmp5_intr_polarity pol,
                                           enum bmp5_intr_drive drive, bool enable)
{
    return bmp5_configure_interrupt(mode, pol, drive, enable ? BMP5_INTR_ENABLE : BMP5_INTR_DISABLE, &_dev);
}

int8_t Soldered_BMP585::setInterruptSource(bool dataReady, bool fifoFull, bool fifoThreshold, bool pressureOOR)
{
    struct bmp5_int_source_select source = {0};
    source.drdy_en = dataReady ? BMP5_ENABLE : BMP5_DISABLE;
    source.fifo_full_en = fifoFull ? BMP5_ENABLE : BMP5_DISABLE;
    source.fifo_thres_en = fifoThreshold ? BMP5_ENABLE : BMP5_DISABLE;
    source.oor_press_en = pressureOOR ? BMP5_ENABLE : BMP5_DISABLE;

    return bmp5_int_source_select(&source, &_dev);
}

int8_t Soldered_BMP585::getSensorData()
{
    struct bmp5_osr_odr_press_config config = {0};
    int8_t result = bmp5_get_osr_odr_press_config(&config, &_dev);
    if (result != BMP5_OK)
    {
        return result;
    }

    struct bmp5_sensor_data data = {0};
    result = bmp5_get_sensor_data(&data, &config, &_dev);
    if (result != BMP5_OK)
    {
        return result;
    }

    pressure = data.pressure;
    temperature = data.temperature;

    return BMP5_OK;
}

BMP5_INTF_RET_TYPE Soldered_BMP585::i2cRead(uint8_t regAddr, uint8_t *readData, uint32_t len, void *intfPtr)
{
    I2CContext *ctx = (I2CContext *)intfPtr;

    ctx->wire->beginTransmission(ctx->address);
    ctx->wire->write(regAddr);
    if (ctx->wire->endTransmission(false) != 0)
    {
        return BMP5_E_COM_FAIL;
    }

    uint8_t bytesRead = ctx->wire->requestFrom(ctx->address, (uint8_t)len);
    if (bytesRead != len)
    {
        return BMP5_E_COM_FAIL;
    }

    for (uint32_t i = 0; i < len; i++)
    {
        readData[i] = ctx->wire->read();
    }

    return BMP5_OK;
}

BMP5_INTF_RET_TYPE Soldered_BMP585::i2cWrite(uint8_t regAddr, const uint8_t *data, uint32_t len, void *intfPtr)
{
    I2CContext *ctx = (I2CContext *)intfPtr;

    ctx->wire->beginTransmission(ctx->address);
    ctx->wire->write(regAddr);
    ctx->wire->write(data, (size_t)len);

    return (ctx->wire->endTransmission() == 0) ? BMP5_OK : BMP5_E_COM_FAIL;
}

void Soldered_BMP585::delayUs(uint32_t period, void *intfPtr)
{
    (void)intfPtr;

    if (period >= 1000)
    {
        delay(period / 1000);
        period %= 1000;
    }
    delayMicroseconds(period);
}
