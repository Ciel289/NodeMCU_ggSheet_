// Uncomment if use ESP 32
//#include <WiFi.h> 
//#include <HTTPClient.h>

// Uncomment if use ESP 8266
//#include <ESP8266WiFi.h>


char ssid[]         = ""; // Wifi's name (2.4GHz)
char pass[]         = ""; // Wifi's password
String GAS_ID       = ""; // spreadsheet script ID
const char* host    = "script.google.com"; // only google.com not https://google.com
const int httpPort  = 443; // 80 is for HTTP / 443 is for HTTPS!

int v1, v2;
double v3;

String setDataa(String key, String values) {  //  setDataa("v1", "Hello");
  return (key + '=' + values + '&');          //  "v1=Hello&"
}
String setDataa(String key, int values) { 
  return (key + '=' + String(values) + '&');
}
String setDataa(String key, double values) {
  return (key + '=' + String(values,2) + '&');
}

void update_google_sheet(String data) {
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;
  client.setInsecure(); // this is the magical line that makes everything work

  if (!client.connect(host, httpPort)) { //works!
    Serial.println("connection failed");
    return;
  }

  String url = "/macros/s/" + GAS_ID + "/exec?" + data;
  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  Serial.println("closing connection");
}


void setup() {
  Serial.begin(115200);

  Serial.print("Connecting to " + String(ssid));
  WiFi.begin(ssid, pass); // Connect to your WiFi router
  byte i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    i++;
    if (i >= 20 ) { // 10 Sec
      Serial.println("ESP Restart!");
      ESP.restart();  // Restart
    }
    delay(500);
  }

  update_google_sheet("v1=Start!"); // If can connect to wifi
  Serial.println("Start!");
  Serial.println("");
}

unsigned long t0, t1;
unsigned long time_data = 1000;
unsigned long time_update = 1000 * 30; // 1000 millisec * 30 = 30seccond

String values = "";

void loop() {
  if ( millis() - t0 >= time_data ) {
    values = "";
    t0 = millis();
    v1 = random(0, 260);
    v2 = random(0, 260);
    v3 = (double)random(0, 26000) / 100.0;
    
    Serial.print("V1:" + String(v1));
    Serial.print(" V2:" + String(v2));
    Serial.println(" V3:" + String(v3));
    values = setDataa("v1", v1) + setDataa("v2", v2) + setDataa("v3", v3);  // Set values to string
  }

  if ( millis() - t1 >= time_update) {
    t1 = millis();
    update_google_sheet(values);  // Send values to google sheet
  }
}
