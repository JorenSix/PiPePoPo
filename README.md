# PiPePoPo - Pitch Perfect Pointer Positioning - 

This repository contains code to allow a flute to control the mouse cursor. See the movie below to get an idea of the goal. It contains two implementations. The first is a version for microcontrollers which implements the USB HID class and presents itself as a standard mouse to computing hardware. The second implementation is a Chrome browser extention which does not need any hardware components.

https://github.com/JorenSix/PiPePoPo/assets/60453/c97f2a60-9c33-4117-ab7b-841a2256f898

## PiPePoPo Hardware 

[Arduino Nano RP2040 Connect](https://docs.arduino.cc/hardware/nano-rp2040-connect/)

## PiPePoPo Browser extension 

To install the extension, simply download the repository and install an unpacked extension. Since the extension code is not signed it does not work in standard Chrome but it unsigned extensions do work in Chromium. If you want to use the extension as-is, install [Chromium](https://www.chromium.org/Home/) and install the unpacked extension there.

The extension uses a [Web Audio API AudioWorklet](https://developer.mozilla.org/en-US/docs/Web/API/AudioWorklet) to [estimate pitch using a background worklet](). 

## Credits

The Yin is based on the TarsosDSP implementation which itself is based on [aubio](https://aubio.org/).

PiPePoPo by Joren Six

![Using PiPePoPo](./media/pipepopo-in-action.jpg)
