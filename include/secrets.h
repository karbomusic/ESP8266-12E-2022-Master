/*+===================================================================
  File:      secrets.h

  Summary:   Device and WiFi specfic details go here such as 
             SSID, password and hostname. Also some are used
             for OTA updates and the about page.

  Notes:     deviceFamily and softwareVersion are used by your
             remote update server to determine if a new update is
             available for this device. If you need multiple devices
             with identical firmware you can use the same for all.

             It wouldn't be a bad idea to combine these strings as the
             filename: mydevice-12.10.21.bin. This would allow the 
             server to match mydevice-12.10.21.bin to a file on the 
             server named mydevice-12.22.21.bin. Assuming you are
             using automatic updates.

  Kary Wall 1/20/2022.            
===================================================================+*/

#include <Arduino.h>

#ifndef STASSID
#define STASSID "YOURSSID"
#define STAPSK  "YOURPWD"
#endif

const String ssid = STASSID;                                            // WiFi ssid
const String password = STAPSK;                                         // WiFi password

#if defined(esp12e) 
const String hostName = "ESP12E-DEV";                                  // hostname as seen on network
#elif defined(esp07s)   
const String hostName = "ESP07-DEV";                        
#else
const String hostName = "ESP01-DEV";                                   
#endif

const String softwareVersion = "1.10.21";                               // used for OTA updates & about page
const String deviceFamily = "ESP8266-NOTSET";                           // used for OTA updates & about page
const String description = "Description";                               // used for about page
