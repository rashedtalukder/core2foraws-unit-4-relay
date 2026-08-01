# M5Stack Unit 4Relay ESP-IDF Component

Driver for the [M5Stack Unit 4Relay](https://docs.m5stack.com/en/unit/4relay) (U097), a four-channel I2C relay unit at address `0x26`.

M5Stack rates each channel for 250 VAC or 28 VDC at 10 A, with 16 A maximum instantaneous current. Disconnect power before wiring loads and follow applicable electrical codes.

## Usage

```c
#include "core2foraws.h"
#include "unit_4_relay.h"

core2foraws_init();
ESP_ERROR_CHECK( core2foraws_expports_i2c_begin() );
ESP_ERROR_CHECK( unit_4_relay_init( UNIT_4_RELAY_MODE_SYNC ) );
ESP_ERROR_CHECK( unit_4_relay_relay_set( 0, true ) );
```

API channel `0` is the enclosure's Relay 1; channel `3` is Relay 4. Relay writes wait 15 ms before returning as a conservative contact-settling policy.

In synchronous mode each LED follows its relay. `unit_4_relay_led_set()` and `unit_4_relay_led_all()` return `ESP_ERR_INVALID_STATE` until asynchronous mode is selected.

Enable `CONFIG_UNIT_4_RELAY_USE_PAHUB` and select channel `0..5` to route transfers through Unit PaHub.

See [datasheet/unit-4.md](datasheet/unit-4.md) for the board register protocol.
