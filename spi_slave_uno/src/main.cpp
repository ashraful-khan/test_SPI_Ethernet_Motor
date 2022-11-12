//#include "dev_init.h"
#include"motor_driver.h"
#include <EtherCard.h>
#include <SPI.h>
#include <SPI1.h>

#if ETHERNET

    // ethernet interface mac address, must be unique on the LAN
    static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

    byte Ethernet::buffer[700];
    static uint32_t timer;

// called when a ping comes in (replies to it are automatic)
static void gotPinged (byte* ptr) {
  ether.printIp(">>> ping from: ", ptr);
}
#endif

#if NTP
// Ethernet mac address - must be unique on your network
const byte myMac[] PROGMEM = { 0x70, 0x69, 0x69, 0x2D, 0x30, 0x31 };
const char NTP_REMOTEHOST[] PROGMEM = "ntp.bit.nl";  // NTP server name
const unsigned int NTP_REMOTEPORT = 123;             // NTP requests are to port 123
const unsigned int NTP_LOCALPORT = 8888;             // Local UDP port to use
const unsigned int NTP_PACKET_SIZE = 48;             // NTP time stamp is in the first 48 bytes of the message
byte Ethernet::buffer[350];                          // Buffer must be 350 for DHCP to work


// send an NTP request to the time server at the given address
void sendNTPpacket(const uint8_t* remoteAddress) {
  // buffer to hold outgoing packet
  byte packetBuffer[ NTP_PACKET_SIZE];
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;            // Stratum, or type of clock
  packetBuffer[2] = 6;            // Polling Interval
  packetBuffer[3] = 0xEC;         // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  ether.sendUdp(packetBuffer, NTP_PACKET_SIZE, NTP_LOCALPORT, remoteAddress, NTP_REMOTEPORT );
  Serial.println("NTP request sent.");
}

void udpReceiveNtpPacket(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port, const char *packetBuffer, uint16_t len) {
  Serial.println("NTP response received.");

  // the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, extract the two words:
  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;
  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
  // subtract seventy years:
  unsigned long epoch = secsSince1900 - seventyYears;

  // print Unix time:
  Serial.print("Unix time = ");
  Serial.println(epoch);
}


#endif

Motor *motor;

void setup() {
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);

  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);

  #if ETHERNET
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  #endif

  Serial.begin(9600);
  Serial.println("uart init done.");
  #if ETHERNET_PING
  Serial.println(F("\n[EtherCard NTP Client]"));
  //SPI1.begin();
  //SPI1.beginTransaction(SPI1Settings(1000000, MSBFIRST, SPI_MODE0));

  Serial.println("\n[pings]");

  // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  if (ether.begin(sizeof Ethernet::buffer, mymac, 10) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);

#if 1
  // use DNS to locate the IP address we want to ping
  if (!ether.dnsLookup(PSTR("www.facebook.com")))
    Serial.println("DNS failed");
#else
  ether.parseIp(ether.hisip, "74.125.77.99");
#endif
  ether.printIp("SRV: ", ether.hisip);

  // call this to report others pinging us
  ether.registerPingCallback(gotPinged);

  timer = -9999999; // start timing out right away
#endif

#if NTP

  Serial.println(F("\n[EtherCard NTP Client]"));

  // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  if (ether.begin(sizeof Ethernet::buffer, myMac, SS) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

  if (!ether.dnsLookup(NTP_REMOTEHOST))
    Serial.println("DNS failed");

  uint8_t ntpIp[IP_LEN];
  ether.copyIp(ntpIp, ether.hisip);
  ether.printIp("NTP: ", ntpIp);

  ether.udpServerListenOnPort(&udpReceiveNtpPacket, NTP_LOCALPORT);
  Serial.println("Started listening for response.");

  sendNTPpacket(ntpIp);
#endif

  spi1_init();
  Serial.println("SPI init done.");
  //SPI.begin();
  //SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
 

#if MOTOR_DRIVE
  //motor->resetBoard(4);
  //delay(10);
  //motor->resetBoard(5);
  //delay(10);
  motor->resetAllBoard();
  Serial.println("Board reset done.");

  //Check if the boards are available or not. If not available wait here unitl available.
  //This is a blocking code. Need to find a better way to work on this.
  while(!(motor->Tle_Board->sendMessageAndCheckAnswer(4,0x0901))){
    Serial.println("Board 1 not Available.");
    delay(2000);
  }
  Serial.println("Board 1 Initialised.");
  delay(100);

  while(!(motor->Tle_Board->sendMessageAndCheckAnswer(5,0x0901))){
    Serial.println("Board 2 not Available.");
    delay(2000);
  }
  Serial.println("Board 2 Initialised.");
  delay(100);

  motor->setOperationMode(FOC, 4);
  motor->setOperationMode(FOC, 5);
  delay(10);
  motor->setMotorSpeed(3000, 4);
  motor->setMotorSpeed(3000, 5);
  delay(10);

  motor->Tle_Board->sendMessageAndCheckAnswer(4, 0x0800);
  motor->Tle_Board->sendMessageAndCheckAnswer(5, 0x0800);
  delay(10);
  #endif
}

void loop() {
    Serial.println("Running in Loop");
  //motor->resetAllBoard();
  #if MOTOR_DRIVE
  motor->Tle_Board->sendMessageAndCheckAnswer(4, 0x0800);
  motor->Tle_Board->sendMessageAndCheckAnswer(5, 0x0800);
  delay(5000);
  motor->Tle_Board->sendMessageAndCheckAnswer(4, 0x0801);
  motor->Tle_Board->sendMessageAndCheckAnswer(5, 0x0801);
  delay(5000);
  #endif

  #if NTP
    // this must be called for ethercard functions to work.
  ether.packetLoop(ether.packetReceive());
  #endif

  #if ETHERNET_PING
  // this must be called for ethercard functions to work.
  word len = ether.packetReceive(); // go receive new packets
  word pos = ether.packetLoop(len); // respond to incoming pings

  // report whenever a reply to our outgoing ping comes back
  if (len > 0 && ether.packetLoopIcmpCheckReply(ether.hisip)) {
    Serial.print("  ");
    Serial.print((micros() - timer) * 0.001, 3);
    Serial.println(" ms");
  }

  // ping a remote server once every few seconds
  if (micros() - timer >= 5000000) {
    ether.printIp("Pinging: ", ether.hisip);
    timer = micros();
    ether.clientIcmpRequest(ether.hisip);
  }
Serial.println("====================================================");
//readAll();
#endif
}