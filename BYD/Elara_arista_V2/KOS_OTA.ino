//--------------------------------------------------BBK-------------------------------------------------------//
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <EEPROM.h>
#include "KOS_Certificates.h"

//--------------------------------------------------Defines------------------------------------------------------//
#define OTA_BASE_URL "https://mykitchenos.com/assets/ota/"
#define MAX_RETRIES 3         //for ota
#define OTA_FLAG_ADD 171      // Start address for OTA flag
#define OTA_VERSION_ADDR 173  // OTA version number starts after 2 bytes for the flag

//--------------------------------------------------Variables-----------------------------------------------------//
String chipId;
unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 15000;

String OTA_CHECK_TOPIC;
String OTA_PUB_TOPIC;

const char* AWS_IOT_ENDPOINT = "a1bmjgj4h06eyc-ats.iot.ap-south-1.amazonaws.com";
const char* THINGNAME = "D48C49E97AFC";

const char* WIFI_SSID = "MFPL-BALAKRISHN";  //"BBK";
const char* WIFI_PASSWORD = "123456789";    //"bbk12345";

const char* bin_file_url_kitchen_os = "https://mykitchenos.com/assets/ota/Beyond2.bin";

int ota_retries, update_progrss_bar;
float firmware_verison_saved;
extern float firmware_verison;
extern bool wifi_flag;
bool wifi_flag = 0;
WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

//-------------------------------------------------Function Declarations------------------------------------------------//
void OTASetup();
void OTALoop();
void ConnectToWiFi();
void GetDeviceId();
void Get_OTA_Filename();
void ConnectToAWS();
void MQTTCallback(char* topic, byte* payload, unsigned int length);
void OTAUpdate(const char* filename);
void OTAUpdateRoutine();
bool ValidateFirmwareSize(const char* url);

//---------------------------------------------------------------------------------------------------------------//
//void ConnectToWiFi()
//{
//  Serial.println("==========================================");
//  Serial.println("[INFO] Connecting to Wi-Fi");
//  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//  while (WiFi.status() != WL_CONNECTED)
//  {
//    Serial.print(".");
//    delay(1000);
//  }
//  Serial.println("[SUCCESS] Connected to Wi-Fi");
//  wifi_flag = 1;
//  Serial.println("==========================================");
//}

void ConnectToWiFi() {
  //  Serial.println("==========================================");
  //  Serial.println("[INFO] Attempting to connect to known Wi-Fi networks...");
  const char* ssidList[] = {
    "MFPL-BALAKRISHN",
    "BBK",
    "MFPL-IOT"
  };

  const char* passList[] = {
    "123456789",
    "bbk12345",
    "Robot@2024"
  };

  const int wifiCount = sizeof(ssidList) / sizeof(ssidList[0]);
  bool connected = false;

  for (int i = 0; i < wifiCount; i++) {
    //    Serial.print("[INFO] Trying SSID: ");
    //    Serial.println(ssidList[i]);

    WiFi.begin(ssidList[i], passList[i]);

    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 10) {
      delay(1000);
      //      Serial.print(".");
      attempt++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      //      Serial.println("");
      //      Serial.print("[SUCCESS] Connected to: ");
      //      Serial.println(ssidList[i]);
      //      Serial.print("IP Address: ");
      //      Serial.println(WiFi.localIP());
      wifi_flag = 1;
      connected = true;
      break;
    } else {
      //      Serial.println("");
      //      Serial.println("[WARNING] Failed to connect to: " + String(ssidList[i]));
    }
  }

  if (!connected) {
    //    Serial.println("[ERROR] Could not connect to any known Wi-Fi network.");
  }

  //  Serial.println("==========================================");
}


