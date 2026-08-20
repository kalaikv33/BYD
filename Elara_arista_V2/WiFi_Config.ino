//--------------------------------------------------BBK-------------------------------------------------------//
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
//--------------------------------------------------HTML------------------------------------------------------//

const char* webpageHTML = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0">
    <title>Beyond Appliances- Wi-Fi Config</title>
    <style>
        body {
            background-color: #f4f4f4;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            color: #333;
            text-align: center;
            margin: 0;
            padding: 0;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100vh;
        }

        h1 {
            color: #4caf50;
            margin-bottom: 20px;
        }

        form {
            max-width: 400px;
            margin: 20px auto;
            padding: 20px;
            background-color: #fff;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }

        label {
            display: block;
            margin: 10px 0 5px;
            font-size: 16px;
        }

        input, select {
            width: 100%;
            padding: 10px;
            margin: 5px 0 15px;
            box-sizing: border-box;
            border: 1px solid #ccc;
            border-radius: 5px;
            font-size: 16px;
        }

        input[type="submit"] {
            background-color: #4caf50;
            color: white;
            cursor: pointer;
        }

        input[type="submit"]:hover {
            background-color: #45a049;
        }

        .password-container {
            display: flex;
            align-items: center;
        }

        .password-toggle {
            margin-left: 10px;
            cursor: pointer;
            color: #007BFF;
            font-size: 14px;
        }

        button {
            background-color: #007BFF;
            color: white;
            padding: 10px 20px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-size: 16px;
            margin-top: 10px;
        }

        button:hover {
            background-color: #0056b3;
        }

        /* Spinner CSS */
        .spinner {
            display: none;
            border: 4px solid #f3f3f3;
            border-top: 4px solid #3498db;
            border-radius: 50%;
            width: 20px;
            height: 20px;
            animation: spin 1s linear infinite;
            margin-left: 10px;
        }

        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
    </style>
</head>
<body>
    <h1>Beyond Appliances- Wi-Fi Config</h1>
    <form action="/connect" method="post">
        <label for="ssid">Select Wi-Fi Network:</label>
        <select name="ssid" required>
            <!-- Available Wi-Fi networks will be dynamically added here -->
        </select><br>
        <label for="password">Password:</label>
        <div class="password-container">
            <input type="password" name="password" id="password" placeholder="Enter Wi-Fi Password" required>
            <span class="password-toggle" onclick="togglePassword()">Show Password</span>
        </div><br>
        <input type="submit" value="Connect">
    </form>

    <button onclick="refreshWifiList()">Refresh Wi-Fi List</button>
    <div id="spinner" class="spinner"></div>

    <script>
        function togglePassword() {
            var passwordInput = document.getElementById("password");
            passwordInput.type = (passwordInput.type === "password") ? "text" : "password";
        }

        async function populateWifiList() {
            var wifiSelect = document.getElementsByName("ssid")[0];
            var spinner = document.getElementById("spinner");

            // Show the spinner
            spinner.style.display = 'inline-block';

            try {
                const response = await fetch("/wifiList");
                const wifiList = await response.json();

                wifiSelect.innerHTML = ''; // Clear existing options

                wifiList.forEach((network) => {
                    var option = document.createElement("option");
                    option.text = network.ssid;
                    wifiSelect.add(option);
                });
            } catch (error) {
                console.error("Error fetching Wi-Fi list:", error);
            } finally {
                // Hide the spinner
                spinner.style.display = 'none';
            }
        }
        // Function to refresh the Wi-Fi list
        function refreshWifiList() {
            populateWifiList();
        }

        // Call the function when the page is loaded
        window.onload = function() {
            populateWifiList();
        };
    </script>
</body>
</html>
)rawliteral";
//--------------------------------------------------Variables------------------------------------------------------//
Preferences preferences;
WebServer server(80);

// Access Point config
const char* apSSID = "Beyond-WiFi";
const char* apPassword = "";
IPAddress local_IP(192, 168, 1, 4);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

bool isProvisioning = false;
extern bool wifi_flag;
//--------------------------------------------------Function Declaration------------------------------------------------------//
void startAPMode();
void handleRootPage();
void handleWifiList();
void handleConnect();
void WifiSetup();
void WifiLoop();
void reset_wifi();
void wifi_off();
//--------------------------------------------------Function Defnition------------------------------------------------------//
//void startAPMode()
//{
//  isProvisioning = true;
//  WiFi.disconnect(true);
//  WiFi.mode(WIFI_AP);
//  WiFi.softAPConfig(local_IP, gateway, subnet);
//  WiFi.softAP(apSSID, apPassword);
//  delay(1000);
//
//  Serial.println("[Info]AP Mode started");
//  Serial.println("[Info]SSID: " + String(apSSID));
//  Serial.println("[Info]AP IP: " + WiFi.softAPIP().toString());
//
//  server.on("/", HTTP_GET, handleRootPage);
//  server.on("/wifiList", HTTP_GET, handleWifiList);
//  server.on("/connect", HTTP_POST, handleConnect);
//
//  server.begin();
//  Serial.println("[Info]Web server running");
//}

