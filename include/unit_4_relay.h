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

#ifndef _UNIT_4_RELAY_H_
#define _UNIT_4_RELAY_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <esp_err.h>
#include <stdbool.h>
#include <stdint.h>

/** @brief 7-bit I2C address of the 4-Relay Unit. */
#define UNIT_4_RELAY_ADDR           0x26
/** @brief Register controlling LED/relay coupling mode (async/sync). */
#define UNIT_4_RELAY_REG_MODE       0x10
/** @brief Register packing the four relay (low nibble) and LED (high nibble) bits. */
#define UNIT_4_RELAY_REG_RELAY      0x11
/** @brief Bit offset of the LED nibble within the relay control register. */
#define UNIT_4_RELAY_LED_BIT_OFFSET 4

/*
 * Channel-to-bit mapping (per datasheet U097 sections 11 and 26).
 *
 * RELAY_CONTROL (0x11) packs the four relays into the LOW nibble with
 * RELAY1 in bit 3 down to RELAY4 in bit 0, and the four LEDs into the HIGH
 * nibble with LED1 in bit 7 down to LED4 in bit 4. The public API uses a
 * 0-based channel where channel 0 == silkscreen "Relay 1". These macros
 * translate that channel into the correct register bit position.
 */
#define UNIT_4_RELAY_RELAY_BIT( channel ) ( 3 - ( channel ) )
#define UNIT_4_RELAY_LED_BIT( channel )                                       \
  ( UNIT_4_RELAY_LED_BIT_OFFSET + 3 - ( channel ) )

/*
 * Mechanical relay settle time (datasheet sections 14 and 23: activation
 * ~10 ms, release ~5 ms; >=10 ms recommended before assuming contact state).
 * The driver blocks for this long after a successful relay state change so a
 * caller can safely assume the contacts have settled on return.
 */
#define UNIT_4_RELAY_SETTLE_MS 15

/**
 * @brief Enable the LEDs and relays to be controlled individually.
 *
 * In asynchronous mode, the LEDs are controlled independently of the relay
 * states. Meaning the relay can be closed (on) and the LED will not change
 * without setting the state of that specific LED channel to turn on using @ref
 * unit_4_relay_led_set.
 *
 */
#define UNIT_4_RELAY_MODE_ASYNC 0

/**
 * @brief Enable the LEDs to automatically follow relay states.
 *
 * In synchronous mode, the LED for each channel mirrors the relay state.
 * Turning a relay on will also turn its LED on, and vice versa.
 * LED control functions have no effect in this mode.
 *
 */
