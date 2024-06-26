/*
  This example reads audio data from the on-board PDM microphones, and prints
  out the samples to the Serial console. The Serial Plotter built into the
  Arduino IDE can be used to plot the audio data (Tools -> Serial Plotter)

  Circuit:
  - Arduino Nano 33 BLE board, or
  - Arduino Nano RP2040 Connect, or
  - Arduino Portenta H7 board plus Portenta Vision Shield, or
  - Arduino Nicla Vision

  This example code is in the public domain.
*/

#include "PluggableUSBHID.h"
#include "USBMouse.h"
#include <PDM.h>
#include <math.h>
#include <pico/float.h>
#include "fixed_point.c"
#include "yin_fixed.c"

USBMouse Mouse;

// default number of output channels
static const char channels = 1;

// default PCM output frequency
static const int frequency = 8000;

const int bufferSize = 256;
// Buffer to read samples into, each sample is 16-bits
short sampleBuffer[bufferSize];
short shortSampleBuffer[bufferSize];

int floatSampleIndex = 0;
long totalSampleIndex = 0;

bool verbose = true;

bool benchmark = true;

int mouseMovement = 5;


// Number of audio samples read
volatile int samplesRead;

Yin_Fixed * yin;

void benchmarkFloat(){
  unsigned long start, time;
  
  start = millis();
  float total = 0;
  float one = 0.001;
  float other = 1.00012654;
  for(size_t i = 0; i < 1000000; i++){
    total += (one * other);
  }
  time = millis() - start;

  Serial.print("10^6 float mul add took ");
  Serial.print(time);
  Serial.print(" milliseconds. Result: ");
  Serial.print(total);
  Serial.println();

  start = micros();
  fixed_point total_fixed = fixed_point_from_f(0);
  fixed_point one_fixed = fixed_point_from_f(one);
  fixed_point other_fixed = fixed_point_from_f(other);
  for(size_t i = 0; i < 1000000; i++){
    total_fixed += fixed_point_mul(one_fixed , other_fixed);
  }
  time = micros() - start;

  Serial.print("10^6 fixed mul add took ");
  Serial.print(time);
  Serial.print(" microseconds. ");
  Serial.print("Result: ");
  Serial.print(fixed_point_to_f(total_fixed));

  Serial.println();

}


void setup() {

  if(benchmark || verbose) {
    Serial.begin(115200);
    while (!Serial);
  }

  if(benchmark ) benchmarkFloat();
  
  
  yin = yin_fixed_new(frequency, bufferSize);
 
  // Configure the data receive callback
  PDM.onReceive(onPDMdata);

  // Optionally set the gain
  // Defaults to 20 on the BLE Sense and 24 on the Portenta Vision Shield
  // PDM.setGain(30);

  // Initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate for the Arduino Nano 33 BLE Sense
  // - a 32 kHz or 64 kHz sample rate for the Arduino Portenta Vision Shield
  if (!PDM.begin(channels, frequency)) {
    if(verbose) Serial.println("Failed to start PDM!");
    while (1);
  }
}

unsigned long last_click = 0;
void mouseEvents(float pitch_in_hz){
  if(pitch_in_hz > 500 && pitch_in_hz < 525) Mouse.move(mouseMovement,0);

  if(pitch_in_hz > 565 && pitch_in_hz < 585) Mouse.move(-mouseMovement,0);

  if(pitch_in_hz > 620 && pitch_in_hz < 655) Mouse.move(0,-mouseMovement);

  if(pitch_in_hz > 670 && pitch_in_hz < 696) Mouse.move(0,mouseMovement);


  if(pitch_in_hz > 960 && pitch_in_hz < 990){
    unsigned long now = millis();
    if(now - last_click > 700){
      last_click = now;
      Mouse.click(1);
    }
    
  }
}

void loop() {

  // Wait for samples to be read
  if (!samplesRead) return;
  
  // Print samples to the serial monitor or plotter
  for (int i = 0; i < samplesRead; i++) {
    float sample = sampleBuffer[i];
    shortSampleBuffer[floatSampleIndex] = sample;
    floatSampleIndex++;
    totalSampleIndex++;
  }

  // Clear the read count
  samplesRead = 0;

  //handle full buffer
  if(floatSampleIndex == bufferSize){
    floatSampleIndex=0;

    float pitch = yin_fixed_estimate_pitch(yin, shortSampleBuffer);

    float probability = yin_fixed_last_pitch_estimate_probability(yin);
  
    float time = totalSampleIndex / (float) frequency;
    mouseEvents(pitch);

    if(verbose){
      Serial.print("time_s ");
      Serial.print(time);

      Serial.print(" probability ");
      Serial.print( probability );

      Serial.print(" pitch (Hz) ");
      Serial.print(pitch);

      Serial.println();
    }
  }
}

/**
 * Callback function to process the data from the PDM microphone.
 * NOTE: This callback is executed as part of an ISR.
 * Therefore using `Serial` to print messages inside this function isn't supported.
 * */
void onPDMdata() {
  // Query the number of available bytes
  int bytesAvailable = PDM.available();

  // Read into the sample buffer
  PDM.read(sampleBuffer, bytesAvailable);

  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;
}
