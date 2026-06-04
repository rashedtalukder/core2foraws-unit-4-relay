/*!
 * @brief Library for the 4 relay unit by M5Stack on the Core2 for AWS
 *
 * @copyright Copyright (c) 2024 by Rashed Talukder[https://rashedtalukder.com]
 *
 * @license SPDX-License-Identifier: Apache 2.0
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * TODO: Return a fail for attempting to change LED in sync mode
 *
 * @Links [4-Relay](https://docs.m5stack.com/en/unit/4relay)
 * @version  V0.0.4
 * @date  2026-06-04
 */

#include "unit_4_relay.h"
#include "core2foraws.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#ifdef CONFIG_UNIT_4_RELAY_USE_PAHUB
#include "unit_pahub.h"
#endif

static const char *_TAG = "UNIT_4_RELAY";

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c\n"
#define BYTE_TO_BINARY( byte )                                                 \
  ( ( byte ) & 0x80 ? '1' : '0' ), ( ( byte ) & 0x40 ? '1' : '0' ),            \
      ( ( byte ) & 0x20 ? '1' : '0' ), ( ( byte ) & 0x10 ? '1' : '0' ),        \
      ( ( byte ) & 0x08 ? '1' : '0' ), ( ( byte ) & 0x04 ? '1' : '0' ),        \
      ( ( byte ) & 0x02 ? '1' : '0' ), ( ( byte ) & 0x01 ? '1' : '0' )

// Static variables
static bool _initialized = false;
static i2c_master_dev_handle_t _relay_dev = NULL;

// I2C communication functions
static esp_err_t _write_i2c( uint8_t reg, const uint8_t *data, size_t len )
{
#ifdef CONFIG_UNIT_4_RELAY_USE_PAHUB
  return unit_pahub_i2c_write( CONFIG_UNIT_4_RELAY_PAHUB_CHANNEL,
                               _relay_dev, reg, data, len );
#else
  return core2foraws_expports_i2c_write( _relay_dev, reg, data, len );
#endif
}

static esp_err_t _read_i2c( uint8_t reg, uint8_t *data, size_t len )
{
#ifdef CONFIG_UNIT_4_RELAY_USE_PAHUB
  return unit_pahub_i2c_read( CONFIG_UNIT_4_RELAY_PAHUB_CHANNEL,
                              _relay_dev, reg, data, len );
#else
  return core2foraws_expports_i2c_read( _relay_dev, reg, data, len );
#endif
}

esp_err_t unit_4_relay_init( bool mode )
{
  ESP_LOGD( _TAG, "Initializing" );

  if( _initialized )
  {
    ESP_LOGW( _TAG, "Already initialized" );
    return ESP_OK;
  }

#ifdef CONFIG_UNIT_4_RELAY_USE_PAHUB
  esp_err_t ret = unit_pahub_init();
  if( ret != ESP_OK )
  {
    ESP_LOGE( _TAG, "PA Hub initialization failed: %s",
              esp_err_to_name( ret ) );
    return ret;
  }

  ret = unit_pahub_channel_set( CONFIG_UNIT_4_RELAY_PAHUB_CHANNEL );
  if( ret != ESP_OK )
  {
    ESP_LOGE( _TAG, "PA Hub channel set failed: %s", esp_err_to_name( ret ) );
    return ret;
  }
#endif

  esp_err_t err = core2foraws_expports_i2c_device_add( UNIT_4_RELAY_ADDR, 100000, &_relay_dev );
  if( err != ESP_OK )
  {
    ESP_LOGE( _TAG, "Failed to add 4-relay I2C device: %s", esp_err_to_name( err ) );
    _relay_dev = NULL;
    return err;
  }

  // Set mode directly without checking _initialized
  ESP_LOGD( _TAG, "Setting mode to %s mode.",
            mode ? "synchronous" : "asynchronous" );
  uint8_t mode_val = mode ? 1 : 0;
  err = _write_i2c( UNIT_4_RELAY_REG_MODE, &mode_val, 1 );
  if( err != ESP_OK )
  {
    ESP_LOGE( _TAG, "Failed to set mode: %s", esp_err_to_name( err ) );
    core2foraws_i2c_device_remove( _relay_dev );
    _relay_dev = NULL;
    return err;
  }

  // Turn off all relays
  uint8_t new_state = 0x00;
  err = _write_i2c( UNIT_4_RELAY_REG_RELAY, &new_state, 1 );
  if( err != ESP_OK )
  {
    ESP_LOGE( _TAG, "Failed to reset relays: %s", esp_err_to_name( err ) );
    core2foraws_i2c_device_remove( _relay_dev );
    _relay_dev = NULL;
    return err;
  }

  _initialized = true;
  ESP_LOGI( _TAG, "4-Relay Unit initialized successfully" );
  return ESP_OK;
}

