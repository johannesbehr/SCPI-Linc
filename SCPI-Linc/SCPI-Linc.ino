// main.ino
#include "scpi_parser.h"
#include <avr/wdt.h>

void setup() {
  wdt_enable(WDTO_2S);
  Serial.begin(115200);
  Wire.begin();
  SPI.begin();
  scpi_init();
  Serial.println("SCPI-Linc Interpreter gestartet.");
}

void loop() {
  wdt_reset(); // Watchdog regelmäßig zurücksetzen
  scpi_loop();
}
