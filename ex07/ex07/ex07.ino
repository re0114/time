#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi 配置（请修改为你的实际网络）
const char* ssid = "70u";
const char* password = "12345678";

// LED 引脚（ESP32 板载 LED 通常为 GPIO2）
const int ledPin = 2;

// PWM 参数
const int freq = 5000;
const int resolution = 8;   // 0～255

WebServer server(80);

// 网页内容（已加入 UTF-8 声明，解决中文乱码）
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>ESP32 无极调光器</title>
    <style>
        body { font-family: Arial; text-align: center; margin-top: 50px; }
        input { width: 80%; margin: 20px; }
        p { font-size: 1.5em; }
    </style>
</head>
<body>
    <h1>ESP32 LED 调光器</h1>
    <p>亮度值: <span id="brightness">0</span></p>
    <input type="range" min="0" max="255" value="0" id="slider">
    <script>
        var slider = document.getElementById("slider");
        var span = document.getElementById("brightness");
        slider.oninput = function() {
            var value = this.value;
            span.innerHTML = value;
            fetch("/set?duty=" + value)
                .then(response => response.text())
                .then(data => console.log(data));
        }
    </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  
  // 初始化 PWM
  ledcAttach(ledPin, freq, resolution);
  ledcWrite(ledPin, 0);

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
  server.on("/set", []() {
    if (server.hasArg("duty")) {
      int duty = server.arg("duty").toInt();
      duty = constrain(duty, 0, 255);
      ledcWrite(ledPin, duty);
      server.send(200, "text/plain", "OK");
      Serial.print("Set brightness: ");
      Serial.println(duty);
    } else {
      server.send(400, "text/plain", "Missing duty");
    }
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}