esp_err_t unit_4_relay_deinit( void )
{
  if( !_initialized )
  {
    return ESP_ERR_INVALID_STATE;
  }

  // Turn off all relays before deinitializing
  esp_err_t ret = unit_4_relay_relay_all( 0 );
  _initialized = false;

  // Release the I2C device handle so a later re-init does not leak a
  // duplicate device registration on the bus.
  if( _relay_dev != NULL )
  {
    core2foraws_i2c_device_remove( _relay_dev );
    _relay_dev = NULL;
  }

  ESP_LOGI( _TAG, "4-Relay Unit deinitialized" );
  return ret;
}

esp_err_t unit_4_relay_relay_get( uint8_t channel, bool *state )
{
  if( !_initialized )
  {
    return ESP_ERR_INVALID_STATE;
  }

  if( channel > 3 || state == NULL )
  {
    return ESP_ERR_INVALID_ARG;
  }

  uint8_t reg_val;
  ESP_LOGD( _TAG, "Getting channel %d state.", channel );
  esp_err_t err = _read_i2c( UNIT_4_RELAY_REG_RELAY, &reg_val, 1 );
  if( err != ESP_OK )
  {
    return err;
  }

  ESP_LOGD( _TAG,
            "Received state from register (first 4 LED's last 4, "
            "relays): " BYTE_TO_BINARY_PATTERN,
            BYTE_TO_BINARY( reg_val ) );

  *state = ( reg_val >> UNIT_4_RELAY_RELAY_BIT( channel ) ) & 0x01;
  return ESP_OK;
}

esp_err_t unit_4_relay_relay_set( uint8_t channel, bool state )
{
  if( !_initialized )
  {
    return ESP_ERR_INVALID_STATE;
  }

  if( channel > 3 )
  {
    return ESP_ERR_INVALID_ARG;
  }

  ESP_LOGD( _TAG, "Setting channel %d relay to %s.", channel,
            state ? "on" : "off" );

  uint8_t current_state;

  esp_err_t ret = _read_i2c( UNIT_4_RELAY_REG_RELAY, &current_state, 1 );
  if( ret != ESP_OK )
  {
    return ret;
  }

  uint8_t bit_mask = 0x01 << UNIT_4_RELAY_RELAY_BIT( channel );

  if( state == 0 )
  {
    current_state &= ~bit_mask;
  }
  else
  {
    current_state |= bit_mask;
  }

  ret = _write_i2c( UNIT_4_RELAY_REG_RELAY, &current_state, 1 );
  if( ret == ESP_OK )
  {
    // Allow the mechanical contacts to settle before returning so the
    // caller can trust the relay state (datasheet sections 14 and 23).
    vTaskDelay( pdMS_TO_TICKS( UNIT_4_RELAY_SETTLE_MS ) );
  }
  return ret;
}

esp_err_t unit_4_relay_led_get( uint8_t channel, bool *state )
{
  if( !_initialized )
  {
    return ESP_ERR_INVALID_STATE;
  }

  if( channel > 3 || state == NULL )
  {
    return ESP_ERR_INVALID_ARG;
  }

  uint8_t reg_val;
  ESP_LOGD( _TAG, "Getting channel %d LED state.", channel );
  esp_err_t err = _read_i2c( UNIT_4_RELAY_REG_RELAY, &reg_val, 1 );
  if( err != ESP_OK )
  {
    return err;
  }

  ESP_LOGD( _TAG,
            "Received state from register (first 4 LED's last 4, "
            "relays): " BYTE_TO_BINARY_PATTERN,
            BYTE_TO_BINARY( reg_val ) );

  *state = ( reg_val >> UNIT_4_RELAY_LED_BIT( channel ) ) & 0x01;
  return ESP_OK;
}

