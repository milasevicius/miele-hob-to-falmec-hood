# Miele Hob to Falmec Hood Bridge

An ESP32 bridge that connects a Miele induction hob to a Falmec cooker hood, automatically adjusting fan speed and lighting based on hob activity, with HomeKit integration for manual control.

### Key Features

- **Seamless automation**: Automatically controls hood fan speed and lighting based on real-time hob activity
- **Smart power mapping**:
  - Hob off → Fan speed 0, light off
  - Keep warm mode → Fan speed 0, light on
  - Boost modes → Fan speed 1, light on
  - Regular levels → Proportional fan speeds (1-3), light on
- **HomeKit integration**:
  - Manual fan speed control (0-4)
  - Manual light control (on / off)
  - Miele automation override switch
  - Real-time fan speed and light display

## Getting Started

Follow these steps to set up the ESP32 bridge and connect your Miele hob to your Falmec hood with HomeKit integration.

### Prerequisites

* Miele hob with [Miele@Home](https://www.miele.com/en/com/mielehome-5235.htm)
* Falmec hood with [remote control](https://www.falmec.com/en-us/accessories/other-accessories/remote-control-2/)
* Apple smart home hub (e.g. [HomePod mini](https://www.apple.com/homepod-mini/))
* ESP32 development board (e.g. [M5Stack Atom Lite](https://shop.m5stack.com/products/atom-lite-esp32-development-kit))
* 433MHz RF transmitter module (e.g. [M5Stack RF433T Unit](https://shop.m5stack.com/products/rf-unit-433mhz-transmitter-syn115))
* PlatformIO CLI or IDE

### Installation

1. Get Miele [developer API credentials](https://www.miele.com/f/com/en/register_api.aspx)
2. Get Miele refresh token using [Swagger OAuth2 authorization flow](https://www.miele.com/developer/openapi/index.html)
3. Get Miele hob device ID (e.g. using Swagger `GET /devices` endpoint)
4. Set up environment variables:
```bash
export MIELE_CLIENT_ID="your_client_id"
export MIELE_CLIENT_SECRET="your_client_secret"
export MIELE_REFRESH_TOKEN="your_refresh_token"
export MIELE_DEVICE_ID="your_device_id"
```
5. Set up your board in `platformio.ini` (e.g. using `[env:m5stack-atom]`)
6. Build and upload the firmware:
```bash
pio run -t upload
```
7. Set up WiFi credentials and pair to HomeKit using [HomeSpan User Guide](https://github.com/HomeSpan/HomeSpan/blob/master/docs/HomeSpanUserGuide.pdf)
