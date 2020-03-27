#include <Wire.h>
#include "IPAddress.h"
#include "painlessMesh.h"

#ifdef ESP8266
#include "Hash.h"
#include <ESPAsyncTCP.h>
#else
#include <AsyncTCP.h>
#endif

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

#define   STATION_SSID     "BTHN"
#define   STATION_PASSWORD "xyz.1804.abc"


#define HOSTNAME "HTTP_BRIDGE"

// Prototype
void receivedCallback( const uint32_t &from, const String &msg );
IPAddress getlocalIP();

painlessMesh  mesh;
//AsyncWebServer server(80);
IPAddress myIP(0,0,0,0);
IPAddress myAPIP(0,0,0,0);
/* */

/* SERVER kutuphaneleri*/
#include <WiFi.h>
#include <WebServer.h>
/**/

// ag adi ver parolasi

long int a = 0;
char htmlResponse[3000];
WebServer server(80);                
String nodeName = "N0"; // Name needs to be unique

/////
IPAddress local_IP(192, 168, 1, 184);

IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
/////

void setup() {
  Serial.begin(115200);
  delay(100);

  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6 );
  mesh.onReceive(&receivedCallback);

  // mesh.setName(nodeName)
  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  mesh.setHostname(HOSTNAME);
  mesh.setRoot(true);
  mesh.setContainsRoot(true);

  myAPIP = IPAddress(mesh.getAPIP());
  Serial.println("My AP IP is " + myAPIP.toString());

  Serial.println("HTTP server started");

  server.on ( "/al", handleRoot );
  server.on ("/save", handleSave);

  server.on("/come", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();

}
void loop() {
  
  server.handleClient();
  mesh.update();
  if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
  }
}


void handle_OnConnect() {

  server.send(200, "text/html",  "ahh mercem "); 
  
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}


void receivedCallback( const uint32_t &from, const String &msg ) {
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
  //server.send(200, "/come/text/html",  msg.c_str());
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}


void handleRoot() {

  snprintf ( htmlResponse, 3000,
"<!DOCTYPE html>\
<html lang=\"en\">\
  <head>\
    <meta charset=\"utf-8\">\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
  </head>\
  <body>\
          <h1>Time</h1>\
          <input type='text' name='date_hh' id='date_hh' size=2 autofocus> hh \
          <div>\
          <br><button id=\"save_button\">Save</button>\
          </div>\
    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js\"></script>\    
    <script>\
      var hh;\
      $('#save_button').click(function(e){\
        e.preventDefault();\
        hh = $('#date_hh').val();\    
        $.get('/save?hh=' + hh , function(data){\
          console.log(data);\
        });\
      });\      
    </script>\
  </body>\
</html>"); 

   server.send ( 200, "text/html", htmlResponse );  

}


void handleSave() {
  if (server.arg("hh")!= ""){
    Serial.println("message: " + server.arg("hh"));
     mesh.sendBroadcast(server.arg("hh"));
  }

}
