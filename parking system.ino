#include <WiFi.h>
#include <HTTPClient.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =================================================
// WIFI DETAILS
// =================================================
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// =================================================
// BACKEND URL
// REPLACE WITH YOUR LAPTOP IP
// =================================================
String serverName =
"http://192.168.1.5:5000/api/parking/hardware-update";

// =================================================
// OLED CONFIG
// =================================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// =================================================
// SLOT 1 SENSOR
// =================================================
#define TRIG1 23
#define ECHO1 19

// =================================================
// SLOT 2 SENSOR
// =================================================
#define TRIG2 25
#define ECHO2 33

// =================================================
// SLOT 1 LEDs
// =================================================
#define GREEN1 5
#define RED1 18

// =================================================
// SLOT 2 LEDs
// =================================================
#define GREEN2 14
#define RED2 12

// =================================================
// BUZZER
// =================================================
#define BUZZER 4

// =================================================
// PARKING LIMIT
// =================================================
#define LIMIT 10

// =================================================
// VARIABLES
// =================================================
float distance1 = 0;
float distance2 = 0;

bool slot1Occupied = false;
bool slot2Occupied = false;

bool buzzerActive = false;

unsigned long lastOLEDUpdate = 0;
unsigned long lastServerUpdate = 0;

// =================================================
// FUNCTION DECLARATION
// =================================================
float readDistance(int trigPin, int echoPin);

// =================================================
// SETUP
// =================================================
void setup() {

  Serial.begin(115200);

  // SENSOR PINS
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);

  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  // LED PINS
  pinMode(GREEN1, OUTPUT);
  pinMode(RED1, OUTPUT);

  pinMode(GREEN2, OUTPUT);
  pinMode(RED2, OUTPUT);

  // BUZZER
  pinMode(BUZZER, OUTPUT);

  // OLED
  Wire.begin(21, 22);

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C
      )) {

    Serial.println("OLED FAILED");

    while (true);
  }

  // =================================================
  // CONNECT WIFI
  // =================================================
  Serial.println();
  Serial.println("Connecting WiFi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected!");

  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // =================================================
  // START SCREEN
  // =================================================
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);

  display.setCursor(10, 15);
  display.println("SMART");

  display.setCursor(10, 40);
  display.println("PARKING");

  display.display();

  delay(2000);

  display.clearDisplay();
  display.display();
}

// =================================================
// LOOP
// =================================================
void loop() {

  // =============================================
  // READ SENSOR VALUES
  // =============================================
  distance1 = readDistance(TRIG1, ECHO1);

  distance2 = readDistance(TRIG2, ECHO2);

  // =============================================
  // SLOT 1 STATUS
  // =============================================
  if (distance1 > 0 &&
      distance1 <= LIMIT) {

    slot1Occupied = true;

    digitalWrite(RED1, HIGH);
    digitalWrite(GREEN1, LOW);

  } else {

    slot1Occupied = false;

    digitalWrite(RED1, LOW);
    digitalWrite(GREEN1, HIGH);
  }

  // =============================================
  // SLOT 2 STATUS
  // =============================================
  if (distance2 > 0 &&
      distance2 <= LIMIT) {

    slot2Occupied = true;

    digitalWrite(RED2, HIGH);
    digitalWrite(GREEN2, LOW);

  } else {

    slot2Occupied = false;

    digitalWrite(RED2, LOW);
    digitalWrite(GREEN2, HIGH);
  }

  // =============================================
  // BUZZER
  // =============================================
  if ((slot1Occupied ||
       slot2Occupied) &&
      !buzzerActive) {

    buzzerActive = true;

    tone(BUZZER, 1000);
    delay(200);

    noTone(BUZZER);

    delay(200);

    tone(BUZZER, 1000);
    delay(200);

    noTone(BUZZER);
  }

  if (!slot1Occupied &&
      !slot2Occupied) {

    buzzerActive = false;
  }

  // =============================================
  // AVAILABLE SLOT LOGIC
  // =============================================
  String recommendedSlot = "NONE";

  if (!slot1Occupied) {

    recommendedSlot = "SLOT 1";

  } else if (!slot2Occupied) {

    recommendedSlot = "SLOT 2";
  }

  // =============================================
  // SERIAL MONITOR
  // =============================================
  Serial.println("========== SMART PARKING ==========");

  Serial.print("Slot 1: ");

  if (slot1Occupied) {

    Serial.println("FULL");

  } else {

    Serial.println("OPEN");
  }

  Serial.print("Slot 2: ");

  if (slot2Occupied) {

    Serial.println("FULL");

  } else {

    Serial.println("OPEN");
  }

  Serial.print("Recommended: ");
  Serial.println(recommendedSlot);

  Serial.println("===================================");
  Serial.println();

  // =============================================
  // SEND DATA TO WEBSITE
  // EVERY 5 SECONDS
  // =============================================
  if (millis() - lastServerUpdate > 5000) {

    lastServerUpdate = millis();

    if (WiFi.status() == WL_CONNECTED) {

      HTTPClient http;

      http.begin(serverName);

      http.addHeader(
        "Content-Type",
        "application/json"
      );

      String jsonData = "{";

      jsonData += "\"slot1\":";
      jsonData +=
      (slot1Occupied ? "true" : "false");

      jsonData += ",";

      jsonData += "\"slot2\":";
      jsonData +=
      (slot2Occupied ? "true" : "false");

      jsonData += ",";

      jsonData += "\"recommendedSlot\":\"";
      jsonData += recommendedSlot;
      jsonData += "\"";

      jsonData += "}";

      int httpResponseCode =
      http.POST(jsonData);

      Serial.print("HTTP Response: ");

      Serial.println(httpResponseCode);

      http.end();
    }
  }

  // =============================================
  // OLED DISPLAY
  // =============================================
  if (millis() - lastOLEDUpdate >= 2000) {

    lastOLEDUpdate = millis();

    display.clearDisplay();

    display.setTextColor(WHITE);

    // SLOT 1
    display.setTextSize(2);

    display.setCursor(0, 0);

    display.print("S1:");

    if (slot1Occupied) {

      display.print("FULL");

    } else {

      display.print("OPEN");
    }

    // SLOT 2
    display.setCursor(0, 25);

    display.print("S2:");

    if (slot2Occupied) {

      display.print("FULL");

    } else {

      display.print("OPEN");
    }

    // RECOMMENDED SLOT
    display.setTextSize(1);

    display.setCursor(0, 55);

    display.print("PARK: ");

    display.print(recommendedSlot);

    display.display();
  }

  delay(500);
}

// =================================================
// DISTANCE FUNCTION
// =================================================
float readDistance(
  int trigPin,
  int echoPin
) {

  long duration;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  duration = pulseIn(
    echoPin,
    HIGH,
    30000
  );

  if (duration == 0) {

    return 999;
  }

  float distance =
  duration * 0.034 / 2;

  return distance;
}