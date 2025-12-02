#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <string.h>
const char* ssid = "Realm of Chaos";
const char* password = "Kittehbuttzinwatah236";
std::string mqtt_username = "Ejolliffe4411";
std::string mqtt_password = "4411";
const char *mqtt_broker = "p07da41d.ala.us-east-1.emqxsl.com";

const int mqtt_port = 8883;


const char* topicDefault = "SENG3030/Thursday/Ejolliffe4411/#";
int carsolCounter = 0;
bool sendMQTT = true;
bool readMic = false;

const char* battery = "battery";
const char* sht40 = "sht40";
const char* bmp280 = "bmp280";
const char* temperature = "temperature";
const char* humidity = "humidity";
const char* pressure = "pressure";
const char* accel = "accel";
const char* gyro = "gyro";
const char* x = "x";
const char* z = "z";
const char* mic = "mic/bytes";

char read_battery[10];
char read_sht40Temp[10];
char read_bmp280Temp[10];
char read_humidity[10];
char read_pressure[10];
char read_accelx[10];
char read_accely[10];
char read_accelz[10];
char read_gyrox[10];
char read_gyroy[10];
char read_gyroz[10];
char* mqtt_to_read;


const float shakeThreshold = 1.5;
float oldAccelX = 0;
float oldAccelY = 0;
float oldAccelZ = 0;

const std::string mqtt_base = "SENG3030/Thursday/" + mqtt_username + "/";
const std::string mqtt_battery = mqtt_base + "battery";

const std::string mqtt_bmp_temperature = mqtt_base + "bmp280/temperature";
const std::string mqtt_bmp_pressure = mqtt_base + "bmp280/pressure";

const std::string mqtt_sht_temperature = mqtt_base + "sht40/temperature";
const std::string mqtt_sht_humidity = mqtt_base + "sht40/humidity";

const std::string mqtt_accel_x = mqtt_base + "accel/x";
const std::string mqtt_accel_y = mqtt_base + "accel/y";
const std::string mqtt_accel_z = mqtt_base + "accel/z";

const std::string mqtt_gyro_x = mqtt_base + "gyro/x";
const std::string mqtt_gyro_y = mqtt_base + "gyro/y";
const std::string mqtt_gyro_z = mqtt_base + "gyro/z";
const std::string mqtt_mic = mqtt_base + "mic/bytes";
const std::string mqtt_meta = mqtt_base + "mic/meta";
WiFiClientSecure espClient;
PubSubClient client(espClient);

int mainLoopCounter;
const int DELAY_MQTT_PUBLISH = 500;//Loop Cycles
const int DELAY_DISPLAY_UPDATE = 100;//Loop Cycles
const int DELAY_BUTTON_READ = 5;

static constexpr const size_t record_number = 200;
static constexpr const size_t record_length = 240;
static constexpr const size_t record_size = record_number * record_length;
static constexpr const size_t record_samplerate = 44100;
static int16_t prev_y[record_length];
static int16_t prev_h[record_length];
static size_t rec_record_idx = 2;
static size_t draw_record_idx = 0;
static int16_t *rec_data;

char mqtt_buffer[40];

const char* ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
)EOF";

#define PIN_CLK 0
#define PIN_DATA 34


#include "M5Unified.h"

// Include required for the ENV IV module
#include <M5UnitENV.h>

// I2C Pins for the external I2C port
#define EXT_SDA 32
#define EXT_SCL 33

// SHT40 Humidity and Temperature I2C sensor
SHT4X sht4;
// BMP280 Pressure and Temperature I2C sensor
BMP280 bmp;
// Flag that ENV was set up
bool env_ok = false;
int selection = 0;

void setup_unit_env(void);
void read_unit_env(void);
int get_Button_Presses(void);
void IMU_reading(void);
void setup_wifi(void);
void connectToMQTTBroker(void);
void mqttCallback(char *topic, byte *payload, unsigned int length);
void send_mqtt_message(const std::string& topic, void* message);
void shakeWake(void);

