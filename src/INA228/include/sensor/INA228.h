/*
INA228.h - Header file for the Bi-directional Current/Power Monitor Arduino Library.

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

#ifndef INA228_h
#define INA228_h

#include "sensor/Wire.h"
// #if ARDUINO >= 100
// #include "Arduino.h"
// #else
// #include "WProgram.h"
// #endif

#define INA228_ADDRESS              (0x40)

//      REGISTERS                   (ADDRESS)    BITS  RW
#define INA228_CONFIG               (0x00)    //  16   RW
#define INA228_ADC_CONFIG           (0x01)    //  16   RW
#define INA228_SHUNT_CAL            (0x02)    //  16   RW
#define INA228_SHUNT_TEMP_CO        (0x03)    //  16   RW
#define INA228_SHUNT_VOLTAGE        (0x04)    //  24   R-
#define INA228_BUS_VOLTAGE          (0x05)    //  24   R-
#define INA228_TEMPERATURE          (0x06)    //  16   R-
#define INA228_CURRENT              (0x07)    //  24   R-
#define INA228_POWER                (0x08)    //  24   R-
#define INA228_ENERGY               (0x09)    //  40   R-
#define INA228_CHARGE               (0x0A)    //  40   R-
#define INA228_DIAG_ALERT           (0x0B)    //  16   RW
#define INA228_SOVL                 (0x0C)    //  16   RW
#define INA228_SUVL                 (0x0D)    //  16   RW
#define INA228_BOVL                 (0x0E)    //  16   RW
#define INA228_BUVL                 (0x0F)    //  16   RW
#define INA228_TEMP_LIMIT           (0x10)    //  16   RW
#define INA228_POWER_LIMIT          (0x11)    //  16   RW
#define INA228_MANUFACTURER         (0x3E)    //  16   R-
#define INA228_DEVICE_ID            (0x3F)    //  16   R-

//  CONFIG MASKS (register 0)
#define INA228_CFG_RST              0x8000
#define INA228_CFG_RSTACC           0x4000
#define INA228_CFG_CONVDLY          0x3FC0
#define INA228_CFG_TEMPCOMP         0x0020
#define INA228_CFG_ADCRANGE         0x0010
#define INA228_CFG_RESERVED         0x000F  //  all unused bits


//  ADC MASKS (register 1)
#define INA228_ADC_MODE             0xF000
#define INA228_ADC_VBUSCT           0x0E00
#define INA228_ADC_VSHCT            0x01C0
#define INA228_ADC_VTCT             0x0038
#define INA228_ADC_AVG              0x0007

#define INA228_BIT_ALATCH              (0x8000)
#define INA228_BIT_CNVR              (0x4000)
#define INA228_BIT_SLOWALERT              (0x2000)
#define INA228_BIT_APOL              (0x1000)
#define INA228_BIT_ENERGYOF              (0x0800)
#define INA228_BIT_CHARGEOF              (0x0400)
#define INA228_BIT_MATHOF              (0x0200)
#define INA228_BIT_RESERVED              (0x0100)
#define INA228_BIT_TMPOL              (0x0080)
#define INA228_BIT_SHNTOL              (0x0040)
#define INA228_BIT_SHNTUL             (0x0020)
#define INA228_BIT_BUSOL              (0x0010)
#define INA228_BIT_BUSUL             (0x0008)
#define INA228_BIT_POL              (0x0004)
#define INA228_BIT_CNVRF             (0x0002)
#define INA228_BIT_MEMSTAT              (0x0001)

typedef enum
{
    INA228_AVERAGES_1             = 0b000,
    INA228_AVERAGES_4             = 0b001,
    INA228_AVERAGES_16            = 0b010,
    INA228_AVERAGES_64            = 0b011,
    INA228_AVERAGES_128           = 0b100,
    INA228_AVERAGES_256           = 0b101,
    INA228_AVERAGES_512           = 0b110,
    INA228_AVERAGES_1024          = 0b111
} INA228_averages_t;

typedef enum
{
    INA228_BUS_CONV_TIME_50US    = 0b000,
    INA228_BUS_CONV_TIME_84US    = 0b001,
    INA228_BUS_CONV_TIME_150US    = 0b010,
    INA228_BUS_CONV_TIME_280US    = 0b011,
    INA228_BUS_CONV_TIME_540US   = 0b100,
    INA228_BUS_CONV_TIME_1052US   = 0b101,
    INA228_BUS_CONV_TIME_2074US   = 0b110,
    INA228_BUS_CONV_TIME_4120US   = 0b111
} INA228_busConvTime_t;

typedef enum
{
    INA228_SHUNT_CONV_TIME_50US    = 0b000,
    INA228_SHUNT_CONV_TIME_84US    = 0b001,
    INA228_SHUNT_CONV_TIME_150US   = 0b010,
    INA228_SHUNT_CONV_TIME_280US   = 0b011,
    INA228_SHUNT_CONV_TIME_540US   = 0b100,
    INA228_SHUNT_CONV_TIME_1052US  = 0b101,
    INA228_SHUNT_CONV_TIME_2074US  = 0b110,
    INA228_SHUNT_CONV_TIME_4120US  = 0b111
} INA228_shuntConvTime_t;

typedef enum
{
    INA228_MODE_POWER_DOWN          = 0b0000,
    INA228_MODE_BUS_TRIG            = 0b0001,
    INA228_MODE_SHUNT_TRIG          = 0b0010,
    INA228_MODE_SHUNT_BUS_TRIG      = 0b0011,
    INA228_MODE_TEMP_TRIG           = 0b0100,
    INA228_MODE_TEMP_BUS_TRIG       = 0b0101,
    INA228_MODE_TEMP_SHUNT_TRIG     = 0b0110,
    INA228_MODE_TEMP_BUS_SHUNT_TRIG = 0b0111,
    INA228_MODE_SHUTDOWN            = 0b1000,
    INA228_MODE_BUS_CONT            = 0b1001,
    INA228_MODE_SHUNT_CONT          = 0b1010,
    INA228_MODE_SHUNT_BUS_CONT      = 0b1011,
    INA228_MODE_TEMP_CONT           = 0b1100,
    INA228_MODE_TEMP_BUS_CONT       = 0b1101,
    INA228_MODE_TEMP_SHUNT_CONT     = 0b1110,
    INA228_MODE_TEMP_BUS_SHUNT_CONT = 0b1111,
} INA228_mode_t;

class INA228
{
    public:
	INA228();

	bool begin(i2c_inst_t* i2c, uint8_t address = INA228_ADDRESS);
	bool configure(INA228_averages_t avg = INA228_AVERAGES_1, INA228_busConvTime_t busConvTime = INA228_BUS_CONV_TIME_1052US, INA228_shuntConvTime_t shuntConvTime = INA228_SHUNT_CONV_TIME_1052US, INA228_mode_t mode = INA228_MODE_SHUNT_BUS_CONT, bool range = false);
	bool calibrate(float rShuntValue = 0.1, float iMaxCurrentExcepted = 2);

  bool powerUp();
  bool powerDown();
  bool triggerAndWait(INA228_mode_t mode);

	INA228_averages_t getAverages(void);
	INA228_busConvTime_t getBusConversionTime(void);
	INA228_shuntConvTime_t getShuntConversionTime(void);
	INA228_mode_t getMode(void);

	bool enableShuntOverLimitAlert(void);
	bool enableShuntUnderLimitAlert(void);
	bool enableBusOvertLimitAlert(void);
	bool enableBusUnderLimitAlert(void);
	bool enableOverPowerLimitAlert(void);
	bool enableConversionReadyAlert(void);

	bool setBusVoltageLimit(float voltage);
	bool setShuntVoltageLimit(float voltage);
	bool setPowerLimit(float watts);

	bool setAlertInvertedPolarity(bool inverted);
	bool setAlertLatch(bool latch);

	bool isMathOverflow(void);
	bool isAlert(void);

	float readShuntCurrent(void);
	float readShuntVoltage(void);
	float readBusPower(void);
	float readBusVoltage(void);

	float getMaxPossibleCurrent(void);
	float getMaxCurrent(void);
	float getMaxShuntVoltage(void);
	float getMaxPower(void);

private:
  Wire wire;

	int8_t inaAddress;
	float currentLSB, powerLSB;
	float vShuntMax, vBusMax, rShunt;

  INA228_averages_t config_avg;
  INA228_busConvTime_t config_busConvTime;
  INA228_shuntConvTime_t config_shuntConvTime;
  INA228_mode_t config_mode;

	bool setMaskEnable(uint16_t mask);
	uint16_t getMaskEnable(void);

	bool writeRegister16(uint8_t reg, uint16_t val);
	int16_t readRegister16(uint8_t reg);
};

#endif