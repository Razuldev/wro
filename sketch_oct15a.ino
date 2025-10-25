#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <WiFi.h>

const char* ssid = "ESP32_AP";      
const char* password = "12345678";  

IPAddress serverIP(192, 168, 4, 2);  
const uint16_t serverPort = 5000;    

WiFiClient client;

int piezoPins[3] = {25, 26, 27};

int vibPins[3] = {13, 14, 15};

Adafruit_MPU6050 mpu;

float velocityX = 0, velocityY = 0, velocityZ = 0;
unsigned long lastTime;

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 3; i++) {
    pinMode(piezoPins[i], INPUT);
    pinMode(vibPins[i], INPUT);
  }

  if (!mpu.begin()) {
    Serial.println("IMU tapilmadi!");
    while (1) delay(10);
  }
  Serial.println("IMU ugurla basladi");

  lastTime = millis();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.println("ESP32 Access Point yaradildi!");
  Serial.print("Wi-Fi adı: "); Serial.println(ssid);
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());

  delay(5000); 
}

void loop() {
  if (!client.connected()) {
    Serial.println("Serverə qoşulmağa çalışır...");
    if (client.connect(serverIP, serverPort)) {
      Serial.println("Serverə qoşuldu!");
    } else {
      Serial.println("Qoşulma alınmadı, 2 saniyə sonra yenidən...");
      delay(2000);
      return;
    }
  }

  String dataStr = "";

  dataStr += "Piezo: ";
  for (int i = 0; i < 3; i++) {
    int val = analogRead(piezoPins[i]);
    dataStr += "P" + String(i+1) + "=" + String(val) + "  ";
  }

  dataStr += "| Vibrasiya: ";
  for (int i = 0; i < 3; i++) {
    int val = digitalRead(vibPins[i]);
    dataStr += "V" + String(i+1) + "=" + String(val) + "  ";
  }

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  unsigned long currentTime = millis();
  float dt = (currentTime - lastTime) / 1000.0;
  lastTime = currentTime;

  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;

  velocityX += ax * dt;
  velocityY += ay * dt;
  velocityZ += az * dt;

  float gx_deg = g.gyro.x * 57.2958;
  float gy_deg = g.gyro.y * 57.2958;
  float gz_deg = g.gyro.z * 57.2958;

  float rpmX = (gx_deg / 360.0) * 60.0;
  float rpmY = (gy_deg / 360.0) * 60.0;
  float rpmZ = (gz_deg / 360.0) * 60.0;

  dataStr += "| Accel: X=" + String(ax) + " Y=" + String(ay) + " Z=" + String(az);
  dataStr += " | Vel: X=" + String(velocityX) + " Y=" + String(velocityY) + " Z=" + String(velocityZ);
  dataStr += " | RPM: X=" + String(rpmX) + " Y=" + String(rpmY) + " Z=" + String(rpmZ);
  dataStr += " | Temp: " + String(temp.temperature) + "C";

  client.println(dataStr);
  Serial.println("Göndərildi: " + dataStr);

  delay(500);
}