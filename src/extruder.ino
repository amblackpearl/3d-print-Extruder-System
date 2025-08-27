#include <Keypad_I2C.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// Define I2C addresses
#define KEYPAD_I2C_ADDR 0x20  // I2C address for keypad
#define LCD_I2C_ADDR 0x27     // I2C address for LCD 20x4

// Initialize keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 4, 5, 6, 7 };
byte colPins[COLS] = { 0, 1, 2, 3 };

// Initialize keypad and LCD
Keypad_I2C keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, KEYPAD_I2C_ADDR);
LiquidCrystal_I2C lcd(LCD_I2C_ADDR, 20, 4);

// Pin definitions for fan and heater
const int heater = 23;

// Motor pins
const int motorPul = 15;
const int motorDir = 2;
const int motorENA = 4;

// Global variables
// float pulseCount = 0;
volatile bool isRunning = false;
volatile bool beneranOFF = false;
// int feedrate = 0;
int setpointSuhu = 0;
const int lelehFilament = 180;  //titik leleh HDPE
String tenS = "";
String feed = "";
String Setpoint = "";
String input = "";
float histerisis = 5.0;
int tiga = 0;


//feedrate-feedratean
int feedrate = 0;
int pulseCount = 0;
double Dnozzle = 0.8;                  // mm
// float volume_Per_Pulse = 2.919218750;  //microstsep 400
float volume_Per_Pulse = 2.92071;  //microstsep 400
bool adaSuhu = false;
bool konfirmasi = false;


//pin ntc
const int ntcA = 34;
const int ntcB = 35;

//setup ntc
const float VCC = 3.3;
const int ADC_RES = 4095;  // 10-bit ADC on Arduino Mega
const float R_FIXED = 9720.0;
const float R0 = 96300.0;        // 96300.0; //98700.0;  //103700.0;
const float to = 25.4 + 273.15;  // 25°C in Kelvin
const float B = 3950;            //3950.0;          // B-constant of the NTC


typedef struct {
  float minTemp;
  float maxTemp;
  float adjustment;
} CalibrationRange;

// const CalibrationRange CALIBRATIONS[] = {
//   { 80.0, 100.0, 3.73 },
//   { 100.0, 120.0, 3.73 },
//   { 120.0, 150.0, 5.22 },
//   { 150.0, 170.0, 11.76 },
//   { 170.0, 200.0, 12.84 },
//   { 200.0, 208.0, 26.23 },
//   { 208.0, 220.0, 7.14 },
//   { 220.0, 250.0, 13.58 }  // Asumsi maks 250°C
// };
// look up table

const CalibrationRange CALIBRATIONS[] = {
  { 20.0, 30.0, -1.74 },
  { 30.0, 40.0, 3.87 },
  { 40.0, 50.0, 7.87 },
  { 50.0, 60.0, 7.95 },  //2.19
  { 60.0, 80.0, 17.14 },  //4.19
  // { 70.0, 80.0, 1.17 },
  { 80.0, 90.0, 1.86 },
  { 90.0, 100.0, 8.41 },
  { 100.0, 140.0, 17.86 },
  // { 120.0, 130.0, 8.12 },
  { 140.0, 160.0, 13.47 },
  // { 140.0, 150.0, 8.97 },
  // { 150.0, 160.0, 5.73 },
  { 160.0, 180.0, 12.78 },  //11.76 },
  // { 170.0, 180.0, 9.97 },  //11.76 },
  { 180.0, 190.0, 9.44 },
  { 190.0, 200.0, 8.64 },
  // { 160.0, 200.0, 34.64 },
  { 200.0, 210.0, 7.44 },
  { 210.0, 220.0, 5.64 },
  { 220.0, 230.0, 3.14 },
  { 230.0, 240.0, 4.43 },
  { 240.0, 250.0, 3.11 },
  { 250.0, 260.0, 8.21 },
  { 260.0, 270.0, 9.29 },
  { 270.0, 280.0, 9.68 },
  { 280.0, 290.0, 9.92 },
  { 290.0, 300.0, 9.97 },
  { 310.0, 320.0, 9.54 },
  { 320.0, 330.0, 9.76 },
  { 330.0, 340.0, 9.52 },
  { 340.0, 350.0, 9.91 },
  { 350.0, 360.0, 9.6 },
  { 360.0, 370.0, 9.46 },
  { 370.0, 380.0, 9.54 },
  { 380.0, 390.0, 9.32 },
  { 390.0, 400.0, 9.87 },
  { 400.0, 410.0, 9.73 },
};


// int adc1 = 0;
// int adc2 = 0;
float suhuAvg = 0.0;
float suhuA = 0.0;
float suhuB = 0.0;