void setup() {
  M5.begin();
  // Serial.begin(115200);
  Serial.println("** Starting Setup **");

  // Set up the ENV Sensor
  setup_unit_env();
  auto cfg = M5.config();
  M5.begin(cfg);

  //Setup Display
  M5.Display.setRotation(1);
  M5.Display.setTextColor(WHITE, BLACK);
  M5.Display.setTextDatum(middle_center);
  M5.Display.setFont(&fonts::FreeSansBold9pt7b);
  M5.Display.setTextSize(1);

  rec_data = (typeof(rec_data))heap_caps_malloc(record_size * sizeof(int16_t), MALLOC_CAP_8BIT);
  memset(rec_data, 0, record_size * sizeof(int16_t));
  M5.Speaker.setVolume(255);
  M5.Speaker.end();
  M5.Mic.begin();

  setup_wifi();

  // Set Root CA certificate
  espClient.setCACert(ca_cert);
  client.setServer(mqtt_broker, mqtt_port);
  client.setKeepAlive(60);
  client.setCallback(mqttCallback);
  connectToMQTTBroker();
  client.subscribe(topicDefault);
  mainLoopCounter = 0;
}


void loop() {

  
  if (M5.Mic.isEnabled()) {
    static constexpr int shift = 6;
    auto data = &rec_data[rec_record_idx * record_length];
    //Record audio
    if (M5.Mic.record(data, record_length, record_samplerate)) {
      data = &rec_data[draw_record_idx * record_length];
      //Get width of waveform
      int32_t w = M5.Display.width();
      if (w > record_length - 1) {
        w = record_length - 1;
      }
      if (++draw_record_idx >= record_number) {
        draw_record_idx = 0;
      }
      if (++rec_record_idx >= record_number) {
        rec_record_idx = 0;
      }
    }
  }

  selection = get_Button_Presses();
  switch(selection)
  {
    case 1:
      if (M5.Speaker.isEnabled()) {
        while (M5.Mic.isRecording()) {
          delay(1);
        }
        /// Since the microphone and speaker cannot be used at the same
        /// time, turn off the microphone here turn on the speaker.
        M5.Mic.end();
        M5.Speaker.begin();
        int start_pos = rec_record_idx * record_length;
        if (start_pos < record_size) {
          client.publish(mqtt_mic.c_str(),(char*) &rec_data[start_pos]);
          sprintf(mqtt_buffer, "%d", start_pos);
          client.publish(mqtt_meta.c_str(), mqtt_buffer);
          M5.Speaker.playRaw(&rec_data[start_pos], record_size - start_pos, record_samplerate, false, 1, 0);
        }
        if (start_pos > 0) {
          client.publish(mqtt_mic.c_str(),(char*) rec_data);
          sprintf(mqtt_buffer, "%d", start_pos);
          client.publish(mqtt_meta.c_str(), mqtt_buffer);
          M5.Speaker.playRaw(rec_data, start_pos, record_samplerate, false, 1, 0);
        }
        do {
          delay(1);
          M5.update();
        } while (M5.Speaker.isPlaying());
        M5.Speaker.end();
        M5.Mic.begin();
      }
      break;
    case 2:
      IMU_reading();
      break;
    case 3:
      if(sendMQTT)
      {
        sendMQTT = false;
        Serial.println("MQTT Publishing Paused");
      }
      else
      {
        sendMQTT = true;
      }
      break;
    default:
      break;
  }
  shakeWake();  
  
  if(mainLoopCounter % DELAY_DISPLAY_UPDATE == 0)
  {
    
    M5.update();
    M5.Display.setCursor(0, 40);
    M5.Display.clear();
    if(sendMQTT)
    {
      M5.Display.fillRect(0, 0, 240, 130, GREEN);
    }
    else
    {
      M5.Display.fillRect(0, 0, 240, 130, BLUE);
    }
    char temp[50] = {"\0"};
    M5.Display.setCursor(10, 30);
    switch(carsolCounter)
    {
      case 1:
        sprintf(temp, "SHT40");
        M5.Display.println(temp);
        sprintf(temp, "Temperature: %s", read_sht40Temp);
        M5.Display.println(temp);
        sprintf(temp, "Humidity: %s", read_humidity);
        M5.Display.println(temp);
        break;
      case 2:
        sprintf(temp, "BMP280");
        M5.Display.println(temp);
        sprintf(temp, "Temperature: %s", read_bmp280Temp);
        M5.Display.println(temp);
        sprintf(temp, "Pressure: %s", read_pressure);
        M5.Display.println(temp);
        break;
      case 3:
        sprintf(temp, "Accel");
        M5.Display.println(temp);
        sprintf(temp, "X: %s", read_accelx);
        M5.Display.println(temp);
        sprintf(temp, "Y: %s", read_accely);
        M5.Display.println(temp);
        sprintf(temp, "Z: %s", read_accelz);
        M5.Display.println(temp);
        break;
      case 4:
        sprintf(temp, "Gyro");
        M5.Display.println(temp);
        sprintf(temp, "X: %s", read_gyrox);
        M5.Display.println(temp);
        sprintf(temp, "Y: %s", read_gyroy);
        M5.Display.println(temp);
        sprintf(temp, "Z: %s", read_gyroz);
        M5.Display.println(temp);
        break;
      case 5:
        sprintf(temp, "Mic");
        M5.Display.println(temp);
        break;
      default:
        break;
    }
    
  }

  if(mainLoopCounter >= DELAY_MQTT_PUBLISH)
  {
    
    if (!client.connected()) {
        connectToMQTTBroker();
    }
    // Required for subscriptions to fire callback
    client.loop();
    if(!sendMQTT)
    {
      mainLoopCounter = 0;
      return;
    }
    if (!env_ok)
    {
      Serial.println("**** Unit ENV not set up ****");
      mainLoopCounter = 0;
      return;
    }
    read_unit_env();
    //Publish Battery Voltage to MQTT
    int voltage = M5.Power.getBatteryLevel();
    send_mqtt_message(mqtt_battery + "1", &voltage);
    sprintf(mqtt_buffer, "%d", voltage);
    client.publish(mqtt_battery.c_str(), mqtt_buffer);
    //Publish SHT40 to MQTT
    sprintf(mqtt_buffer, "%0.2f", sht4.cTemp);
    client.publish(mqtt_sht_temperature.c_str(), mqtt_buffer);
    sprintf(mqtt_buffer, "%0.2f", sht4.humidity);
    client.publish(mqtt_sht_humidity.c_str(), mqtt_buffer);
    //Publish BMP to MQTT
    sprintf(mqtt_buffer, "%0.2f", bmp.cTemp);
    client.publish(mqtt_bmp_temperature.c_str(), mqtt_buffer);
    sprintf(mqtt_buffer, "%0.2f", bmp.pressure);
    client.publish(mqtt_bmp_pressure.c_str(), mqtt_buffer);
    

    //Get IMU data and publish to MQTT
    auto IMU_update = M5.Imu.update();
    if (IMU_update) {
        auto data = M5.Imu.getImuData();
        //Publish Accel to MQTT
        sprintf(mqtt_buffer, "%0.2f", data.accel.x);
        client.publish(mqtt_accel_x.c_str(), mqtt_buffer);
        sprintf(mqtt_buffer, "%0.2f", data.accel.y);
        client.publish(mqtt_accel_y.c_str(), mqtt_buffer);
        sprintf(mqtt_buffer, "%0.2f", data.accel.z);
        client.publish(mqtt_accel_z.c_str(), mqtt_buffer);
        //Publish Gyro to MQTT
        sprintf(mqtt_buffer, "%0.2f", data.gyro.x);
        client.publish(mqtt_gyro_x.c_str(), mqtt_buffer);
        sprintf(mqtt_buffer, "%0.2f", data.gyro.y);
        client.publish(mqtt_gyro_y.c_str(), mqtt_buffer);
        sprintf(mqtt_buffer, "%0.2f", data.gyro.z); 
        client.publish(mqtt_gyro_z.c_str(), mqtt_buffer);
    }
    mainLoopCounter = 0;
  }
  mainLoopCounter++;
}

