/*+===================================================================
  Project:  ESP8266 Project template with builtin OTA,
            HTTP Server, WiFi connectivity and About page.
            This can be deployed to any ESP8266 module
            provided you use the correct build targets:
                esp01, esp12e, esp07s
  
  File:      main.cpp

  Summary:   Project template that includes plubming and code for 
             a WiFi client + OTA updates via manual update.
             Automatic updates are not yet implemented but
             may be ported over from my legacy projects.

             EspUpdater: To upload through terminal you can use: 
             curl -F "image=@firmware.bin" host.ra.local/update

             Architecture: ESP8266 specific.

  Building:  pio run -t <target> -e envName

             Examples:
                pio run -t upload, monitor -e esp12e
                pio run -t upload, monitor -e esp01
                pio run -t upload -e esp07s --upload-port COM6
                pio run -t monitor -e fm-dev-kit 

  Config:    You must update secrets.h with your WiFi credentials
             and the hostname you choose for this device.

  Kary Wall 1/16/2022.
===================================================================+*/
#include <localWiFi.h>
#include <localUpdateServer.h>

IPAddress ipAddress;     // IP address of the device
const char *host;        // hostname as seen on the network
WiFiClient updateClient; // client for OTA updates

void setup(void)
{
    /*--------------------------------------------------------------------
     Boot and I/O initialization.
    ---------------------------------------------------------------------*/
    Serial.begin(115200);
    Serial.println();
    Serial.println("Booting up...");
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // I have zero idea why HIGH == OFF

    /*--------------------------------------------------------------------
     Start WiFi & OTA HTTP update server
    ---------------------------------------------------------------------*/
    ipAddress = startWiFi(); // hostname, ssid & pwd in secrets.h
    host = getHostName();
    startServers(host, ipAddress.toString()); // from localUpdateServer.h

#if defined(esp12e) || defined(esp07s)      
    autoUpdate(updateClient);                    
#endif

    /*--------------------------------------------------------------------
     Project specific setup code
    ---------------------------------------------------------------------*/
}

void loop(void)
{
    /*--------------------------------------------------------------------
     Project specific loop code
    ---------------------------------------------------------------------*/

    /*--------------------------------------------------------------------
     Required for OTA updates and/or HTTP server
    ---------------------------------------------------------------------*/
    httpServer.handleClient();
    MDNS.update();
}
