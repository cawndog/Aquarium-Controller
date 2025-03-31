#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

//#define ENVIRONMENT_DEV
#define ENVIRONMENT_PROD

#define WIFI_SSID "Pepper" //Add your WiFi SSID
#define WIFI_PASSWORD "unlawfulOwl69!" ///Add your WiFi Password
#define SOFT_AP_SSID "AquariumController" //Soft AP SSID. ESP32 falls back to hosting its own WiFi network if it cannot connect to the above network.
#define SOFT_AP_PASSWORD "password" //Soft AP Password.
#define API_KEY "Bearer ayAkxuh5A1JFLmDkpO8iEFiq1RYhQztX" //Not my real API key. Add your own. 
#define DYNAMIC_IP_UPDATE_URL "http://api.dynu.com/nic/update?username=cawndog&mypassword=myDDNS"
#define MAIL_SERVER         "smtp.googlemail.com"
#define MAIL_PORT           "587"
#define SENDER_MAIL         "c.jwilliams0332@gmail.com" //Gmail Email
#define SENDER_PASSWORD     "xhxw yctk aodt sozp" //Use Google App Password for better security.
#define RECIPIENT_MAIL      "c.jwilliams@me.com" //Email address to send alerts to.

#endif