void send_mqtt_message(const std::string& topic, void* message) {
    sprintf(mqtt_buffer, "%0.2f", message);
    client.publish(topic.c_str(), mqtt_buffer);
}

void setup_unit_env()
{
  Serial.println("** Setting Up Unit ENV **");

  // IMPORTANT - for the M5Stick-C Plus2, the pins we need are 32 and 33,
  // not what the example at github uses
  if (!sht4.begin(&Wire, SHT40_I2C_ADDR_44, EXT_SDA, EXT_SCL, 400000U)) {
      Serial.println("Couldn't find SHT4x");
      while (1) delay(1);
  }

  // You can have 3 different precisions, higher precision takes longer
  sht4.setPrecision(SHT4X_HIGH_PRECISION);
  sht4.setHeater(SHT4X_NO_HEATER);

  if (!bmp.begin(&Wire, BMP280_I2C_ADDR, EXT_SDA, EXT_SCL, 400000U)) {
      Serial.println("Couldn't find BMP280");
      while (1) delay(1);
  }
  /* Default settings from datasheet. */
  bmp.setSampling(BMP280::MODE_NORMAL,     /* Operating Mode. */
                  BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  BMP280::FILTER_X16,      /* Filtering. */
                  BMP280::STANDBY_MS_500); /* Standby time. */  
  
  env_ok = true;
}

