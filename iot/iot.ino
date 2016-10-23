#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, 10, NEO_GRB + NEO_KHZ800);

#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
GPRS gprs(2, 3, 9600); //TX_PIN, RX_PIN, BAUDRATE
char getwaktu[25];
char buffer[64];

#include <SPI.h>
#include <SD.h>
File myFile;

#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


unsigned long intervallog = 5000;   // the time we need to wait
unsigned long previousMillisLog = 0; // millis() returns an unsigned long.

unsigned long intervalsycn = 20000;   // the time we need to wait
unsigned long previousMillisSycn = 0; // millis() returns an unsigned long.

/*
  #include <TaskScheduler.h>
   Scheduler runner;
   void bacasensor();
   void kirimserver();
   Task taskbacasensor(11000, TASK_FOREVER, &bacasensor);//jalan setiap 11 detik sekali
   Task taskkirimserver(1800000, TASK_FOREVER, &kirimserver);//jalan setiap 11 detik sekali
*/
String uniq_device = "halaman1";
#define pin_ldr     0 //A0
#define pin_tanah   1 //A1
#define pin_battery 2 //A2

#define led_notif 8
#define jeda      300

void setup() {
  Serial.begin(9600);

  if (!SD.begin(5)) {
    Serial.println(F("gagal sdcard."));
    return;
  }
  Serial.println(F("SD CARD OKE!!!!!!!!"));

  dht.begin();
  pinMode(led_notif, OUTPUT);
  digitalWrite(led_notif, LOW);

  // use DHCP
  ceksim();
  while (!gprs.init()) {
    Serial.println(F("gagal cek sim card"));
    gagalsim();
  }
  delay(3000);
  digitalWrite(led_notif, HIGH);

  pixels.begin();
  pixels.Color(255, 0, 0); // Moderately bright green color.
  pixels.show();
  /*runner.init();
     runner.addTask(taskbacasensor);
     runner.addTask(taskkirimserver);
     delay(5000);
     taskbacasensor.enable();
     taskkirimserver.enable();*/

}

void loop() {
  //runner.execute();
  //delay((LOG_INTERVAL - 1) - (millis() % LOG_INTERVAL));

  unsigned long currentMillis = millis();
  unsigned long currentMillis1 = millis();


  // time to toggle LED on Pin 12?
  if ((unsigned long)(currentMillis - previousMillisSycn) >= intervalsycn) {
    bacalog ();
    // save current time to pin 12's previousMillis
    previousMillisSycn = currentMillis;
  }

  // time to toggle LED on Pin 13?
  if ((unsigned long)(currentMillis1 - previousMillisLog) >= intervallog) {
    bacasensor();
    // save current time to pin 12's previousMillis
    previousMillisLog = currentMillis1;
  }


}

void kirimserver () {
  cekapn();
  while (!gprs.join(F("internet"))) {
    Serial.println(F("gagal masuk setting apn"));
    gagalapn();
  }
  digitalWrite(led_notif, HIGH);

  // successful DHCP
  Serial.println(gprs.getIPAddress());
  digitalWrite(led_notif, HIGH);

  if (!gprs.connect(TCP, "184.106.153.149", 80)) {
    Serial.println(F("koneksi gagal"));
    gagalkoneksi();
  } else {
    digitalWrite(led_notif, HIGH);

  }

  String cmd = "SDfsdfsdf";//"GET /update?key=22TIOV8LXVYMAKX6&field1=" + C + "&field2=" + T + "&field3=" + S + "&field4=" + K + "&field5=" + B + "\r\n\r\n";

  //client.publish("iot/live", "device-86a36925d58960bdd6e3d1c9d883bc51"); //Masukkan device id

  String pubString  = "{\"code\":\"815ecd3ed3818462038e91ac00467620:6984188513627be3422e357115c62a53\",";//masukkan username dan password anda sesuai yang ada pada detail device
  pubString += "\"attributes\": {";
  pubString += "\"h1\":\"" + String("k") + "\",\"";
  pubString += "t1\":\"" + String("t") + "\",\"";
  pubString += "watt\":\"" + String("power") + "\",\"";
  pubString += "current\":\"" + String("Irms") + "\",\"";
  pubString += "voltb\":\"" + String("val2") + "\",\"";
  pubString += "volt\":\"" + String("vpln") + "\"";
  pubString += "}}";
  //pubString.toCharArray(buffer, pubString.length() + 1);
  //client.publish("iot/data", buffer);

  char http_cmd[pubString.length() + 1];
  pubString.toCharArray(http_cmd, pubString.length() + 1);
  gprs.send(http_cmd, sizeof(http_cmd) - 1);

  berhasilupload();
  gprs.close();
  gprs.disconnect();
}

String ambiljam () {

  gprs.getDateTime(getwaktu);
  String cmd(getwaktu);
  return cmd;
}

void bacasensor() {

  //panggil fungsi bacasensor
  //Serial.println(F("membaca sensor yang terhubung..."));

  int baca_cahaya = analogRead(pin_ldr); baca_cahaya = map(baca_cahaya, 0, 1023, 0, 100);
  int baca_tanah = analogRead(pin_tanah); baca_tanah = map(baca_tanah, 0, 1023, 0, 100);
  int sensorValue = analogRead(pin_battery); float voltage = (sensorValue * (5.0 / 1023.0)) - 0.45;
  byte h = dht.readHumidity();
  byte t = dht.readTemperature();

  String waktu = ambiljam ();

  /*Serial.print(F("Jam: ")); Serial.print(waktu);
     Serial.print(F(" uniq_device: ")); Serial.print(uniq_device);
     Serial.print(F(" Intensitas cahaya: ")); Serial.print(baca_cahaya);
     Serial.print(F(" Kelembaban Tanah: ")); Serial.print(baca_tanah);
     Serial.print(F(" Suhu Sekitar: ")); Serial.print(t);
     Serial.print(F(" Kelembaban Sekitar: ")); Serial.print(h);
     Serial.print(F(" Battery: ")); Serial.print(voltage);
     Serial.println();

     /*String C = String(baca_cahaya);
     String T = String(baca_tanah);
     String S = String(t);
     String K = String(h);
     String B = String(voltage);*/

  String logdata = waktu;
  logdata += "," + uniq_device;
  logdata += "," + String(baca_cahaya);
  logdata += "," + String(baca_tanah);
  logdata += "," + String(t);
  logdata += "," + String(h);
  logdata += "," + String(voltage);
  logdata += "\r\n";

  //String cmd = "GET /update?key=22TIOV8LXVYMAKX6&field1=" + C + "&field2=" + T + "&field3=" + S + "&field4=" + K + "&field5=" + B + "\r\n\r\n";

  myFile = SD.open("log5.txt", FILE_WRITE);
  if (myFile) {
    myFile.println(logdata);
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println(F("gagal nulis log"));
  }
}

void bacalog () {
  myFile = SD.open("log5.txt");
  if (myFile) {
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("gagal baca log");
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
