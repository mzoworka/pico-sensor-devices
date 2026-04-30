if (PICO_ON_DEVICE)
    if (TARGET sensor_INA228)

        add_executable(INA228_example
                main.cpp
                )

        target_link_libraries(INA228_example pico_stdlib 
          hardware_i2c
          sensor_INA228)
        pico_add_extra_outputs(INA228_example)
    endif()
endif()