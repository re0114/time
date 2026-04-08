#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi 配置
const char* ssid = "70u";
const char* password = "12345678";

// 触摸引脚
const int touchPin = 4;

WebServer server(80);

// 网页 HTML（包含实时数值显示和进度条）
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>ESP32 实时传感器仪表盘</title>
    <style>
        body { font-family: Arial; text-align: center; margin-top: 50px; }
        .value { font-size: 4em; font-weight: bold; margin: 20px; padding: 20px; background-color: #f0f0f0; border-radius: 20px; display: inline-block; }
        .label { font-size: 1.5em; }
        .bar-container { width: 80%; margin: 20px auto; background-color: #ddd; border-radius: 10px; }
        .bar { height: 30px; background-color: #4CAF50; border-radius: 10px; width: 0%; transition: width 0.1s; }
    </style>
</head>
<body>
    <h1>📊 实时触摸传感器仪表盘</h1>
    <div class="label">当前触摸数值</div>
    <div class="value" id="touchValue">--</div>
    <div class="bar-container">
        <div class="bar" id="bar"></div>
    </div>
    <p>🤚 用手靠近或触摸 GPIO4 引脚，数值会变小</p>
    <p>📈 数值越小，代表触摸强度越大</p>
    <script>
        function fetchData() {
            fetch("/data")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("touchValue").innerHTML = data;
                    // 将数值映射为进度条宽度（假设最大值为100，如果实际超过100可调整）
                    let percent = Math.min(100, (data / 100) * 100);
                    document.getElementById("bar").style.width = percent + "%";
                });
        }
        // 每 100 毫秒更新一次（10Hz 刷新率）
        setInterval(fetchData, 100);
        fetchData(); // 页面加载时立即获取一次
    </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  
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

  // 配置路由
  server.on("/", []() {
    server.send(200, "text/html", index_html);
  });
  server.on("/data", []() {
    int touchVal = touchRead(touchPin);
    server.send(200, "text/plain", String(touchVal));
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}