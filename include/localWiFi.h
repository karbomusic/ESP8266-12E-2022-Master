/*+===================================================================
  File:      localWiFi.h

  Summary:   Start WiFi and connect.

  Dependencies:
              secrets.h
              Arduino.h

  Kary Wall 2022.
===================================================================+*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <secrets.h>

int retryTrigger = 30;
int timerSeconds = 0;

const char* getHostName()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return WiFi.getHostname();
    }
    else
    {
        return "WiFi not connected.";
    }
}
// ToDo: Change return type back to String if possible, so we can return more
//       than just the IP address; such as error messages if we can't connect.
IPAddress startWiFi()
{
    Serial.println("Starting WiFi...");
    WiFi.enableAP(false);
    WiFi.persistent(false);
    WiFi.enableSTA(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // Connect to WiFi. Reboot and try again if we can't connect within 30 seconds.
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print("-->");
        delay(500);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);

        if (timerSeconds >= retryTrigger)
        {
            Serial.print("WiFi connection failed due to timeout, rebooting...");
            ESP.restart();
        }
        timerSeconds++;
    }
    
    WiFi.setHostname(hostName.c_str());
    Serial.println("\n-------------------------------------");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Hostname: ");
    Serial.println(WiFi.getHostname());
    Serial.println("Device Family: " + deviceFamily);
    Serial.println("-------------------------------------\n");
    return WiFi.localIP();
}