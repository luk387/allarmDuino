// Demo using DHCP and DNS to perform a web client request.
// 2011-06-08 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <EtherCard.h>

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = {0x74,0x69,0x69,0x2D,0x30,0x31};
static byte myip[] = { 10,0,0,200 };
static byte gwip[] = { 10,0,0,1 };
static byte dns[] = { 8,8,8,8 };
static byte sn[] = { 255,255,255,0 };
int ATTIVATO=19;
int POWER=A4;
int count=0;

byte Ethernet::buffer[700];
static uint32_t timer;
static uint32_t timer2;

const char website[] PROGMEM = "lucantonello.no-ip.biz";

// called when the client request is complete
static void my_callback (byte status, word off, word len) {
  Serial.println(">>>");
  Ethernet::buffer[off+300] = 0;
  Serial.print((const char*) Ethernet::buffer + off);
  Serial.println("...");
  pinMode(ATTIVATO, INPUT);
  pinMode(POWER, INPUT);
}

void setup () {
  Serial.begin(57600);
  Serial.println(F("\n[webClient]"));

/*  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));*/
  if (!ether.begin(sizeof Ethernet::buffer, mymac, 8)) {
    Serial.println( "Failed to access Ethernet controller");
  } else Serial.println("Ethernet controller initialized");
  Serial.println();
 
  ether.staticSetup(myip, gwip, dns, sn); 

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  

  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
    
  ether.printIp("SRV: ", ether.hisip);
}

void loop () {
  int avviso=digitalRead(ATTIVATO); 
  int pwr=analogRead(POWER);
  if(pwr<500 || avviso==1){
    Serial.println("power");
    Serial.println(pwr);
    ether.packetLoop(ether.packetReceive());
    if (millis() > timer && pwr<500) {
      timer = millis() + 5000;
      Serial.println();
      Serial.print("<<< REQ ");
      ether.browseUrl(PSTR("/secure/allarmmail.php?allarm=pwr"), "", website, my_callback);
    }
    if (millis() > timer && avviso==1) {
      timer = millis() + 5000;
      Serial.println();
      Serial.print("<<< REQ ");
      ether.browseUrl(PSTR("/secure/allarmmail.php?allarm=start"), "", website, my_callback);
    }
    pwr=analogRead(POWER);
    avviso=digitalRead(ATTIVATO);
  }
 
}
