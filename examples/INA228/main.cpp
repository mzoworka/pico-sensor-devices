#include <stdio.h>
#include "pico/stdlib.h"

#include "sensor/INA228.h"

INA228 INA228;

int main(void){
  stdio_init_all();

  i2c_init(i2c_default, 100 * 1000);
  gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
  gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

  INA228.begin(i2c_default, INA228_ADDRESS);
  INA228.calibrate();
  INA228.configure(INA228_AVERAGES_16, INA228_BUS_CONV_TIME_1052US, INA228_SHUNT_CONV_TIME_1052US, INA228_MODE_SHUNT_BUS_TRIG);

  while(true){
    if (INA228.triggerAndWait(INA228_MODE_SHUNT_BUS_TRIG)) {
      printf("Battery Voltage: %.3f V\n", INA228.readBusVoltage());
      printf("Battery Current: %.3f mA\n", INA228.readShuntCurrent() * 1000);
    } else {
      printf("Battery Voltage/Current Conversion Failed: Timeout\n");
    }
    INA228.powerDown();

    sleep_ms(5000);
  }
}