//-------------------------------------------BBK ENV APPROACH Holtek--------------------------------//
// Uses ENV pin amplitude + time logic with min/max whistle duration + cooldown
//-------------------------------------------------------------------------------------------//
#define ENV_PIN 12  // Connect BMV23M001 ENV pin here
// #define COOLDOWN_PERIOD 1500  // ms, cooldown period between valid whistles
#define COOLDOWN_PERIOD settings_buff[4] * 1000
#define BEEP_FREQUENCY 1000   // Frequency of the beep (1 kHz)
#define BEEP_DURATION_MS 500  // Duration of beep in milliseconds
//-----------------------------------Global Variables----------------------------------------//
extern int set_cooker_wistle_cnt, whistle_ka_count;  // Desired whistle count
int prevwhistleCount = 0;
unsigned long whistleStart = 0;
unsigned long lastWhistleTime = 0;

bool inWhistle = false;
bool whistleCompleteFlag = false;

int whistleCount = 0, WHISTLE_THRESHOLD = 0;
int MIN_WHISTLE_DURATION = 0, MAX_WHISTLE_DURATION = 0;
int prev_ADC = 0;
//-----------------------------------Function Definitions-----------------------------------//
void Whistle_Setup() {
  pinMode(ENV_PIN, INPUT);
  whistleCount = 0;
  inWhistle = false;
  whistleCompleteFlag = false;
  WHISTLE_THRESHOLD = settings_buff[0];
  // MIN_WHISTLE_DURATION = settings_buff[1];
  // MAX_WHISTLE_DURATION = settings_buff[2];
  MIN_WHISTLE_DURATION = settings_buff[1] * 1000;  // sec → ms
  MAX_WHISTLE_DURATION = settings_buff[2] * 1000;  // sec → ms
  Serial.println(WHISTLE_THRESHOLD);
  Serial.println(MIN_WHISTLE_DURATION);
  Serial.println(MAX_WHISTLE_DURATION);
  Serial.println(COOLDOWN_PERIOD);
  Serial.println(settings_buff[3]);
  //  Serial.println(WHISTLE_THRESHOLD);
  //  Serial.println("ENV-PIN, based Whistle Detection Initialized");
  initI2S_alert();
}
//-------------------------------------------------------------------------------------------//
void Process_Whistle_Count() {
  int current_adc = analogRead(ENV_PIN);z
  prev_ADC = ((prev_ADC * 0.9) + (current_adc * 0.1));
  Serial.print("current_adc: ");
  Serial.print(current_adc);
  Serial.print("\tprev_ADC: ");
  Serial.println(prev_ADC);
  // tft.fillRect(220, 180, 45, 45, ILI9341_BLACK);
  // display_value(220, 200, prev_ADC, ILI9341_RED, ILI9341_BLACK, 1);
  unsigned long now = millis();

  // tft.fillRect(190, 175, 20 * 3, 30, ILI9341_BLACK);
  // display_value(200, 190, settings_buff[3], ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
  int barStartX = 170;  // starting X position for the bar
  int barStartY = 185;  // aligned near text
  int barWidth = 10;    // width of each rectangle
  int barHeight = 10;   // height of each rectangle
  int barGap = 5;       // spacing between rectangles

  for (int i = 0; i < 5; i++) {
    uint16_t color = (i < settings_buff[3]) ? ILI9341_GREEN : ILI9341_DARKGREY;
    tft.fillRect(barStartX + (i * (barWidth + barGap)), barStartY, barWidth, barHeight, color);
  }
  if (settings_buff[3] == 1) {
    WHISTLE_THRESHOLD = settings_buff[0] + 200;
  } else if (settings_buff[3] == 2) {
    WHISTLE_THRESHOLD = settings_buff[0] + 150
  } else if (settings_buff[3] == 3) {
    WHISTLE_THRESHOLD = settings_buff[0] + 100;
  } else if (settings_buff[3] == 4) {
    WHISTLE_THRESHOLD = settings_buff[0] + 50;
  } else if (settings_buff[3] == 5) {
    WHISTLE_THRESHOLD = settings_buff[0];
  }
  Serial.print(WHISTLE_THRESHOLD);
  if (prev_ADC > WHISTLE_THRESHOLD)  //rising edge count here
  {
    if (!inWhistle) {
      whistleStart = now;
      inWhistle = true;
    } else {
      unsigned long whistleDuration = now - whistleStart;
      if (whistleDuration > MAX_WHISTLE_DURATION) {
        inWhistle = false;
        //        Serial.println("Whistle ignored: too long (while high)");
      }
    }
  } else  //falling edge count here
  {
    if (inWhistle) {
      unsigned long whistleDuration = now - whistleStart;

      if (whistleDuration >= MIN_WHISTLE_DURATION && whistleDuration <= MAX_WHISTLE_DURATION) {
        if ((now - lastWhistleTime) >= COOLDOWN_PERIOD) {
          whistleCount++;
          whistle_ka_count = whistleCount;
          lastWhistleTime = now;
          if (whistle_ka_count >= set_cooker_wistle_cnt) {
            whistleCompleteFlag = true;
            //            Serial.println("Whistle target reached!");
          }
          tft.fillRect(180, 130, 20 * 1, 25, ILI9341_BLACK);
          if (whistle_ka_count <= set_cooker_wistle_cnt) {
            display_value(180, 140, prevwhistleCount, ILI9341_BLACK, ILI9341_BLACK, 1);
            display_value(180, 140, whistleCount, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);

          } else {

            display_value(180, 140, prevwhistleCount, ILI9341_BLACK, ILI9341_BLACK, 1);
            display_value(180, 140, whistleCount, ILI9341_RED, ILI9341_BLACK, 1);
          }
          prevwhistleCount = whistleCount;
        } else {
          //          Serial.println("Whistle ignored: cooldown not finished");
        }
      } else if (whistleDuration > MAX_WHISTLE_DURATION) {
        //        Serial.println("Whistle ignored: too long (on fall)");
      } else  // too short
      {
        //        Serial.println("Whistle ignored: too short");
      }
      inWhistle = false;
    }
  }

  if (whistleCompleteFlag) {
    // Serial.println("Desired number of whistles detected. Resetting count...");
    //    whistleCount = 0;
    for (int i = 0; i < 10; i++) {
      beep(BEEP_FREQUENCY, BEEP_DURATION_MS);
    }
    whistleCompleteFlag = false;
    return;
  }
}
//-------------------------------------------------------------------------------------------------------------------//

