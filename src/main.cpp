/*+===================================================================
  Project:  ESP8266 Project template with builtin OTA,
            HTTP Server, WiFi connectivity and About page.
  
  File:      main.cpp

  Summary:   Project template that includes plubming and code for 
             a WiFi client + OTA updates via manual update.
             Automatic updates are not yet implemented but
             may be ported over from my legacy projects.

             EspUpdater: To upload through terminal you can use: 
             curl -F "image=@firmware.bin" host.ra.local/update

             Architecture: ESP8266 specific.

  Dependencies:.

             localWifi.h 
                -- <Arduino.h>
                -- <ESP8266WiFi.h>
                -- <secrets.h>               
             localUPdateServer.h
                -- <Arduino.h>
                -- <ESP8266WiFi.h>
                -- <ESP8266mDNS.h>
                -- <ESP8266HTTPUpdateServer.h>
                -- <ESP8266httpUpdate.h>
                -- <secrets.h>

  Config:    You must update secrets.h with your WiFi credentials
             and the hostname you choose for this device.

  Kary Wall 1/20/2022.
===================================================================+*/

#include <localWiFi.h>
#include <localUpdateServer.h>

IPAddress ipAddress;                // IP address of the device
const char *host;                   // hostname as seen on the network
WiFiClient updateClient;            // client for OTA updates 

void setup(void)
{
    /*--------------------------------------------------------------------
     Boot and I/O initialization.
    ---------------------------------------------------------------------*/
    Serial.begin(115200);
    Serial.println();
    Serial.println("Booting up...");
    pinMode(LED_BUILTIN, OUTPUT);

    /*--------------------------------------------------------------------
     Start WiFi & OTA HTTP update server
    ---------------------------------------------------------------------*/
    ipAddress = startWiFi();                                // hostname, ssid & pwd in secrets.h
    host = getHostName();                                   // from localWiFi.h
    startUpdateServer(host, ipAddress.toString());          // from localUpdateServer.h
    digitalWrite(LED_BUILTIN, HIGH);                        // I have zero idea why HIGH == OFF
    autoUpdate(updateClient);                               // uncomment if not using auto updates

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
     Required for OTA updates
    ---------------------------------------------------------------------*/
    httpServer.handleClient();
    MDNS.update();
}
