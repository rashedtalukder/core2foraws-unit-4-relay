cmake_minimum_required( VERSION 3.16.0 )

set( requires "Core2-for-AWS-IoT-Kit" )
set( include_dirs "./include" )

idf_component_register(
    SRCS "unit_4_relay.c"
    INCLUDE_DIRS ${include_dirs}
    REQUIRES ${requires}
)

if( CONFIG_UNIT_4_RELAY_USE_PAHUB )
    idf_component_optional_requires( PRIVATE core2foraws-unit-pahub )
endif()