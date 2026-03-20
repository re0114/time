// 实验2：使用millis()实现1Hz LED闪烁,非阻塞延迟
const int ledPin = 2;          // ESP32板载LED通常为GPIO2

unsigned long previousMillis = 0;
const long interval = 1000;    // 1秒 = 1000毫秒
int ledState = LOW;

void setup() {
  Serial.begin(115200);        // 初始化串口，用于调试
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  // 非阻塞延时：检查是否到达切换时间
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // 翻转LED状态
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(ledPin, ledState);

    // 串口输出状态（方便观察）
    if (ledState == HIGH) {
      Serial.println("LED ON");
    } else {
      Serial.println("LED OFF");
    }
  }

  // 注意：这里可以添加其他任务，不会被delay()阻塞
}