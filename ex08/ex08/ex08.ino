#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi 配置（修改为你的网络信息）
const char* ssid = "70u";
const char* password = "12345678";

// 引脚定义
const int ledPin = 2;       // 报警 LED
const int touchPin = 4;     // 触摸引脚 T0

// 系统状态变量
bool armed = false;         // 是否布防
bool alarming = false;      // 是否正在报警（锁定状态）

// 触摸阈值（通过串口监视器观察触摸值后调整）
const int touchThreshold = 20;

WebServer server(80);

// 网页 HTML（带布防/撤防按钮和状态显示）
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>ESP32 安防报警器</title>
    <style>
        body { font-family: Arial; text-align: center; margin-top: 50px; }
        button { font-size: 1.5em; padding: 10px 20px; margin: 10px; }
        .armed { color: red; font-weight: bold; }
        .disarmed { color: green; font-weight: bold; }
    </style>
</head>
<body>
    <h1>🔒 物联网安防报警器</h1>
    <p>系统状态: <span id="state" class="disarmed">未布防</span></p>
    <button onclick="sendCommand('arm')">🔐 布防</button>
    <button onclick="sendCommand('disarm')">🔓 撤防</button>
    <script>
        function sendCommand(cmd) {
            fetch("/" + cmd)
                .then(response => response.text())
                .then(data => {
                    if (cmd === "arm") {
                        document.getElementById("state").innerHTML = "已布防";
                        document.getElementById("state").className = "armed";
                    } else {
                        document.getElementById("state").innerHTML = "未布防";
                        document.getElementById("state").className = "disarmed";
                    }
                });
        }
        // 每秒查询一次当前状态（保持同步）
        setInterval(() => {
            fetch("/status")
                .then(res => res.text())
                .then(status => {
                    if (status === "armed") {
                        document.getElementById("state").innerHTML = "已布防";
                        document.getElementById("state").className = "armed";
                    } else {
                        document.getElementById("state").innerHTML = "未布防";
                        document.getElementById("state").className = "disarmed";
                    }
                });
        }, 1000);
    </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // 连接 Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // 配置 Web 路由
  server.on("/", []() {
    server.send(200, "text/html", index_html);
  });
  server.on("/arm", []() {
    armed = true;
    alarming = false;          // 新布防时清除报警锁定
    digitalWrite(ledPin, LOW);
    server.send(200, "text/plain", "Armed");
    Serial.println("System Armed");
  });
  server.on("/disarm", []() {
    armed = false;
    alarming = false;
    digitalWrite(ledPin, LOW);
    server.send(200, "text/plain", "Disarmed");
    Serial.println("System Disarmed");
  });
  server.on("/status", []() {
    if (armed) server.send(200, "text/plain", "armed");
    else server.send(200, "text/plain", "disarmed");
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  
  // 布防且未报警时，检测触摸入侵
  if (armed && !alarming) {
    int touchValue = touchRead(touchPin);
    if (touchValue < touchThreshold) {
      Serial.println("🚨 Intrusion detected!");
      alarming = true;   // 锁定报警
    }
  }
  
  // 报警闪烁效果（高频闪烁，10Hz）
  if (alarming) {
    static unsigned long lastToggle = 0;
    static bool ledState = false;
    unsigned long now = millis();
    if (now - lastToggle > 100) {  // 100ms 翻转一次
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      lastToggle = now;
    }
  } else {
    digitalWrite(ledPin, LOW);
  }
}