////-------------------------------BBK----ESP.getEfuseMac();----------------//
//#include "BluetoothA2DPSink.h"
//#include "driver/i2s.h"
//#include "AudioTools.h"
//#include <math.h>
//#include "esp_bt.h"
////-----------------------------------------------------------------------//
//BluetoothA2DPSink a2dp_sink;          // Bluetooth A2DP Sink instance
//I2SStream i2s;
//
//#define I2S_BCK_PIN       14          // Bit Clock Pin
//#define I2S_WS_PIN        15          // Word Select Pin
//#define I2S_DOUT_PIN      22          // Data Output Pin
//#define SAMPLERATE        44100       // Sample rate (44.1 kHz)
//#define BEEP_FREQUENCY    1000        // Frequency of the beep (1 kHz)
//#define BEEP_DURATION_MS  500         // Duration of beep in milliseconds
//#define BUFFER_SIZE       256         // Buffer size for I2S
//#define I2S_PORT          I2S_NUM_0   // Use I2S_NUM_0 or I2S_NUM_1 depending on your setup
//
////-----------------------------------------------------------------------//
//String currentTrack = "";              // Current track title
//String currentArtist = "";             // Current artist name
//String currentAlbum = "";              // Current album name
//String display_devicename = "";        // Connected device name for display
//String display_currentTrack = "";      // Current track for display
//String display_currentArtist = "";     // Current artist for display
//String display_currentAlbum = "";      // Current album for display
//bool isConnected = false;              // Connection status
//bool i2s_initialized = false;          // Flag to track I2S initialization
//char btName[32];
////-----------------------------------------------------------------------//
//void displayStatus();
//void processCommand(String command);
//void a2dp_play();
//void a2dp_pause();
//void initI2S();
//void read_data_stream(const uint8_t *data, uint32_t length) ;
//void avrc_metadata_callback(uint8_t attribute_id, const uint8_t *value);
//void connection_state_callback(esp_a2d_connection_state_t state, void *param);
//void connection_state_callback(esp_a2d_connection_state_t state, void *param);
//void Ble_sound_init();
//void Ble_sound_deinit();
//void deinitI2S();
//void deinitBluetoothA2DP();
//void resetBluetoothStack();
//void cleanUp();
//void disconnectAndPrepareForPairing();
//void clearBondedDevices();
//void enterPairingMode();
//void beep(int frequency, int duration_ms);
////-----------------------------------------------------------------------//
////void displayStatus()
////{
////  Serial.println("========== Current Status ==========");
////  Serial.print("Connected to: ");
////  if (isConnected)
////  {
////    display_devicename = a2dp_sink.get_connected_source_name();
////    Serial.println(display_devicename);
////  }
////  else
////  {
////    Serial.println("Not Connected");
////  }
////  Serial.println("Current Metadata:");
////  Serial.print("Track Title: ");
////  display_currentTrack = currentTrack;
////  Serial.println(display_currentTrack);
////  Serial.print("Artist: ");
////  display_currentArtist = currentArtist;
////  Serial.println(display_currentArtist);
////  Serial.print("Album: ");
////  display_currentAlbum = currentAlbum;
////  Serial.println(display_currentAlbum);
////  Serial.println("====================================");
////}
//
//
//void displayStatus()
//{
//  Serial.println("========== Current Status ==========");
//  Serial.print("Connected to: ");
//  if (isConnected)
//  {
//    display_devicename = a2dp_sink.get_connected_source_name();
//
//    if (display_devicename.length() > 15)
//    {
//      display_devicename = display_devicename.substring(0, 15);
//    }
//
//    Serial.println(display_devicename);
//  }
//  else
//  {
//    Serial.println("Not Connected");
//  }
//
//  Serial.println("Current Metadata:");
//  Serial.print("Track Title: ");
//  display_currentTrack = currentTrack;
//  Serial.println(display_currentTrack);
//  Serial.print("Artist: ");
//  display_currentArtist = currentArtist;
//  Serial.println(display_currentArtist);
//  Serial.print("Album: ");
//  display_currentAlbum = currentAlbum;
//  Serial.println(display_currentAlbum);
//  Serial.println("====================================");
//}
//
//
////-----------------------------------------------------------------------//
////void processCommand(String command)
////{
////  command.trim();
////  command.toLowerCase();
////
////  if (command == "play")
////  {
////    a2dp_sink.play();
////    Serial.println("Command Sent: Play");
////  }
////  else if (command == "pause")
////  {
////    a2dp_sink.pause();
////    Serial.println("Command Sent: Pause");
////  }
////  else if (command == "stop")
////  {
////    a2dp_sink.stop();
////    Serial.println("Command Sent: Stop");
////  }
////  else if (command == "next")
////  {
////    a2dp_sink.next();
////    Serial.println("Command Sent: Next");
////  }
////  else if (command == "previous")
////  {
////    a2dp_sink.previous();
////    Serial.println("Command Sent: Previous");
////  }
////  else if (command == "fast_forward")
////  {
////    a2dp_sink.fast_forward();
////    Serial.println("Command Sent: Fast Forward");
////  }
////  else if (command == "rewind")
////  {
////    a2dp_sink.rewind();
////    Serial.println("Command Sent: Rewind");
////  }
////  else
////  {
////    Serial.println("Invalid Command!");
////  }
////}
////-----------------------------------------------------------------------//
//void a2dp_play()
//{
//  a2dp_sink.play();
//}
////-----------------------------------------------------------------------//
//void a2dp_pause()
//{
//  a2dp_sink.pause();
//}
////-----------------------------------------------------------------------//
//void a2dp_previous()
//{
//  a2dp_sink.previous();
//}
////-----------------------------------------------------------------------//
//void a2dp_forward()
//{
//  a2dp_sink.fast_forward();
//}
////-----------------------------------------------------------------------//
//void a2dp_rewind()
//{
//  a2dp_sink.rewind();
//}
////-----------------------------------------------------------------------//
//void a2dp_stop()
//{
//  a2dp_sink.stop();
//}
////-----------------------------------------------------------------------//
//void initI2S()
//{
//  i2s_config_t i2s_config =
//  {
//    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
//    .sample_rate = 44100,
//    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
//    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
//    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
//    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
//    .dma_buf_count = 16,
//    .dma_buf_len = 512,
//    .use_apll = true,
//    .tx_desc_auto_clear = true,
//    .fixed_mclk = 0
//  };
//  i2s_pin_config_t pin_config =
//  {
//    .bck_io_num = I2S_BCK_PIN,
//    .ws_io_num = I2S_WS_PIN,
//    .data_out_num = I2S_DOUT_PIN,
//    .data_in_num = I2S_PIN_NO_CHANGE
//  };
//  esp_err_t result = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
//  if (result == ESP_OK)
//  {
//    i2s_set_pin(I2S_NUM_0, &pin_config);
//    i2s_initialized = true;
//    Serial.println("I2S Initialized.");
//  }
//  else
//  {
//    Serial.printf("Failed to initialize I2S. Error: %d\n", result);
//    i2s_initialized = false;
//  }
//}
//
//
//
////-----------------------------------------------------------------------//
////void read_data_stream(const uint8_t *data, uint32_t length)
////{
////  size_t bytes_written;
////  if (data == nullptr || length == 0)
////  {
////    Serial.println("Invalid data stream.");
////    return;
////  }
////
////  esp_err_t result = i2s_write(I2S_NUM_0, data, length, &bytes_written, portMAX_DELAY);
////  if (result != ESP_OK)
////  {
////    Serial.printf("Error writing to I2S: %d\n", result);
////  }
////}
//
////-----------------------------------------------------------------------//
////void read_data_stream(const uint8_t *data, uint32_t length)
////{
////  size_t bytes_written = 0;
////  if (data == nullptr || length == 0)
////  {
////    Serial.println("Invalid data stream: Data is nullptr or length is zero.");
////    return;
////  }
////  esp_err_t result = i2s_write(I2S_NUM_0, data, length, &bytes_written, portMAX_DELAY);
////
////  if (result != ESP_OK)
////  {
////    Serial.printf("Error writing to I2S. Error code: %d (%s)\n", result, esp_err_to_name(result));
////    return;
////  }
////  if (bytes_written < length)
////  {
////    Serial.printf("Partial write: Expected %u bytes, but only %u bytes were written.\n", length, bytes_written);
////  }
////  else
////  {
////    Serial.printf("Data written successfully: %u bytes.\n", bytes_written);
////  }
////}
//
//
//void read_data_stream(const uint8_t *data, uint32_t length)
//{
//  if (data == nullptr || length == 0) {
//    Serial.println("Invalid data stream: Data is nullptr or length is zero.");
//    return;
//  }
//
//  // Copy incoming 16-bit stereo samples
//  int16_t *samples = (int16_t *)data;
//  size_t sampleCount = length / 2;  // since 16-bit = 2 bytes
//
//  // Create buffer for scaled samples
//  static int16_t scaledBuffer[1024];  // enough for 512 stereo frames
//  if (sampleCount > (sizeof(scaledBuffer) / sizeof(scaledBuffer[0]))) {
//    Serial.println("Sample buffer too small!");
//    return;
//  }
//
//  // Apply attenuation (80% volume scaling)
//  for (size_t i = 0; i < sampleCount; i++) {
//    int32_t val = samples[i];
//    val = (val * 80) / 100;   // scale to 80% (-1.9 dB)
//    if (val > 32767) val = 32767;
//    if (val < -32768) val = -32768;
//    scaledBuffer[i] = (int16_t)val;
//  }
//
//  // Write scaled samples to I2S
//  size_t bytes_written = 0;
//  esp_err_t result = i2s_write(I2S_NUM_0, scaledBuffer, sampleCount * 2, &bytes_written, portMAX_DELAY);
//
//  if (result != ESP_OK) {
//    Serial.printf("Error writing to I2S. Error code: %d (%s)\n", result, esp_err_to_name(result));
//  } else if (bytes_written < sampleCount * 2) {
//    Serial.printf("Partial write: Expected %u bytes, but only %u bytes written.\n", sampleCount * 2, bytes_written);
//  }
//}
//
//
//
//
////-----------------------------------------------------------------------//
//void avrc_metadata_callback(uint8_t attribute_id, const uint8_t *value)
//{
//  if (value == nullptr || strlen((const char*)value) == 0) return;
//
//  String metadata = String((const char *)value);
//  if (metadata.length() > 50)
//  {
//    metadata = metadata.substring(0, 50) + "...";
//  }
//
//  switch (attribute_id)
//  {
//    case ESP_AVRC_MD_ATTR_TITLE:
//      currentTrack = metadata;
//      break;
//    case ESP_AVRC_MD_ATTR_ARTIST:
//      currentArtist = metadata;
//      break;
//    case ESP_AVRC_MD_ATTR_ALBUM:
//      currentAlbum = metadata;
//      break;
//    default:
//      Serial.printf("Unknown Metadata Attribute ID: %d\n", attribute_id);
//      break;
//  }
//}
////-----------------------------------------------------------------------//
//void connection_state_callback(esp_a2d_connection_state_t state, void *param)
//{
//  if (state == ESP_A2D_CONNECTION_STATE_CONNECTED)
//  {
//    isConnected = true;
//    Serial.println("Bluetooth Connected. Initializing I2S...");
//    initI2S();
//  }
//  else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED)
//  {
//    isConnected = false;
//    Serial.println("Bluetooth Disconnected. Deinitializing I2S...");
//    deinitI2S();
//  }
//}
////-----------------------------------------------------------------------//
////void Ble_sound_init()
////{
////  Serial.println("Initializing Bluetooth A2DP Sink...");
////  delay(200); // Allow resources to stabilize before initializing again
////
////  a2dp_sink.set_stream_reader(read_data_stream, false);
////  a2dp_sink.set_auto_reconnect(true);
////  a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
////  a2dp_sink.set_on_connection_state_changed(connection_state_callback);
////  a2dp_sink.set_event_stack_size(4096);
////  //    a2dp_sink.set_task_priority(5);     // Ensure Bluetooth task gets enough priority
////  a2dp_sink.set_volume(50);
////
////  a2dp_sink.start("Elara");
////  Serial.println("Bluetooth A2DP Sink Started.");
////}
////-----------------------------------------------------------------------//
////void Ble_sound_deinit()
////{
////  Serial.println("Stopping Bluetooth A2DP Sink...");
////  a2dp_sink.disconnect(); // Disconnect from Bluetooth source
////  delay(100);             // Allow time for disconnection to complete
////
////  a2dp_sink.end();        // Stop the A2DP sink to free resources
////  Serial.println("Bluetooth A2DP Sink Stopped.");
////
////  delay(200);             // Ensure all pending operations are completed
////  deinitI2S();            // Deinitialize I2S safely
////  Serial.println("I2S Deinitialized.");
////}
////-----------------------------------------------------------------------//
//void deinitI2S()
//{
//  if (i2s_initialized)
//  {
//    esp_err_t result = i2s_driver_uninstall(I2S_NUM_0);
//    if (result == ESP_OK)
//    {
//      Serial.println("--I2S Deinitialized---");
//      i2s_initialized = false;
//    }
//    else
//    {
//      Serial.printf("Failed to deinitialize I2S. Error: %d\n", result);
//    }
//  }
//  else
//  {
//    Serial.println("I2S was not initialized.");
//  }
//}
////-----------------------------------------------------------------------//
//void deinitBluetoothA2DP()
//{
//  Serial.println("----Stopping Bluetooth A2DP Sink-----");
//  a2dp_sink.end();
//  Serial.println("---Bluetooth A2DP Sink Stopped-------");
//}
////-----------------------------------------------------------------------//
//void resetBluetoothStack()
//{
//  Serial.println("-------------Bluetooth stack----------------");
//  if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_ENABLED)
//  {
//    esp_bluedroid_disable();
//    esp_bluedroid_deinit();
//  }
//  if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_ENABLED)
//  {
//    esp_bt_controller_disable();
//    esp_bt_controller_deinit();
//  }
//  Serial.println("Bluetooth stack reset");
//}
////-----------------------------------------------------------------------//
//void cleanUp()
//{
//  Serial.println("--------Performing clean-up--------");
//  if (isConnected)
//  {
//    Ble_sound_deinit();
//  }
//  resetBluetoothStack();
//  isConnected = false;
//  i2s_initialized = false;
//  Serial.println("--------Clean-up completed--------");
//}
////-----------------------------------------------------------------------//
////void disconnectAndPrepareForPairing()
////{
////  isConnected = false;
////  Serial.println("BLuetooth Disconnecting");
////  a2dp_sink.disconnect();
////  clearBondedDevices();
////  enterPairingMode();
////  cleanUp();
////  Serial.println("BLuetooth Disconnecting Done");
////}
//
//void disconnectAndPrepareForPairing()
//{
//  Serial.println("Disconnecting from current device...");
//
//  // If connected, drop the link
//  if (a2dp_sink.is_connected())
//  {
//    a2dp_sink.disconnect();
//    delay(200);
//  }
//
//  // Stop sink
//  a2dp_sink.end();
//  delay(200);
//
//  // Restart sink with the same BT name (or a new one if you want)
//  a2dp_sink.start(btName);
//  Serial.print("Ready for new pairing as: ");
//  Serial.println(btName);
//
//  isConnected = false;
//  i2s_initialized = false;
//}
//
//
////-----------------------------------------------------------------------//
//void clearBondedDevices()
//{
//  int dev_num = esp_bt_gap_get_bond_device_num();
//  if (dev_num > 0)
//  {
//    Serial.printf("Clearing %d bonded devices...\n", dev_num);
//    esp_err_t status;
//    esp_bd_addr_t* devices = new esp_bd_addr_t[dev_num];
//    esp_bt_gap_get_bond_device_list(&dev_num, devices);
//    for (int i = 0; i < dev_num; i++)
//    {
//      status = esp_bt_gap_remove_bond_device(devices[i]);
//      if (status == ESP_OK)
//      {
//        Serial.printf("Cleared bonded device %d\n", i + 1);
//      }
//      else
//      {
//        Serial.printf("Failed to clear bonded device %d\n", i + 1);
//      }
//    }
//    delete[] devices;
//  }
//  else
//  {
//    Serial.println("No bonded devices to clear.");
//  }
//}
////-----------------------------------------------------------------------//
//void enterPairingMode()
//{
//  Serial.println("Entering pairing mode...");
//  a2dp_sink.stop();
//  delay(1000);
//  a2dp_sink.start("Beyond-Chimney");
//  Serial.println("Device is now discoverable for new pairing.");
//}
////-----------------------------------------------------------------------//
////void Ble_sound_init()// tested ok
////{
////  //  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);
////  a2dp_sink.set_stream_reader(read_data_stream, false);
////  a2dp_sink.set_auto_reconnect(true);
////  a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
////  a2dp_sink.set_on_connection_state_changed(connection_state_callback);
////  a2dp_sink.start("Beyond-Chimney");
////  auto cfg = i2s.defaultConfig();
////  cfg.pin_data = 22;       // DIN
////  cfg.pin_ws = 15;         // LRCLK
////  cfg.pin_bck = 14;        // BCLK
////  cfg.sample_rate = a2dp_sink.sample_rate();
////  cfg.channels = 2;
////  cfg.bits_per_sample = 32;
////  cfg.buffer_count = 16;
////  cfg.buffer_size = 256;
////  i2s.begin(cfg);
////}
//
//void Ble_sound_init()
//{
//  // Generate short unique name with last 3 bytes of MAC
//  uint64_t chipid = ESP.getEfuseMac();
//  uint32_t last3 = (uint32_t)(chipid & 0xFFFFFF);   // last 3 bytes only
//
//  btName[32];   // buffer for final name
//  sprintf(btName, "BT-%06X", last3);
//
//  // Setup A2DP sink
//  a2dp_sink.set_stream_reader(read_data_stream, false);
//  a2dp_sink.set_auto_reconnect(true);
//  a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
//  a2dp_sink.set_on_connection_state_changed(connection_state_callback);
//  a2dp_sink.start(btName);   // <-- use char array directly
//
//  // Configure I2S
//  auto cfg = i2s.defaultConfig();
//  cfg.pin_data = 22;       // DIN
//  cfg.pin_ws = 15;         // LRCLK
//  cfg.pin_bck = 14;        // BCLK
//  cfg.sample_rate = a2dp_sink.sample_rate();
//  cfg.channels = 2;
//  cfg.bits_per_sample = 32;
//  cfg.buffer_count = 16;
//  cfg.buffer_size = 512;
//  i2s.begin(cfg);
//
//  Serial.print("Started A2DP Sink with name: ");
//  Serial.println(btName);
//}
//
////-----------------------------------------------------------------------//
//
////-----------------------------------------------------------------------//
//void Ble_sound_deinit()
//{
//  a2dp_sink.set_stream_reader(nullptr, false);
//  if (a2dp_sink.is_connected()) //&&isConnected==true
//  {
//    a2dp_sink.stop();
//    a2dp_sink.disconnect();
//    a2dp_sink.end();
//    //    disableBluetooth();
//    Serial.println("stop&disconnect");
//  }
//  delay(500);
//  i2s.end();
//  delay(100);
//  //  isConnected = false;
//  Serial.println("Ble_sound_deinit completed.");
//}
//
////-----------------------------------------------------------------------//
//void disableBluetooth()
//{
//  esp_bluedroid_disable();
//  esp_bluedroid_deinit();
//  esp_bt_controller_disable();
//  esp_bt_controller_deinit();
//
//
//}
////-----------------------------------------------------------------------//
//
//void volumeUp_BLE()
//{
//  uint8_t vol = a2dp_sink.get_volume();
//  if (vol < 127)
//  {
//    vol += 10;
//    if (vol > 127) vol = 127;
//    a2dp_sink.set_volume(vol);
//    Serial.printf("Volume set to: %d\n", vol);
//  }
//}
////-----------------------------------------------------------------------//
//
//void volumeDown_BLE()
//{
//  uint8_t vol = a2dp_sink.get_volume();
//  if (vol > 0)
//  {
//    vol -= 10;
//    if (vol > 127) vol = 0;
//    a2dp_sink.set_volume(vol);
//    Serial.printf("Volume set to: %d\n", vol);
//  }
//}
////-----------------------------------------------------------------------//