void read_unit_env(void)
{
  if (not env_ok)
  {
    Serial.println("**** Unit ENV not set up ****");
    return;
  }

  if (sht4.update()) {
          Serial.println("-----SHT4X-----");
          Serial.print("Temperature: ");
          Serial.print(sht4.cTemp);
          Serial.println(" degrees C");
          Serial.print("Humidity: ");
          Serial.print(sht4.humidity);
          Serial.println("% rH");
          Serial.println("-------------\r\n");
  }

  if (bmp.update()) {
      Serial.println("-----BMP280-----");
      Serial.print(F("Temperature: "));
      Serial.print(bmp.cTemp);
      Serial.println(" degrees C");
      Serial.print(F("Pressure: "));
      Serial.print(bmp.pressure);
      Serial.println(" Pa");
      Serial.print(F("Approx altitude: "));
      Serial.print(bmp.altitude);
      Serial.println(" m");
      Serial.println("-------------\r\n");
  }

  int voltage = M5.Power.getBatteryVoltage();
  Serial.print("Battery Voltage: ");
  Serial.println(voltage);
}

int get_Button_Presses(void)
{
  M5.update();  // Required to get states
  if (M5.BtnA.wasPressed()) {
    return 1;
  }
  if (M5.BtnB.wasPressed()) {
      return 2;
  }
  if (M5.BtnPWR.wasPressed()) {
      return 3;
  }
  return 0;
}

