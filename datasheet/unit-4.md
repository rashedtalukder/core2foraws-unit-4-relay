# Unit 4Relay (SKU: U097)

## Firmware Implementation Specification

---

# 1. Purpose

This document is a **self-contained firmware implementation reference** for the **Unit 4Relay (SKU: U097)** high-voltage relay control module.

The specification contains all information required to implement:

* firmware drivers
* hardware abstraction layer (HAL) support
* board support packages (BSP)
* automated code generation
* test harnesses

The document includes complete details of:

* registers
* communication protocol
* electrical characteristics
* timing behavior
* pin functions
* driver initialization
* runtime operation

This specification is designed so that a firmware driver can be implemented **without access to any other documentation**.

---

# 2. Device Overview

The **Unit 4Relay** module is a **4-channel high-voltage relay control unit** with an integrated microcontroller that provides relay control over an **I2C bus interface**.

The module is designed to switch high-voltage loads while isolating the control electronics from the switched load domain.

### Core MCU

STM32F030F4P6

Key MCU parameters:

| Parameter         | Value         |
| ----------------- | ------------- |
| MCU Core          | ARM Cortex-M0 |
| Flash             | 16 KB         |
| SRAM              | 4 KB          |
| Package           | TSSOP-20      |
| Operating Voltage | 2.4V – 3.6V   |

The MCU manages:

* I2C communication
* relay driver control
* LED indicator control
* state synchronization logic

---

## Key Capabilities

| Feature               | Description                                            |
| --------------------- | ------------------------------------------------------ |
| Relay Channels        | 4 independent relays                                   |
| Relay Control         | I2C controlled                                         |
| Load Capacity         | AC 250V @ 10A                                          |
| DC Capacity           | DC 28V @ 10A                                           |
| Instantaneous Current | 16A                                                    |
| LED Indicators        | Programmable per channel                               |
| LED Modes             | Manual or automatic                                    |
| Isolation             | Physical isolation between high-voltage and logic side |
| Communication         | I2C                                                    |
| I2C Address           | 0x26                                                   |

---

## Internal Subsystems

The device includes the following functional subsystems.

### MCU Control Subsystem

Responsible for:

* protocol decoding
* register control
* relay state management

### Relay Driver Subsystem

Each relay is driven by a transistor stage controlled by the MCU.

Features:

* flyback diode protection
* high-voltage switching capability

### LED Indicator Subsystem

Each relay channel includes a programmable LED.

Modes:

* manual LED control
* LED synchronized with relay state

### Power Regulation Subsystem

Input power:

5V

The circuit contains a **XC6206 low-dropout regulator** producing the internal MCU supply rail.

---

# 3. Electrical Specifications

## Supply Voltage

| Parameter    | Value         |
| ------------ | ------------- |
| Input supply | 5V            |
| MCU supply   | ~3.3V via LDO |
| Logic level  | 3.3V          |

---

## Relay Switching Limits

| Parameter                     | Value |
| ----------------------------- | ----- |
| Maximum AC switching voltage  | 250V  |
| Maximum DC switching voltage  | 28V   |
| Rated current                 | 10A   |
| Maximum instantaneous current | 16A   |

---

## Operating Temperature

Typical operating temperature range:

| Parameter             | Value          |
| --------------------- | -------------- |
| Operating Temperature | -20°C to +70°C |

---

# 4. Absolute Maximum Ratings

| Parameter                   | Limit          |
| --------------------------- | -------------- |
| Supply voltage              | 5.5V           |
| Relay contact voltage       | 250VAC         |
| Relay contact current       | 10A continuous |
| Relay instantaneous current | 16A            |
| Storage temperature         | -40°C to +85°C |

Operation outside these limits may cause permanent damage.

---

# 5. Physical Pins

## Grove HY2.0-4P Interface

| Pin | Name | Direction     | Function         | Notes       |
| --- | ---- | ------------- | ---------------- | ----------- |
| 1   | GND  | Input         | Ground reference | Required    |
| 2   | 5V   | Input         | Module supply    | 5V required |
| 3   | SDA  | Bidirectional | I2C data         | Open drain  |
| 4   | SCL  | Input         | I2C clock        | Open drain  |

