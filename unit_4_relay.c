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
 * @version  V0.0.2
 * @date  2023-12-11
 */

#include "unit_4_relay.h"
#include "core2foraws.h"
#include <esp_log.h>
#include <stdio.h>

static const char *_TAG = "UNIT_4_RELAY";

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c\n"
#define BYTE_TO_BINARY( byte )                                                 \
  ( ( byte ) & 0x80 ? '1' : '0' ), ( ( byte ) & 0x40 ? '1' : '0' ),            \
      ( ( byte ) & 0x20 ? '1' : '0' ), ( ( byte ) & 0x10 ? '1' : '0' ),        \
      ( ( byte ) & 0x08 ? '1' : '0' ), ( ( byte ) & 0x04 ? '1' : '0' ),        \
      ( ( byte ) & 0x02 ? '1' : '0' ), ( ( byte ) & 0x01 ? '1' : '0' )

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
  esp_err_t err = core2foraws_expports_i2c_read(
      UNIT_4_RELAY_ADDR, UNIT_4_RELAY_REG_RELAY, &reg_val, 1 );

  ESP_LOGD( _TAG,
            "Received state from register (first 4 LED's last 4, "
            "relays): " BYTE_TO_BINARY_PATTERN,
            BYTE_TO_BINARY( reg_val ) );

  *state = reg_val & ( 1 << channel );
  return err;
}

esp_err_t unit_4_relay_relay_set( uint8_t channel, bool state )
{
  ESP_LOGD( _TAG, "Setting channel %d relay to %s.", channel,
            state ? "on" : "off" );

  uint8_t current_state;

  core2foraws_expports_i2c_read( UNIT_4_RELAY_ADDR, UNIT_4_RELAY_REG_RELAY,
                                 &current_state, 1 );

  if( state == 0 )
  {
    current_state &= ~( 0x01 << channel );
  }
  else
  {
    current_state |= ( 0x01 << channel );
  }
  return core2foraws_expports_i2c_write(
      UNIT_4_RELAY_ADDR, UNIT_4_RELAY_REG_RELAY, &current_state, 1 );
}

esp_err_t unit_4_relay_led_get( uint8_t channel, bool *state )
{
  uint8_t reg_val;
  ESP_LOGD( _TAG, "Getting channel %d state .", channel );
  esp_err_t err = core2foraws_expports_i2c_read(
      UNIT_4_RELAY_ADDR, UNIT_4_RELAY_REG_RELAY, &reg_val, 1 );

  ESP_LOGD( _TAG,
            "Received state from register (first 4 LED's last 4, "
            "relays): " BYTE_TO_BINARY_PATTERN,
            BYTE_TO_BINARY( reg_val ) );

  *state = ( reg_val >> 0x04 ) & ( 1 << channel );
  return err;
}

esp_err_t unit_4_relay_led_set( uint8_t channel, bool state )
{
  ESP_LOGD( _TAG, "Setting channel %d LED to %s.", channel,
            state ? "on" : "off" );

  uint8_t current_state;

  core2foraws_expports_i2c_read( UNIT_4_RELAY_ADDR, UNIT_4_RELAY_REG_RELAY,
                                 &current_state, 1 );

  if( state == 0 )
  {
    current_state &= ~( UNIT_4_RELAY_REG_MODE << channel );
  }
  else
  {
    current_state |= ( UNIT_4_RELAY_REG_MODE << channel );
  }
  return core2foraws_expports_i2c_write(
      UNIT_4_RELAY_ADDR, UNIT_4_RELAY_REG_RELAY, &current_state, 1 );
}

esp_err_t unit_4_relay_relay_all( bool state )
{
  ESP_LOGD( _TAG, "Setting all relays to %s.", state ? "on" : "off" );

  uint8_t new_state = state * ( 0x0f );
  return core2foraws_expports_i2c_write(
      UNIT_4_RELAY_ADDR, UNIT_4_RELAY_REG_RELAY, &new_state, 1 );
}

esp_err_t unit_4_relay_mode_set( bool *async_mode )
{
  ESP_LOGD( _TAG, "Setting mode to %s mode.",
            async_mode ? "synchronous" : "asynchronous" );
  return core2foraws_expports_i2c_write( UNIT_4_RELAY_ADDR,
                                         UNIT_4_RELAY_REG_MODE,
                                         (const uint8_t *)async_mode, 1 );
}