#define UNIT_4_RELAY_MODE_SYNC 1

  /**
   * @brief Initialize the mode of the unit and turn off all relays.
   *
   * In asynchronous mode, the LEDs are controlled independently of the relay
   * states. Meaning the relay can be closed (on) and the LED will not change
   * without setting the state of that specific LED channel to turn on using
   * @ref unit_4_relay_led_set.
   *
   * In synchronous mode, the LEDs will switch with the relay being switched
   * either on or off. The LEDs cannot be controlled independently.
   *
   * @param mode Async = 0 (UNIT_4_RELAY_MODE_ASYNC), Sync = 1
   *             (UNIT_4_RELAY_MODE_SYNC).
   * @return
   *  - ESP_OK              : Success (or already initialized)
   *  - Other               : Error propagated from PA Hub / I2C device add /
   *                          mode or relay-reset write (e.g.
   *                          ESP_ERR_NOT_FOUND, ESP_ERR_TIMEOUT)
   */
  esp_err_t unit_4_relay_init( bool mode );

  /**
   * @brief Deinitialize the 4-Relay Unit.
   *
   * Turns off all relays, releases the I2C device handle, and marks the unit
   * uninitialized so subsequent calls are rejected until re-initialized.
   *
   * @return
   *  - ESP_OK                : Success
   *  - ESP_ERR_INVALID_STATE : Unit not initialized
   *  - Other                 : I2C error while turning relays off
   */
  esp_err_t unit_4_relay_deinit( void );

  /**
   * @brief Get the on/off state of the specified relay.
   * @param[in]  channel Channel number of relay (0-3, where 0 == silkscreen
   *                     "Relay 1").
   * @param[out] state   Set to the relay state: false = OFF, true = ON.
   * @return
   *  - ESP_OK                : Success
   *  - ESP_ERR_INVALID_STATE : Unit not initialized
   *  - ESP_ERR_INVALID_ARG   : channel out of range or state is NULL
   *  - Other                 : I2C read error
   */
  esp_err_t unit_4_relay_relay_get( uint8_t channel, bool *state );

  /**
   * @brief Set the on/off state of the specified relay.
   *
   * On success this call blocks for UNIT_4_RELAY_SETTLE_MS to let the
   * mechanical contacts settle, so the caller can trust the relay state on
   * return.
   *
   * @param[in] channel Channel number of relay (0-3, where 0 == silkscreen
   *                    "Relay 1").
   * @param[in] state   false = OFF, true = ON.
   * @return
   *  - ESP_OK                : Success
   *  - ESP_ERR_INVALID_STATE : Unit not initialized
   *  - ESP_ERR_INVALID_ARG   : channel out of range
   *  - Other                 : I2C read/write error
   */
  esp_err_t unit_4_relay_relay_set( uint8_t channel, bool state );

  /**
   * @brief Get the on/off state of the specified LED.
   * @param[in]  channel Channel of LED (0-3, where 0 == silkscreen "Relay 1").
   * @param[out] state   Set to the LED state: false = OFF, true = ON.
   * @return
   *  - ESP_OK                : Success
   *  - ESP_ERR_INVALID_STATE : Unit not initialized
   *  - ESP_ERR_INVALID_ARG   : channel out of range or state is NULL
   *  - Other                 : I2C read error
   */
  esp_err_t unit_4_relay_led_get( uint8_t channel, bool *state );

  /**
   * @brief Set the on/off state of the specified LED.
   *
   * Only effective in asynchronous mode; in synchronous mode each LED tracks
   * its relay and this call has no visible effect.
   *
   * @param[in] channel Channel of LED (0-3, where 0 == silkscreen "Relay 1").
   * @param[in] state   false = OFF, true = ON.
   * @return
   *  - ESP_OK                : Success
   *  - ESP_ERR_INVALID_STATE : Unit not initialized
   *  - ESP_ERR_INVALID_ARG   : channel out of range
   *  - Other                 : I2C read/write error
   */
  esp_err_t unit_4_relay_led_set( uint8_t channel, bool state );

  /**
   * @brief Set all four relay channels to the same state.
   *
   * Preserves the LED nibble. On success this call blocks for
   * UNIT_4_RELAY_SETTLE_MS to let the mechanical contacts settle.
   *
   * @param[in] state false = all OFF, true = all ON.
   * @return
   *  - ESP_OK                : Success
   *  - ESP_ERR_INVALID_STATE : Unit not initialized
   *  - Other                 : I2C read/write error
   */
  esp_err_t unit_4_relay_relay_all( bool state );

  /**
   * @brief Set the mode of the unit.
   * @param mode UNIT_4_RELAY_MODE_ASYNC (0) or UNIT_4_RELAY_MODE_SYNC (1).
   * @return
   * [esp_err_t](https://docs.espressif.com/projects/esp-idf/en/release-v4.3/esp32/api-reference/system/esp_err.html#macros).
   *  - ESP_OK                : Success
   *  - ESP_ERR_INVALID_STATE : Unit not initialized
   */
  esp_err_t unit_4_relay_mode_set( bool mode );

  /**
   * @brief Get the current mode of the unit.
   * @param mode Pointer to store the current mode (0 = async, 1 = sync).
   * @return
   * [esp_err_t](https://docs.espressif.com/projects/esp-idf/en/release-v4.3/esp32/api-reference/system/esp_err.html#macros).
   *  - ESP_OK                : Success
   *  - ESP_ERR_INVALID_ARG   : NULL pointer
   *  - ESP_ERR_INVALID_STATE : Unit not initialized
   */
  esp_err_t unit_4_relay_mode_get( bool *mode );

  /**
   * @brief Set all the LEDs to the same state (async mode only).
   * @param[in] state false = all OFF, true = all ON.
   * @return
   *  - ESP_OK                : Success
   *  - ESP_ERR_INVALID_STATE : Unit not initialized
   *  - Other                 : I2C read/write error
   */
  esp_err_t unit_4_relay_led_all( bool state );

  /**
   * @brief Check that the 4-Relay Unit is present and responding on the bus.
   *
   * @return
   *  - ESP_OK                : Unit responded
   *  - ESP_ERR_INVALID_STATE : Unit not initialized
   *  - Other                 : I2C read error (unit not responding)
   */
  esp_err_t unit_4_relay_check_connection( void );

#ifdef __cplusplus
}
#endif
#endif