---

## Relay Output Terminals

Each relay exposes a **4-pin screw terminal connector**.

Relay terminals correspond to the mechanical relay contacts.

Typical relay contact configuration:

| Contact | Function        |
| ------- | --------------- |
| COM     | Common contact  |
| NO      | Normally open   |
| NC      | Normally closed |

Relay contacts are electrically isolated from control circuitry.

---

# 6. Hardware Integration Constraints

## Power Decoupling

The design uses decoupling capacitors:

| Capacitor | Value |
| --------- | ----- |
| C1        | 0.1µF |
| C2        | 0.1µF |
| C3        | 10µF  |
| C4        | 0.1µF |

These must be placed close to the MCU and LDO.

---

## Relay Protection

Each relay driver includes:

* flyback diode protection

This prevents inductive voltage spikes.

---

## Isolation

The relay contacts are physically isolated from the control circuitry.

The user must ensure:

* adequate PCB clearance
* proper wiring for high voltage

---

# 7. Communication Interface

## I2C Interface

### Bus Parameters

| Parameter  | Value           |
| ---------- | --------------- |
| Protocol   | I2C             |
| Address    | 0x26            |
| Data width | 8-bit registers |

---

### Bus Wiring

| Signal | Type       |
| ------ | ---------- |
| SDA    | open drain |
| SCL    | open drain |

External pull-up resistors are required on the host side.

Typical value:

4.7kΩ

---

### Transaction Format

Write transaction:

```
START
ADDR + W
REGISTER_ADDRESS
DATA
STOP
```

Read transaction:

```
START
ADDR + W
REGISTER_ADDRESS
RESTART
ADDR + R
DATA
STOP
```

---

# 8. Addressing and Device Identification

| Parameter    | Value |
| ------------ | ----- |
| I2C address  | 0x26  |
| Address type | Fixed |

There are no configurable address pins.

Device detection procedure:

1. Send I2C probe to address 0x26
2. Expect ACK

---

# 9. Communication Protocol

The device uses a **memory-mapped register protocol** over I2C.

Register access width:

8 bits

Operations:

* read register
* write register

---

## Example Transaction

Enable relay 1:

```
Write register 0x11
Set bit 3 = 1
```

---

# 10. Register Map

| Register      | Address | Access | Reset | Description            |
| ------------- | ------- | ------ | ----- | ---------------------- |
| MODE_CONTROL  | 0x10    | R/W    | 0x00  | LED mode configuration |
| RELAY_CONTROL | 0x11    | R/W    | 0x00  | Relay and LED control  |

---

# 11. Register Bitfields

---

## MODE_CONTROL (0x10)

| Bits | Name          | Description |
| ---- | ------------- | ----------- |
| 7    | R             | Reserved    |
| 6    | R             | Reserved    |
| 5    | R             | Reserved    |
| 4    | R             | Reserved    |
| 3    | R             | Reserved    |
| 2    | R             | Reserved    |
| 1    | R             | Reserved    |
| 0    | LED_SYNC_MODE |             |

### LED_SYNC_MODE

| Value | Meaning                 |
| ----- | ----------------------- |
| 0     | Manual LED control      |
| 1     | LED follows relay state |

---

## RELAY_CONTROL (0x11)

| Bits | Name   | Description     |
| ---- | ------ | --------------- |
| 7    | LED1   | LED1 control    |
| 6    | LED2   | LED2 control    |
| 5    | LED3   | LED3 control    |
| 4    | LED4   | LED4 control    |
| 3    | RELAY1 | Relay channel 1 |
| 2    | RELAY2 | Relay channel 2 |
| 1    | RELAY3 | Relay channel 3 |
| 0    | RELAY4 | Relay channel 4 |

---

### LED Control

| Value | Meaning |
| ----- | ------- |
| 0     | LED OFF |
| 1     | LED ON  |