esp_err_t unit_4_relay_led_set( uint8_t channel, bool state )
{
  if( !_initialized )
  {
    return ESP_ERR_INVALID_STATE;
  }

  if( channel > 3 )
  {
    return ESP_ERR_INVALID_ARG;
  }

  ESP_LOGD( _TAG, "Setting channel %d LED to %s.", channel,
            state ? "on" : "off" );

  uint8_t current_state;

  esp_err_t ret = _read_i2c( UNIT_4_RELAY_REG_RELAY, &current_state, 1 );
  if( ret != ESP_OK )
  {
    return ret;
  }

  uint8_t bit_mask = 1 << UNIT_4_RELAY_LED_BIT( channel );

  if( state == 0 )
  {
    current_state &= ~bit_mask;
  }
  else
  {
    current_state |= bit_mask;
  }
  return _write_i2c( UNIT_4_RELAY_REG_RELAY, &current_state, 1 );
}

esp_err_t unit_4_relay_relay_all( bool state )
{
  if( !_initialized )
  {
    return ESP_ERR_INVALID_STATE;
  }

  ESP_LOGD( _TAG, "Setting all relays to %s.", state ? "on" : "off" );

  // Read current register to preserve LED bits in upper nibble
  uint8_t current_state;
  esp_err_t ret = _read_i2c( UNIT_4_RELAY_REG_RELAY, &current_state, 1 );
  if( ret != ESP_OK )
  {
    return ret;
  }

  if( state )
  {
    current_state |= 0x0F;  // Set all relay bits (lower nibble)
  }
  else
  {
    current_state &= 0xF0;  // Clear all relay bits, keep LED bits
  }

  ret = _write_i2c( UNIT_4_RELAY_REG_RELAY, &current_state, 1 );
  if( ret == ESP_OK )
  {
    // Allow the mechanical contacts to settle before returning
    // (datasheet sections 14 and 23).
    vTaskDelay( pdMS_TO_TICKS( UNIT_4_RELAY_SETTLE_MS ) );
  }
  return ret;
}

esp_err_t unit_4_relay_mode_set( bool mode )
{
  if( !_initialized )
  {
    return ESP_ERR_INVALID_STATE;
  }

  ESP_LOGD( _TAG, "Setting mode to %s mode.",
            mode ? "synchronous" : "asynchronous" );
  uint8_t val = mode ? 1 : 0;
  return _write_i2c( UNIT_4_RELAY_REG_MODE, &val, 1 );
}

esp_err_t unit_4_relay_mode_get( bool *mode )
{
  if( !_initialized )
  {
    return ESP_ERR_INVALID_STATE;
  }

  if( mode == NULL )
  {
    return ESP_ERR_INVALID_ARG;
  }

  uint8_t val;
  esp_err_t err = _read_i2c( UNIT_4_RELAY_REG_MODE, &val, 1 );
  if( err != ESP_OK )
  {
    return err;
  }

  *mode = val & 0x01;
  return ESP_OK;
}

esp_err_t unit_4_relay_led_all( bool state )
{
  if( !_initialized )
  {
    return ESP_ERR_INVALID_STATE;
  }

  ESP_LOGD( _TAG, "Setting all LEDs to %s.", state ? "on" : "off" );

  // Read current register to preserve relay bits in lower nibble
  uint8_t current_state;
  esp_err_t ret = _read_i2c( UNIT_4_RELAY_REG_RELAY, &current_state, 1 );
  if( ret != ESP_OK )
  {
    return ret;
  }

  if( state )
  {
    current_state |= 0xF0;  // Set all LED bits (upper nibble)
  }
  else
  {
    current_state &= 0x0F;  // Clear all LED bits, keep relay bits
  }
  return _write_i2c( UNIT_4_RELAY_REG_RELAY, &current_state, 1 );
}

esp_err_t unit_4_relay_check_connection( void )
{
  if( !_initialized )
  {
    return ESP_ERR_INVALID_STATE;
  }

  // Try to read the relay register to verify connection
  uint8_t reg_val;
  esp_err_t ret = _read_i2c( UNIT_4_RELAY_REG_RELAY, &reg_val, 1 );
  if( ret != ESP_OK )
  {
    ESP_LOGE( _TAG, "4-Relay Unit connection check failed: %s",
              esp_err_to_name( ret ) );
    return ret;
  }

  return ESP_OK;
}
