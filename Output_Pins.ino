#include <SPI.h>
#include <Ethernet.h>

//  MAC Address
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//  IP Adress
byte ip[] = { 192, 168, 10, 30 };
EthernetServer server(80);

int numPins = 5;
int pins[] = {3, 4, 5, 6, 7};
int pinState[] = {0, 0, 0, 0, 0};
char line1[100];

void setup() {
  for (int i = 0; i < numPins; i++)
  {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], 1);
    delay(2000);
    digitalWrite(pins[i], 0);
  }
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();

  delay(1000);
}

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client)
  {
    while (client.connected())
    {
      readHeader(client);
      if (! pageNameIs("/")) {
        client.stop();
        Serial.println("Bombing out of readHeader");
        return;
      }
      //  send a standard http response header
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type:  text/html");
      client.println();

      //send the body
      client.println("<html><body>");
      client.println("<h1>Output Pins</h1>");
      client.println("<form method='GET'>");
      setValuesFromParams();
      setPinStates();
      for (int i = 0; i < numPins; i++) {
        writeHTMLforPin(client, i);
      }
      client.println("<input type='submit' value='Update'/>");
      client.println("</form>");
      client.println("</body></html>");

      client.stop();
    }
  }
}

void readHeader(EthernetClient client) {
  // read first line of header
  Serial.println("readHeader Function");
  char ch;
  int i = 0;
  while (ch != '\n') {
    Serial.print("ReadHeader Function 'char ch' vaulue:  ");
    Serial.println(ch);
    if (client.available()) {
      ch = client.read();
      line1[i] = ch;
      i ++;
    }
  }
  line1[i] = '\0';
  Serial.print("ReadHeader Function:  ");
  Serial.println(line1);
}

boolean pageNameIs(char* name) {
  // page name starts at char pos 4
  // ends with space
  int i = 4;
  char ch = line1[i];
  while (ch != ' ' && ch != '\n' && ch != '?') {
    if (name[i-4] != line1[i]) {
      return false;
    }
    i++;
    ch = line1[i];
  }
  return true;
}
void writeHTMLforPin(EthernetClient client, int i) {
  client.print("<p>Pin ");
  client.print(pins[i]);
  client.print("<select name='");
  client.print(i);
  client.println("'>");
  client.print("<option value='0'");
  if (pinState[i] == 0) {
    client.print(" selected");
  }
  client.println(">Off</option>");
  client.print("<option value='1'");
  if (pinState[i] == 1) {
    client.print(" selected");
  }
  client.println(">On</option>");
  client.println("</select></p>");
}

void setPinStates() {
  Serial.println("setPinStates Function");
  for (int i = 0; i < numPins; i++) {
    digitalWrite(pins[i], pinState[i]);
    Serial.print("Pin " + String(pins[i]));
    Serial.println("   State " + String(pinState[i]));
  }
  Serial.println("********************");
}

void setValuesFromParams() {
  Serial.println("setValuesFromParams Function");
  for (int i = 0; i < numPins; i++) {
    pinState[i] = valueOfParam(i + '0');
    Serial.print("pinState" + String(i));
    Serial.println("  " + String(pinState[i]));
  }
}



 int valueOfParam (char param) {
  for (int i = 0; i < strlen(line1); i++) {
    Serial.print("ValueOfParam function:  ");
    Serial.println(line1);
    if (line1[i] == param && line1[i+1] == '=') {
      return (line1[i+2] - '0');
    }
  }
  return 0;
 }