//-------------------------------BBK----------------------------------------//
#include "BluetoothA2DPSink.h"
#include "AudioTools.h"
#include <math.h>
#include "esp_bt.h"
//-----------------------------------------------------------------------//
BluetoothA2DPSink a2dp_sink;  // Bluetooth A2DP Sink instance
I2SStream i2s;

#define I2S_BCK_PIN 14        // Bit Clock Pin
#define I2S_WS_PIN 15         // Word Select Pin (LRCLK)
#define I2S_DOUT_PIN 22       // Data Output Pin (DIN)
#define SAMPLERATE 44100      // Sample rate (44.1 kHz)
#define BEEP_FREQUENCY 1000   // Frequency of the beep (1 kHz)
#define BEEP_DURATION_MS 500  // Duration of beep in milliseconds

// Attenuation and chunk size
#define OUTPUT_VOLUME_PERCENT 80  // 80% software attenuation
#define CHUNK_SAMPLES 1024        // number of int16 samples per chunk (2 KB)

//----------------------------------------------------------------------//
String currentTrack = "";           // Current track title
String currentArtist = "";          // Current artist name
String currentAlbum = "";           // Current album name
String display_devicename = "";     // Connected device name for display
String display_currentTrack = "";   // Current track for display
String display_currentArtist = "";  // Current artist for display
String display_currentAlbum = "";   // Current album for display
bool isConnected = false;           // Connection status
char btName[32];                    // Dynamic BT name

