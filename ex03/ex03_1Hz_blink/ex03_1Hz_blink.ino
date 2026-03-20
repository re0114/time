// 实验3任务1：使用millis()实现1Hz LED闪烁非阻塞延时实现
const int ledPin = 2;
unsigned long previousMillis = 0;
const long interval = 1000;   // 1Hz
int ledState = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    Serial.println(ledState ? "ON" : "OFF");
  }
}