//---------------------------------------------------------------------------------------------------------------//
void GetDeviceId() {
  char array_mac[18];
  char mac1[18];
  String temps;
  int j = 0;
  temps = WiFi.macAddress();
  temps.toCharArray(array_mac, 18);
  for (int i = 0; i < 18; i++) {
    if (array_mac[i] == ':')
      continue;
    mac1[j++] = array_mac[i];
  }
  chipId = String(mac1);

  // Dynamically set the MQTT topics
  OTA_CHECK_TOPIC = "OTA/check/" + chipId;
  OTA_PUB_TOPIC = "OTA/update/" + chipId;

  //  Serial.println("==========================================");
  //  Serial.println("[INFO] Device MAC ID (LOCAL):");
  //  Serial.println(chipId);
  //  Serial.println("[INFO] OTA_CHECK_TOPIC: " + OTA_CHECK_TOPIC);
  //  Serial.println("[INFO] OTA_PUB_TOPIC: " + OTA_PUB_TOPIC);
  //  Serial.println("==========================================");
}


//void GetDeviceId()
//{
//  //(from eFuse)
//  uint64_t efuseMac = ESP.getEfuseMac();
//  char mac1[13];
//  sprintf(mac1, "%012llX", efuseMac);
//  chipId = String(mac1);
//  OTA_CHECK_TOPIC = "OTA/check/" + chipId;
//  OTA_PUB_TOPIC   = "OTA/update/" + chipId;
//
//  Serial.println("==========================================");
//  Serial.println("[INFO] Device MAC ID (eFuse):");
//  Serial.println(chipId);
//  Serial.println("[INFO] OTA_CHECK_TOPIC: " + OTA_CHECK_TOPIC);
//  Serial.println("[INFO] OTA_PUB_TOPIC: " + OTA_PUB_TOPIC);
//  Serial.println("==========================================");
//}

//---------------------------------------------------------------------------------------------------------------//
void Get_OTA_Filename() {
  DynamicJsonDocument doc(1024);
  doc["MACID"] = chipId;
  String shadow;
  serializeJson(doc, shadow);
  //  Serial.println("==========================================");
  //  Serial.println("[INFO] PUB MAC ID To Topic");
  mqttClient.publish(OTA_PUB_TOPIC.c_str(), shadow.c_str());
  //  Serial.println(shadow);
  //  Serial.println("[SUCCESS] PUB Successfully");
  //  Serial.println("==========================================");
}

