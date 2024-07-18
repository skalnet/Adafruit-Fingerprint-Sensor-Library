#include <Adafruit_Fingerprint.h>


#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
//SoftwareSerial mySerial(2, 3);
SoftwareSerial mySerial(A3, A2); // For debugging
SoftwareSerial fingerSerial(A0, A1); // Fingerprint reader

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif

#define DEBUG 1
#if DEBUG==1
#define debugPrint(x); mySerial.print(x);
#define debugPrintln(x); mySerial.println(x);
#define debugPrintHex(x); mySerial.println(x, HEX);
#else
#define debugPrint(x); 
#define debugPrintln(x);
#define debugPrintHex(x); 
#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

uint8_t id;

void setup()
{
  //Serial.begin(9600);
  mySerial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  //mySerial.println("\n\nAdafruit Fingerprint sensor enrollment");
  debugPrintln("\n\nAdafruit Fingerprint sensor enrollment");

  // set the data rate for the sensor serial port
  //finger.begin(57600);
  finger.begin(19200);
  if (finger.verifyPassword()) {
    //Serial.println("Found fingerprint sensor!");
    debugPrintln("Found fingerprint sensor!");
  } else {
    //Serial.println("Did not find fingerprint sensor :(");
    debugPrintln("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  //Serial.println(F("Reading sensor parameters"));
  debugPrintln(("Reading sensor parameters"));

  finger.getParameters();
  //Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  //Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  //Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  //Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  //Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  //Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  //Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
  debugPrint(("Status: 0x")); debugPrintln(finger.status_reg);
  debugPrint(("Sys ID: 0x")); debugPrintln(finger.system_id);
  debugPrint(("Capacity: ")); debugPrintln(finger.capacity);
  debugPrint(("Security level: ")); debugPrintln(finger.security_level);
  debugPrint(("Device address: ")); debugPrintln(finger.device_addr);
  debugPrint(("Packet len: ")); debugPrintln(finger.packet_len);
  debugPrint(("Baud rate: ")); debugPrintln(finger.baud_rate);
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop()                     // run over and over again
{
  //Serial.println("Ready to enroll a fingerprint!");
  //Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  debugPrintln("Ready to enroll a fingerprint!");
  debugPrintln("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  id = readnumber();
    //Serial.print("Enrolling ID #");
    //Serial.println(id);
    debugPrint("Enrolling ID #");
    debugPrintln(id);
  if (id == 0) {// ID #0 not allowed, try again!
    //Serial.println("ID = 0 so enrolling at first free position");
    debugPrintln("ID = 0 so enrolling at first free position");
  }

  while (! getFingerprintEnroll() );
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  //Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  debugPrint("Waiting for valid finger to enroll as #"); debugPrintln(id);
  uint8_t aE = 0xff;
  uint8_t prevAe;
  while (aE != FINGERPRINT_OK) {
    if (aE != prevAe) {
      debugPrint(" AE is now: "); debugPrintln(aE);
      prevAe = aE;
    }
    aE = finger.autoEnroll();
    if (aE == 0x26) break; // Time-out is reached
  }
  debugPrint(finger.fingerID) debugPrint(" written with result: "); debugPrintln(aE);
  if (aE == FINGERPRINT_OK && finger.fingerID > 0) {
    debugPrintln("Succes!");
  }
  else {
    debugPrintln("Failure");
  }

  //if (mode == 0) mode = 99;
  delay(5000);
  debugPrintln("Exit auto enroll");

  return true;
}
