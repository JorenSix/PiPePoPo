class PitchDetectorAudioProcessor extends AudioWorkletProcessor {

    constructor(options) {
      super();
      this.sampleRate = options.processorOptions.sampleRate;
      console.log(this.sampleRate);
      this.bufferSize = 2048;
      this.bufferIndex = 0;  
      this.buffer = new Float32Array(this.bufferSize);
      this.sampleIndex=0;
    }
  
    process(inputList, outputList, parameters) {
      
      //we only expect one input
      var input = inputList[0]
      var channels = input;
      var analysisChannel = channels[0];
  
      var max = 0;
      for(var i = 0 ; i < analysisChannel.length ; i++){
        this.buffer[this.bufferIndex] = analysisChannel[i];
        max = Math.max(analysisChannel[i],max);
        this.bufferIndex = this.bufferIndex + 1;
        this.sampleIndex = this.sampleIndex + 1;
      }
      //console.log(max);
  
      //if we have a full buffer
      if(this.bufferIndex==this.bufferSize){
        this.bufferIndex = 0;
        var result = this.detectPitchYin(this.buffer,this.sampleRate);
        if(result){
          var detected_pitch_in_hz = result.frequencyInHz;
          var detection_probability = result.probability;
          var detected_pitch_in_cents = this.hzToAbsoluteCents(detected_pitch_in_hz);
          var buffer_start_time_in_s = this.sampleIndex / this.sampleRate;
          var data = {time_in_s: buffer_start_time_in_s, pitch_in_hz: detected_pitch_in_hz, pitch_in_cents: detected_pitch_in_cents,probability: detection_probability};
          this.port.postMessage(data);
        }
      }
      return true;
    }
  
    hzToAbsoluteCents(frequencyInHz){
      //always returns the closest integer, no fractional cents are used
      return Math.round(6900 + 1200 * Math.log(frequencyInHz/440.0)/Math.log(2));
    }
  
    detectPitchYin(float32Buffer,sampleRate) {
  
      const threshold =  0.2;
      
      // Set buffer size to the highest power of two below the provided buffer's length.
     
      // Set up the yinBuffer as described in step one of the YIN paper.
      const yinBufferLength = float32Buffer.length / 2;
      const yinBuffer = new Float32Array(yinBufferLength);
  
      let probability, tau;
  
      // Compute the difference function as described in step 2 of the YIN paper.
      for (let t = 0; t < yinBufferLength; t++) {
        yinBuffer[t] = 0;
      }
      for (let t = 1; t < yinBufferLength; t++) {
        for (let i = 0; i < yinBufferLength; i++) {
          const delta = float32Buffer[i] - float32Buffer[i + t];
          yinBuffer[t] += delta * delta;
        }
      }
  
      // Compute the cumulative mean normalized difference as described in step 3 of the paper.
      yinBuffer[0] = 1;
      let runningSum = 0;
      for (let t = 1; t < yinBufferLength; t++) {
        runningSum += yinBuffer[t];
        yinBuffer[t] *= t / runningSum;
      }
  
      // Compute the absolute threshold as described in step 4 of the paper.
      // Since the first two positions in the array are 1,
      // we can start at the third position.
      for (tau = 2; tau < yinBufferLength; tau++) {
        if (yinBuffer[tau] < threshold) {
          while (tau + 1 < yinBufferLength && yinBuffer[tau + 1] < yinBuffer[tau]) {
            tau++;
          }
          // found tau, exit loop and return
          // store the probability
          // From the YIN paper: The threshold determines the list of
          // candidates admitted to the set, and can be interpreted as the
          // proportion of aperiodic power tolerated
          // within a periodic signal.
          //
          // Since we want the periodicity and and not aperiodicity:
          // periodicity = 1 - aperiodicity
          probability = 1 - yinBuffer[tau];
          break;
        }
      }
  
      // if no pitch found, return null.
      if (tau == yinBufferLength || yinBuffer[tau] >= threshold) {
        return null;
      }
  
      // If probability too low, return -1.
      if (probability < threshold) {
        return null;
      }
  
  
      /**
       * Implements step 5 of the AUBIO_YIN paper. It refines the estimated tau
       * value using parabolic interpolation. This is needed to detect higher
       * frequencies more precisely. See http://fizyka.umk.pl/nrbook/c10-2.pdf and
       * for more background
       * http://fedc.wiwi.hu-berlin.de/xplore/tutorials/xegbohtmlnode62.html
       */
      let betterTau, x0, x2;
      if (tau < 1) {
        x0 = tau;
      } else {
        x0 = tau - 1;
      }
      if (tau + 1 < yinBufferLength) {
        x2 = tau + 1;
      } else {
        x2 = tau;
      }
      if (x0 === tau) {
        if (yinBuffer[tau] <= yinBuffer[x2]) {
          betterTau = tau;
        } else {
          betterTau = x2;
        }
      } else if (x2 === tau) {
        if (yinBuffer[tau] <= yinBuffer[x0]) {
          betterTau = tau;
        } else {
          betterTau = x0;
        }
      } else {
        const s0 = yinBuffer[x0];
        const s1 = yinBuffer[tau];
        const s2 = yinBuffer[x2];
        // fixed AUBIO implementation, thanks to Karl Helgason:
        // (2.0f * s1 - s2 - s0) was incorrectly multiplied with -1
        betterTau = tau + (s2 - s0) / (2 * (2 * s1 - s2 - s0));
      }
  
      return { frequencyInHz: sampleRate / betterTau, probability: probability};;
    };
  
  };
  
  registerProcessor("pitch-detector-audio-processor", PitchDetectorAudioProcessor);