# PiPePoPo - Pitch Perfect Pointer Positioning

This repository contains code to allow a flute to control the mouse cursor. See the movie below to get an idea of the concept.

https://github.com/JorenSix/PiPePoPo/assets/60453/c97f2a60-9c33-4117-ab7b-841a2256f898

It contains two implementations. The first implementation is microcontroller-based: a microphone-enabled microcontoller presents itself as a standard mouse to computing hardware and translates detected pitch in mouse movement. The second implementation is a Chrome browser extention and it does not need additional hardware: it uses the device microphone and Javascript to translate pitch into cursor movement.

## PiPePoPo Hardware

The hardware implementation uses an [Arduino Nano RP2040 Connect](https://docs.arduino.cc/hardware/nano-rp2040-connect/). The Arduino Nano RP2040 Connect is a microcontroller board based on the Raspberry Pi RP2040 microcontroller chip. The board has a built-in microphone and can be programmed using the Arduino IDE. The Arduino Nano RP2040 Connect is connected to a computer using a USB cable. The Arduino Nano RP2040 Connect presents itself as a standard mouse to the computer and translates detected pitch into mouse movement.

Similar microcontrollers with a built-in microphone can be used. The code is written in C/C++. The pitch detection is done using the [Yin pitch detection algorithm](http://audition.ens.fr/adc/pdf/2002_JASA_YIN.pdf).

## PiPePoPo Browser extension

The browser extension implementation uses the device microphone and Javascript to translate pitch into cursor movement. The extension is a Chrome extension and can be installed in Chromium.

To install the extension, simply download the repository and install an unpacked extension. Since the extension code is not signed it does not work in standard Chrome but it unsigned extensions do work in Chromium. If you want to use the extension as-is, install [Chromium](https://www.chromium.org/Home/) and install the unpacked extension there.

The extension uses a [Web Audio API AudioWorklet](https://developer.mozilla.org/en-US/docs/Web/API/AudioWorklet) to [estimate pitch using a background worklet](./PiPe_PoPo_chrome_ext/pitch.detector.audioworklet.js).

## Credits

The Yin is based on the TarsosDSP implementation which itself is based on [aubio](https://aubio.org/).

PiPePoPo by Joren Six

![Using PiPePoPo](./media/pipepopo-in-action.jpg)
