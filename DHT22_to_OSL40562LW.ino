#include <DHT.h>
#include <Ticker.h>

#define DHTPIN 0
#define DHTTYPE DHT22
#define TIME_INTERVAL_MILLI 10000

const int anode_pins[] = {12, 2, 15, 13}; // D0, D1, D2, D3
const int cathode_pins[] = {16, 5, 14, 4}; // 4桁目, 3桁目, 2桁目, 1桁目
const int number_of_anode_pins = sizeof(anode_pins);
const int number_of_cathode_pins = sizeof(cathode_pins);

// http://akizukidenshi.com/catalog/g/gI-03673/
// http://akizukidenshi.com/catalog/g/gI-08878/
// http://garretlab.web.fc2.com/arduino/introduction/beginning_with_7segment_led/
const int digits[] = {
  0b0000, // 0
  0b0001, // 1
  0b0010, // 2
  0b0011, // 3
  0b0100, // 4
  0b0101, // 5
  0b0110, // 6
  0b0111, // 7
  0b1000, // 8
  0b1001, // 9
};

// https://github.com/adafruit/DHT-sensor-library/blob/master/examples/DHTtester/DHTtester.ino
DHT dht(DHTPIN, DHTTYPE);

// http://qiita.com/exabugs/items/34aab51eb2d35207629b
Ticker ticker;

float hic_now = 0;
unsigned long nextTime = 0;

void display_number(int number) {
  for (int i = 0; i < number_of_anode_pins; i++) {
    digitalWrite(anode_pins[i], digits[number] & (1 << i) ? HIGH : LOW );
  }
}

void clear_segments() {
  for (int i = 0; i < number_of_anode_pins; i++) {
    digitalWrite(anode_pins[i], LOW);
  }
}

void display_numbers(int number) {
  for (int i = 0; i < number_of_cathode_pins; i++) {
    digitalWrite(cathode_pins[i], LOW);
    display_number(number % 10);
    delay(1);
    clear_segments();
    digitalWrite(cathode_pins[i], HIGH);
    number = number / 10;
  }
}

float getHeatIndex() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  Serial.print("Humidity: ");
  Serial.println(h);
  Serial.print("Temperature: ");

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return 9999;
  }

  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.println(" *C ");

  return hic;
}

void setup() {
  Serial.println("setup start.");
  // setup for led
  for (int i = 0; i < number_of_anode_pins; i++) {
    pinMode(anode_pins[i], OUTPUT);
  }
  for (int i = 0; i < number_of_cathode_pins; i++) {
    pinMode(cathode_pins[i], OUTPUT);
    digitalWrite(cathode_pins[i], HIGH);
  }

  // setup for dht22
  Serial.begin(115200);
  dht.begin();

  Serial.println("setup done.");
}

void loop() {
  unsigned long nowTime = millis();

  // 時計あふれリセット問題対策
  if (nextTime > ( nowTime + TIME_INTERVAL_MILLI) && TIME_INTERVAL_MILLI > nowTime) {
    nextTime = nowTime + TIME_INTERVAL_MILLI;
  }

  if (nextTime < nowTime ) {
    hic_now = getHeatIndex();
    nextTime = nowTime + TIME_INTERVAL_MILLI;
  }
  
  // 表示
  display_numbers(hic_now);
}
