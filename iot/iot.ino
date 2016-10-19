#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
File myFile;

#define pin_ldr     0
#define pin_tanah   1
#define pin_battery 2

#define led_notif 13
#define jeda      300

#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#include <TaskScheduler.h>
Scheduler runner;

// Callback methods prototypes
void bacasensor();
Task t2(5000, TASK_FOREVER, &bacasensor);

//char buffer[64];
GPRS gprs(2, 3, 9600); //TX_PIN, RX_PIN, BAUDRATE
void setup() {
  Serial.begin(9600);
  Serial.print(F("Cek SD..."));

  if (!SD.begin(5)) {
    Serial.println(F("gagal sdcard."));
    // don't do anything more:
    return;
  }
  Serial.println(F("sdcard ok!!!"));

  dht.begin();
  pinMode(led_notif, OUTPUT);
  digitalWrite(led_notif, LOW);

  // use DHCP
  ceksim();
  while (!gprs.init()) {
    Serial.println(F("gagal cek sim card"));
    gagalsim();
  }
  Serial.println(F("sim card ok"));
  delay(3000);
  digitalWrite(led_notif, HIGH);

  runner.init();
  Serial.println("Initialized scheduler");
  runner.addTask(t2);
  Serial.println("added t2");

  delay(5000);
  t2.enable();
  Serial.println("Enabled t2");

}

void loop() {

  runner.execute();

}

void kirimserver () {
  Serial.println(F("dapat kan ip."));
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

  String cmd = "GET /update?key=22TIOV8LXVYMAKX6&field1=" + C + "&field2=" + T + "&field3=" + S + "&field4=" + K + "&field5=" + B + "\r\n\r\n";
  char http_cmd[cmd.length() + 1];
  cmd.toCharArray(http_cmd, cmd.length() + 1);
  gprs.send(http_cmd, sizeof(http_cmd) - 1);
  berhasilupload();
  gprs.close();
  gprs.disconnect();
}
void bacasensor() {

  //panggil fungsi bacasensor
  Serial.println(F("membaca sensor yang terhubung..."));

  int baca_cahaya = analogRead(pin_ldr);
  baca_cahaya = map(baca_cahaya, 0, 1023, 0, 100);
  int baca_tanah = analogRead(pin_tanah);
  baca_tanah = map(baca_tanah, 0, 1023, 0, 100);
  int sensorValue = analogRead(pin_battery);
  float voltage = (sensorValue * (5.0 / 1023.0)) - 0.45;

  byte h = dht.readHumidity();
  byte t = dht.readTemperature();


  Serial.println(F("print hasil baca sensor :"));
  Serial.print(F("Intensitas cahaya: "));
  Serial.print(baca_cahaya);
  Serial.print(F(" "));
  Serial.print(analogRead(pin_ldr));
  Serial.print(F(" Kelmbaban Tanah: "));
  Serial.print(baca_tanah);
  Serial.print(F(" Suhu Sekitar: "));
  Serial.print(t);
  Serial.print(F(" Kelembaban Sekitar: "));
  Serial.print(h);
  Serial.print(F(" Battery: "));
  Serial.println(voltage);

  String C = String(baca_cahaya); // turn integer to string
  String T = String(baca_tanah); // turn integer to string
  String S = String(t); // turn integer to string
  String K = String(h); // turn integer to string
  String B = String(voltage); // turn integer to string

  String cmd = "GET /update?key=22TIOV8LXVYMAKX6&field1=" + C + "&field2=" + T + "&field3=" + S + "&field4=" + K + "&field5=" + B + "\r\n\r\n";

  myFile = SD.open("datalog5.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print(F("Writing to datalog.txt..."));
    myFile.println(cmd);
    // close the file:
    myFile.close();
    Serial.println(F("done."));
  } else {
    // if the file didn't open, print an error:
    Serial.println(F("error opening test.txt"));
  }





}

void bacalog () {
  // re-open the file for reading:
  myFile = SD.open("datalog5.txt");
  if (myFile) {
    Serial.println("datalog.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
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
