#ifndef AQ_WEB_SERVER_H
#define AQ_WEB_SERVER_H
#include "Environment.h"
#include "AquariumController.h"
#include "AqController.h"
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
//#include <AsyncElegantOTA.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <cstring>
#include "AqWebServerInterface.h"
#include <ESP_Mail_Client.h>

#define SMTP_HOST "smtp.office365.com"
#define SMTP_PORT esp_mail_smtp_port_465
#define AUTHOR_EMAIL "c.jwilliams0332@gmail.com"
#define AUTHOR_PASSWORD "ljjt mmlp wzxp jlro"
#define RECIPIENT_EMAIL "c.jwilliams@me.com"
static SMTPSession smtp;

//App Password:ljjt mmlp wzxp jlro
#ifdef useSerial

#endif
#ifdef useSerialBT
  extern BleSerial SerialBT;
#endif
  extern AqController aqController;
  extern Preferences savedState;
class AqWebServer: public AqWebServerInterface {
  public: 
    AsyncWebServer* server;
    AsyncWebSocket* ws;
    AsyncCallbackJsonWebHandler* setSettingsHandler;
    AsyncCallbackJsonWebHandler* setDeviceStateHandler;
    AsyncCallbackJsonWebHandler* sendMessageHandler;
    //AqWebServer(int port, const char* path);
    AqWebServer();
    void init();
    ~AqWebServer();
    //void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
    //void webServerSetup();
    bool checkAuthorization(AsyncWebServerRequest *request);
    void deviceStateUpdate(Device** devices, int numDevices); //Sends update to Web Socket clients when a device state changes.
    void sensorReadingUpdate(Sensor* sensor); //Sends update to Web Socket clients when a sensor value changes.
    void updateDynamicIP();
    bool processAqControllerMessage(JsonVariant &json);

    //void updateSensorValsOnClients(Sensor* sensors[]);
    //void updateDeviceStatesOnClients(TimedDevice* devices[]);
};

class AqWebServerDummy: public AqWebServerInterface {
  public: 
    AqWebServerDummy() {};
    virtual void deviceStateUpdate(Device** devices, int numDevices) {};
    virtual void sensorReadingUpdate(Sensor* sensor) {};
    virtual void updateDynamicIP() {};
    ~AqWebServerDummy() {};
};
static void smtpCallback(SMTP_Status status)
{
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success())
  {
    // MailClient.printf used in the examples is for format printing via debug Serial port
    // that works for all supported Arduino platform SDKs e.g. SAMD, ESP32 and ESP8266.
    // In ESP8266 and ESP32, you can use Serial.printf directly.

    Serial.println("----------------");
    MailClient.printf("Message sent success: %d\n", status.completedCount());
    MailClient.printf("Message sent failed: %d\n", status.failedCount());
    Serial.println("----------------\n");

    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
    {
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);

      // In case, ESP32, ESP8266 and SAMD device, the timestamp get from result.timestamp should be valid if
      // your device time was synched with NTP server.
      // Other devices may show invalid timestamp as the device time was not set i.e. it will show Jan 1, 1970.
      // You can call smtp.setSystemTime(xxx) to set device time manually. Where xxx is timestamp (seconds since Jan 1, 1970)

      MailClient.printf("Message No: %d\n", i + 1);
      MailClient.printf("Status: %s\n", result.completed ? "success" : "failed");
      MailClient.printf("Date/Time: %s\n", MailClient.Time.getDateTimeString(result.timestamp, "%B %d, %Y %H:%M:%S").c_str());
      MailClient.printf("Recipient: %s\n", result.recipients.c_str());
      MailClient.printf("Subject: %s\n", result.subject.c_str());
    }
    Serial.println("----------------\n");

    // You need to clear sending result as the memory usage will grow up.
    smtp.sendingResult.clear();
  }
}
/*

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
void webServerSetup();
bool checkAuthorization(AsyncWebServerRequest *request);
void notFound(AsyncWebServerRequest *request);
void updateDDNS();
void updateSensorValsOnClients(Sensor* sensors[]);
void updateDeviceStatesOnClients(TimedDevice* devices[]);
*/
#endif