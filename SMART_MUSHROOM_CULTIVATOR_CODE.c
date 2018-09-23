#include <ESP8266WiFi.h>  
#include <WiFiClient.h> 
#include <ESP8266WebServer.h> 
#include <ThingSpeak.h> 
#include <DHT.h>
#include <Wire.h>  
#include "SSD1306.h" 
#define DHTPIN 5     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
int relay1=13;
int relay2=15;
// Initialize DHT sensor for normal 16mhz Arduino 
const char* ssid = "name of your hotspot";  
const char* password = "password of your hotspot";  
WiFiClient client;  
WiFiServer server(80);
unsigned long myChannelNumber = Channel number;  
const char * myWriteAPIKey = "API KEY";  
uint8_t temperature, humidity;  
// Initialize the OLED display using SPI
// D5 -> CLK
// D7 -> MOSI (DOUT)
// D0 -> RES
// D2 -> DC
// D8 -> CS
// SSD1306Spi        display(D0, D2, D8);
// or
// SH1106Spi         display(D0, D2);
// Initialize the OLED display using brzo_i2c
// D3 -> SDA
// D5 -> SCL
// SSD1306Brzo display(0x3c, D3, D5);
// or
// SH1106Brzo  display(0x3c, D3, D5);
// Initialize the OLED display using Wire library
SSD1306  display(0x3c, D3, D5);
void setup(){
  Serial.begin(115200);  
  dht.begin();   // initialize dht
  delay(10);  \
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  // Connect to WiFi network  
  Serial.println();  
  Serial.println();  
  Serial.print("Connecting to ");  
  Serial.println(ssid);  
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED)  
  {  
   delay(500);  
   Serial.print(".");  
  }  
  Serial.println("");  
  Serial.println("WiFi connected");  
   /*-------- server started---------*/ 
  server.begin();
  Serial.println("Server started");
  // Print the IP address  
  Serial.println(WiFi.localIP());  
  ThingSpeak.begin(client);  
  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
}

void displayTempHumid(){
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) ){
    display.clear(); // clearing the display
    display.drawString(5,0, "Failed DHT");
    return;
  }
  display.clear();
  display.drawString(0, 0, "Humidity: " + String(h) + "%\t"); 
  display.drawString(0, 16, "Temp: " + String(t) + "C"); 
  
}
void loop(){
  displayTempHumid();
  display.display();
  delay(2000);
  static boolean data_state = false;  
  temperature = dht.readTemperature();  
  humidity = dht.readHumidity();  
  Serial.print("Temperature Value is :");  
  Serial.print(temperature);  
  Serial.println("C");  
  Serial.print("Humidity Value is :");  
  Serial.print(humidity);  
  Serial.println("%");  
  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different  
  // pieces of information in a channel. Here, we write to field 1.  
  if( data_state )  
  {  
   ThingSpeak.writeField(myChannelNumber, 1, temperature, myWriteAPIKey);  
   data_state = false;  
  }  
  else  
  {  
   ThingSpeak.writeField(myChannelNumber, 2, humidity, myWriteAPIKey);  
   data_state = true;  
  }  
  delay(30000); // ThingSpeak will only accept updates every 15 seconds. 
  //relay1 and relay2 access from wifi
  WiFiClient client = server.available();    
    if (!client) 
    {
      return;
    }
  Serial.println("Waiting for new client");   
  while(!client.available())
  {
    delay(1);
  }
 
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
 
  int value = LOW;
  if (request.indexOf("/relay1=ON")||request.indexOf("/relay2=ON") != -1)  
  {
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2,HIGH);
    value = HIGH;
  }
  if (request.indexOf("/relay1=OFF")||request.indexOf("/relay2=OFF") != -1)  
  {
    digitalWrite(relay1, LOW);
    digitalWrite(relay2,LOW);
    value = LOW;
    
  }
 
 
/*------------------Creating html page---------------------*/

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); 
  client.println("");
  client.println("");
  client.print("relay1 is: ");
  client.print("relay2 is:");
  if(value == HIGH) 
  {
    client.print(" relay1 ON");
    client.print("relay2 ON");
  } 
  else 
  {
    client.print("relay1 OFF");
    client.print("relay2 OFF");
  }
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
  
  
}

