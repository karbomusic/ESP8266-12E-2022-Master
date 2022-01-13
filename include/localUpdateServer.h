/*+===================================================================
  File:      localUpdateServer.h

  Summary:   Provides an HTTP server and HTTPUploader for OTA updates,
             both automatic and manual, and a detailed about page.

  Kary Wall 2022.
===================================================================+*/

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266httpUpdate.h>

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

void initWebServer();
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

void startUpdateServer(const char *host, String ipAddress)
{
    localIP = ipAddress;
    Serial.println("Starting HTTP update server...");
    MDNS.begin(host);
    httpUpdater.setup(&httpServer);
    initWebServer();
    MDNS.addService("http", "tcp", 80);
    Serial.printf("HTTP Update Server running! Open http://%s.ra.local/update in your browser to update manually.\n", host);
}

void initWebServer()
{
    httpServer.on("/", handleRoot);
    httpServer.on("/restart", handleRestart);
    httpServer.on("/about", handleAbout);
    httpServer.onNotFound(handleNotFound);
    httpServer.begin();
    Serial.println("HTTP Server running!");
}

void handleAbout()
{
    String aboutResponse = "<body style=\"background-color:#222222;color:#cccccc;font-family:arial\"><b>[About ESP8266]</b><br><br>";
    aboutResponse += "Device Family: " + deviceFamily + "<br>";
    aboutResponse += "ESP Core Version: " + String(ESP.getCoreVersion()) + "<br>";
    aboutResponse += "CPU Frequency: " + String(ESP.getCpuFreqMHz()) + "<br>";
    aboutResponse += "Free Heap Mem: " + String(ESP.getFreeHeap()) + "<br>"; 
    aboutResponse += "Flash Mem Size: " + String(ESP.getFlashChipRealSize()/1024/1024) + " MB<br>"; 
    aboutResponse += "Hostname: " + hostName + "<br>";
    aboutResponse += "IPAddress: " + localIP + "<br>";
    aboutResponse += "MAC Address: " + String(WiFi.macAddress()) + "<br>";
    aboutResponse += "Software Version: " + softwareVersion + "<br>";
    aboutResponse += "SSID: " + ssid + "<br>";
    aboutResponse += "Description: " + description + "<br>";
    aboutResponse += "Update: http://" + hostName + ".ra.local/update<br><br>";
    aboutResponse += "<button onclick=\"window.location.href='/restart'\">Restart</button></body>";
    httpServer.send(200, "text/html", aboutResponse);
    aboutResponse.clear();
}

void handleRestart(){
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
void autoUpdate(WiFiClient updateClient)
{
    Serial.println("Current Version: [" + softwareVersion + "] Checking for update...");

    t_httpUpdate_return ret = ESPhttpUpdate.update(updateClient, "192.168.2.101", 80, "/themothership/api/admin/UpdateESP8266OTA?softwareVersion=" + softwareVersion + "&deviceFamily=" + deviceFamily);

    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        Serial.println("Update failed.");
        Serial.printf("Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        if(ESPhttpUpdate.getLastError() == -102)
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