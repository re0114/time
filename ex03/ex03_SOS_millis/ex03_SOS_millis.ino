// 实验3任务2：使用millis()实现SOS信号（三短、三长、三短，长停顿）
const int ledPin = 2;

// 时序定义（单位：毫秒）
const int dot = 200;          // 短闪时长
const int dash = 600;         // 长闪时长
const int intraCharGap = 200; // 同一字母内点划间隔
const int interCharGap = 500; // 字母间间隔
const int interWordGap = 2000;// 单词结束后停顿

// SOS序列：正数表示亮灯时长，负数表示灭灯时长
const int sosSequence[] = {
  dot, -intraCharGap, dot, -intraCharGap, dot, -interCharGap,     // S
  dash, -intraCharGap, dash, -intraCharGap, dash, -interCharGap,   // O
  dot, -intraCharGap, dot, -intraCharGap, dot, -interWordGap       // S + 结束停顿
};
const int seqLength = sizeof(sosSequence) / sizeof(sosSequence[0]);

int stepIndex = 0;                 // 当前步骤索引
unsigned long previousMillis = 0;  // 上一次动作开始时间

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  unsigned long currentMillis = millis();

  if (previousMillis == 0 || (currentMillis - previousMillis >= abs(sosSequence[stepIndex]))) {
    stepIndex = (stepIndex + 1) % seqLength;

    int action = sosSequence[stepIndex];
    if (action > 0) {
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    }
    previousMillis = currentMillis;
  }
}