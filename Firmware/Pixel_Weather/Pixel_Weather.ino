
// Libraries
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>.
#include <NTPClient.h>
#include <WiFiUdp.h>


// PIN Definitions
#define LEDPIN 10
#define BUTTONPIN 4
#define BUTTONVINPIN 5
#define LIGHTSENSORPIN A0


// Brightnesses
#define BRIGHTNESS_HIGH 185
#define BRIGHTNESS_LOW 2


// Light sensor threshold
#define BRIGHTHRESHOLD 450



// >>>>> Color Definitions

// black
#define rBLACK 0
#define gBLACK 0
#define bBLACK 0

// sun
#define rSUN 255
#define gSUN 120
#define bSUN 0

// cloud1
#define rCLOUD1 255
#define gCLOUD1 255
#define bCLOUD1 255

// cloud2
#define rCLOUD2 5
#define gCLOUD2 33
#define bCLOUD2 100

// rain
#define rRAIN 0
#define gRAIN 176
#define bRAIN 255

// Current Temp
#define rCURRENT 255
#define gCURRENT 0
#define bCURRENT 200

// Low Temp
#define rLOW 0
#define gLOW 0
#define bLOW 255

// High Temp
#define rHIGH 255
#define gHIGH 0
#define bHIGH 0

// >>>>>>>>>>>>>>>>>>>>



// Weather/Forecast Location (Latitude & Longitude)
const String lat = "47.664983";
const String lon = "-122.315186";

// OpenWeather API Key
const String api_key = "d6e752ebc6f043373fd3e61671fa3430";

// WiFi Connection
const char* ssid = "ItsNotATownhouseItsATownhome";
const char* password = "BeanFanClub";

// NTP Parameters
const long utcOffsetSeconds = -25200;
const long updateInterval = 3600000;   // update every hour



// Initialize LED matrix
Adafruit_NeoPixel matrix(256, LEDPIN, NEO_GRB + NEO_KHZ800);

// Initialize WiFi and NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetSeconds, updateInterval);



// Graphics
// 0 = black
// 1 = sun
// 2 = cloud1
// 3 = cloud2
// 4 = rain
int icons[][80] = {
{0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,1,0,1,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0,1,0,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,2,2,0,0,1,0,0,0,0,0,0,2,2,2,2,2,2,2,2,1,1,0,1,0,0,1,1,1,2,2,2,2,2,2,2,1,1,0,0,1,0,1,1,2,2,2,2,2,2,2,2,2,0,0,0,0,0,1,0,2,2,2,0,0,0,0,0,0,0,0,0},
{0,0,0,2,2,0,0,0,0,0,2,2,2,2,0,0,0,2,2,2,2,2,2,0,0,2,2,2,2,2,2,0,0,2,2,2,2,2,2,0,0,2,2,2,2,2,0,0,0,0,2,2,2,2,2,0,0,0,2,2,2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,2,2,0,0,0,0,0,2,2,2,2,0,0,0,2,2,2,2,2,2,0,0,2,2,2,2,2,2,0,3,2,2,2,2,2,2,0,0,2,2,2,2,2,3,3,3,3,2,2,2,2,2,0,0,0,2,2,2,3,3,0,0,3,3,3,2,3,0,0,0,0,0,3,3,3,0,0},
{0,0,0,2,0,0,0,0,0,0,0,2,2,2,0,0,0,2,2,2,2,2,0,0,0,0,0,2,2,2,2,2,2,2,2,2,4,4,0,0,4,4,0,0,2,2,2,2,0,2,2,2,4,4,0,0,4,4,0,0,2,2,2,0,0,0,2,2,4,4,0,0,4,4,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,2,2,2,2,4,0,0,4,0,0,2,2,2,2,1,1,1,2,2,2,0,0,0,0,0,4,2,2,2,2,1,2,2,2,2,2,0,0,4,0,0,0,2,2,2,2,2,0,2,2,2,0,4,0,0,4,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,0,0,0,0,0,1,1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,2,0,2,0,2,0,2,0,0,0,0,0,2,2,2,0,0,2,2,2,0,2,2,2,0,0,0,0,2,2,2,0,0,0,2,0,2,0,2,0,0,0,2,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,2,0,2,0,0,2,0,0,2,2,0,0,2,0,0,2,0,2,0,0,2,0,0,2,0,0,2,0,0,2,0,0,2,2,0,0,2,0,0,2,0,2,0,0,2,0,0,2,0,0,2,0,0,2,0,0,2,0,2,0,0,2,0,0,2}};
// Digits
int digits_up[][24] = {
{0,0,1,1,1,1,1,0,0,1,0,0,0,1,0,0,0,0,1,1,1,1,1,0},
{0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,1,1,1,0},
{0,0,1,1,1,0,1,0,0,1,0,1,0,1,0,0,0,0,1,0,1,1,1,0},
{0,0,1,0,1,0,1,0,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,0},
{0,0,0,0,1,1,1,0,0,0,0,1,0,0,0,0,0,0,1,1,1,1,1,0},
{0,0,1,0,1,1,1,0,0,1,0,1,0,1,0,0,0,0,1,1,1,0,1,0},
{0,0,1,1,1,1,1,0,0,1,0,1,0,1,0,0,0,0,1,1,1,0,1,0},
{0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,1,1,1,1,0},
{0,0,1,1,1,1,1,0,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,0},
{0,0,0,0,1,1,1,0,0,1,0,1,0,0,0,0,0,0,1,1,1,1,1,0}};
int digits_down[][24] = {
{0,1,1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,1,1,1,1,1,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,0,0},
{0,1,0,1,1,1,0,0,0,0,1,0,1,0,1,0,0,1,1,1,0,1,0,0},
{0,1,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0,1,1,1,1,1,0,0},
{0,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,1,1,1,0,0},
{0,1,1,1,0,1,0,0,0,0,1,0,1,0,1,0,0,1,0,1,1,1,0,0},
{0,1,1,1,1,1,0,0,0,0,1,0,1,0,1,0,0,1,0,1,1,1,0,0},
{0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,0,0},
{0,1,1,1,1,1,0,0,0,0,1,0,1,0,1,0,0,1,1,1,1,1,0,0},
{0,1,1,1,0,0,0,0,0,0,0,0,1,0,1,0,0,1,1,1,1,1,0,0}};
int degree[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0};
int f[] = {0,1,1,1,1,1,0,0,0,0,0,0,1,0,1,0,0,1,0,1,0,0,0,0};


