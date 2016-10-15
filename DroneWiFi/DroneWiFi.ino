#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Servo.h>

Servo aileron;
Servo elevator;

WiFiUDP Udp;

char ssid[] = "WestNet";
char pass[] = "";

int status = WL_IDLE_STATUS;

unsigned int localPort = 9446;

char packetBuffer[255];
char replyBuffer[] = "";

void setup() {
  Serial.begin(9600);

  aileron.attach(2);
  elevator.attach(3);

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if ( fv != "1.1.0" ){
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    
    //status = WiFi.begin(ssid, pass);
    status = WiFi.begin(ssid, pass);
    
    delay(2000); // wait for connection
  }
  
  printWifiStatus();

  Udp.begin(9446);

  pinMode(13, OUTPUT);
}

void loop() {
  
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    String data = String(packetBuffer);

    int firstCommaIndex = data.indexOf(',');
    int aileronVal = data.substring(0, firstCommaIndex).toInt();

    int secondCommaIndex = data.indexOf(',', firstCommaIndex + 1);
    int elevatorVal = data.substring(firstCommaIndex + 1, secondCommaIndex).toInt();

    Serial.println("Aileron: " + String(aileronVal));
    Serial.println("Elevator: " + String(elevatorVal));
    Serial.println();

    aileron.write(aileronVal);
    elevator.write(elevatorVal);
    
    // send a reply, to the IP address and port that sent us the packet we received
    data.toCharArray(replyBuffer,data.length);
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(replyBuffer);
    Udp.endPacket();
  }
}

void printWifiStatus() {
  // Print the SSID of the network:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Print IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // Print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
