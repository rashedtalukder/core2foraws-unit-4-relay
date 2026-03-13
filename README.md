# M5Stack 4-Relay Unit ESP-IDF Component

This is a component library for use with the M5Stack 4-Relay unit over I2C on the Core2 for AWS IoT Kit. Uses the abstractions built in to the [BSP for the Core2 for AWS](https://github.com/m5stack/Core2-for-AWS-IoT-Kit/tree/BSP-dev).

## Features

- Control 4 independent relay channels
- LED indicators for each relay
- Synchronous and asynchronous operating modes
- Support for PA Hub connectivity

## Hardware Information

- **Product Page**: [M5Stack 4-Relay Unit](https://docs.m5stack.com/en/unit/4relay)
- **I2C Address**: 0x26
- **Operating Voltage**: 5V
- **Relay Type**: SPDT (Single Pole Double Throw)
- **Current Rating**: 10A @ 250VAC, 10A @ 28VDC

## Configuration

### PA Hub Support

If you're using a PA Hub to connect multiple I2C devices, enable PA Hub support in menuconfig:
Component config → M5Stack 4-Relay Unit Configuration → [*] Use PA Hub for 4-Relay Unit

Set the appropriate PA Hub channel (0-5) where your 4-Relay Unit is connected.

### Operating Modes

- **Asynchronous Mode (0)**: LEDs are controlled independently of relay states
- **Synchronous Mode (1)**: LEDs automatically follow relay states

## Usage Example

```c
#include "esp_log.h"
#include "unit_4_relay.h"

static const char *TAG = "4_relay_example";

void app_main( void )
{
  // Initialize in asynchronous mode (LEDs controlled independently of relays)
  esp_err_t ret = unit_4_relay_init( UNIT_4_RELAY_MODE_ASYNC );
  if( ret != ESP_OK )
  {
    ESP_LOGE( TAG, "Failed to initialize 4-Relay Unit: %s",
              esp_err_to_name( ret ) );
    return;
  }

  // Check connection
  ret = unit_4_relay_check_connection();
  if( ret != ESP_OK )
  {
    ESP_LOGE( TAG, "4-Relay Unit not connected" );
    return;
  }

  ESP_LOGI( TAG, "4-Relay Unit initialized successfully" );

  // Turn on relay 0
  ret = unit_4_relay_relay_set( 0, true );
  if( ret == ESP_OK )
  {
    ESP_LOGI( TAG, "Relay 0 turned ON" );
  }

  // Turn on LED for relay 0 (in async mode)
  ret = unit_4_relay_led_set( 0, true );
  if( ret == ESP_OK )
  {
    ESP_LOGI( TAG, "LED 0 turned ON" );
  }

  // Check relay state
  bool relay_state;
  ret = unit_4_relay_relay_get( 0, &relay_state );
  if( ret == ESP_OK )
  {
    ESP_LOGI( TAG, "Relay 0 state: %s", relay_state ? "ON" : "OFF" );
  }

  // Turn on all relays at once (preserves LED states)
  ret = unit_4_relay_relay_all( true );
  if( ret == ESP_OK )
  {
    ESP_LOGI( TAG, "All relays turned ON" );
  }

  // Turn on all LEDs at once (async mode only; preserves relay states)
  ret = unit_4_relay_led_all( true );
  if( ret == ESP_OK )
  {
    ESP_LOGI( TAG, "All LEDs turned ON" );
  }

  // Wait 5 seconds
  vTaskDelay( pdMS_TO_TICKS( 5000 ) );

  // Turn off all relays (LED states are preserved)
  ret = unit_4_relay_relay_all( false );
  if( ret == ESP_OK )
  {
    ESP_LOGI( TAG, "All relays turned OFF" );
  }

  // Switch to synchronous mode
  ret = unit_4_relay_mode_set( UNIT_4_RELAY_MODE_SYNC );
  if( ret == ESP_OK )
  {
    ESP_LOGI( TAG, "Switched to synchronous mode" );

    // In sync mode, LEDs will follow relay states automatically
    unit_4_relay_relay_set( 1, true ); // LED 1 will also turn on
    ESP_LOGI( TAG, "Relay 1 ON (LED 1 follows automatically)" );
  }

  // Cleanup
  unit_4_relay_deinit();
}
```

## API Reference

Initialization Functions

- `unit_4_relay_init(bool mode)` - Initialize with operating mode
- `unit_4_relay_deinit(void)` - Deinitialize and turn off all relays
- `unit_4_relay_check_connection(void)` - Verify unit connectivity
  Relay Control Functions
- `unit_4_relay_relay_set(uint8_t channel, bool state)` - Control individual relay
- `unit_4_relay_relay_get(uint8_t channel, bool *state)` - Get relay state
- `unit_4_relay_relay_all(bool state)` - Control all relays simultaneously
  LED Control Functions (Async Mode Only)
- `unit_4_relay_led_set(uint8_t channel, bool state)` - Control individual LED
- `unit_4_relay_led_get(uint8_t channel, bool *state)` - Get LED state
- `unit_4_relay_led_all(bool state)` - Control all LEDs simultaneously
  Configuration Functions
- `unit_4_relay_mode_set(bool mode)` - Set operating mode
- `unit_4_relay_mode_get(bool *mode)` - Get current operating mode

## Safety Considerations

⚠️ Warning: This unit controls high-voltage relays. Always observe proper electrical safety procedures:

- Ensure power is disconnected before making connections
- Use appropriate wire gauges for your current requirements
- Follow local electrical codes and regulations
- Test your setup thoroughly before deploying

## Error Handling

All functions return `esp_err_t` values:

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid parameter
- `ESP_ERR_INVALID_STATE` - Unit not initialized
- Other I2C-related errors

### Dependencies

- ESP-IDF I2C driver
- Core2 for AWS BSP (if not using PA Hub)
- PA Hub component (if using PA Hub)
