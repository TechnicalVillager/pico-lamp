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
"<!doctype html>"
"<html lang=\"en\">"
"  <head>"
"    <meta charset=\"utf-8\">"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
"    <link rel=\"stylesheet\" href=\"/dark.css\">"
"    <title>Pico Lamp</title>"
"  </head>"
"  <body>"
"    <h1>"
"      <center>"
"        PICO <kbd>LAMP</kbd>"
"      </center>"
"    </h1>"
"    <center>"
"      <button onclick=\"window.location.href='/light'\" style=\"margin: 0px; padding: 10px; line-height: 1;\">"
"        <svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" class=\"feather feather-moon\">"
"          <path d=\"M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z\"></path>"
"        </svg>"
"      </button>"
"    </center>"
"    <h2>"
"      <center>"
"        Patterns"
"      </center>"
"    </h2>"
"    <div style=\"display: flex; flex-direction: row; flex-wrap: wrap; justify-content: center;\">"
"        <button onclick=\"window.location.href='/breathing-lights'\">"
"          Breathing Lights"
"        </button>"
"        <button onclick=\"window.location.href='/snake-pattern'\">"
"          Snake Pattern"
"        </button>"
"    </div>"
"    <h2>"
"      <center>"
"        Color Picker"
"      </center>"
"    </h2>"
"    <div style=\"display: flex; flex-direction: row; flex-wrap: wrap; justify-content: center;\">"
"        <button onclick=\"window.location.href='/red'\" style=\"color: red;\">"
"          RED"
"        </button>"
"        <button onclick=\"window.location.href='/green'\" style=\"color: green;\">"
"          GREEN"
"        </button>"
"        <button onclick=\"window.location.href='/blue'\" style=\"color: blue;\">"
"          BLUE"
"        </button>"
"        <button onclick=\"window.location.href='/yellow'\" style=\"color: yellow;\">"
"          YELLOW"
"        </button>"
"        <button onclick=\"window.location.href='/pink'\" style=\"color: deeppink;\">"
"          PINK"
"        </button>"
"        <button onclick=\"window.location.href='/white'\">"
"          WHITE"
"        </button>"
"        <button onclick=\"window.location.href='/off'\">"
"          OFF"
"        </button>"
"    </div>"
"    <!-- Footer -->"
"      <center style=\"padding-top: 30px; padding-bottom: 30px;\">"
"        <small>"
"          Copyright © <a href=\"https://technicalvillager.github.io/\" class=\"secondary\" target=\"_blank\">Saiffullah Sabir Mohamed</a> <script>document.write(new Date().getFullYear())</script>"
"        </small>"
"      </center>"
"  </body>"
"</html>"
  ;
}

String darkCSS()
{
  return 
"@charset \"UTF-8\";body{font-family:system-ui,-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,Oxygen,Ubuntu,Cantarell,'Fira Sans','Droid Sans','Helvetica Neue',sans-serif;line-height:1.4;max-width:800px;margin:20px auto;padding:0 10px;color:#dbdbdb;background:#202b38;text-rendering:optimizeLegibility}h1{font-size:2.2em;margin-top:0}h1,h2{margin-bottom:12px;color:#fff;font-weight:600}button{transition:background-color .1s linear,border-color .1s linear,color .1s linear,box-shadow .1s linear,transform .1s;cursor:pointer;color:#fff;background-color:#161f27;font-family:inherit;font-size:inherit;margin:10px;padding:20px 30px;border:none;border-radius:6px;outline:0;-webkit-appearance:none}button:hover{background:#324759}button:focus{box-shadow:0 0 0 2px #0096bfab}button:active{transform:translateY(2px)}a{text-decoration:none;color:#41adff}a:hover{text-decoration:underline}code,kbd{background:#161f27;color:#ffbe85;padding:5px;border-radius:6px}"
  ;
}