// Menu
const char *menuItems[] = {
  "1. Input Setpoint",
  "2. Input Feedrate",
  "3. Check Suhu",
  "4. Reset Value",
  "5. Push HDPE",
  "6. Value Check"
};

const int totalMenu = sizeof(menuItems) / sizeof(menuItems[0]);
const int itemsPerPage = 4;
int currentPage = 0;

enum Mode { MENU,
            KONTINU };
Mode modeAktif = MENU;
bool pertamaMasukKontinu = true;
bool pertamaMasukMenu = true;


// Add these global variables near your other global variables
typedef struct {
  int targetCount;
  int currentCount;
  unsigned long previousMicros;
  unsigned long interval;
  bool isRunning;
} PulseState;

PulseState pulseState = { 0, 0, 0, 0, false };



// FreeRTOS task handles
TaskHandle_t motorTaskHandle = NULL;

void setup() {
  Serial.begin(19200);
  Wire.begin(21, 22);
  keypad.begin();

  pinMode(heater, OUTPUT);
  pinMode(motorDir, OUTPUT);
  pinMode(motorENA, OUTPUT);
  pinMode(motorPul, OUTPUT);

  digitalWrite(motorDir, LOW);
  digitalWrite(motorENA, LOW);
  digitalWrite(heater, LOW);
  analogReadResolution(12);

  lcd.begin();
  lcd.backlight();
  // lcd.clear();

  // Display initial menu
  tampilkanMenu();
  // Create tasks for Core 0 (main task) and Core 1 (motor task)
  xTaskCreatePinnedToCore(mainTask, "Main Task", 8192, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(motorTask, "Motor Task", 4096, NULL, 2, &motorTaskHandle, 1);
  xTaskCreatePinnedToCore(suhuTask, "Suhu Task", 4096, NULL, 1, NULL, 0);
}
void loop() {
}
void suhuTask(void *parameter) {
  while (true) {
    Suhu();
  }
}
void mainTask(void *parameter) {
  while (true) {
    char key = keypad.getKey();  // Baca input keypad

    // Kontrol pemanas & kipas berdasarkan logika suhu
    if (setpointSuhu <= 0) {
      digitalWrite(heater, LOW);
      Serial.println("heater mati");
    }
    if (suhuA <= -273.0) {
      digitalWrite(heater, LOW);
    }

    // =================== MODE HANDLING ====================
    switch (modeAktif) {
      case MENU:
        if (pertamaMasukMenu) {
          tampilkanMenu();
          pertamaMasukMenu = false;
        }
        break;

      case KONTINU:
        monitorSuhu();
        if (key == '*') {
          modeAktif = MENU;
          pertamaMasukMenu = true;
          isRunning = false;
        }
        break;
    }
    // =================== INPUT HANDLING ===================
    if (key) {
      switch (key) {
        case 'A':  // Next page
          currentPage++;
          if (currentPage * itemsPerPage >= totalMenu) currentPage = 0;
          tampilkanMenu();
          break;

        case 'B':  // Previous page
          currentPage = (currentPage == 0) ? (totalMenu - 1) / itemsPerPage : currentPage - 1;
          tampilkanMenu();
          break;

        case '*':
          modeAktif = MENU;
          pertamaMasukMenu = true;
          isRunning = false;
          break;

        case '#':
          if (setpointSuhu > 0 && feedrate > 0) {
            // if (feedrate > 0){
            sistemON();
          } else {
            error();
          }
          break;

        default:
          if (key >= '1' && key <= '7') {
            int menuIndex = key - '1';
            if (menuIndex < totalMenu) {
              lcd.clear();
              switch (key) {
                case '1':
                  while (true) {
                    lcd.setCursor(0, 0);
                    lcd.print("Masukkan Setpoint:");
                    setpointSuhu = bacaSetpoint();
                    if (setpointSuhu > lelehFilament && setpointSuhu > 0) {
                      lcd.clear();
                      lcd.setCursor(7, 1);
                      lcd.print("Heater");
                      lcd.setCursor(7, 2);
                      lcd.print("Aktif!");
                      delay(1000);
                      lcd.clear();
                    }
                    if (setpointSuhu < lelehFilament && setpointSuhu > 0) {
                      lcd.clear();
                      lcd.setCursor(7, 1);
                      lcd.print("Error!");
                      lcd.setCursor(1, 2);
                      lcd.print("suhu tidak cukup!!");
                      delay(1000);
                      lcd.clear();
                    } else {
                      break;
                    }
                  }
                  break;
                case '2':
                  lcd.setCursor(0, 0);
                  lcd.print("Masukkan Feedrate:");
                  feedrate = bacaFeed();
                  break;
                case '3':
                  modeAktif = KONTINU;
                  pertamaMasukKontinu = true;
                  break;

                case '4':
                  feedrate = 0;
                  setpointSuhu = 0;
                  feed = "";
                  Setpoint = "";
                  lcd.clear();
                  lcd.setCursor(2, 1);
                  lcd.print("Nilai di Reset");
                  delay(500);
                  break;

                case '5':
                  if (suhuA > lelehFilament + 10.0) {
                    lcd.setCursor(5, 1);
                    lcd.print("Tunggu....");
                    turuninHDPE(2000);
                    delay(5000);
                    break;
                  } else if (suhuA < lelehFilament) {
                    lcd.setCursor(5, 1);
                    lcd.print("SUHU TIDAK");
                    lcd.setCursor(7, 2);
                    lcd.print("CUKUP!");
                    delay(1000);
                    break;
                  }
                  break;

                case '6':
                  nilaiTersimpan();
                  break;
              }
              delay(2000);
              pertamaMasukMenu = true;  // Kembali ke menu utama
            }
          }
          break;
      }
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);  // Delay agar task lain jalan
  }
}

void motorTask(void *parameter) {
  while (true) {
    if (isRunning && pulseCount >= 0 && suhuA >= lelehFilament + 10.0) {
      // if(isRunning && pulseCount > 0){
      digitalWrite(motorDir, LOW);
      generatePulses(pulseCount);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);  // Yield to other tasks
  }
}

float bacaNTC(int pin) {
  int adc = analogRead(pin);
  float voltage = (adc / (float)ADC_RES) * VCC;

  // Avoid division by zero
  if (voltage >= VCC) voltage = VCC - 0.001;

  // Calculate thermistor resistance
  float rThermistor = (voltage * R_FIXED) / (VCC - voltage);

  // Calculate temperature using Beta formula
  float steinhart;
  if (rThermistor > 0) {
    steinhart = log(rThermistor / R0);
    steinhart = 1.0 / ((steinhart / B) + (1.0 / to));
    steinhart = steinhart - 273.15;  // Convert Kelvin to °C

    for (const auto &cal : CALIBRATIONS) {
      if (steinhart > cal.minTemp && steinhart < cal.maxTemp) {
        steinhart -= cal.adjustment;
        break;  // Hanya terapkan satu kalibrasi
      }
    }
  } else {
    steinhart = 350.02;  // error case
  }
  return steinhart;
}

float readAverageTemp(int pin, int baseSamples = 8) {
  int samples = baseSamples;
  float total = 0;

  // Ambil satu pembacaan awal
  float tempInitial = bacaNTC(pin);

  // Jika suhu tinggi, naikkan jumlah sample
   if (tempInitial >= 130) {
    samples = 32;  // misalnya tingkatkan jadi 5 sample
  }else if (tempInitial >= 170) {
    samples = 32;  // misalnya tingkatkan jadi 5 sample
  } else if (tempInitial >= 200) {
    samples = 64; //3
  }

  // Total ulang dengan jumlah sample yang disesuaikan
  for (int i = 0; i < samples; i++) {
    total += bacaNTC(pin);
    delay(10);  // sampling delay
  }

  return total / samples;
}
void Suhu() {
  const int pwmChannel = 0;
  const int freq = 5000;
  const int resolution = 8;  // 0 - 255
  static bool initialized = false;

  // Inisialisasi PWM hanya sekali di awal
  if (!initialized) {
    ledcSetup(pwmChannel, freq, resolution);
    ledcAttachPin(heater, pwmChannel);
    initialized = true;
  }

  suhuA = readAverageTemp(ntcB);
  suhuB = readAverageTemp(ntcA);

  delay(1000);

  // Kontrol PWM bertingkat berdasarkan suhu
  if (suhuA >= (setpointSuhu - histerisis)) {
    // Matikan heater jika mencapai atau melebihi setpoint
    ledcWrite(pwmChannel, 0);
  } else if (suhuA >= 190 && suhuA < (setpointSuhu + histerisis)) {
    ledcWrite(pwmChannel, 69);
  } else if (setpointSuhu > 0 && suhuA >= 170) {
    ledcWrite(pwmChannel, 80);
  } else if (setpointSuhu > 0 && suhuA >= 150) {
    ledcWrite(pwmChannel, 95);
  } else if (setpointSuhu > 0 && suhuA >= 125) {
    ledcWrite(pwmChannel, 110);
  } else if (setpointSuhu > 0 && suhuA >= 100) {
    ledcWrite(pwmChannel, 175);
  } else if (setpointSuhu > 0 && suhuA < 100) {
    // Suhu di bawah 100°C - full power
    ledcWrite(pwmChannel, 255);
  }
}


void tampilkanMenu() {
  lcd.clear();
  int start = currentPage * itemsPerPage;
  for (int i = 0; i < itemsPerPage; i++) {
    int idx = start + i;
    if (idx < totalMenu) {
      lcd.setCursor(0, i);
      lcd.print(menuItems[idx]);
    }
  }
}
void monitorSuhu() {
  if (pertamaMasukKontinu) {
    lcd.clear();
    pertamaMasukKontinu = false;
  }
  lcd.setCursor(0, 0);
  lcd.print("NTC : ");
  lcd.print(suhuA);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Pulse/s : ");
  lcd.print(pulseCount);
  lcd.setCursor(0, 2);
  lcd.print("Setpoint Suhu :");
  lcd.print(setpointSuhu);
  delay(500);
}
int bacaSetpoint() {
  Setpoint = "";
  lcd.setCursor(0, 1);
  lcd.print("> ");

  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') {
        break;
      } else if (key == '*') {
        Setpoint = "0";
        lcd.clear();
        lcd.setCursor(5, 1);
        lcd.print("Keluar....");
        delay(1000);
        break;
      } else if (key >= '0' && key <= '9') {
        Setpoint += key;
        lcd.print(key);
      } else if (key == 'D') {
        if (Setpoint.length() > 0) {
          Setpoint.remove(Setpoint.length() - 1);
          lcd.setCursor(2, 1);
          lcd.print("                ");
          lcd.setCursor(2, 1);
          lcd.print(Setpoint);
        }
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  return Setpoint.toInt();
}
//BACA KHUSUS FEEDRATE
int bacaFeed() {
  feed = "";
  lcd.setCursor(0, 1);
  lcd.print("> ");

  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') {
        break;
      } else if (key == '*') {
        feed = "0";
        lcd.clear();
        lcd.setCursor(5, 1);
        lcd.print("Keluar....");
        delay(1000);
        break;
      } else if (key >= '0' && key <= '9') {
        feed += key;
        lcd.print(key);
        Serial.println(key);
      } else if (key == 'D') {
        if (feed.length() > 0) {
          feed.remove(feed.length() - 1);
          lcd.setCursor(2, 1);
          lcd.print("                ");
          lcd.setCursor(2, 1);
          lcd.print(feed);
        }
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  return feed.toInt();
}

void nilaiTersimpan() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Feedrate:");
  lcd.print(feedrate);
  lcd.setCursor(0, 2);
  lcd.print("Setpoint:");
  lcd.print(setpointSuhu);
}
void error() {
  lcd.clear();
  lcd.setCursor(3, 1);
  lcd.print("Sistem Error");
  delay(1000);
  tampilkanMenu();
}

