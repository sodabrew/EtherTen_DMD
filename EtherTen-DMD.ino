/*
 * EtherTen with DMD
 * Aaron Stone, April, 2012
 *
 * This project has the Arduino DHCP for an address, advertise itself
 * over Bonjour, and respond to HTTP requests. A simple HTTP form is
 * presented, allowing the client to POST text to be displayed on the
 * DMD display.
 *
 * Board: Freetronics EtherTen - ethernet shield on pins 10, 11, 12, 13
 * Display: Freetronics  DMD - on pins FIXME
 *
 * Libraries:
 *   Ethernet (to run the WIZ chipset)
 *   ArduinoEthernet (modules: DHCP, Bonjour)
 *   Webduino (for HTTP service)
 */

/* Headers */

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetDHCP.h>
#include <EthernetBonjour.h>
#include <WebServer.h>

/* Defines */

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAC }; // Note the vendor 0xDEADBEEF :)

#define PREFIX "" // pages will be at root of server
WebServer webserver(PREFIX, 80);

/* Prototypes */

bool runDHCPandBonjour(void);
void web_index(WebServer &server, WebServer::ConnectionType type, char *, bool);

/* Code! */

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting up!");

  EthernetDHCP.begin(mac, 1); // Polling DHCP mode
  EthernetBonjour.begin(); // we'll set the hostname in the DHCP lease code below

  webserver.setDefaultCommand(&web_index);
  webserver.addCommand("index.html", &web_index);
  webserver.begin();
}

void loop()
{
  if (runDHCPandBonjour()) {
    char buff[64];
    int len = 64;
    webserver.processConnection(buff, &len);
  }
}

// Utility function to nicely format an IP address.
const char* ip_to_str(const uint8_t* ipAddr)
{
 static char buf[16];
 sprintf(buf, "%d.%d.%d.%d\0", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
 return buf;
}

// Adapted from http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1236679841/45
bool runDHCPandBonjour() {
  static DhcpState prevState = DhcpStateNone;
  static unsigned long prevTime = 0;
  static bool run_bonjour = false;

  if (run_bonjour) EthernetBonjour.run();
  DhcpState state = EthernetDHCP.poll();

  if (prevState != state) {
    Serial.println();

    switch (state) {
      case DhcpStateDiscovering:
        Serial.print("Discovering servers.");
        run_bonjour = false;
        break;
      case DhcpStateRequesting:
        Serial.print("Requesting lease.");
        run_bonjour = false;
        break;
      case DhcpStateRenewing:
        Serial.print("Renewing lease.");
        run_bonjour = false;
        break;
      case DhcpStateLeased: {
        Serial.println("Obtained lease!");

        // Since we're here, it means that we now have a DHCP lease, so we
        // print out some information.
        const byte* ipAddr = EthernetDHCP.ipAddress();
        const byte* gatewayAddr = EthernetDHCP.gatewayIpAddress();
        const byte* dnsAddr = EthernetDHCP.dnsIpAddress();

        Serial.print("My IP address is ");
        Serial.println(ip_to_str(ipAddr));

        Serial.print("Gateway IP address is ");
        Serial.println(ip_to_str(gatewayAddr));

        Serial.print("DNS IP address is ");
        Serial.println(ip_to_str(dnsAddr));

        delay(2000);

        Serial.print("hostName is ");
        Serial.println(EthernetDHCP.hostName());
        EthernetBonjour.setBonjourName(EthernetDHCP.hostName());
        run_bonjour = true;
        break;
      }
    }
  } else if (state != DhcpStateLeased && millis() - prevTime > 300) {
     prevTime = millis();
     Serial.print('.');
  }

  prevState = state;
  return (state == DhcpStateLeased);
}

void web_index(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  server.httpSuccess(); // 200 OK

  if (type != WebServer::HEAD) // Send a body unless the HTTP verb is HEAD
  {
    P(helloMsg) = "<h1>Hello, World!</h1>";
    server.printP(helloMsg);
  }
}
