#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "DHT.h"

// WiFi
#define WIFI_SSID "TD302"
#define WIFI_PASSWORD "0986602203"

// Firebase
#define API_KEY "AIzaSyCI8T41kd_xgC8GcujIyubfRqieNKvksYM"
#define DATABASE_URL "https://duandautien-2511-default-rtdb.firebaseio.com/"
#define USER_EMAIL "vuhuuhuy25115@gmail.com"
#define USER_PASS "88888888"
// DHT sensor
#define DHTPIN 4
#define DHTTYPE DHT11

// BUZZER
#define BUZZER_PIN 5

DHT dht(DHTPIN, DHTTYPE); //tạo đói tượng cảm biến

// Firebase objects
FirebaseData fbdo;   //gửi và nhận dữ liệu
FirebaseAuth auth;   //xác thực đăng nhập
FirebaseConfig config;  //cấu hình Firebase

float threshold = 30; // ngưỡng cảnh báo

void setup()
{
    Serial.begin(115200);

    dht.begin(); //bật cảm biến

   pinMode(BUZZER_PIN, OUTPUT);


    // WiFi connect

    WiFi.begin( WIFI_SSID , WIFI_PASSWORD);  //ESP32 bắt đầu kết nôí Wifi
    Serial.print("Connecting WiFi");

    Serial.println("Dang ket noi WiFi...");   //chờ Wifi kết nối
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }

    Serial.println();
    Serial.println("WiFi connected");   //khi kết nối thành công
  Serial.println(WiFi.localIP());  //in ra IP

    //cấu hình Firebase 
    config.api_key = API_KEY; //thiết lập khóa API
    config.database_url = DATABASE_URL; // địa chỉ database

    if (Firebase.signUp(&config, &auth, "", "")) { //đăng ký Firebase
  Serial.println("Firebase signup OK");
} else {
  Serial.printf("Signup failed: %s\n", config.signer.signupError.message.c_str());
}

//khởi động Firebase
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}

void loop()
{
    float temperature = dht.readTemperature(); //đọc dữ liệu cảm biến
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) //kiểm tra lỗi cảm biến
    {
        Serial.println("Failed to read from DHT sensor");
        return;
    }

    Serial.print("Temp: ");  //in dứ liệu ra Serial
    Serial.println(temperature);
    delay(2000);

    Serial.print("Hum: ");
    Serial.println(humidity);
    
    
    // Send to Firebase
    if (Firebase.RTDB.setFloat(&fbdo, "/TEMP", temperature)) {
  Serial.println("Send TEMP OK");
} else {
  Serial.println(fbdo.errorReason());
  delay(2000);
}

if (Firebase.RTDB.setFloat(&fbdo, "/HUMI", humidity)) {
  Serial.println("Send HUMI OK");
} else {
  Serial.println(fbdo.errorReason());
  delay(2000);
}
    // ===== ALERT =====
    if (temperature > threshold)
    {
        Serial.println("ALERT!!!");

        // buzzer kêu
        digitalWrite(BUZZER_PIN, HIGH);
        delay(200);
        digitalWrite(BUZZER_PIN, LOW);

        Firebase.RTDB.setInt(&fbdo, "/ALERT", 1);
    }
    else
    {
        digitalWrite(BUZZER_PIN, LOW);
        Firebase.RTDB.setInt(&fbdo, "/ALERT", 0);
    }

    delay(2000);
   
}