---

### Relay Control

| Value | Meaning   |
| ----- | --------- |
| 0     | Relay OFF |
| 1     | Relay ON  |

---

# 12. Reserved Bit Handling

All reserved bits must be:

* preserved during read-modify-write operations
* written as 0 if the register value is unknown

Reserved bits must not be modified.

---

# 13. Data Formats

All registers are **8-bit unsigned values**.

Byte order:

Little-endian (single byte).

---

# 14. Timing Requirements

Typical relay switching delay:

| Parameter             | Value  |
| --------------------- | ------ |
| Relay activation time | ~10 ms |
| Relay release time    | ~5 ms  |

Firmware should allow **≥10 ms delay** after switching state before assuming contact state.

---

# 15. Operating Modes

## LED Manual Mode

LED state controlled directly by register bits.

## LED Automatic Mode

LED state mirrors relay state.

LED control bits are ignored.

---

# 16. Reset Behavior

Power-on reset state:

| Register      | Value |
| ------------- | ----- |
| MODE_CONTROL  | 0x00  |
| RELAY_CONTROL | 0x00  |

Result:

* all relays OFF
* all LEDs OFF

---

# 17. Status and Diagnostics

No diagnostic registers are implemented.

State must be tracked by the driver.

---

# 18. Interrupts

The module does not implement interrupt outputs.

All operations are **polling based**.

---

# 19. Measurement or Operation Flow

Typical relay activation:

1. Initialize I2C bus
2. Detect device at 0x26
3. Configure LED mode
4. Write RELAY_CONTROL register
5. Wait relay activation delay

---

# 20. Driver Initialization Sequence

Recommended initialization:

```
1. Initialize I2C controller
2. Probe address 0x26
3. Write MODE_CONTROL = 0x01 (optional LED auto mode)
4. Write RELAY_CONTROL = 0x00
```

---

# 21. Runtime Operation Sequences

## Turn Relay On

```
read RELAY_CONTROL
set bit relay_n
write RELAY_CONTROL
```

---

## Turn Relay Off

```
read RELAY_CONTROL
clear bit relay_n
write RELAY_CONTROL
```

---

# 22. Recommended Driver State Model

Driver should track:

```
struct relay_device_state
{
    uint8_t relay_state;
    uint8_t led_state;
    uint8_t led_mode;
}
```

---

# 23. Required Safety and Correctness Rules for Code Generation

1. Always preserve reserved bits.
2. Do not toggle relay faster than mechanical limits.
3. Allow ≥10 ms relay switching delay.
4. Perform read-modify-write operations.
5. Ensure correct I2C address (0x26).
6. Avoid simultaneous large load switching.

---

# 24. Minimal Functional Feature Set

A correct driver must support:

* device detection
* relay ON/OFF control
* LED control
* LED mode switching

---

# 25. Canonical Constants

```
I2C_ADDRESS = 0x26

REG_MODE_CONTROL = 0x10
REG_RELAY_CONTROL = 0x11
```

---

# 26. Bitfield Constants

```
MODE_LED_SYNC_MASK = 0x01

RELAY1_MASK = 0x08
RELAY2_MASK = 0x04
RELAY3_MASK = 0x02
RELAY4_MASK = 0x01

LED1_MASK = 0x80
LED2_MASK = 0x40
LED3_MASK = 0x20
LED4_MASK = 0x10
```

---

# 27. Enumerations

```
enum led_mode
{
    LED_MODE_MANUAL = 0,
    LED_MODE_AUTO = 1
};

enum relay_state
{
    RELAY_OFF = 0,
    RELAY_ON = 1
};
```

---

# 28. Final Implementation Intent

A generated firmware driver must:

* communicate via I2C
* manipulate registers 0x10 and 0x11
* support relay and LED control
* correctly preserve reserved bits
* respect relay mechanical timing

Common mistakes to avoid:

* writing registers without read-modify-write
* toggling relays too quickly
* assuming LED control works in auto mode
* failing to wait for relay switching time
