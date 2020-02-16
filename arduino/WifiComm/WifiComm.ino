#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <string.h>

const char* ssid = "Cirke";
const char* password = "kerdojel?";

WiFiUDP Udp;
//unsigned int localUdpPort = 4210;

const unsigned int ArudinoToPcPort = 42100;
const unsigned int PcToArduinoPort = 42110;

char incomingPacket[256];
char replyPacket[] = "Hi there! Got the message :-)";

void setup()
{
	Serial.begin(115200);
	Serial.println();

	WiFi.begin(ssid, password);

	Serial.print("Connecting");
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println();

	Serial.print("Connected, IP address: ");
	Serial.println(WiFi.localIP());

	Udp.begin(PcToArduinoPort);
}

void HandleIcomingPacket() {

	const int packetSize = Udp.parsePacket();
	if (packetSize)
	{
		Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
		int len = Udp.read(incomingPacket, 255);
		if (len > 0)
		{
			incomingPacket[len] = '\0';
		}
		Serial.printf("UDP packet contents: %s\n", incomingPacket);

		if(strcmp(incomingPacket, "query") == 0) {
			strcpy(replyPacket, "{\"succeeded\": true}");
		} else {
			strcpy(replyPacket, "{\"succeeded\": false}");
		}


		if(!Udp.beginPacket(Udp.remoteIP(), ArudinoToPcPort)) {
			Serial.print("beginPacket error");
			return;
		}
		
		
		const size_t bytesWritten = Udp.write(replyPacket);
		Serial.printf("Bytes written: %d", (int)bytesWritten);
		
		if(!Udp.endPacket()) {
			Serial.print("endPacket error");
		}
	}
}

void loop()
{
	// put your main code here, to run repeatedly:
	HandleIcomingPacket();
}
