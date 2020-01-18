#define _MAIN_OBD_
#ifdef _MAIN_OBD_

#include <FreematicsPlus.h>

#define PIN_LED 4

FreematicsESP32 sys;
COBD obd;
bool connected = false;
unsigned long count = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);
  delay(1000);
  digitalWrite(PIN_LED, LOW);
  Serial.begin(115200);

  // initializations
  while (!sys.begin());
  Serial.print("Firmware: V");
  Serial.println(sys.version);
  obd.begin(sys.link);
}

void loop() {
  digitalWrite(PIN_LED, HIGH);
  // put your main code here, to run repeatedly:
  if (!connected) {
    digitalWrite(PIN_LED, HIGH);
    Serial.print("Connecting to OBD...");
    if (obd.init()) {
      Serial.println("OK");
      connected = true;
    } else {
      Serial.println();
    }
    digitalWrite(PIN_LED, LOW);
    return;
  }

  int value;
  Serial.print('[');
  Serial.print(millis());
  Serial.print("] #");
  Serial.print(count++);
  if (obd.readPID(PID_RPM, value)) {
    Serial.print(" RPM:");
    Serial.print(value);
  }
  if (obd.readPID(PID_SPEED, value)) {
    Serial.print(" SPEED:");
    Serial.print(value);
  }

  Serial.print(" BATTERY:");
  Serial.print(obd.getVoltage());
  Serial.print('V');

  Serial.print(" CPU TEMP:");
  Serial.print(readChipTemperature());
  Serial.println();
  if (obd.errors > 2) {
    Serial.println("OBD disconnected");
    connected = false;
    obd.reset();
  }
  digitalWrite(PIN_LED, LOW);

  delay(100);
}

#endif