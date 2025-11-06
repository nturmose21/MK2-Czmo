
#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// === Servo setup ===
Servo leftServo;
Servo rightServo;
const int leftPin = 6;
const int rightPin = 7;

// === OLED setup ===
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// === Wi-Fi credentials ===
const char* ssid = "WIFI NAME";
const char* password = "PASSWORD";

// === Server ===
AsyncWebServer server(80);
// === Animations ===
#include <Emotions.h>

// === Your animation data (already defined elsewhere) ===
extern const unsigned char* epd_bitmap_allArray[110];
//const int epd_bitmap_allArray_LEN = 110;
// === Car parameters ===
const float CAR_SPEED_CM_PER_SEC = 20.0;

// === State ===
bool started = false;
bool forwardActive = false;
bool backwardActive = false;
bool leftActive = false;
bool rightActive = false;
bool wiggleActive = false;

// === Helper Functions ===
void stopServos() {
  leftServo.write(90);
  rightServo.write(90);
}

void updateCar() {
  // Non-blocking control
  if (forwardActive) { leftServo.write(0); rightServo.write(180); }
  else if (backwardActive) { leftServo.write(180); rightServo.write(0); }
  else if (leftActive) { leftServo.write(180); rightServo.write(180); }
  else if (rightActive) { leftServo.write(0); rightServo.write(0); }
  else if (wiggleActive) {
    // simple wiggle step (could be improved)
    leftServo.write(180); rightServo.write(180);
    delay(50);
    leftServo.write(0); rightServo.write(0);
    delay(50);
		leftServo.write(180); rightServo.write(180);
    delay(50);
    leftServo.write(0); rightServo.write(0);
    delay(50);
  }
  else stopServos();
}

void playAnimation60fps() {
  const int frameDelay = 16;
  for (int i = 0; i < epd_bitmap_allArray_LEN; i++) {
    display.clearDisplay();
    display.drawBitmap(0, 0, epd_bitmap_allArray[i], SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    display.display();
    delay(frameDelay);

    // Allow car control updates during animation
    updateCar();
  }
}

void showMessage(const char* line1, const char* line2 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println(line1);
  if (line2[0] != '\0') display.println(line2);
  display.display();
}

// === Setup ===
void setup() {
  Serial.begin(115200);

  leftServo.attach(leftPin);
  rightServo.attach(rightPin);
  stopServos();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { for(;;); }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  WiFi.begin(ssid, password);

  // === Web UI ===
 server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<style>";
    html += "body { font-family: sans-serif; text-align: center; margin:0; padding:0; background:#111; color:white; }";
    html += ".btn { display: block; width: 80%; max-width: 300px; height: 60px; margin: 10px auto; font-size: 24px; background: #0a74da; color: white; line-height: 60px; user-select:none; -webkit-user-select:none; border-radius:15px; }";
    html += ".btn:active { background: #005bb5; }";
    html += "</style></head><body>";
    
    html += "<h2>ESP32 Car Control</h2>";
    html += "<p>Press Start to enable controls:</p>";
    html += "<div class='btn' onclick='fetch(\"/start\")'>Start</div>";

    html += "<p>Hold buttons to drive:</p>";

    // Mobile-friendly hold buttons, stacked vertically
    html += "<div class='btn' ontouchstart='fetch(\"/backward/on\")' ontouchend='fetch(\"/backward/off\")'>Forward</div>";
    html += "<div class='btn' ontouchstart='fetch(\"/forward/on\")' ontouchend='fetch(\"/forward/off\")'>Backward</div>";
    html += "<div class='btn' ontouchstart='fetch(\"/left/on\")' ontouchend='fetch(\"/left/off\")'>Left</div>";
    html += "<div class='btn' ontouchstart='fetch(\"/right/on\")' ontouchend='fetch(\"/right/off\")'>Right</div>";
    html += "<div class='btn' ontouchstart='fetch(\"/wiggle/on\")' ontouchend='fetch(\"/wiggle/off\")'>Wiggle</div>";

    html += "</body></html>";
    request->send(200, "text/html", html);
});


  // Start button
  server.on("/start", HTTP_GET, [](AsyncWebServerRequest *request){
    started = true;
    request->send(200, "text/plain", "Started!");
  });

  // Control routes
  server.on("/forward/on", HTTP_GET, [](AsyncWebServerRequest *request){ forwardActive = true; request->send(200,"text/plain","ok");});
  server.on("/forward/off", HTTP_GET, [](AsyncWebServerRequest *request){ forwardActive = false; request->send(200,"text/plain","ok");});
  server.on("/backward/on", HTTP_GET, [](AsyncWebServerRequest *request){ backwardActive = true; request->send(200,"text/plain","ok");});
  server.on("/backward/off", HTTP_GET, [](AsyncWebServerRequest *request){ backwardActive = false; request->send(200,"text/plain","ok");});
  server.on("/left/on", HTTP_GET, [](AsyncWebServerRequest *request){ leftActive = true; request->send(200,"text/plain","ok");});
  server.on("/left/off", HTTP_GET, [](AsyncWebServerRequest *request){ leftActive = false; request->send(200,"text/plain","ok");});
  server.on("/right/on", HTTP_GET, [](AsyncWebServerRequest *request){ rightActive = true; request->send(200,"text/plain","ok");});
  server.on("/right/off", HTTP_GET, [](AsyncWebServerRequest *request){ rightActive = false; request->send(200,"text/plain","ok");});
  server.on("/wiggle/on", HTTP_GET, [](AsyncWebServerRequest *request){ wiggleActive = true; request->send(200,"text/plain","ok");});
  server.on("/wiggle/off", HTTP_GET, [](AsyncWebServerRequest *request){ wiggleActive = false; request->send(200,"text/plain","ok");});

  server.begin();
}

// === Loop ===
void loop() {
  if (!started) {
    if (WiFi.status() == WL_CONNECTED) {
      char urlStr[32];
      sprintf(urlStr, "http://%s/", WiFi.localIP().toString().c_str());
      showMessage("Connect to:", urlStr);
    } else {
      showMessage("No connection");
    }
    delay(500);
  } else {
    if (WiFi.status() == WL_CONNECTED) {
      playAnimation60fps(); // animation + non-blocking drive updates
    } else {
      showMessage("Wi-Fi lost!");
      delay(500);
    }
  }
}