String lightHTML()
{
  return 
"<!doctype html>"
"<html lang=\"en\">"
"  <head>"
"    <meta charset=\"utf-8\">"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
"    <link rel=\"stylesheet\" href=\"/light.css\">"
"    <title>Pico Lamp</title>"
"  </head>"
"  <body>"
"    <h1>"
"      <center>"
"        PICO <kbd>LAMP</kbd>"
"      </center>"
"    </h1>"
"    <center>"
"      <button onclick=\"window.location.href='/dark'\" style=\"margin: 0px; padding: 10px; line-height: 1;\">"
"        <svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" class=\"feather feather-sun\">"
"          <circle cx=\"12\" cy=\"12\" r=\"5\"></circle>"
"          <line x1=\"12\" y1=\"1\" x2=\"12\" y2=\"3\"></line>"
"          <line x1=\"12\" y1=\"21\" x2=\"12\" y2=\"23\"></line>"
"          <line x1=\"4.22\" y1=\"4.22\" x2=\"5.64\" y2=\"5.64\"></line>"
"          <line x1=\"18.36\" y1=\"18.36\" x2=\"19.78\" y2=\"19.78\"></line>"
"          <line x1=\"1\" y1=\"12\" x2=\"3\" y2=\"12\"></line><line x1=\"21\" y1=\"12\" x2=\"23\" y2=\"12\"></line>"
"          <line x1=\"4.22\" y1=\"19.78\" x2=\"5.64\" y2=\"18.36\"></line>"
"          <line x1=\"18.36\" y1=\"5.64\" x2=\"19.78\" y2=\"4.22\"></line>"
"        </svg>"
"      </button>"
"    </center>"
"    <h2>"
"      <center>"
"        Patterns"
"      </center>"
"    </h2>"
"    <div style=\"display: flex; flex-direction: row; flex-wrap: wrap; justify-content: center;\">"
"        <button onclick=\"window.location.href='/breathing-lights'\">"
"          Breathing Lights"
"        </button>"
"        <button onclick=\"window.location.href='/snake-pattern'\">"
"          Snake Pattern"
"        </button>"
"    </div>"
""
"    <h2>"
"      <center>"
"        Color Picker"
"      </center>"
"    </h2>"
"    <div style=\"display: flex; flex-direction: row; flex-wrap: wrap; justify-content: center;\">"
"        <button onclick=\"window.location.href='/red'\" style=\"color: red;\">"
"          RED"
"        </button>"
"        <button onclick=\"window.location.href='/green'\" style=\"color: green;\">"
"          GREEN"
"        </button>"
"        <button onclick=\"window.location.href='/blue'\" style=\"color: blue;\">"
"          BLUE"
"        </button>"
"        <button onclick=\"window.location.href='/yellow'\" style=\"color: yellow;\">"
"          YELLOW"
"        </button>"
"        <button onclick=\"window.location.href='/pink'\" style=\"color: deeppink;\">"
"          PINK"
"        </button>"
"        <button onclick=\"window.location.href='/white'\">"
"          WHITE"
"        </button>"
"        <button onclick=\"window.location.href='/off'\">"
"          OFF"
"        </button>"
"    </div>"
"    <!-- Footer -->"
"      <center style=\"padding-top: 30px; padding-bottom: 30px;\">"
"        <small>"
"          Copyright © <a href=\"https://technicalvillager.github.io/\" class=\"secondary\" target=\"_blank\">Saiffullah Sabir Mohamed</a> <script>document.write(new Date().getFullYear())</script>"
"        </small>"
"      </center>"
"  </body>"
"</html>"
  ;
}

String lightCSS()
{
  return 
"@charset \"UTF-8\";button,code,h1,h2,kbd{color:#000}body{font-family:system-ui,-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,Oxygen,Ubuntu,Cantarell,'Fira Sans','Droid Sans','Helvetica Neue',sans-serif;line-height:1.4;max-width:800px;margin:20px auto;padding:0 10px;color:#363636;background:#fff;text-rendering:optimizeLegibility}h1{font-size:2.2em;margin-top:0}h1,h2{margin-bottom:12px;font-weight:600}button{transition:background-color .1s linear,border-color .1s linear,color .1s linear,box-shadow .1s linear,transform .1s;cursor:pointer;background-color:#efefef;font-family:inherit;font-size:inherit;margin:10px;padding:20px 30px;border:none;border-radius:6px;outline:0;-webkit-appearance:none}button:hover{background:#ddd}button:focus{box-shadow:0 0 0 2px #0096bfab}button:active{transform:translateY(2px)}a{text-decoration:none;color:#0076d1}a:hover{text-decoration:underline}code,kbd{background:#efefef;padding:5px;border-radius:6px}"
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
