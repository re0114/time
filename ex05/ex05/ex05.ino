// 引脚定义
#define TOUCH_PIN 4
#define LED_PIN   2

// 触摸阈值
#define THRESHOLD 20

// PWM参数
const int freq = 5000;          // 频率5kHz
const int resolution = 8;       // 8位分辨率，占空比0~255

// 速度档位及对应的延时时间（ms）
int speedLevel = 1;             // 当前档位：1、2、3
const int speedDelays[4] = {0, 15, 8, 3}; // 索引1~3：慢、中、快

// 触摸边缘检测与防抖变量
bool lastTouchState = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 100;

void setup() {
  Serial.begin(115200);
  ledcAttach(LED_PIN, freq, resolution);
  Serial.println("Multi-speed Breathing Light Ready");
  Serial.print("Initial speed level: ");
  Serial.println(speedLevel);
}

void loop() {
  // 1. 处理触摸输入（切换速度档位）
  int touchValue = touchRead(TOUCH_PIN);
  bool currentTouchState = (touchValue < THRESHOLD);
  
  if (currentTouchState && !lastTouchState) {
    if (millis() - lastDebounceTime > debounceDelay) {
      // 切换档位：1->2->3->1
      speedLevel = (speedLevel % 3) + 1;
      Serial.print("Speed Level changed to: ");
      Serial.println(speedLevel);
      lastDebounceTime = millis();
    }
  }
  lastTouchState = currentTouchState;
  
  // 2. 执行呼吸灯效果（根据当前档位的delay值改变速度）
  int delayTime = speedDelays[speedLevel];
  
  // 逐渐变亮（占空比从0到255）
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(LED_PIN, duty);
    delay(delayTime);
  }
  // 逐渐变暗（占空比从255到0）
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(LED_PIN, duty);
    delay(delayTime);
  }
}