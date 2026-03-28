// 引脚定义
#define TOUCH_PIN 4    // 触摸引脚（T0）
#define LED_PIN   2    // LED引脚

// 触摸阈值（需根据实际串口输出调整）
#define THRESHOLD 20

// 状态变量
bool ledState = false;      // LED当前状态
bool lastTouchState = false; // 上一次触摸状态（用于边缘检测）

// 防抖相关
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // 防抖时间（ms）

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState); // 初始熄灭
}

void loop() {
  // 读取触摸值（未触摸时数值较大，触摸时数值变小）
  int touchValue = touchRead(TOUCH_PIN);
  
  // 判断当前是否处于“按下”状态（触摸）
  bool currentTouchState = (touchValue < THRESHOLD);
  
  // 边缘检测：只有从“未触摸”变为“触摸”的瞬间才触发
  if (currentTouchState && !lastTouchState) {
    // 防抖处理：若距离上一次触发时间超过防抖延迟，则认为是有效触发
    if (millis() - lastDebounceTime > debounceDelay) {
      ledState = !ledState;           // 翻转LED状态
      digitalWrite(LED_PIN, ledState);
      lastDebounceTime = millis();     // 记录本次触发时间
      
      // 串口输出状态
      Serial.print("LED State: ");
      Serial.println(ledState ? "ON" : "OFF");
    }
  }
  
  // 更新上一次触摸状态
  lastTouchState = currentTouchState;
  
  // 简单延时，避免串口输出过快
  delay(20);
}