// void beep(int frequency, int duration_ms) {
//   int samples = (SAMPLERATE * duration_ms) / 1000;  // Total samples based on duration
//   int16_t buffer[BUFFER_SIZE];                      // 16-bit buffer for I2S

//   // Generate the sine wave for the given frequency
//   float phase = 0.0;                                         // Phase of the sine wave (ranging from 0 to 2*pi)
//   float phaseIncrement = 2 * M_PI * frequency / SAMPLERATE;  // Phase increment per sample

//   // Calculate when to start the fade-out (fade starts after half of the duration)
//   int fadeOutStartSample = samples - (SAMPLERATE * 0.5);  // Fade-out starts after half the duration

//   // Volume scaling factor (lower values = quieter sound)
//   float volumeScale = 0.10;  // Scale amplitude to 25% of max volume

//   for (int i = 0; i < samples; i++) {
//     // Calculate fade-out effect based on the sample index
//     float amplitude = (i < fadeOutStartSample) ? 1.0f : (1.0f - ((float)(i - fadeOutStartSample) / (samples - fadeOutStartSample)));

//     // Apply volume scale to amplitude for reduced volume
//     amplitude *= volumeScale;  // Apply volume scaling factor

//     // Generate sine wave values with amplitude
//     buffer[i % BUFFER_SIZE] = (int16_t)(32767 * amplitude * sin(phase));  // 16-bit sine wave

//     // Increment phase for the next sample
//     phase += phaseIncrement;

//     // Wrap phase if it exceeds 2*pi
//     if (phase >= 2 * M_PI) {
//       phase -= 2 * M_PI;
//     }

//     // Once the buffer is full, send it out to I2S
//     if (i % BUFFER_SIZE == 0) {
//       read_data_stream((uint8_t *)buffer, BUFFER_SIZE * sizeof(uint16_t));  // Write data stream to I2S
//     }
//   }
// }

void whistleAlert() {
  for (int i = 0; i < 3; i++) {
    beep(BEEP_FREQUENCY, BEEP_DURATION_MS);
    delay(150);
  }
}


