#include <stdio.h>
#include <stdbool.h>
#include <esp_log.h>
#include "core2foraws.h"
#include "unit_4relay.h"

static const char *_TAG = "UNIT_4RELAY";

esp_err_t unit_4relay_init( bool mode )
{
    ESP_LOGD( _TAG, "Initializing" );
    esp_err_t err = unit_4relay_mode_set( &mode );
    return err |= unit_4relay_relay_all( 0 );
}

esp_err_t unit_4relay_relay_set( uint8_t channel, bool state )
{
    ESP_LOGD( _TAG, "Setting channel %d relay to %s.", channel, state ? "on" : "off" );

    uint8_t current_state;
    
    core2foraws_expports_i2c_read( UNIT_4RELAY_ADDR, UNIT_4RELAY_REG_RELAY, &current_state, 1 );
        
    if ( state == 0 )
    {
        current_state &= ~( 0x01 << channel );
    }
    else
    {
        current_state |= ( 0x01 << channel );
    }
    return core2foraws_expports_i2c_write( UNIT_4RELAY_ADDR, UNIT_4RELAY_REG_RELAY, &current_state, 1 );
}

esp_err_t unit_4relay_led_set( uint8_t channel, bool state )
{
    ESP_LOGD( _TAG, "Setting channel %d LED to %s.", channel, state ? "on" : "off" );

    uint8_t current_state;

    core2foraws_expports_i2c_read( UNIT_4RELAY_ADDR, UNIT_4RELAY_REG_RELAY, &current_state, 1 );
        
    if ( state == 0 )
    {
        current_state &= ~( UNIT_4RELAY_REG_MODE << channel );
    }
    else
    {
        current_state |= ( UNIT_4RELAY_REG_MODE << channel );
    }
    return core2foraws_expports_i2c_write( UNIT_4RELAY_ADDR, UNIT_4RELAY_REG_RELAY, &current_state, 1 );
}

esp_err_t unit_4relay_relay_all( bool state )
{
    ESP_LOGD( _TAG, "Setting all relays to %s.", state ? "on" : "off" );

    uint8_t new_state = state * ( 0x0f );
    return core2foraws_expports_i2c_write( UNIT_4RELAY_ADDR, UNIT_4RELAY_REG_RELAY, &new_state, 1 ); 
}

esp_err_t unit_4relay_mode_set( bool *mode )
{
    ESP_LOGD( _TAG, "Setting mode to %s mode.", mode ? "synchronous" : "asynchronous" );
    return core2foraws_expports_i2c_write( UNIT_4RELAY_ADDR, UNIT_4RELAY_REG_MODE, (const uint8_t *)mode, 1 ); 
}