void IMU_reading(void){
  auto imu_update = M5.Imu.update();
    if (imu_update) {

        auto data = M5.Imu.getImuData();
        Serial.println("-----IMU-----");
        Serial.println("-----Accel-----");
        Serial.print("Accel X: ");
        Serial.println(data.accel.x);
        Serial.print("Accel Y: ");
        Serial.println(data.accel.y);
        Serial.print("Accel Z: ");
        Serial.println(data.accel.z);
        Serial.println("-----Gyro-----");
        Serial.print("Gyro X: ");
        Serial.println(data.gyro.x);
        Serial.print("Gyro Y: ");
        Serial.println(data.gyro.y);
    }
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void connectToMQTTBroker() {
    while (!client.connected()) {
        String client_id = "esp32-client-" + String(WiFi.macAddress());
        Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username.c_str(), mqtt_password.c_str())) {
            Serial.println("Connected to MQTT broker");
            client.subscribe((mqtt_base + "#").c_str());
            // Publish message upon successful connection
            //client.publish((mqtt_base + "hello").c_str(), "Hello SENG3030!");
        } else {
            Serial.print("Failed to connect to MQTT broker, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(25000);
        }
    }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (unsigned int i = 0; i < length; i++) {
      Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");

  if(strstr(topic, battery))
  {
    mqtt_to_read = read_battery;
    carsolCounter = 0;
  }
  if(strstr(topic, sht40))
  {
    carsolCounter = 1;
    if(strstr(topic, temperature))
    {
      mqtt_to_read = read_sht40Temp;
    }
    if(strstr(topic, humidity))
    {
      mqtt_to_read = read_humidity;
    }
  }
  if(strstr(topic, bmp280))
  {
    carsolCounter = 2;
    if(strstr(topic, temperature))
    {
      mqtt_to_read = read_bmp280Temp;
    }
    if(strstr(topic, pressure))
    {
      mqtt_to_read = read_pressure;
    }
  }
  if(strstr(topic, accel) != NULL)
  {
    carsolCounter = 3;
    if(strstr(topic, x) != NULL)
    {
      mqtt_to_read = read_accelx;
    }

    int temp = strlen(topic);
    if(topic[temp-1] =='y')
    {
      mqtt_to_read = read_accely;
    }

    if(strstr(topic, z) != NULL)
    {
      mqtt_to_read = read_accelz;
    }
  }

  if(strstr(topic, gyro) != NULL)
  {
    carsolCounter = 4;
    if(strstr(topic, x) != NULL)
    {
      mqtt_to_read = read_gyrox;
    }

    int temp = strlen(topic);
    if(topic[temp-1] =='y')
    {
      mqtt_to_read = read_gyroy;
    }

    if(strstr(topic, z) != NULL)
    {
      mqtt_to_read = read_gyroz;
    }
    if(strstr(topic, mqtt_mic.c_str()))
    {
      carsolCounter = 5;
      mqtt_to_read = 0;
      if(readMic)
      {
          if (M5.Speaker.isEnabled()) {
          while (M5.Mic.isRecording()) {
            delay(1);
          }
          /// Since the microphone and speaker cannot be used at the same
          /// time, turn off the microphone here turn on the speaker.
          M5.Mic.end();
          M5.Speaker.begin();
          M5.Speaker.playRaw(payload, length, record_samplerate, false, 1, 0);
          do {
            delay(1);
            M5.update();
          } while (M5.Speaker.isPlaying());
          M5.Speaker.end();
          M5.Mic.begin();
          readMic = false;
        }
      }
    } 
  }
  if(mqtt_to_read > 0)
  {
    for(int i = 0; i < length; i++)
    {
      mqtt_to_read[i] = payload[i];
    }
  }
    // switch (carsolCounter)
    // {
    // case 0: // BATTERY----------------------------------------

    // if(strstr(topic, battery) != NULL)
    //   {
    //     M5.Display.clear();
    //     M5.Display.setCursor(10, 30);
    //     M5.Display.print("BAT: %");
    //     for (unsigned int i = 0; i < length; i++) 
    //     {
    //       M5.Display.print((char) payload[i]);
    //     }
    //   }
    //   break;
    
    // case 1: // TEMP & HUMD-------------------------------------
    //   if(strstr(topic, sht40) != NULL)
    //   {
    //     if(strstr(topic, temperature) != NULL)
    //     {
    //       M5.Display.clear();
    //       M5.Display.setCursor(10, 30);
    //       M5.Display.print("Temperature: ");
    //       for (unsigned int i = 0; i < length; i++) 
    //       {
    //         M5.Display.print((char) payload[i]);
    //       }
    //     }

    //      if(strstr(topic, humidity) != NULL)
    //     {
    //       M5.Display.println();
    //       M5.Display.print("Humidity: ");
    //       for (unsigned int i = 0; i < length; i++) 
    //       {
    //         M5.Display.print((char) payload[i]);
    //       }
    //     }
    //   }
    //   break;

    // case 2: // TEMP AND PRESS-----------------------------------
    //   if(strstr(topic, bmp280) != NULL)
    //   {
    //     if(strstr(topic, temperature) != NULL)
    //     {
    //       M5.Display.clear();
    //       M5.Display.setCursor(10, 30);
    //       M5.Display.print("Temperature: ");
    //       for (unsigned int i = 0; i < length; i++) 
    //       {
    //         M5.Display.print((char) payload[i]);
    //       }
    //     }

    //      if(strstr(topic, pressure) != NULL)
    //     {
    //       M5.Display.println();
    //       M5.Display.print("Pressure: ");
    //       for (unsigned int i = 0; i < length; i++) 
    //       {
    //         M5.Display.print((char) payload[i]);
    //       }
    //     }
    //   }
    //   break;
    
    // case 3: // ACCEL------------------------------------------
    //   if(strstr(topic, accel) != NULL)
    //   {
    //     if(strstr(topic, x) != NULL)
    //     {
    //       M5.Display.clear();
    //       M5.Display.setCursor(10, 30);
    //       M5.Display.print("Accel X: ");
    //       for (unsigned int i = 0; i < length; i++) 
    //       {
    //         M5.Display.print((char) payload[i]);
    //       }
    //     }

    //     int temp = strlen(topic);
    //     if(topic[temp-1] =='y')
    //     {
    //       M5.Display.println();
    //       M5.Display.print("Accel Y: ");
    //       for (unsigned int i = 0; i < length; i++) 
    //       {
    //         M5.Display.print((char) payload[i]);
    //       }
    //     }

    //     if(strstr(topic, z) != NULL)
    //     {
    //       M5.Display.println();
    //       M5.Display.print("Accel Z: ");
    //       for (unsigned int i = 0; i < length; i++) 
    //       {
    //         M5.Display.print((char) payload[i]);
    //       }
    //     }
    //   }
    //   break;
    
    // case 4: // GYRO
    //   if(strstr(topic, gyro) != NULL)
    //   {
    //     if(strstr(topic, x) != NULL)
    //     {
    //       M5.Display.clear();
    //       M5.Display.setCursor(10, 30);
    //       M5.Display.print("Gyro X: ");
    //       for (unsigned int i = 0; i < length; i++) 
    //       {
    //         M5.Display.print((char) payload[i]);
    //       }
    //     }

    //     int temp = strlen(topic);
    //     if(topic[temp-1] =='y')
    //     {
    //       M5.Display.println();
    //       M5.Display.print("Gyro Y: ");
    //       for (unsigned int i = 0; i < length; i++) 
    //       {
    //         M5.Display.print((char) payload[i]);
    //       }
    //     }

    //     if(strstr(topic, z) != NULL)
    //     {
    //       M5.Display.println();
    //       M5.Display.print("Gyro Z: ");
    //       for (unsigned int i = 0; i < length; i++) 
    //       {
    //         M5.Display.print((char) payload[i]);
    //       }
    //     }
    //   }
    //   break;
    
    
    // default:
    // //   break;
    // // }
}

void shakeWake(void)
{
  // get accel values
  auto imu_update = M5.Imu.update();
  float accelX = 0;
  float accelY = 0;
  float accelZ = 0;
  M5.Imu.getAccelData(&accelX, &accelY, &accelZ);
  
  // calculate delta to see if shake was big enough to change
  float deltaX = abs(accelX - oldAccelX);
  float deltaY = abs(accelY - oldAccelY);
  float deltaZ = abs(accelZ - oldAccelZ);

  float delta = deltaX + deltaY + deltaZ;
  
  
  if(delta > shakeThreshold)
  {
    if(!readMic)
    {
      readMic = true;
      M5.Speaker.tone(4000, 20);
    }
    
  }

  // change old values 
  oldAccelX = accelX;
  oldAccelY = accelY;
  oldAccelZ = accelZ;
}

