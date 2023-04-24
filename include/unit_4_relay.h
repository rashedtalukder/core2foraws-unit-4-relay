/*!
 * @brief Library for the 4 relay unit by M5Stack on the Core2 for AWS
 * @copyright Copyright (c) 2022 by Rashed Talukder[https://rashedtalukder.com]
 * 
 * TODO: Return a fail for attempting to change LED in sync mode
 *
 * @Links [4-Relay](https://docs.m5stack.com/en/unit/4relay)
 * @version  V0.0.1
 * @date  2022-11-09
 */
#ifndef _UNIT_4_RELAY_H_
#define _UNIT_4_RELAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <esp_err.h>

#define UNIT_4_RELAY_ADDR      0x26
#define UNIT_4_RELAY_REG_MODE  0x10
#define UNIT_4_RELAY_REG_RELAY 0x11

/** 
 * @brief Initialize the mode of the unit and turn off all relays.
 * 
 * In asynchronous mode, the LEDs are controlled independently of the relay states. Meaning 
 * the relay can be closed (on) and the LED will not change without setting the state of 
 * that specific LED channel to turn on using @ref unit_4_relay_led_set.
 * 
 * In synchronous mode, the LEDs will switch with the relay being switched either on or off. 
 * The LEDs cannot be controlled independently.
 * 
 * @param mode Async = 0, Sync = 1.
 * @return [esp_err_t](https://docs.espressif.com/projects/esp-idf/en/release-v4.3/esp32/api-reference/system/esp_err.html#macros).
 *  - ESP_OK                : Success
 *  - ESP_ERR_INVALID_ARG	: Driver parameter error
 */
esp_err_t unit_4_relay_init( bool mode );

/** 
 * @brief Set the on/off state of the specified relay.
 * @param channel Relay channel number of relay (0~3).
 * @param state OFF = 0, ON = 1.
 * @return [esp_err_t](https://docs.espressif.com/projects/esp-idf/en/release-v4.3/esp32/api-reference/system/esp_err.html#macros).
 *  - ESP_OK                : Success
 *  - ESP_ERR_INVALID_ARG	: Driver parameter error
 */
esp_err_t unit_4_relay_relay_set( uint8_t channel, bool state );

/** 
 * @brief Set the on/off state of the specified LED.
 * @param channel Relay channel of LED (0~3).
 * @param state OFF = 0, ON = 1.
 * @return [esp_err_t](https://docs.espressif.com/projects/esp-idf/en/release-v4.3/esp32/api-reference/system/esp_err.html#macros).
 *  - ESP_OK                : Success
 *  - ESP_ERR_INVALID_ARG	: Driver parameter error
 */
esp_err_t unit_4_relay_led_set( uint8_t channel, bool state );

/** 
 * @brief Set all the relay channels to the same state.
 * @param state 
 * @return [esp_err_t](https://docs.espressif.com/projects/esp-idf/en/release-v4.3/esp32/api-reference/system/esp_err.html#macros).
 *  - ESP_OK                : Success
 *  - ESP_ERR_INVALID_ARG	: Driver parameter error
 */
esp_err_t unit_4_relay_relay_all( bool state );

/** 
 * @brief Set the mode of the unit.
 * @param mode Async = 0, Sync = 1.
 * @return [esp_err_t](https://docs.espressif.com/projects/esp-idf/en/release-v4.3/esp32/api-reference/system/esp_err.html#macros).
 *  - ESP_OK                : Success
 *  - ESP_ERR_INVALID_ARG	: Driver parameter error
 */
esp_err_t unit_4_relay_mode_set( bool *mode );

#ifdef __cplusplus
}
#endif
#endif
