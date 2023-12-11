#include <stdio.h>
#include <stdbool.h>
#include <esp_log.h>
#include "core2foraws.h"
#include "unit_4_relay.h"

static const char *_TAG = "UNIT_4_RELAY";

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c\n"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0')

esp_err_t unit_4_relay_init( bool mode )
{
    ESP_LOGD( _TAG, "Initializing" );
    esp_err_t err = unit_4_relay_mode_set( &mode );
    return err |= unit_4_relay_relay_all( 0 );
}

esp_err_t unit_4_relay_relay_get( uint8_t channel, bool *state )
{
    uint8_t reg_val;
    ESP_LOGD( _TAG, "Getting channel %d state .", channel );
    esp_err_t err = core2foraws_expports_i2c_read( UNIT_4_RELAY_ADDR, UNIT_4_RELAY_REG_RELAY, &reg_val, 1 );

    ESP_LOGD( _TAG, "Received state from register (first 4 LED's last 4, relays): "BYTE_TO_BINARY_PATTERN, 
        BYTE_TO_BINARY( reg_val ) );
    
    *state = reg_val & ( 1 << channel );
    return err;
}

esp_err_t unit_4_relay_relay_set( uint8_t channel, bool state )
{
    ESP_LOGD( _TAG, "Setting channel %d relay to %s.", channel, state ? "on" : "off" );

    uint8_t current_state;
    
    core2foraws_expports_i2c_read( UNIT_4_RELAY_ADDR, UNIT_4_RELAY_REG_RELAY, &current_state, 1 );

    if ( state == 0 )
    {
        current_state &= ~( 0x01 << channel );
    }
    else
    {
        current_state |= ( 0x01 << channel );
    }
    return core2foraws_expports_i2c_write( UNIT_4_RELAY_ADDR, UNIT_4_RELAY_REG_RELAY, &current_state, 1 );
}

esp_err_t unit_4_relay_led_get( uint8_t channel, bool *state )
{
    uint8_t reg_val;
    ESP_LOGD( _TAG, "Getting channel %d state .", channel );
    esp_err_t err = core2foraws_expports_i2c_read( UNIT_4_RELAY_ADDR, UNIT_4_RELAY_REG_RELAY, &reg_val, 1 );

    ESP_LOGD( _TAG, "Received state from register (first 4 LED's last 4, relays): "BYTE_TO_BINARY_PATTERN, 
        BYTE_TO_BINARY( reg_val ) );
    
    *state = ( reg_val >> 0x04 ) & ( 1 << channel );
    return err;
}

esp_err_t unit_4_relay_led_set( uint8_t channel, bool state )
{
    ESP_LOGD( _TAG, "Setting channel %d LED to %s.", channel, state ? "on" : "off" );

    uint8_t current_state;

    core2foraws_expports_i2c_read( UNIT_4_RELAY_ADDR, UNIT_4_RELAY_REG_RELAY, &current_state, 1 );
        
    if ( state == 0 )
    {
        current_state &= ~( UNIT_4_RELAY_REG_MODE << channel );
    }
    else
    {
        current_state |= ( UNIT_4_RELAY_REG_MODE << channel );
    }
    return core2foraws_expports_i2c_write( UNIT_4_RELAY_ADDR, UNIT_4_RELAY_REG_RELAY, &current_state, 1 );
}

esp_err_t unit_4_relay_relay_all( bool state )
{
    ESP_LOGD( _TAG, "Setting all relays to %s.", state ? "on" : "off" );

    uint8_t new_state = state * ( 0x0f );
    return core2foraws_expports_i2c_write( UNIT_4_RELAY_ADDR, UNIT_4_RELAY_REG_RELAY, &new_state, 1 ); 
}

esp_err_t unit_4_relay_mode_set( bool *async_mode )
{
    ESP_LOGD( _TAG, "Setting mode to %s mode.", async_mode ? "synchronous" : "asynchronous" );
    return core2foraws_expports_i2c_write( UNIT_4_RELAY_ADDR, UNIT_4_RELAY_REG_MODE, (const uint8_t *)async_mode, 1 ); 
}
