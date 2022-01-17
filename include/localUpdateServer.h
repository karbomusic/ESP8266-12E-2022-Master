/*+===================================================================
  File:      localUpdateServer.h

  Summary:   Provides an HTTP server and HTTPUploader for OTA updates,
             both automatic and manual, and a detailed about page.

  Kary Wall 2022.
===================================================================+*/

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#if defined(esp12e) || defined(esp07s)
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266httpUpdate.h>
#endif

ESP8266WebServer httpServer(80);
#if defined(esp12e) || defined(esp07s)
ESP8266HTTPUpdateServer httpUpdater;
#endif

void startWebServer();
void handleRoot();
void handleNotFound();
void handleAbout();
void handleRestart();

String localIP;

// variables in secrets.h
extern const String deviceFamily;
extern const String softwareVersion;
extern const String deviceFamily;
extern const String description;
extern const String ssid;
extern const String hostName;

void startServers(const char *host, String ipAddress)
{
    localIP = ipAddress;
    MDNS.begin(host);

#if defined(esp12e) || defined(esp07s)
    Serial.println("Starting HTTP update server...");
    httpUpdater.setup(&httpServer);
#endif

    startWebServer();
    MDNS.addService("http", "tcp", 80);
    
#if defined(esp12e) || defined(esp07s)
    Serial.printf("HTTP Update & Web Servers running! Open http://%s.ra.local/update in your browser to update manually.\n", host);
#else
    Serial.printf("HTTP Server running! Open http://%s.ra.local/about in your browser.\n", host);
#endif
}

void startWebServer()
{
    httpServer.on("/", handleRoot);
    httpServer.on("/restart", handleRestart);
    httpServer.on("/about", handleAbout);
    httpServer.onNotFound(handleNotFound);
    httpServer.begin();
}

void handleAbout()
{
    String aboutResponse = "<body style=\"background-color:#222222;color:#cccccc;font-family:arial\"><b>[About ESP8266]</b><br><br>"
    "Device Family: " + deviceFamily + "<br>"
    "ESP Core Version: " + String(ESP.getCoreVersion()) + "<br>"
    "CPU Frequency: " + String(ESP.getCpuFreqMHz()) + "<br>"
    "Free Heap Mem: " + String(ESP.getFreeHeap()) + "<br>"
    "Flash Mem Size: " + String(ESP.getFlashChipRealSize() / 1024 / 1024) + " MB<br>"
    "<b>Uptime:</b> " + String(millis()/1000/60) + " minutes<br>"
    "Hostname: " + hostName + "<br>"
    "IPAddress: " + localIP + "<br>"
    "MAC Address: " + String(WiFi.macAddress()) + "<br>"
    "Software Version: " + softwareVersion + "<br>"
    "SSID: " + ssid + "<br>"
    "Description: " + description + "<br>"
    "Update: http://" + hostName + ".ra.local/update<br><br>"
    "<button onclick=\"window.location.href='/restart'\">Restart</button></body>";
    httpServer.send(200, "text/html", aboutResponse);
    aboutResponse.clear();
}

void handleRestart()
{
    httpServer.send(200, "text/plain", "Restarting in 5 seconds...");
    delay(5000);
    ESP.restart();
}

void handleRoot()
{
    String response = "ESP8266 HTTP Server running...";
    httpServer.send(200, "text/plain", response);
}

void handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += httpServer.uri();
    message += "\nMethod: ";
    message += (httpServer.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += httpServer.args();
    message += "\n";
    for (uint8_t i = 0; i < httpServer.args(); i++)
    {
        message += " " + httpServer.argName(i) + ": " + httpServer.arg(i) + "\n";
    }
    httpServer.send(404, "text/plain", message);
}

// Used for automatic updates only.
// Requires your own server code to decide whether to update or not.
// Then respond to this request with the update.bin file.
// Once all devices that use this method have been updated,
// Consider removing this method and using the HTTPUpdateServer instead.
#if defined(esp12e) || defined(esp07s)
void autoUpdate(WiFiClient updateClient)
{
    Serial.println("Current Version: [" + softwareVersion + "] Checking for update...");
    t_httpUpdate_return ret = ESPhttpUpdate.update(updateClient, "192.168.2.101", 80, "/themothership/api/admin/UpdateESP8266OTA?softwareVersion=" + softwareVersion + "&deviceFamily=" + deviceFamily);

    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        Serial.println("Update failed.");
        Serial.printf("Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        if (ESPhttpUpdate.getLastError() == -102)
        {
            Serial.println("Wrong URL or the remote update server is unaware of this device.");
        }
        break;
    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("No update needed.");
        break;
    case HTTP_UPDATE_OK:
        Serial.println("Update ok."); // may not called we reboot the ESP
        break;
    }
}
#endif