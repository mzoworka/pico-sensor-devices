# pico_simple_sensor_target(INA228)
# target_link_libraries(sensor_INA228 INTERFACE sensor_Wire)

if (NOT TARGET sensor_INA228)
    add_library(sensor_INA228 INTERFACE)
    
    # pico_generate_pio_header(pico_sd_card ${CMAKE_CURRENT_LIST_DIR}/sd_card.pio)
    
    target_sources(sensor_INA228 INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/INA228.cpp
    )
    
    target_include_directories(sensor_INA228 INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
    target_link_libraries(sensor_INA228 INTERFACE sensor_Wire)
endif()