// void beep(int frequency, int duration_ms) {
//   int samples = (SAMPLERATE * duration_ms) / 1000;
//   int16_t buffer[BUFFER_SIZE];
//   float phase = 0.0f;
//   float phaseIncrement = 2.0f * M_PI * frequency / SAMPLERATE;

//   int fadeOutStartSample = samples / 2;  // Start fade-out halfway
//   float volumeScale = 0.10f;             // Volume (10%)

//   for (int i = 0; i < samples; i++) {
//     float amplitude = (i < fadeOutStartSample)
//                         ? 1.0f
//                         : 1.0f - ((float)(i - fadeOutStartSample) / (samples - fadeOutStartSample));
//     amplitude *= volumeScale;

//     float sample = amplitude * sin(phase);
//     if (sample > 1.0f) sample = 1.0f;
//     else if (sample < -1.0f) sample = -1.0f;

//     buffer[i % BUFFER_SIZE] = (int16_t)(32767 * sample);

//     phase += phaseIncrement;
//     if (phase >= 2.0f * M_PI) phase -= 2.0f * M_PI;

//     // Send buffer when full
//     if ((i + 1) % BUFFER_SIZE == 0) {
//       read_data_stream((uint8_t *)buffer, BUFFER_SIZE * sizeof(int16_t));
//     }
//   }

//   // Send any remaining samples
//   int remaining = samples % BUFFER_SIZE;
//   if (remaining > 0) {
//     read_data_stream((uint8_t *)buffer, remaining * sizeof(int16_t));
//   }
// }

//-------------------------------------------------------------------------------------------------------------------//
#define BUFFER_SIZE 256

void beep(int frequency, int duration_ms) {
  int samples = (SAMPLERATE * duration_ms) / 1000;
  int16_t buffer[BUFFER_SIZE];
  float phase = 0.0f;
  float phaseIncrement = 2.0f * M_PI * frequency / SAMPLERATE;

  int fadeOutStartSample = samples / 2;  // Fade out starts halfway
  float volumeScale = 0.10f;             // 10% of max volume

  for (int i = 0; i < samples; i++) {
    float amplitude = (i < fadeOutStartSample)
                        ? 1.0f
                        : 1.0f - ((float)(i - fadeOutStartSample) / (samples - fadeOutStartSample));
    amplitude *= volumeScale;

    float sample = amplitude * sin(phase);
    if (sample > 1.0f) sample = 1.0f;
    else if (sample < -1.0f) sample = -1.0f;

    buffer[i % BUFFER_SIZE] = (int16_t)(32767 * sample);

    phase += phaseIncrement;
    if (phase >= 2.0f * M_PI) phase -= 2.0f * M_PI;

    // Send full buffer
    if ((i + 1) % BUFFER_SIZE == 0) {
      read_data_stream((uint8_t *)buffer, BUFFER_SIZE * sizeof(int16_t));
    }
  }

  // Send any remaining samples
  int remaining = samples % BUFFER_SIZE;
  if (remaining > 0) {
    read_data_stream((uint8_t *)buffer, remaining * sizeof(int16_t));
  }
}

int Get_Filtered_ADC_Int() 
{
  static int filteredADC = 0;
  int rawADC = analogRead(ENV_PIN);
  filteredADC = (filteredADC * 0.9) + (rawADC * 0.1);   // 0.9 / 0.1 integer LPF
  return filteredADC;
}



//-----------------------------------------------------------------------//
//-----------------------------------------------------------------------//
//-----------------------------------------------------------------------//


// //------------------------------------BBK- MIC MAX448--------------------------------------------//
// #include <arduinoFFT.h>
// //---------------------------------------Parameters-----------------------------------//
// #define SAMPLES 128                // Must be a power of 2
// #define SAMPLING_FREQ 2000         // Sampling frequency in Hz
// #define ADC_PIN 12                 // Microphone ADC pin
// #define WHISTLE_MIN_FREQ 370       // Minimum frequency for whistle detection (Hz)
// #define WHISTLE_MAX_FREQ 430       // Maximum frequency for whistle detection (Hz)
// #define WHISTLE_MIN_DURATION 1500  // Minimum whistle duration (ms)
// #define WHISTLE_MAX_DURATION 3000  // Maximum whistle duration (ms)
// #define COOLDOWN_PERIOD 12000      // Cooldown period between whistles (ms)
// #define CALIBRATION_SAMPLES 100    // Number of samples for background noise calibration

// extern int set_cooker_wistle_cnt, whistle_ka_count;  // Desired number of whistles