// Initialize some variables
int load = 0;
String jsonBuffer;
JSONVar myObject;
int lastEpochTime1;
int lastEpochTime2;
int lastEpochTime3;
long currentEpochTime;
long currentDisplay = 0; // 0 = current temp, 1 = low/high
long elapsedSeconds1 = 0;
long elapsedSeconds2 = 0;
long elapsedSeconds3 = 0;
String serverPath;
bool currentBrightness;


void setup() {

  //Serial.begin(115200);

  // Provide 5V Vin for touch sensor
  pinMode(BUTTONVINPIN, OUTPUT);
  digitalWrite(BUTTONVINPIN, HIGH);

  matrix.begin();
  currentBrightness = brightnessCheck();
  setBrightness(currentBrightness);
 // matrix.setBrightness(BRIGHTNESS);
  matrix.clear();
  matrix.show();

  WiFi.begin(ssid, password);

  // Displays loading animation while waiting for WiFi connection
  while(WiFi.status() != WL_CONNECTED) {
    loadAnimation();
    delay(500);
    matrix.clear();
    matrix.show();
  }

  timeClient.begin();
  timeClient.update();
  lastEpochTime1 = timeClient.getEpochTime();
  lastEpochTime2 = timeClient.getEpochTime();


  // Setup server path for weather API
  serverPath = "http://api.openweathermap.org/data/2.5/onecall?lat=" + lat + "&lon=" + lon + "&appid=" + api_key + "&exclude=minutely,hourly,alerts&units=imperial";


  // Initialize display with current conditions
  myObject = weatherAPICall();
  Serial.println(JSON.stringify(myObject["current"]["weather"][0]["icon"]));
  drawIcon(JSON.stringify(myObject["current"]["weather"][0]["icon"]));
  drawCurrentTemp(myObject["current"]["temp"]);

}



void loop() {

  currentEpochTime = timeClient.getEpochTime();
  elapsedSeconds1 = currentEpochTime - lastEpochTime1;
  elapsedSeconds2 = currentEpochTime - lastEpochTime2;
  elapsedSeconds3 = currentEpochTime - lastEpochTime3;


  // Alternate between displays every 5 seconds
  if(elapsedSeconds1 > 5) {
    if(currentDisplay == 0) {
      drawLowHigh(myObject["daily"][0]["temp"]["min"],myObject["daily"][0]["temp"]["max"]);
      currentDisplay = 1;
    } else {
    drawIcon(JSON.stringify(myObject["current"]["weather"][0]["icon"]));
    drawCurrentTemp(myObject["current"]["temp"]);
    currentDisplay = 0;
    }
    lastEpochTime1 = currentEpochTime;
  }


  // Pull update from API every 10 minutes
  if(elapsedSeconds2 > 600) {

    myObject = weatherAPICall();
    lastEpochTime2 = currentEpochTime;
    timeClient.update();
    
  }


  // Check and adjust brightness every 1 second
  if(elapsedSeconds3 >= 1) {

    bool newBrightness = brightnessCheck();

    if(currentBrightness != newBrightness) {

      setBrightness(newBrightness);
      currentBrightness = newBrightness;
      
    }

    lastEpochTime3 = currentEpochTime;
    
  }

}



// >>>>>>>>>> Helper Functions



// TRUE = high brightness
// FALSE = low brightness
bool brightnessCheck() {

  return TRUE;
  
}



