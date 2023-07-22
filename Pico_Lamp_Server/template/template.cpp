#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

/*
  Replace the SSID and Password according to your wifi
*/
const char *ssid       = "Criminal";
const char *password   = "mit123mesh";
bool mode              = false; //false - Dark | true - Light
const unsigned long eventInterval = 10000;
unsigned long previousTime        = 0;

// For Looping the patterns
typedef enum { breathe, snake, none } patterns;
patterns enabled = none;

// Webserver
AsyncWebServer server(80);

String PARAM_MESSAGE = "status";

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

String darkHTML()
{
  return 
%HTML-DARK%
  ;
}

String darkCSS()
{
  return 
%CSS-DARK%
  ;
}

String lightHTML()
{
  return 
%HTML-LIGHT%
  ;
}

String lightCSS()
{
  return 
%CSS-LIGHT%
  ;
}

String darkModeToggle (bool mode)
{
  return (mode == true) ? lightHTML() : darkHTML();
}

void send_serial (const char* message) {
  Serial.write (message);
  Serial.flush();
}

void setup()
{

  Serial.begin(9600);

  // Connect to WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Route for root index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              mode = false;
              request->send(200, "text/html", darkHTML());
            });

  server.on("/dark", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              mode = false;
              request->send(200, "text/html", darkHTML());
            });

  server.on("/light", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              mode = true;
              request->send(200, "text/html", lightHTML());
            });

  server.on("/breathing-lights", HTTP_GET, [](AsyncWebServerRequest *request)
          { 
            enabled = breathe;
            send_serial ("{BREATHING_PATTERN}");
            request->send(200, "text/html", darkModeToggle(mode));
          });

  server.on("/snake-pattern", HTTP_GET, [](AsyncWebServerRequest *request)
          { 
            enabled = snake;
            send_serial ("{SNAKE_PATTERN}");
            request->send(200, "text/html", darkModeToggle(mode));
          });

  server.on("/red", HTTP_GET, [](AsyncWebServerRequest *request)
          {
            enabled = none;
            send_serial ("{RED}");
            request->send(200, "text/html", darkModeToggle(mode));
          });
          
  server.on("/green", HTTP_GET, [](AsyncWebServerRequest *request)
          { 
            enabled = none;
            send_serial ("{GREEN}");
            request->send(200, "text/html", darkModeToggle(mode));
          });

  server.on("/blue", HTTP_GET, [](AsyncWebServerRequest *request)
          { 
            enabled = none;
            send_serial ("{BLUE}");
            request->send(200, "text/html", darkModeToggle(mode));
          });

  server.on("/yellow", HTTP_GET, [](AsyncWebServerRequest *request)
          { 
            enabled = none;
            send_serial ("{YELLOW}");
            request->send(200, "text/html", darkModeToggle(mode));
          });
 
   server.on("/pink", HTTP_GET, [](AsyncWebServerRequest *request)
          { 
            enabled = none;
            send_serial ("{PINK}");
            request->send(200, "text/html", darkModeToggle(mode));
          });

   server.on("/white", HTTP_GET, [](AsyncWebServerRequest *request)
          { 
            enabled = none;
            send_serial ("{WHITE}");
            request->send(200, "text/html", darkModeToggle(mode));
          });

   server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request)
          { 
            enabled = none;
            send_serial ("{OFF}");
            request->send(200, "text/html", darkModeToggle(mode));
          });

  server.on("/dark.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/css", darkCSS()); });

  server.on("/light.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/css", lightCSS()); });

  server.onNotFound(notFound);

  server.begin();
}

void loop()
{
  unsigned long currentTime = millis();

  if (currentTime - previousTime >= eventInterval) {
    if (enabled == breathe) {
      send_serial ("{BREATHING_PATTERN}");
    } else if (enabled == snake) {
      send_serial ("{SNAKE_PATTERN}");
    }
    previousTime = currentTime;
  }
}