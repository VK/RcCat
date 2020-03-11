/* This file is part of VK RcCat.
 *
 * Copyright 2020 Viktor Krueckl (viktor@krueckl.de). All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ==============================================================================
 */

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

const byte DNS_PORT = 53;       // Capture DNS requests on port 53
IPAddress apIP(10, 10, 10, 1);  // Private network for server
DNSServer dnsServer;            // Create the DNS object
ESP8266WebServer webServer(80); // HTTP server

String dummyResponseHTML =
    ""
    "<!DOCTYPE html><html><head><title>RcCat</title></head><body>"
    "<h1>Hello RcCat User</h1><p>This is a captive portal for the"
    "RcCat serial relais. The monitor web app can use this"
    "interface to record car data on the fly."
    "</p></body></html>";

String emptyResponseJSON = "{}";

#define BUFFER_SIZE 32768
char binaryBuffer[BUFFER_SIZE];
unsigned int bufferStart = 0;
unsigned int bufferEnd = 0;

void clear_buffer() {
  bufferStart = 0;
  bufferEnd = 0;
  for (int i = 1; i < BUFFER_SIZE; i++)
    binaryBuffer[i] = 0;
}

void setup() {

  // initialize drive data buffer
  clear_buffer();

  // initialize wifi
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("RcCat-Wifi");

  // start dns server for captive portal
  dnsServer.start(DNS_PORT, "*", apIP);

  // register rccat.local dns entry
  if (!MDNS.begin("rccat")) {
    Serial.println("Error setting up MDNS responder!");
  }
  MDNS.addService("http", "tcp", 80);

  // replay to all unknown requests with same HTML
  webServer.onNotFound(
      []() { webServer.send(200, "text/html", dummyResponseHTML); });

  // clear the drive data buffer
  webServer.on("/clear", [] {
    webServer.send(200, "application/json", emptyResponseJSON);
    clear_buffer();
  });

  // send the request data directly over serial to the RcCat
  webServer.on("/write", [] {
    Serial.print(webServer.arg("plain"));
    webServer.send(200, "application/json", emptyResponseJSON);
  });

  // send the buffered drive data as binary file
  webServer.on("/get", [] {
    WiFiClient client = webServer.client();

    client.print("HTTP/1.1 200 OK\r\n");
    client.print("Content-Disposition: attachment; filename=rccat.bin\r\n");
    client.print("Content-Type: application/octet-stream\r\n");

    // if there was no buffer overflow, we send back the size
    if (bufferStart == 0) {
      client.print("Content-Length: " + String(bufferEnd) + "\r\n");
    } else {
      // with an overflow we will dump the full buffer
      client.print("Content-Length: " + String(BUFFER_SIZE) + "\r\n");
    }
    client.print("Connection: close\r\n");
    client.print("Access-Control-Allow-Origin: *\r\n");
    client.print("\r\n");

    if (bufferStart == 0) {
      // append the buffer, to the end
      client.write((const char *)(binaryBuffer), bufferEnd);
    } else {

      // first append the buffer from the start to the buffer end
      client.write((const char *)(&(binaryBuffer[bufferStart - 1])),
                   BUFFER_SIZE - bufferStart + 1);
      // then the buffer start to the end
      if (bufferEnd > 0)
        client.write((const char *)(binaryBuffer), bufferEnd);
    }

    clear_buffer();
  });

  // start the webserver
  webServer.begin();

  // set baud rate and switch to RX TX pins
  Serial.begin(115200);
  Serial.swap();
}

void loop() {
  // loop functions for DNS, WebServer and local DNS
  dnsServer.processNextRequest();
  webServer.handleClient();
  MDNS.update();

  // handle serial input
  if (Serial.available() > 0) // Checks is there any data in buffer
  {
    // put it into the internal running buffer array
    binaryBuffer[bufferEnd] = char(Serial.read());

    // go to the next buffer Position
    bufferEnd++;
    // check the buffer end overflow
    if (bufferEnd == BUFFER_SIZE) {
      bufferEnd = 0;
    }
    // once we have a buffer overflow, the buffer start moves too
    if (bufferStart == bufferEnd) {
      bufferStart = bufferEnd + 1;
    }
    // and we can also get a buffer start overflow
    if (bufferStart == BUFFER_SIZE) {
      bufferStart = 0;
    }
  }
}