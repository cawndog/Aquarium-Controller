#Replace the values in this file with your own.

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

//#define ENVIRONMENT_DEV
#define ENVIRONMENT_PROD

#define WIFI_SSID "WiFi SSID" //Add your WiFi SSID
#define WIFI_PASSWORD "WiFi Password!" ///Add your WiFi Password
#define SOFT_AP_SSID "Soft AP WiFi SSID" //Soft AP SSID. ESP32 falls back to hosting its own WiFi network if it cannot connect to the above network.
#define SOFT_AP_PASSWORD "Soft AP Wifi Password" //Soft AP Password.
#define API_KEY "Bearer RwSDw4J3gpfD42jJ9s10IVEERGMem2RZ44EAxerdUIqT5lVIzTmq2UQc36CWtrHC" //Not my real API key. Add your own. 
#define DYNAMIC_IP_UPDATE_URL "http://api.dynu.com/nic/update?username=myusername&mypassword=myDDNS"
#define MAIL_SERVER         "smtp.googlemail.com"
#define MAIL_PORT           "587"
#define SENDER_MAIL         "email@gmail.com" //Gmail Email
#define SENDER_PASSWORD     "AppPassword" //Use Google App Password for better security.
#define RECIPIENT_MAIL      "email@me.com" //Email address to send alerts to.

#endif