void startAPMode() {
  isProvisioning = true;
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  delay(100);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(apSSID, apPassword);
  delay(1000);

  //  Serial.println("[Info] AP Mode started");
  //  Serial.println("[Info] SSID: " + String(apSSID));
  //  Serial.println("[Info] AP IP: " + WiFi.softAPIP().toString());

  server.on("/", HTTP_GET, handleRootPage);
  server.on("/wifiList", HTTP_GET, handleWifiList);
  server.on("/connect", HTTP_POST, handleConnect);

  server.begin();
  //  Serial.println("[Info] Web server running");
}

//--------------------------------------------------------------------------------------------------------------------------//
void handleRootPage() {
  server.send(200, "text/html", webpageHTML);
}
//--------------------------------------------------------------------------------------------------------------------------//
void handleWifiList() {
  int n = WiFi.scanNetworks();
  String json = "[";

  for (int i = 0; i < n; ++i) {
    json += "{\"ssid\":\"" + WiFi.SSID(i) + "\"}";
    if (i < n - 1) json += ",";
  }
  json += "]";
  server.send(200, "application/json", json);
}
//--------------------------------------------------------------------------------------------------------------------------//
//void handleConnect()
//{
//  if (server.hasArg("ssid") && server.hasArg("password"))
//  {
//    String ssid = server.arg("ssid");
//    String pass = server.arg("password");
//
//    Serial.println("SSID: " + ssid);
//    Serial.println("Password: " + pass);
//
//    preferences.putString("ssid", ssid);
//    preferences.putString("pass", pass);
//
//    WiFi.softAPdisconnect(true);
//    WiFi.disconnect(true);
//    WiFi.mode(WIFI_STA);
//    delay(1000);
//    WiFi.begin(ssid.c_str(), pass.c_str());
//
//    int attempts = 0;
//    while (WiFi.status() != WL_CONNECTED && attempts < 20)
//    {
//      delay(500);
//      Serial.print(".");
//      attempts++;
//    }
//
//    if (WiFi.status() == WL_CONNECTED)
//    {
//      wifi_flag=1;
//      Serial.println("[Sucess]Connected to new WiFi!");
//      Serial.println("IP: " + WiFi.localIP().toString());
//      server.send(200, "text/html", "<html><body><h1>Connected successfully!</h1></body></html>");
//    }
//    else
//    {
//      Serial.println("[Error] Failed to connect.");
//      server.send(200, "text/html", "<html><body><h1>Failed to connect. Try again.</h1></body></html>");
//    }
//  }
//  else
//  {
//    server.send(400, "text/plain", "[Error]Missing SSID or Password");
//  }
//}

void handleConnect() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String pass = server.arg("password");

    if (ssid.length() == 0 || pass.length() < 8) {
      server.send(400, "text/html", "<html><body><h1>Invalid SSID or Password</h1></body></html>");
      return;
    }

    //    Serial.println("SSID: " + ssid);
    //    Serial.println("Password: " + pass);

    preferences.putString("ssid", ssid);
    preferences.putString("pass", pass);

    WiFi.softAPdisconnect(true);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    delay(1000);
    WiFi.begin(ssid.c_str(), pass.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      //      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      wifi_flag = 1;
      //      Serial.println("[Success] Connected to new WiFi!");
      //      Serial.println("IP: " + WiFi.localIP().toString());
      server.send(200, "text/html", "<html><body><h1>Connected successfully!</h1></body></html>");
    } else {
      //      Serial.println("[Error] Failed to connect.");
      server.send(200, "text/html", "<html><body><h1>Failed to connect. Try again.</h1></body></html>");
    }
  } else {
    server.send(400, "text/plain", "[Error] Missing SSID or Password");
  }
}



//--------------------------------------------------------------------------------------------------------------------------//
void WifiSetup() {
  wifi_flag = 0;
  //  Serial.println("==========================================");
  //  Serial.println("========== Wifi Initialization ==========");
  preferences.begin("wifiCreds", false);
  //  preferences.clear();
  //  preferences.end();

  String savedSSID = preferences.getString("ssid", "");
  String savedPASS = preferences.getString("pass", "");
  if (savedSSID != "") {
    WiFi.mode(WIFI_STA);
    WiFi.begin(savedSSID.c_str(), savedPASS.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
      delay(500);
      //      Serial.print(".");
      attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      wifi_flag = 1;
      //      Serial.println("[Sucess]Connected to saved WiFi!");
      //      Serial.println("IP: " + WiFi.localIP().toString());
      return;
    } else {
      //      Serial.println("[Error] Failed to connect to saved WiFi.");
    }
  }

  startAPMode();
}
//--------------------------------------------------------------------------------------------------------------------------//
void WifiLoop() {
  if (isProvisioning) {
    server.handleClient();
  }
}
//--------------------------------------------------------------------------------------------------------------------------//
void reset_wifi() {
  wifi_flag = 0;
  //  Serial.println("[Info] Clearing Wi-Fi Credentials");
  preferences.begin("wifiCreds", false);
  preferences.clear();
  preferences.end();

  WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_OFF);  // Ensure Wi-Fi is completely turned off
  delay(100);

  //  ESP.restart();
}
//--------------------------------------------------------------------------------------------------------------------------//
void wifi_off() {
  WiFi.disconnect(true);
  delay(50);
  WiFi.mode(WIFI_OFF);
  delay(50);
}