// //-----------------------------------Global Variables-----------------------------------//
// ArduinoFFT<double> FFT = ArduinoFFT<double>();

// unsigned int sampling_period_us;
// double vReal[SAMPLES];
// double vImag[SAMPLES];

// int whistleCount = 0;
// bool whistleDetected = false;
// unsigned long whistleStartTime = 0;
// unsigned long lastWhistleTime = 0;
// float detectedFrequency = 0.0;
// float detectedMagnitude = 0.0;
// float dynamicThreshold = 0.0;
// bool whistleCompleteFlag = false;
// const float frequencyResolution = SAMPLING_FREQ / (float)SAMPLES;
// const int minFreqIndex = settings_buff[0] / frequencyResolution;
// const int maxFreqIndex = settings_buff[1] / frequencyResolution;
// const float MIN_MAGNITUDE_THRESHOLD = 50.0;

// //-----------------------------------Function Declarations-----------------------------------//
// void Whistle_Setup();
// void CalibrateBackgroundNoise();
// void Process_Whistle_Count();
// void checkWhistle(float frequency, float magnitude);
// void beep(int frequency, int duration_ms);
// //-----------------------------------Function Definitions-----------------------------------//
// void Whistle_Setup() {
//   sampling_period_us = round(1000000.0 / SAMPLING_FREQ);
//   Serial.println("Pressure Cooker Whistle Detection Initialized");

//   Serial.print("Sampling Frequency: ");
//   Serial.print(SAMPLING_FREQ);
//   Serial.println(" Hz");

//   Serial.print("FFT Samples: ");
//   Serial.println(SAMPLES);

//   Serial.print("Frequency Resolution: ");
//   Serial.print(frequencyResolution, 2);
//   Serial.println(" Hz");

//   Serial.print("Whistle Detection Frequency Range: ");
//   Serial.print(settings_buff[0]);
//   Serial.print(" Hz - ");
//   Serial.print(settings_buff[1]);
//   Serial.println(" Hz");
//   Serial.print("Whistle Count Target: ");
//   Serial.println(set_cooker_wistle_cnt);
//   Serial.println("Calibrating background noise...");
//   CalibrateBackgroundNoise();
//   // dynamicThreshold=1500;
//   Serial.print("Dynamic Threshold Set: ");
//   Serial.println(dynamicThreshold);
//   Serial.println("Setup Complete. Ready for detection.");
//   whistleCount = 0;
//   whistleCount = 0;
// }
// //-------------------------------------------------------------------------------------------------------------------//

// void CalibrateBackgroundNoise() {
//   float noiseSum = 0;
//   for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
//     noiseSum += analogRead(ADC_PIN);
//     delay(10);
//   }
//   dynamicThreshold = (noiseSum / CALIBRATION_SAMPLES) * 3.0;
// }

// //-------------------------------------------------------------------------------------------------------------------//

// void Process_Whistle_Count() {
//   if (whistleCompleteFlag) {
//     Serial.println("Desired number of whistles detected. Resetting count...");
//     //    whistleCount = 0;
//     for (int i = 0; i < 10; i++) {
//       beep(BEEP_FREQUENCY, BEEP_DURATION_MS);
//     }
//     whistleCompleteFlag = false;
//     return;
//   }

//   for (int i = 0; i < SAMPLES; i++) {
//     vReal[i] = analogRead(ADC_PIN);
//     vImag[i] = 0;
//     delayMicroseconds(sampling_period_us);  //vTaskDelay(100 / portTICK_PERIOD_MS);
//   }

//   FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  //Freq=pitch, Magnitude=amplitute
//   FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD);
//   FFT.complexToMagnitude(vReal, vImag, SAMPLES);

//   detectedFrequency = 0.0;
//   detectedMagnitude = 0.0;

//   for (int i = minFreqIndex; i <= maxFreqIndex; i++) {
//     float currentFrequency = i * frequencyResolution;
//     float currentMagnitude = vReal[i];

//     if (currentMagnitude > detectedMagnitude) {
//       detectedFrequency = currentFrequency;
//       detectedMagnitude = currentMagnitude;
//     }
//   }
//   checkWhistle(detectedFrequency, detectedMagnitude);
//   //display on screen
//   tft.fillRect(180, 130, 20 * 1, 25, ILI9341_BLACK);
//   if (whistle_ka_count <= set_cooker_wistle_cnt)
//     display_value(180, 150, whistleCount, ILI9341_CUSTOM_BEIGE_DARKER, ILI9341_BLACK, 1);
//   else
//     display_value(180, 150, whistleCount, ILI9341_RED, ILI9341_BLACK, 1);
// }
// //-------------------------------------------------------------------------------------------------------------------//