//-----------------------------------------------------------------------//
void displayStatus();
void processCommand(String command);
void a2dp_play();
void a2dp_pause();
void a2dp_previous();
void a2dp_forward();
void a2dp_rewind();
void a2dp_stop();
void read_data_stream(const uint8_t *data, uint32_t length);
void avrc_metadata_callback(uint8_t attribute_id, const uint8_t *value);
void connection_state_callback(esp_a2d_connection_state_t state, void *param);
void Ble_sound_init();
void Ble_sound_deinit();
void deinitBluetoothA2DP();
void resetBluetoothStack();
void cleanUp();
void disconnectAndPrepareForPairing();
void clearBondedDevices();
void enterPairingMode();
void beep(int frequency, int duration_ms);
void disableBluetooth();
void volumeUp_BLE();
void volumeDown_BLE();
void initI2S();
void deinitI2S();
//-----------------------------------------------------------------------//
void displayStatus() {
  //  Serial.println("========== Current Status ==========");
  //  Serial.print("Connected to: ");
  if (isConnected) {
    display_devicename = a2dp_sink.get_connected_source_name();
    if (display_devicename.length() > 15) {
      display_devicename = display_devicename.substring(0, 15);
    }
    //    Serial.println(display_devicename);
  } else {
    display_devicename = "";
    display_currentTrack = "";
    display_currentArtist = "";
    display_currentAlbum = "";
    currentTrack = "";
    currentArtist = "";
    currentAlbum = "";
    display_devicename.clear();  // does the same thing
    //    Serial.println("Not Connected");
  }


  //  Serial.println("Current Metadata:");
  //  Serial.print("Track Title: ");
  display_currentTrack = currentTrack;
  //  Serial.println(display_currentTrack);
  //  Serial.print("Artist: ");
  display_currentArtist = currentArtist;
  //  Serial.println(display_currentArtist);
  //  Serial.print("Album: ");
  display_currentAlbum = currentAlbum;
  //  Serial.println(display_currentAlbum);
  //  Serial.println("====================================");
}
//-----------------------------------------------------------------------//
void a2dp_play() {
  a2dp_sink.play();
}
//-----------------------------------------------------------------------//

