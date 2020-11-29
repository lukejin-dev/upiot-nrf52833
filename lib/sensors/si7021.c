
#include "i2c.h"
#include "si7021.h"
#include "uplog.h"

static I2c_t SI7021_I2c;
static const uint8_t I2C_ADDR = 0x40;           // Si7021 I2C address

static void SI7021_Read_Reg( uint8_t reg, uint8_t *data, uint16_t len )
{
    I2cRegRead(&SI7021_I2c, I2C_ADDR, reg, data, len);
}

uint8_t SI7021_GetFirmwareRevision()
{
    uint16_t cmd = (R_Firm_rev2<<8) | R_Firm_rev1;
    uint8_t data;

    I2cWrite(&SI7021_I2c, I2C_ADDR, (uint8_t*)&cmd, 2, true);
    I2cRead(&SI7021_I2c, I2C_ADDR, &data, 1);

    UP_INFO("SI7021 Firmware Version: 0x%x", data);
    return data;
}

uint16_t SI7021_GetDeviceID()
{
    uint16_t cmd = (R_ID_Byte22<<8) | R_ID_Byte21;
    uint8_t id[2] = {0};

    // Read the first byte
    cmd = (R_ID_Byte12<<8) | R_ID_Byte11;
    I2cWrite(&SI7021_I2c, I2C_ADDR, (uint8_t*)&cmd, 2, true);
    I2cRead(&SI7021_I2c, I2C_ADDR, &id[0], 1);

    cmd = (R_ID_Byte22<<8) | R_ID_Byte21;
    I2cWrite(&SI7021_I2c, I2C_ADDR, (uint8_t*)&cmd, 2, true);
    I2cRead(&SI7021_I2c, I2C_ADDR, &id[1], 1);
    UP_INFO("SI7021 Electronic ID: 0x%x", (id[1]<<8 | id[0]));

    return (id[1]<<8 | id[0]);
}

void SI7021_Reset()
{
    uint8_t cmd = Si7021_Reset;

    I2cWrite(&SI7021_I2c, I2C_ADDR, (uint8_t*)&cmd, 1, true);
}

bool SI7021_Init(I2cId_t id, PinNames scl, PinNames sda)
{
    UP_INFO("SI7021 Init");

    I2cInit(&SI7021_I2c, id, scl, sda);

    //SI7021_Reset();
    SI7021_GetFirmwareRevision();
    SI7021_GetDeviceID();

    return true;
}