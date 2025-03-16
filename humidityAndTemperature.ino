#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

const char* ssid = "berna";  
const char* password = "123456789";  
const char* serverUrl = "http://192.168.200.75/api.php";  

// DHT Sensor
#define DHTPIN 6  
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(115200);
    
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");

    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 20) {
        delay(500);
        Serial.print(".");
        attempt++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to Wi-Fi");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFailed to connect! Restart.");
        ESP.restart();
    }

    // Initialize DHT sensor
    dht.begin();
    delay(2000);  
}

// Function to check and reconnect Wi-Fi
void checkWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi Disconnected, attempting to reconnect...");
        WiFi.disconnect();
        WiFi.reconnect();

        int attempt = 0;
        while (WiFi.status() != WL_CONNECTED && attempt < 10) {
            delay(500);
            Serial.print(".");
            attempt++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nReconnected to Wi-Fi!");
        } else {
            Serial.println("\nStill disconnected. Restarting ESP...");
            ESP.restart();
        }
    }
}

void transmitSensorData(const char* sensorType, float value) {
  if (WiFi.status() == WL_CONNECTED) { 
    HTTPClient http;
    Serial.print("Connecting to server: ");
    Serial.println(serverUrl);

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"sensorType\":\"" + String(sensorType) + "\", \"value\":" + String(value) + "}";
    Serial.print("Sending JSON: ");
    Serial.println(payload);

    int httpResponseCode = http.POST(payload);

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server Response: " + response);
    } else {
      Serial.println("Error: " + http.errorToString(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected, trying to reconnect...");
  }
}

void loop() {
    checkWiFi();  // Ensure Wi-Fi is connected

    // Read DHT sensor values
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor! Retrying...");
        dht.begin();
        delay(2000);
        return;
    }

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("°C, Humidity: ");
    Serial.print(humidity);
    Serial.println("%");

    // Send data to API
    transmitSensorData("temperature", temperature);
    transmitSensorData("humidity", humidity);

    yield();  
    delay(5000);  // Wait 5 seconds before sending again
}