// void checkWhistle(float frequency, float magnitude) {
//   //begin time cnt
//   unsigned long currentTime = millis();
//   Serial.print("Frequency: ");
//   Serial.print(frequency);
//   Serial.print(" Hz, Magnitude: ");
//   Serial.println(magnitude);

//   // Ignore low noise
//   //  const float MIN_MAGNITUDE_THRESHOLD = 50.0;
//   if (magnitude < MIN_MAGNITUDE_THRESHOLD) {
//     Serial.println("Ignored: Low magnitude signal.");
//     return;
//   }

//   if (frequency >= settings_buff[0] && frequency <= settings_buff[1] && magnitude >= 1000) {  //dynamicThreshold) {
//     if (!whistleDetected) {
//       whistleStartTime = currentTime;
//       whistleDetected = true;
//       Serial.println("Whistle started!");
//     } else {
//       unsigned long whistleDuration = currentTime - whistleStartTime;
//       if (whistleDuration >= (settings_buff[2] * 100) && whistleDuration <= (settings_buff[3] * 100)) {
//         if (currentTime - lastWhistleTime >= (settings_buff[4] * 1000)) {
//           whistleCount++;
//           lastWhistleTime = currentTime;
//           whistle_ka_count = whistleCount;
//           Serial.print("Whistle detected! Total Count: ");
//           Serial.println(whistleCount);

//           if (whistleCount >= set_cooker_wistle_cnt) {
//             Serial.println("Desired number of whistles detected!");
//             whistleCompleteFlag = true;
//           }
//         } else {
//           Serial.println("Cooldown active. Ignoring whistle.");
//         }
//         whistleDetected = false;
//       } else if (whistleDuration > (settings_buff[3] * 100)) {
//         Serial.println("Whistle exceeded maximum duration. Ignoring.");
//         whistleDetected = false;
//       }
//     }
//   } else if (whistleDetected) {
//     Serial.println("Whistle stopped before valid duration.");
//     whistleDetected = false;
//   }
// }



// //-------------------------------------------------------------------------------------------------------------------/ /
// void beep(int frequency, int duration_ms) {
//   int samples = (SAMPLERATE * duration_ms) / 1000;  // Total samples based on duration
//   int16_t buffer[BUFFER_SIZE];                      // 16-bit buffer for I2S

//   // Generate the sine wave for the given frequency
//   float phase = 0.0;                                         // Phase of the sine wave (ranging from 0 to 2*pi)
//   float phaseIncrement = 2 * M_PI * frequency / SAMPLERATE;  // Phase increment per sample

//   // Calculate when to start the fade-out (fade starts after half of the duration)
//   int fadeOutStartSample = samples - (SAMPLERATE * 0.5);  // Fade-out starts after half the duration

//   // Volume scaling factor (lower values = quieter sound)
//   float volumeScale = 0.10;  // Scale amplitude to 25% of max volume

//   for (int i = 0; i < samples; i++) {
//     // Calculate fade-out effect based on the sample index
//     float amplitude = (i < fadeOutStartSample) ? 1.0f : (1.0f - ((float)(i - fadeOutStartSample) / (samples - fadeOutStartSample)));

//     // Apply volume scale to amplitude for reduced volume
//     amplitude *= volumeScale;  // Apply volume scaling factor

//     // Generate sine wave values with amplitude
//     buffer[i % BUFFER_SIZE] = (int16_t)(32767 * amplitude * sin(phase));  // 16-bit sine wave

//     // Increment phase for the next sample
//     phase += phaseIncrement;

//     // Wrap phase if it exceeds 2*pi
//     if (phase >= 2 * M_PI) {
//       phase -= 2 * M_PI;
//     }

//     // Once the buffer is full, send it out to I2S
//     if (i % BUFFER_SIZE == 0) {
//       read_data_stream((uint8_t *)buffer, BUFFER_SIZE * sizeof(uint16_t));  // Write data stream to I2S
//     }
//   }
// }
// //-----------------------------------------------------------------------//
