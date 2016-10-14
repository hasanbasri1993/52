#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#define pin_ldr     0
#define pin_tanah   1
#define pin_battery 2
#define led_notif 13
#define jeda      300
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

char buffer[256];
//Buat ARDUINO MEGA
//GPRS gprs(51, 50, 9600);
GPRS gprs(2, 3, 9600); //TX_PIN, RX_PIN, BAUDRATE
void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(led_notif, OUTPUT);
  digitalWrite(led_notif, LOW);
  // use DHCP
  ceksim();
  while (!gprs.init()) {
    Serial.print(F("init error\r\n"));
    gagalsim();
  }
  Serial.println(F("init Sukses"));
  delay(3000);
  digitalWrite(led_notif, HIGH);

}

void loop() {
  Serial.println(F("dapat kan ip ... "));
  cekapn();
  while (!gprs.join(F("internet"))) {
    Serial.println(F("gagal masuk setting apn"));
    gagalapn();
  }
  digitalWrite(led_notif, HIGH);

  // successful DHCP
  Serial.print(F("IP Address is "));
  Serial.println(gprs.getIPAddress());
  digitalWrite(led_notif, HIGH);

  if (!gprs.connect(TCP, "184.106.153.149", 80)) {
    Serial.println(F("koneksi gagal"));
    gagalkoneksi();
  } else {
    Serial.println(F("Telah terbhubung ke thingspeak.com"));
    digitalWrite(led_notif, HIGH);

  }
  delay(500);

  Serial.println(F("membaca sensor yang terhubung..."));
  delay(500);

  int baca_cahaya = analogRead(pin_ldr);
  baca_cahaya = map(baca_cahaya, 0, 1023, 0, 100);
  int baca_tanah = analogRead(pin_tanah);
  baca_tanah = map(baca_tanah, 0, 1023, 0, 100);
  int sensorValue = analogRead(pin_battery);
  float voltage = (sensorValue * (5.0 / 1023.0)) - 0.45;

  delay(2000);



  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):

  int h = dht.readHumidity();
  int t = dht.readTemperature();

  Serial.println(F("print hasil baca sensor :"));
  Serial.print(F("Intensitas cahaya: "));
  Serial.print(baca_cahaya);
  Serial.print(F(" Kelmbaban Tanah: "));
  Serial.print(baca_tanah);
  Serial.print(F(" Suhu Sekitar: "));
  Serial.print(t);
  Serial.print(F(" Kelembaban Sekitar: "));
  Serial.println(h);
  delay(500);

  String C = String(baca_cahaya); // turn integer to string
  String T = String(baca_tanah); // turn integer to string
  String S = String(t); // turn integer to string
  String K = String(h); // turn integer to string
  String B = String(voltage); // turn integer to string

  String cmd = "GET /update?key=22TIOV8LXVYMAKX6&field1=" + C + "&field2=" + T + "&field3=" + S + "&field4=" + K + "&field5=" + B + "\r\n\r\n";
  char http_cmd[cmd.length() + 1];
  cmd.toCharArray(http_cmd, cmd.length() + 1);
  gprs.send(http_cmd, sizeof(http_cmd) - 1);

  while (true) {
    uint8_t ret = gprs.recv(buffer, sizeof(buffer) - 1);
    if (ret <= 0) {
      Serial.println(F("fetch over..."));
      break;
    }
    buffer[ret] = '\0';
    Serial.print(F("Recv: "));
    Serial.print(ret);
    Serial.print(F(" bytes: "));
    Serial.println(buffer);
    berhasilupload();
  }
  gprs.close();
  gprs.disconnect();

  delay(20000);
}
void berhasilupload() {
  digitalWrite(led_notif, HIGH); delay(100); digitalWrite(led_notif, LOW); delay(100);
  digitalWrite(led_notif, HIGH); delay(100); digitalWrite(led_notif, LOW); delay(100);
  digitalWrite(led_notif, HIGH); delay(100); digitalWrite(led_notif, LOW); delay(100);
  digitalWrite(led_notif, HIGH); delay(100); digitalWrite(led_notif, LOW); delay(100);
  digitalWrite(led_notif, HIGH);
}


void ceksim() {
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW); delay(jeda);
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW);
}

void cekapn() {
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW); delay(jeda);
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW); delay(jeda);
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW);
}


void gagalsim() {
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW); delay(jeda);
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW); delay(jeda);
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW); delay(jeda);
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW);
}

void gagalapn() {
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW); delay(jeda);
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW); delay(jeda);
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW); delay(jeda);
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW); delay(jeda);
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW);
}

void gagalkoneksi() {

  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW); delay(jeda);
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW); delay(jeda);
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW); delay(jeda);
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW); delay(jeda);
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW); delay(jeda);
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW); delay(jeda);
  digitalWrite(led_notif, HIGH); delay(jeda); digitalWrite(led_notif, LOW);
}
