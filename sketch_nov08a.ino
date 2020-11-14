#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET 4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char *ssid = "ASUS";
const char *password = "1234567890";

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

String ramBuffer;
String tempBuffer;

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void loop()
{
  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay)
  {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED)
    {
      String getRam = "http://192.168.1.232:5000/api/system/ram";
      String getTemp = "http://192.168.1.232:5000/api/system/temp";

      ramBuffer = httpGETRequest(getRam.c_str());
      JSONVar ramObject = JSON.parse(ramBuffer);

      tempBuffer = httpGETRequest(getTemp.c_str());
      JSONVar tempObject = JSON.parse(tempBuffer);

      if (JSON.typeof(ramObject) == "undefined")
      {
        Serial.println("Parsing input failed!");
        return;
      }

      int total = ramObject["data"]["total"];
      int freee = ramObject["data"]["free"];
      int used = ramObject["data"]["used"];

      int main = tempObject["data"]["main"];
      int cores = tempObject["data"]["cores"][];
      int maxx = tempObject["data"]["max"];

      Serial.println("Total RAM : ");
      Serial.println(total);
      Serial.println("Free RAM : ");
      Serial.println(freee);
      Serial.println("Used RAM : ");
      Serial.println(used);

      Serial.println("Main Temp : ");
      Serial.println(main);
      Serial.println("Cores Temp : ");
      Serial.println(cores);
      Serial.println("Max Temp : ");
      Serial.println(maxx);

      ram();
      delay(3000);
      display.clearDisplay();
      temp();
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

void ram()
{
  String getRam = "http://192.168.1.232:5000/api/system/ram";
  ramBuffer = httpGETRequest(getRam.c_str());
  JSONVar ramObject = JSON.parse(ramBuffer);

  int total = ramObject["data"]["total"];
  int freee = ramObject["data"]["free"];
  int used = ramObject["data"]["used"];

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(String("TOTAL RAM: ") + String(total));
  display.setCursor(0, 10);
  display.println(String("USED RAM: ") + used);
  display.setCursor(0, 20);
  display.println("FREE RAM: " + String(freee));
  display.display();
}
void temp()

{
  String getTemp = "http://192.168.1.232:5000/api/system/temp";
  tempBuffer = httpGETRequest(getTemp.c_str());
  JSONVar tempObject = JSON.parse(tempBuffer);

  int main = tempObject["data"]["main"];
  int cores = tempObject["data"]["cores"][""];
  int maxx = tempObject["data"]["max"];

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(String("MAIN TEMP: ") + String(main));
  display.setCursor(0, 10);
  display.println(String("CORES TEMP: ") + cores);
  display.setCursor(0, 20);
  display.println("MAX TEMP: " + String(maxx));
  display.display();
}
String httpGETRequest(const char *serverName)
{
  HTTPClient http;

  http.begin(serverName);

  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();

  return payload;
}