void a2dp_pause() {
  a2dp_sink.pause();
}
//-----------------------------------------------------------------------//

void a2dp_previous() {
  a2dp_sink.previous();
}
//-----------------------------------------------------------------------//

void a2dp_forward() {
  a2dp_sink.fast_forward();
}
//-----------------------------------------------------------------------//

void a2dp_rewind() {
  a2dp_sink.rewind();
}
//-----------------------------------------------------------------------//

void a2dp_stop() {
  a2dp_sink.stop();
}
//-----------------------------------------------------------------------//
void read_data_stream(const uint8_t *data, uint32_t length) {
  if (!data || length == 0) return;

  // Incoming Bluetooth PCM is 16-bit signed little-endian
  const int16_t *inSamples = (const int16_t *)data;
  size_t totalSamples = length / 2;

  static int16_t scaledBuffer[CHUNK_SAMPLES];
  size_t processed = 0;

  // Diagnostics: uncomment if you want to see how many bytes arrive
  // Serial.printf("read_data_stream: length=%u samples=%u\n", length, totalSamples);

  while (processed < totalSamples) {
    size_t block = totalSamples - processed;
    if (block > CHUNK_SAMPLES) block = CHUNK_SAMPLES;

    // Apply attenuation
    for (size_t i = 0; i < block; ++i) {
      int32_t val = inSamples[processed + i];
      val = (val * OUTPUT_VOLUME_PERCENT) / 100;
      if (val > 32767) val = 32767;
      if (val < -32768) val = -32768;
      scaledBuffer[i] = (int16_t)val;
    }

    // Write scaled block to I2S (AudioTools)
    size_t bytesToWrite = block * 2;  // int16 -> bytes
    size_t written = i2s.write((const uint8_t *)scaledBuffer, bytesToWrite);
    if (written < bytesToWrite) {
      // Partial write - print once to help debugging (don't spam)
      Serial.printf("i2s.write partial: wanted=%u wrote=%u\n", (unsigned)bytesToWrite, (unsigned)written);
    }

    processed += block;
  }
}
//-----------------------------------------------------------------------//
void avrc_metadata_callback(uint8_t attribute_id, const uint8_t *value) {
  if (value == nullptr || strlen((const char *)value) == 0) return;

  String metadata = String((const char *)value);
  if (metadata.length() > 50) {
    metadata = metadata.substring(0, 50) + "...";
  }

  switch (attribute_id) {
    case ESP_AVRC_MD_ATTR_TITLE:
      currentTrack = metadata;
      break;
    case ESP_AVRC_MD_ATTR_ARTIST:
      currentArtist = metadata;
      break;
    case ESP_AVRC_MD_ATTR_ALBUM:
      currentAlbum = metadata;
      break;
    default:
      //      Serial.printf("Unknown Metadata Attribute ID: %d\n", attribute_id);
      break;
  }
}
//-----------------------------------------------------------------------//
//void connection_state_callback(esp_a2d_connection_state_t state, void *param)
//{
//  if (state == ESP_A2D_CONNECTION_STATE_CONNECTED)
//  {
//    isConnected = true;
//    Serial.println("Bluetooth Connected.");
//  }
//  else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED)
//  {
//    isConnected = false;
//    display_devicename = "";
//    Serial.println("Bluetooth Disconnected.");
//  }
//}