void sistemON() {
  lcd.clear();
  lcd.setCursor(1, 1);
  lcd.print("Ekstruder Aktif");
  delay(2000);
  stepper();
}

void sistemOFF(bool off) {
  if (off) {
    beneranOFF = true;
    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("Ekstruder Berhenti");
    delay(1000);
    digitalWrite(heater, LOW);
    Serial.println("heater mati");
    modeAktif = MENU;
    isRunning = false;
  }
}

float calc(float feed) {
  float r = Dnozzle / 2;                  //jari jari
  float luasNozzle = 3.14 * pow(r, 2);    //luas nozzle
  float Fmmps = feed / 60;                //feedrate mm/s
  float volumePerS = Fmmps * luasNozzle;  //volume hdpe/s
  float pulsa = volumePerS / volume_Per_Pulse;
  return pulsa;
}

void stepper() {
  lcd.clear();
  lcd.setCursor(5, 1);
  lcd.print("Stepper");
  lcd.setCursor(6, 2);
  lcd.print("Aktif");
  delay(1000);

  pulseCount = calc(feedrate);
  if (pulseCount > 0) {
    digitalWrite(motorDir, LOW);
    isRunning = true;
    modeAktif = KONTINU;
    pertamaMasukKontinu = true;
  } else {
    isRunning = false;
  }
}
void generatePulses(float count) {
  unsigned long interval = 1000000 / count;  // Interval in microseconds
  for (int i = 0; i < count; i++) {
    digitalWrite(motorPul, HIGH);
    delayMicroseconds(interval / 2);
    digitalWrite(motorPul, LOW);
    delayMicroseconds(interval / 2);
  }
}
void turuninHDPE(float count) {
  unsigned long interval = 5000000 / count;  // Interval in microseconds
  for (int i = 0; i < count; i++) {
    digitalWrite(motorPul, HIGH);
    delayMicroseconds(interval / 2);
    digitalWrite(motorPul, LOW);
    delayMicroseconds(interval / 2);
  }
}
