/*
INA228.cpp - Class file for the INA228 Bi-directional Current/Power Monitor Arduino Library.

(c) 2014 Korneliusz Jarzebski, modified 2020 by Peter Buchegger
www.jarzebski.pl

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// #if ARDUINO >= 100
// #include "Arduino.h"
// #else
// #include "WProgram.h"
// #endif

// #include <Wire.h>
#include <math.h>
// #include <stdio.h>
#include "sensor/INA228.h"

INA228::INA228(){
}

bool INA228::begin(i2c_inst_t* i2c, uint8_t address)
{
    wire.begin(i2c);
    inaAddress = address ;
    return configure(
        INA228_AVERAGES_1,
        INA228_BUS_CONV_TIME_1052US,
        INA228_SHUNT_CONV_TIME_1052US,
        INA228_MODE_SHUNT_BUS_CONT,
        false
    );
}

bool INA228::configure(INA228_averages_t avg, INA228_busConvTime_t busConvTime, INA228_shuntConvTime_t shuntConvTime, INA228_mode_t mode, bool range)
{
    config_avg = avg;
    config_busConvTime = busConvTime;
    config_shuntConvTime = shuntConvTime;
    config_mode = mode;
    config_range = range;

    uint16_t config = 0;

    config = range ? 1<<4 : 0;
    writeRegister16(INA228_CONFIG, config);

    config = (mode << 12 | busConvTime << 9 | shuntConvTime << 6 | avg);

    vBusMax = 85;
    vShuntMax = 0.015f;

    return writeRegister16(INA228_ADC_CONFIG, config);
}

bool INA228::powerUp(){
    bool result = configure(config_avg, config_busConvTime, config_shuntConvTime, config_mode, false);
    sleep_us(40); // startup time.
    return result;
}

bool INA228::powerDown(){
    uint16_t config = INA228_MODE_POWER_DOWN << 12;
    return writeRegister16(INA228_ADC_CONFIG, config);
}

const uint16_t averages[] = {1,4,16,64,128,256,512,1024};

// Minimum wait in us for 1 average
const uint16_t wait[] = {50,84,150,280,540,1052,2074,4120};

bool INA228::triggerAndWait(INA228_mode_t mode){
    this->trigger(mode);

	uint32_t total_wait = this->getWaitTimeUs();
    sleep_us(total_wait);

    auto timeout = make_timeout_time_ms(1000);
    while(absolute_time_diff_us(timeout, get_absolute_time()) < 0){
        if (this->isReady()) {
            return true;
        }
    }
    return false;
}


bool INA228::trigger(INA228_mode_t mode){
    config_mode = mode;
    uint16_t config = (mode << 12 | config_busConvTime << 9 | config_shuntConvTime << 6 | config_avg);

    vBusMax = 85;
    vShuntMax = 0.015f;
    bool result = writeRegister16(INA228_ADC_CONFIG, config);
    sleep_us(40); // startup time.

    return result;
}

bool INA228::calibrate(float rShuntValue, float iMaxCurrentExcepted)
{
    uint16_t calibrationValue;
    rShunt = rShuntValue;

    currentLSB = iMaxCurrentExcepted / 524288;

    // currentLSB = (uint32_t)(minimumLSB * 100000000);
    // currentLSB /= 100000000;
    // currentLSB /= 0.0001;
    // currentLSB = ceil(currentLSB);
    // currentLSB *= 0.0001;

    powerLSB = currentLSB * 32;

    calibrationValue = (uint16_t)13107.2 * 1000000.0 * currentLSB * rShunt;

    return writeRegister16(INA228_SHUNT_CAL, calibrationValue);
}

float INA228::getMaxPossibleCurrent(void)
{
    return (vShuntMax / rShunt);
}

float INA228::getMaxCurrent(void)
{
    float maxCurrent = (currentLSB * 524288);
    float maxPossible = getMaxPossibleCurrent();

    if (maxCurrent > maxPossible)
    {
        return maxPossible;
    } else
    {
        return maxCurrent;
    }
}

float INA228::getMaxShuntVoltage(void)
{
    float maxVoltage = getMaxCurrent() * rShunt;

    if (maxVoltage >= vShuntMax)
    {
        return vShuntMax;
    } else
    {
        return maxVoltage;
    }
}

float INA228::getMaxPower(void)
{
    return (getMaxCurrent() * vBusMax);
}

uint32_t INA228::getWaitTimeUs(void)
{
    uint16_t config = readRegister16(INA228_ADC_CONFIG);
    uint8_t average = config & 7;
	uint8_t bus = (config>>9) & 7;
	uint8_t shunt = (config>>6) & 7;
    uint8_t temp = (config>>3) & 7;
	uint32_t total_wait = (wait[bus] + wait[shunt] + wait[temp]) * averages[average];

    return total_wait;
}

float INA228::readBusPower(void)
{
    return (readRegister24(INA228_POWER) * powerLSB);
}

float INA228::readShuntCurrent(void)
{
    return (readRegister24(INA228_CURRENT) * currentLSB);
}


float INA228::readShuntVoltage(void)
{
    float voltage;

    voltage = readRegister24(INA228_SHUNT_VOLTAGE) >> 4;

    if (config_range)
    {
        return (voltage * 0.000000078125);
    } else {
        return (voltage * 0.0000003125);
    }
}


float INA228::readBusVoltage(void)
{
    float voltage;

    voltage = readRegister24(INA228_BUS_VOLTAGE) >> 4;

    return (voltage * 0.0001953125);
}

INA228_averages_t INA228::getAverages(void)
{
    uint16_t value;

    value = readRegister16(INA228_ADC_CONFIG);
    value &= INA228_ADC_AVG;

    return (INA228_averages_t)value;
}

INA228_busConvTime_t INA228::getBusConversionTime(void)
{
    uint16_t value;

    value = readRegister16(INA228_ADC_CONFIG);
    value &= INA228_ADC_VBUSCT;
    value >>= 6;

    return (INA228_busConvTime_t)value;
}

INA228_shuntConvTime_t INA228::getShuntConversionTime(void)
{
    uint16_t value;

    value = readRegister16(INA228_ADC_CONFIG);
    value &= INA228_ADC_VSHCT;
    value >>= 3;

    return (INA228_shuntConvTime_t)value;
}

INA228_mode_t INA228::getMode(void)
{
    uint16_t value;

    value = readRegister16(INA228_ADC_CONFIG);
    value &= INA228_ADC_MODE;

    return (INA228_mode_t)value;
}

bool INA228::setMaskEnable(uint16_t mask)
{
    return writeRegister16(INA228_DIAG_ALERT, mask);
}

uint16_t INA228::getMaskEnable(void)
{
    return readRegister16(INA228_DIAG_ALERT);
}

bool INA228::enableShuntOverLimitAlert(void)
{
    return writeRegister16(INA228_DIAG_ALERT, INA228_BIT_SHNTOL);
}

bool INA228::enableShuntUnderLimitAlert(void)
{
    return writeRegister16(INA228_DIAG_ALERT, INA228_BIT_SHNTUL);
}

bool INA228::enableBusOvertLimitAlert(void)
{
    return writeRegister16(INA228_DIAG_ALERT, INA228_BIT_BUSOL);
}

bool INA228::enableBusUnderLimitAlert(void)
{
    return writeRegister16(INA228_DIAG_ALERT, INA228_BIT_BUSUL);
}

bool INA228::enableOverPowerLimitAlert(void)
{
    return writeRegister16(INA228_DIAG_ALERT, INA228_BIT_POL);
}

bool INA228::enableConversionReadyAlert(void)
{
    return writeRegister16(INA228_DIAG_ALERT, INA228_BIT_CNVR);
}

bool INA228::setBusVoltageLimit(float voltage)
{
    uint16_t value = voltage / 0.00125;
    return writeRegister16(INA228_BOVL, value);
}

bool INA228::setShuntVoltageLimit(float voltage)
{
    uint16_t value = voltage / 0.0000025;
    return writeRegister16(INA228_SOVL, value);
}

bool INA228::setPowerLimit(float watts)
{
    uint16_t value = watts / powerLSB;
    return writeRegister16(INA228_POWER_LIMIT, value);
}

bool INA228::setAlertInvertedPolarity(bool inverted)
{
    uint16_t temp = getMaskEnable();

    if (inverted)
    {
        temp |= INA228_BIT_APOL;
    } else
    {
        temp &= ~INA228_BIT_APOL;
    }

    return setMaskEnable(temp);
}

bool INA228::setAlertLatch(bool latch)
{
    uint16_t temp = getMaskEnable();

    if (latch)
    {
        temp |= INA228_BIT_ALATCH;
    } else
    {
        temp &= ~INA228_BIT_ALATCH;
    }

    return setMaskEnable(temp);
}

bool INA228::isMathOverflow(void)
{
    return ((getMaskEnable() & INA228_BIT_MATHOF) == INA228_BIT_MATHOF);
}

bool INA228::isAlert(void)
{
    return ((getMaskEnable() & INA228_BIT_SLOWALERT) == INA228_BIT_SLOWALERT);
}

bool INA228::isReady(void)
{
    return ((getMaskEnable() & INA228_BIT_CNVRF) == INA228_BIT_CNVRF);
}

int16_t INA228::readRegister16(uint8_t reg)
{
    int16_t value;

    wire.beginTransmission(inaAddress);
    wire.write(reg);
    wire.endTransmission();

    wire.requestFrom(inaAddress, 2);
    uint8_t vha = wire.read();
    uint8_t vla = wire.read();
    value = vha << 8 | vla;

    return value;
}

bool INA228::writeRegister16(uint8_t reg, uint16_t val)
{
    uint8_t vla;
    vla = (uint8_t)val;
    val >>= 8;

    wire.beginTransmission(inaAddress);
    wire.write(reg);
    wire.write((uint8_t)val);
    wire.write(vla);
    return wire.endTransmission() != 0xff;
}

int32_t INA228::readRegister24(uint8_t reg)
{
    int32_t value;

    wire.beginTransmission(inaAddress);
    wire.write(reg);
    wire.endTransmission();

    wire.requestFrom(inaAddress, 3);
    uint8_t val1 = wire.read();
    uint8_t val2 = wire.read();
    uint8_t val3 = wire.read();
    value = (val1 << 16) | (val2 << 8) | val3;

    if (value & (1 << 23)) {
        value |= 0xFF000000;
    }


    return value;
}

bool INA228::writeRegister24(uint8_t reg, uint32_t val)
{
    uint8_t val2;
    val2 = (uint8_t)(val >> 8);
    uint8_t val3;
    val3 = (uint8_t)(val);
    val >>= 16;

    wire.beginTransmission(inaAddress);
    wire.write(reg);
    wire.write((uint8_t)val);
    wire.write(val2);
    wire.write(val3);
    return wire.endTransmission() != 0xff;
}
