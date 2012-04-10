EtherTen with DMD
=================
Aaron Stone, April, 2012

This project has the Arduino DHCP for an address, advertise itself
over Bonjour, and respond to HTTP requests. A simple HTTP form is
presented, allowing the client to POST text to be displayed on the
DMD display.

Hardware
--------
Board: Freetronics EtherTen - ethernet shield on pins 10, 11, 12, 13
Display: Freetronics  DMD - on pins FIXME

Libraries
---------
* Ethernet (to run the WIZ chipset)
* ArduinoEthernet (modules: DHCP, Bonjour)
* Webduino (for HTTP service)