//void connection_state_callback(esp_a2d_connection_state_t state, void *param)
//{
//
//  if (state == ESP_A2D_CONNECTION_STATE_CONNECTED)
//  {
//    isConnected = true;
//    Serial.println("Bluetooth Connected.");
//    display_devicename   = "";
//    display_devicename.clear();
//    delay(200);
////    Immediately update with the connected device name
//    display_devicename = a2dp_sink.get_connected_source_name();
//    if (display_devicename.length() > 15) {
//      display_devicename = display_devicename.substring(0, 15);
//      delay(200);
//    }
//  }
//  else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED)
//  {
//    isConnected = false;
//    Serial.println("Bluetooth Disconnected.");
//  }
//}
//-----------------------------------------------------------------------//

void connection_state_callback(esp_a2d_connection_state_t state, void *param) {
  if (state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
    isConnected = true;
    //    Serial.println("Bluetooth Connected.");

    // Get candidate name
    String newName = a2dp_sink.get_connected_source_name();

    // Ignore if it's the same as the last one (stale)
    if (newName == display_devicename || newName.length() == 0) {
      //      Serial.println("Stale or empty device name, waiting for refresh...");
      return;  // don’t overwrite yet
    }

    // Update only when it's fresh
    display_devicename = newName;
    if (display_devicename.length() > 15) {
      display_devicename = display_devicename.substring(0, 15);
    }
    //    Serial.println("Connected Device: " + display_devicename);
    displayStatus();
  } else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
    isConnected = false;
    //    Serial.println("Bluetooth Disconnected.");
    display_devicename.clear();
  }
}