//---------------------------------------------------------------------------------------------------------------//
void ConnectToAWS() {
  //  Serial.println("==========================================");
  //  Serial.println("========== MQTT Initialization ==========");
  //  Serial.println("[INFO] Setting AWS IoT certificates...");
  wifiClient.setCACert(AWS_CERT_CA);
  wifiClient.setCertificate(AWS_CERT_CRT);
  wifiClient.setPrivateKey(AWS_CERT_PRIVATE);
  //  Serial.print("[INFO] Setting MQTT endpoint: ");
  //  Serial.println(AWS_IOT_ENDPOINT);
  mqttClient.setServer(AWS_IOT_ENDPOINT, 8883);
  mqttClient.setCallback(MQTTCallback);
  //  Serial.println("==========================================");

  while (!mqttClient.connected()) {
    //    Serial.println("[INFO] Connecting to AWS");
    if (mqttClient.connect(THINGNAME)) {
      //      Serial.println("[SUCCESS] Connected to AWS IoT MQTT broker.");
      if (mqttClient.subscribe(OTA_CHECK_TOPIC.c_str())) {
        //        Serial.println("[SUCCESS] Subscribed to OTA_CHECK_TOPIC.");
      } else {
        machine_state = settings_screen_wifi_OTA_error;
        //        Serial.println("[ERROR] Subscription to OTA_CHECK_TOPIC failed!");
      }
      Serial.println("[INFO] AWS IoT setup complete.\n");
    } else {
      machine_state = settings_screen_wifi_OTA_error;
      //      Serial.printf("[ERROR] MQTT connection failed. Return code = %d. Retrying in 5 seconds...\n", mqttClient.state());
      delay(5000);
      break;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------//
void MQTTCallback(char* topic, byte* payload, unsigned int length) {
  //  Serial.println("==========================================");
  //  Serial.printf("[INFO] Message arrived on topic: %s\n", topic);

  String receivedPayload = "";
  for (unsigned int i = 0; i < length; i++) {
    receivedPayload += (char)payload[i];
  }
  //  Serial.printf("[INFO] Payload: %s\n", receivedPayload.c_str());

  if (String(topic) == OTA_CHECK_TOPIC) {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, receivedPayload);

    if (error) {
      //      Serial.printf("[ERROR] JSON deserialization failed: %s\n", error.c_str());
      return;
    }

    const char* filename = doc["filename"];
    float newVersion = doc["version"];

    if (newVersion > firmware_verison) {
      //      Serial.print(newVersion);
      write_firmware_version(newVersion);
      //      Serial.printf("[INFO] New firmware version available: %.1f. Initiating OTA...\n", newVersion);
      OTAUpdate(filename);
    } else {
      machine_state = settings_screen_wifi_OTA_no_update;
      //      Serial.println("[INFO] No new firmware update available.");
    }
  } else {
    //    Serial.println("[INFO] Message received on an unknown topic.");
  }
  //  Serial.println("==========================================");
}

//---------------------------------------------------------------------------------------------------------------//
//void OTAUpdate(const char* filename)
//{
//  Serial.println("==========================================");
//
//  if (!filename || strlen(filename) == 0)
//  {
//    Serial.println("[ERROR] Filename is empty or null.");
//    return;
//  }
//
//  String otaUrl = String(OTA_BASE_URL) + filename;
//  Serial.printf("[INFO] OTA URL: %s\n", otaUrl.c_str());
//
//  WiFiClientSecure client;
//  client.setInsecure(); // REMOVE in production
//
//  HTTPUpdate httpUpdater;
//  t_httpUpdate_return ret = httpUpdater.update(client, otaUrl);
//
//  switch (ret)
//  {
//    case HTTP_UPDATE_FAILED:
//      Serial.printf("[ERROR] OTA failed. Error: %d, %s\n", httpUpdater.getLastError(), httpUpdater.getLastErrorString().c_str());
//      EEPROM.write(OTA_FLAG_ADDR, 0);
//      EEPROM.commit();
//      break;
//
//    case HTTP_UPDATE_NO_UPDATES:
//      Serial.println("[INFO] No OTA updates found.");
//      break;
//
//    case HTTP_UPDATE_OK:
//      Serial.println("[SUCCESS] OTA update completed successfully. Rebooting...");
//      EEPROM.write(OTA_FLAG_ADDR, 1);
//      EEPROM.commit();
//      delay(1000);
//      ESP.restart();
//      break;
//  }
//  Serial.println("==========================================");
//}

//void OTAUpdate(const char* filename)
//{
//  if (!filename || strlen(filename) == 0)
//  {
//    Serial.println("[ERROR] Filename is empty or null.");
//    return;
//  }
//
//  String otaUrl = String(OTA_BASE_URL) + filename;
//  Serial.printf("[INFO] OTA URL: %s\n", otaUrl.c_str());
//
//  WiFiClientSecure client;
//  client.setInsecure();
//
//  HTTPClient http;
//  http.begin(client, otaUrl);
//
//  int httpCode = http.GET();
//  if (httpCode == HTTP_CODE_OK)
//  {
//    int totalSize = http.getSize();
//    int downloadedSize = 0;
//
//    Serial.printf("[INFO] Total OTA size: %d bytes\n", totalSize);
//
//    WiFiClient* stream = http.getStreamPtr();
//    uint8_t buffer[128];
//
//    while (http.connected() && (downloadedSize < totalSize))
//    {
//      size_t size = stream->available();
//      if (size)
//      {
//        size_t bytesRead = stream->readBytes(buffer, size > sizeof(buffer) ? sizeof(buffer) : size);
//        downloadedSize += bytesRead;
//
//        int progress = (downloadedSize * 100) / totalSize;
//        Serial.printf("\n[PROGRESS] Downloaded %d%%\r", progress);
//      }
//    }
//
//    Serial.println();
//    if (Update.begin(totalSize))
//    {
//      size_t written = Update.writeStream(*stream);
//      if (written == totalSize && Update.end())
//      {
//        Serial.println("[SUCCESS] OTA update completed. Rebooting...");
//        delay(1000);
//        ESP.restart();
//      }
//      else
//      {
//        Serial.printf("[ERROR] OTA update failed: %s\n", Update.errorString());
//      }
//    }
//    else
//    {
//      Serial.printf("[ERROR] OTA begin failed: %s\n", Update.errorString());
//    }
//  }
//  else
//  {
//    Serial.printf("[ERROR] HTTP request failed. Code: %d\n", httpCode);
//  }
//
//  http.end();
//}

//void OTAUpdate(const char* filename)
//{
//  if (!filename || strlen(filename) == 0)
//  {
//    Serial.println("[ERROR] Filename is empty or null.");
//    return;
//  }
//
//  String otaUrl = String(OTA_BASE_URL) + filename;
//  Serial.printf("[INFO] OTA URL: %s\n", otaUrl.c_str());
//
//  const int maxRetries = 3;
//  int attempt = 0;
//  bool success = false;
//
//  while (attempt < maxRetries && !success)
//  {
//    attempt++;
//    Serial.printf("[INFO] OTA Update Attempt %d of %d\n", attempt, maxRetries);
//
//    WiFiClientSecure client;
//    client.setInsecure(); // REMOVE in production
//
//    HTTPClient http;
//    http.begin(client, otaUrl);
//
//    int httpCode = http.GET();
//    if (httpCode == HTTP_CODE_OK)
//    {
//      int totalSize = http.getSize();
//      int downloadedSize = 0;
//
//      Serial.printf("[INFO] Total OTA size: %d bytes\n", totalSize);
//
//      WiFiClient* stream = http.getStreamPtr();
//      uint8_t buffer[128]; // Adjust buffer size as needed
//
//      while (http.connected() && (downloadedSize < totalSize))
//      {
//        size_t size = stream->available();
//        if (size)
//        {
//          size_t bytesRead = stream->readBytes(buffer, size > sizeof(buffer) ? sizeof(buffer) : size);
//          downloadedSize += bytesRead;
//
//          int progress = (downloadedSize * 100) / totalSize;
//          Serial.printf("\n[PROGRESS] Downloaded %d%%\r", progress);
//        }
//      }
//
//      Serial.println();
//
//      // Proceed with the update
//      if (Update.begin(totalSize))
//      {
//        size_t written = Update.writeStream(*stream);
//        if (written == totalSize && Update.end())
//        {
//          Serial.println("[SUCCESS] OTA update completed. Rebooting...");
//          EEPROM.write(OTA_FLAG_ADD, 1);
//          EEPROM.commit();
//          delay(1000);
//          ESP.restart();
//          success = true; // Update was successful
//        }
//        else
//        {
//          Serial.printf("[ERROR] OTA update failed: %s\n", Update.errorString());
//        }
//      }
//      else
//      {
//        Serial.printf("[ERROR] OTA begin failed: %s\n", Update.errorString());
//      }
//    }
//    else
//    {
//      Serial.printf("[ERROR] HTTP request failed. Code: %d\n", httpCode);
//    }
//
//    http.end();
//
//    if (!success)
//    {
//      Serial.println("[INFO] Retrying...");
//      delay(2000); // Wait 2 seconds before retrying
//    }
//  }
//
//  if (!success)
//  {
//    Serial.println("[ERROR] OTA update failed after maximum retries.");
//    EEPROM.write(OTA_FLAG_ADDR, 0);
//    EEPROM.commit();
//  }
//}

void OTAUpdate(const char* filename)  //this function has ota with retry logic and download progress in serial monitor
{
  if (!filename || strlen(filename) == 0) {
    //    Serial.println("[ERROR] Filename is empty or null.");
    return;
  }

  String otaUrl = String(OTA_BASE_URL) + filename;
  //  Serial.printf("[INFO] OTA URL: %s\n", otaUrl.c_str());

  const int maxRetries = 3;
  for (int attempt = 1; attempt <= maxRetries; ++attempt) {
    if (!wifi_flag)  //wifi handle
    {
      machine_state = settings_screen_wifi_OTA_error;
    }
    //    Serial.printf("[INFO] OTA Update Attempt %d of %d\n", attempt, maxRetries);
    ota_retries = attempt;
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    http.begin(client, otaUrl);
    http.addHeader("Cache-Control", "no-cache");

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      int totalSize = http.getSize();
      int downloadedSize = 0;

      //      Serial.printf("[INFO] Total OTA size: %d bytes\n", totalSize);

      WiFiClient* stream = http.getStreamPtr();
      uint8_t buffer[128];

      if (Update.begin(totalSize)) {
        size_t written = 0;

        while (http.connected() && downloadedSize < totalSize) {
          size_t size = stream->available();
          if (size) {
            size_t bytesRead = stream->readBytes(buffer, size > sizeof(buffer) ? sizeof(buffer) : size);
            downloadedSize += bytesRead;

            written += Update.write(buffer, bytesRead);

            int progress = (downloadedSize * 100) / totalSize;
            //            Serial.printf("\n\r[PROGRESS] Downloaded %d%%", progress);
          }
        }

        //        Serial.println();

        if (written == totalSize && Update.end()) {
          //          Serial.println("[SUCCESS] OTA update completed. Rebooting...");
          delay(1000);
          ESP.restart();
        } else {
          machine_state = settings_screen_wifi_OTA_fail;
          //          Serial.printf("[ERROR] OTA update failed: %s\n", Update.errorString());
          break;
        }
      } else {
        //        Serial.printf("[ERROR] OTA begin failed: %s\n", Update.errorString());
      }
    } else {
      //      Serial.printf("[ERROR] HTTP request failed. Code: %d\n", httpCode);
      //      Serial.println("[DEBUG] Response:");
      //      Serial.println(http.getString());
    }

    http.end();

    if (attempt < maxRetries) {
      //      Serial.println("[INFO] Retrying OTA update...");
      delay(2000);
    } else {
      machine_state = home_screen_init;
      //      Serial.println("[ERROR] Maximum retry attempts reached. OTA update failed.");
    }
  }
}

//---------------------------------------------------------------------------------------------------------------//
float read_firmware_version() {
  float firmware_version = 0.0;  // Initialize the variable
  //  Serial.print("Reading firmware version from address: ");
  //  Serial.println(OTA_VERSION_ADDR);

  EEPROM.get(OTA_VERSION_ADDR, firmware_version);  // Read the firmware version (float) from EEPROM
  //  Serial.print("[FROM EEPROM] Firmware Version: ");
  //  Serial.println(firmware_version);  // Debug log
  return firmware_version;  // Return the retrieved value
}

//---------------------------------------------------------------------------------------------------------------//
void write_firmware_version(float firmware_version) {
  //  Serial.print("Writing firmware version to address: ");
  //  Serial.println(OTA_VERSION_ADDR);
  EEPROM.put(OTA_VERSION_ADDR, firmware_version);  // Write the firmware version (float) to EEPROM
  EEPROM.commit();                                 // Ensure the write is saved
  //  Serial.print("[TO EEPROM] Firmware Version: ");
  //  Serial.println(firmware_version);  // Debug log
}
//---------------------------------------------------------------------------------------------------------------//
void OTASetup() {
  //  Serial.println("==========================================");
  //  Serial.println("--------------BBK OTA---------------------");
  //  Serial.println("==========================================");
  firmware_verison_saved = read_firmware_version();
  //  Serial.print(firmware_verison_saved);
  ConnectToAWS();
}

//---------------------------------------------------------------------------------------------------------------//
void OTALoop() {
  unsigned long currentTime = millis();
  if (currentTime - lastPublishTime >= publishInterval) {
    Get_OTA_Filename();  // To publish topic and get file name and version
    lastPublishTime = currentTime;
  }
  if (!mqttClient.connected()) {
    ConnectToAWS();
  }
  mqttClient.loop();
}

//---------------------------------------------------------------------------------------------------------------//
