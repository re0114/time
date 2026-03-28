// LED引脚定义
#define LED_A_PIN 2   // 第一个LED（如GPIO2）
#define LED_B_PIN 4   // 第二个LED（如GPIO4）

// PWM参数
const int freq = 5000;          // 频率5kHz
const int resolution = 8;       // 8位分辨率，占空比0~255

void setup() {
  Serial.begin(115200);
  
  // 初始化两个PWM通道（直接绑定引脚）
  ledcAttach(LED_A_PIN, freq, resolution);
  ledcAttach(LED_B_PIN, freq, resolution);
  
  Serial.println("Police Car Dual Flash Started");
}

void loop() {
  // 让LED_A从暗到亮，LED_B从亮到暗同步变化
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(LED_A_PIN, duty);       // A亮度递增
    ledcWrite(LED_B_PIN, 255 - duty); // B亮度递减
    delay(5); // 控制渐变平滑度，数值越小变化越快
  }
  
  // 可选：反向再变化一次，形成连续来回效果（取消注释可实现来回扫描）
  // for (int duty = 255; duty >= 0; duty--) {
  //   ledcWrite(LED_A_PIN, duty);
  //   ledcWrite(LED_B_PIN, 255 - duty);
  //   delay(5);
  // }
}