//-----------------------------------------------------------------------//
void Ble_sound_init() {
  // Generate short unique name with last 3 bytes of MAC
  uint64_t chipid = ESP.getEfuseMac();
  uint32_t last3 = (uint32_t)(chipid & 0xFFFFFF);
  sprintf(btName, "BT-%06X", last3);

  // Setup A2DP sink
  a2dp_sink.set_stream_reader(read_data_stream, false);
  a2dp_sink.set_auto_reconnect(true);
  a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
  a2dp_sink.set_on_connection_state_changed(connection_state_callback);

  // Set safe default volume (0..127). Keep it below full-scale.
  a2dp_sink.set_volume(100);  // safe default (you can change)

  a2dp_sink.start(btName);

  // Configure AudioTools I2S
  auto cfg = i2s.defaultConfig();
  cfg.pin_data = I2S_DOUT_PIN;
  cfg.pin_ws = I2S_WS_PIN;
  cfg.pin_bck = I2S_BCK_PIN;
  cfg.sample_rate = a2dp_sink.sample_rate();
  cfg.channels = 2;
  cfg.bits_per_sample = 16;  // match incoming PCM
  cfg.buffer_count = 16;
  cfg.buffer_size = 384;  //512;      //buffer chnage to reduce noise
  i2s.begin(cfg);

  //  Serial.print("Started A2DP Sink with name: ");
  //  Serial.println(btName);
}
//-----------------------------------------------------------------------//
void Ble_sound_deinit() {
  a2dp_sink.set_stream_reader(nullptr, false);
  if (a2dp_sink.is_connected()) {
    a2dp_sink.stop();
    a2dp_sink.disconnect();
    a2dp_sink.end();
    //    Serial.println("Bluetooth A2DP Sink stopped & disconnected.");
  }
  delay(200);
  i2s.end();
  //  Serial.println("I2S stopped.");
}
//-----------------------------------------------------------------------//
void deinitBluetoothA2DP() {
  //  Serial.println("Stopping Bluetooth A2DP Sink...");
  a2dp_sink.end();
  //  Serial.println("Bluetooth A2DP Sink Stopped.");
}
//-----------------------------------------------------------------------//
void resetBluetoothStack() {
  //  Serial.println("Resetting Bluetooth stack...");
  if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_ENABLED) {
    esp_bluedroid_disable();
    esp_bluedroid_deinit();
  }
  if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_ENABLED) {
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
  }
  //  Serial.println("Bluetooth stack reset complete.");
}
//-----------------------------------------------------------------------//
void cleanUp() {
  //  Serial.println("Performing clean-up...");
  if (isConnected) {
    Ble_sound_deinit();
  }
  resetBluetoothStack();
  isConnected = false;
  //  Serial.println("Clean-up completed.");
}
//-----------------------------------------------------------------------//
void disconnectAndPrepareForPairing() {
  //  Serial.println("Disconnecting from current device...");
  if (a2dp_sink.is_connected()) {
    a2dp_sink.disconnect();
    delay(200);
  }

  a2dp_sink.end();
  delay(200);

  a2dp_sink.start(btName);
  //  Serial.print("Ready for new pairing as: ");
  //  Serial.println(btName);
  isConnected = false;
  display_devicename.clear();
  displayStatus();
}
//-----------------------------------------------------------------------//
void clearBondedDevices() {
  int dev_num = esp_bt_gap_get_bond_device_num();
  if (dev_num > 0) {
    //    Serial.printf("Clearing %d bonded devices...\n", dev_num);
    esp_err_t status;
    esp_bd_addr_t *devices = new esp_bd_addr_t[dev_num];
    esp_bt_gap_get_bond_device_list(&dev_num, devices);
    for (int i = 0; i < dev_num; i++) {
      status = esp_bt_gap_remove_bond_device(devices[i]);
      if (status == ESP_OK) {
        //        Serial.printf("Cleared bonded device %d\n", i + 1);
      } else {
        //        Serial.printf("Failed to clear bonded device %d\n", i + 1);
      }
    }
    delete[] devices;
  } else {
    //    Serial.println("No bonded devices to clear.");
  }
}
//-----------------------------------------------------------------------//
void enterPairingMode() {
  //  Serial.println("Entering pairing mode...");
  a2dp_sink.stop();
  delay(1000);
  a2dp_sink.start("Beyond-Chimney");
  //  Serial.println("Device is now discoverable for new pairing.");
}
//-----------------------------------------------------------------------//
void disableBluetooth() {
  esp_bluedroid_disable();
  esp_bluedroid_deinit();
  esp_bt_controller_disable();
  esp_bt_controller_deinit();
}
//-----------------------------------------------------------------------//
void volumeUp_BLE() {
  uint8_t vol = a2dp_sink.get_volume();
  if (vol < 110)  // cap to safe max
  {
    vol += 10;
    if (vol > 110) vol = 110;
    a2dp_sink.set_volume(vol);
    //    Serial.printf("Volume set to (capped): %d\n", vol);
  } else {
    //    Serial.println("Max safe volume reached (110).");
  }
}
//-----------------------------------------------------------------------//
void volumeDown_BLE() {
  uint8_t vol = a2dp_sink.get_volume();
  if (vol > 0) {
    if (vol < 10) vol = 0;
    else vol -= 10;
    a2dp_sink.set_volume(vol);
    //    Serial.printf("Volume set to: %d\n", vol);
  }
}
//-----------------------------------------------------------------------//

void initI2S() {
  //  Serial.println("initI2S() called - handled automatically in Ble_sound_init()");
}
//-----------------------------------------------------------------------//
void deinitI2S() {
  //  Serial.println("deinitI2S() called - closing I2S");
  i2s.end();
}
//-----------------------------------------------------------------------//
void initI2S_alert() {
  //  Serial.println("initI2S() called - handled automatically in Ble_sound_init()");

  // Configure AudioTools I2S
  auto cfg = i2s.defaultConfig();
  cfg.pin_data = I2S_DOUT_PIN;
  cfg.pin_ws = I2S_WS_PIN;
  cfg.pin_bck = I2S_BCK_PIN;
  cfg.sample_rate = a2dp_sink.sample_rate();
  cfg.channels = 2;
  cfg.bits_per_sample = 16;  // match incoming PCM
  cfg.buffer_count = 16;
  cfg.buffer_size = 384;  //512;
  i2s.begin(cfg);
}