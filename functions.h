// functions.h
#pragma once

// ==========================================================
// ==    DEKLARASI OBJEK GLOBAL & PROTOTYPE FUNGSI         ==
// ==========================================================

// --- Deklarasi Eksternal (Global Objects & Variables) ---
class WebServer; 
extern WebServer server;
extern WiFiClientSecure espClient;
extern PubSubClient mqttClient;
extern LiquidCrystal_I2C lcd;
extern Preferences preferences;
extern DHT dht;

struct DeviceConfig {
    float humidity_critical;
    float humidity_warning;
    int schedule_hours[5];
    int schedule_count;
};
extern DeviceConfig config;

enum AppState { STATE_BOOTING, STATE_AP_MODE, STATE_CONNECTING, STATE_NORMAL_OPERATION, STATE_MENU_INFO };
extern AppState currentState;

extern float currentHumidity, currentTemperature;
extern bool isPumpOn;
extern char mqttClientId[40];
extern int lastScheduledHour;
extern unsigned long pumpStopTime;
extern unsigned long btnOkPressTime;
extern bool okButtonLongPress;
extern bool okButtonPressed;

// --- Deklarasi Fungsi (Function Prototypes) ---
// Inisialisasi
void init_hardware();
void init_storage_and_wifi();
void init_mqtt();
void load_config();
void save_config();

// Mode Operasi
void start_ap_mode();
void handle_connecting_state();
void handle_normal_operation();

// Web Server
void handle_web_root();
void handle_web_save();

// Logika Utama
void handle_main_logic();

// Tampilan & Input
void display_boot_screen();
void display_normal_info();
void display_menu_info();
void display_ap_info(IPAddress ip);
void display_connecting_wifi();
void check_buttons();

// MQTT & Komunikasi
void reconnect_mqtt();
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void handle_config_update(byte* payload, unsigned int length);
void publish_telemetry();
void publish_wifi_signal();
void publish_config();

// Kontrol & Aksi
void run_humidity_control_logic(float humidity);
void run_scheduled_control(float humidity);
void turn_pump_on(const char* reason);
void turn_pump_off();