//
void setBrightness(bool bright) {

  if(bright == TRUE) {

    matrix.setBrightness(BRIGHTNESS_HIGH);
    
  } else {

    matrix.setBrightness(BRIGHTNESS_LOW);
    
  }

  matrix.show();
  
}




//
JSONVar weatherAPICall() {

  jsonBuffer = httpGETRequest(serverPath.c_str());
  return JSON.parse(jsonBuffer);

}



//
String tempParse(double temp) {
  temp = round(temp);
  return ((String) temp).substring(0,2);
}



//
int toDigit(String temp, int i) {
  String digit = temp.substring(i, i + 1);
  return digit.toInt();
}



//
String httpGETRequest(const char* serverName) {
  HTTPClient http;
  http.begin(serverName);
  int httpResponseCode = http.GET();
  String payload = "{}"; 
  if (httpResponseCode>0) {
    payload = http.getString();
  }
  else {
  }
  http.end();
  return payload;
}




// >>>>>>>>>> Draw to Matrix Helpers


// 
void drawIcon(String icon) {
  int x = 0;

  
  if (icon == "\"01d\"" || icon == "\"01n\"") x = 0;
  else if (icon == "\"02d\"" || icon == "\"02n\"") x = 1;
  else if (icon == "\"03d\"" || icon == "\"03n\"") x = 2;
  else if (icon == "\"04d\"" || icon == "\"04n\"") x = 3;
  else if (icon == "\"09d\"" || icon == "\"09n\"") x = 4;
  else if (icon == "\"10d\"" || icon == "\"10n\"") x = 5;
  else if (icon == "\"11d\"" || icon == "\"11n\"") x = 6;
  else if (icon == "\"13d\"" || icon == "\"13n\"") x = 7;
  else if (icon == "\"50d\"" || icon == "\"50n\"") x = 8;


  int pixel = 16;
  for(int i = 0; i < 80; i++) {
    if(icons[x][i] == 0) matrix.setPixelColor(pixel, matrix.Color(rBLACK,gBLACK,bBLACK));
    else if(icons[x][i] == 1)matrix.setPixelColor(pixel, matrix.Color(rSUN,gSUN,bSUN));
    else if(icons[x][i] == 2)matrix.setPixelColor(pixel, matrix.Color(rCLOUD1,gCLOUD1,bCLOUD1));
    else if(icons[x][i] == 3)matrix.setPixelColor(pixel, matrix.Color(rCLOUD2,gCLOUD2,bCLOUD2));
    else if(icons[x][i] == 4)matrix.setPixelColor(pixel, matrix.Color(rRAIN,gRAIN,bRAIN));
    pixel++;
  }
  matrix.show();
}



// 
void drawDigit(int bmp[], int pixel, int r, int g, int b) {
  for(int i = 0; i < 24; i++) {
    if(bmp[i] == 1) {
      matrix.setPixelColor(pixel, matrix.Color(r,g,b));
    } else {
      matrix.setPixelColor(pixel, matrix.Color(0,0,0));
    }
    pixel++;
  }
  matrix.show();
}



//
void drawCurrentTemp(double temp) {
  clearDigits();
  String tempS = tempParse(temp);
  int d1 = toDigit(tempS, 0);
  int d2 = toDigit(tempS, 1);
  drawDigit(digits_up[d1], 120, rCURRENT, gCURRENT, bCURRENT);
  drawDigit(digits_up[d2], 152, rCURRENT, gCURRENT, bCURRENT);
  drawDigit(degree, 176, rCURRENT, gCURRENT, bCURRENT);
  drawDigit(f, 208, rCURRENT, gCURRENT, bCURRENT);
}



//
void drawLowHigh(double low, double high) {
  clearDigits();
  String tempSL = tempParse(low);
  String tempSH = tempParse(high);
  int d1 = toDigit(tempSL, 0);
  int d2 = toDigit(tempSL, 1);
  int d3 = toDigit(tempSH, 0);
  int d4 = toDigit(tempSH, 1);
  drawDigit(digits_down[d1], 112, rLOW, gLOW, bLOW);
  drawDigit(digits_down[d2], 144, rLOW, gLOW, bLOW);
  drawDigit(digits_up[d3], 184, rHIGH, gHIGH, bHIGH);
  drawDigit(digits_up[d4], 216, rHIGH, gHIGH, bHIGH);
}



//
void clearDigits() {
  for(int i = 112; i < 240; i++) {
    matrix.setPixelColor(i, matrix.Color(0, 0, 0));
  }
}



//
void loadAnimation() {
  if(load == 0) {
    matrix.clear();
    matrix.setPixelColor(114, matrix.Color(0, 0, 255));
    matrix.show();
    load ++;
  } else {
    matrix.clear();
    matrix.setPixelColor(141, matrix.Color(0, 0, 255));
    matrix.show();
    load = 0;
  }
}
