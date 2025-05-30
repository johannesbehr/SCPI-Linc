#include "scpi_parser.h"
#include <avr/wdt.h>

#define BUFFER_SIZE 128
static char inputBuffer[BUFFER_SIZE];
static uint8_t inputPos = 0;
static uint8_t currentI2CAddress = 0x00;

void scpi_init() {
  inputPos = 0;
}

static void printVersion() {
    Serial.println(VERSION);
    Serial.println(GIT_URL);
    return;
}

static void printHelp() {
  Serial.println("SCPI-Linc Hilfe:");
  Serial.println("*IDN?                      - Geräteidentifikation");
  Serial.println("*RST                       - Gerät zurücksetzen");
  Serial.println("I2C:ADDR <hex>             - I2C-Adresse setzen");
  Serial.println("I2C:WRITE <bytes>          - I2C-Daten schreiben (hex)");
  Serial.println("I2C:READ? <count>          - I2C-Daten lesen");
  Serial.println("I2C:SCAN?                  - I2C-Bus nach Geräten durchsuchen");
  Serial.println("SPI:WRITE <bytes>          - SPI-Daten schreiben/lesen (hex)");
  Serial.println("SPI:READ? <count>          - SPI-Daten lesen");
}

static void handleCommand(const String& cmdLine);

void scpi_loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputPos > 0) {
        inputBuffer[inputPos] = '\0';
        handleCommand(String(inputBuffer));
        inputPos = 0;
      }
    } else if (inputPos < BUFFER_SIZE - 1) {
      inputBuffer[inputPos++] = c;
    }
  }
}

static void handleCommand(const String& cmdLine) {
  String line = cmdLine;
  line.trim();

  // Basisbefehle
  if (line.equalsIgnoreCase("*IDN?")) {
    Serial.println(VERSION);
    return;
  }
  if (line.equalsIgnoreCase("*RST")) {
    //currentI2CAddress = 0x00;
    Serial.println("System reset...");
    wdt_enable(WDTO_1S);
    while(1); // Warten auf Watchdog...
    return;
  }
  if (line.equalsIgnoreCase("HELP") || line.equalsIgnoreCase("?")) {
    printHelp();
    return;
  }

   bool recognized = false;

  // Zerlegen in Teilbefehle durch Semikolon
  int start = 0;
  while (start < line.length()) {
    int end = line.indexOf(';', start);
    if (end == -1) end = line.length();
    String part = line.substring(start, end);
    part.trim();
    
    // Weiter zerlegen durch Doppelpunkte
    int subStart = 0;
    String root, subcmd;
    int colon = part.indexOf(':');
    if (colon >= 0) {
      root = part.substring(0, colon);
      subcmd = part.substring(colon + 1);
    } else {
      root = part;
      subcmd = "";
    }

    root.toUpperCase();
    subcmd.trim();

    // I2C-Befehle
    if (root == "I2C") {
      recognized = true;
      if (subcmd.startsWith("ADDR")) {
        int space = subcmd.indexOf(' ');
        String value = subcmd.substring(space + 1);
        currentI2CAddress = strtol(value.c_str(), NULL, 16);
        Serial.print("I2C address set to 0x");
        Serial.println(currentI2CAddress, HEX);

      } else if (subcmd.startsWith("WRITE")) {
        int space = subcmd.indexOf(' ');
        String data = subcmd.substring(space + 1);
        Wire.beginTransmission(currentI2CAddress);
        while (data.length() > 0) {
          int sep = data.indexOf(' ');
          String byteStr = sep >= 0 ? data.substring(0, sep) : data;
          uint8_t val = strtol(byteStr.c_str(), NULL, 16);
          Wire.write(val);
          if (sep < 0) break;
          data = data.substring(sep + 1);
          data.trim();
        }
        Wire.endTransmission();
        Serial.println("I2C write OK");

      } else if (subcmd.startsWith("READ?")) {
        int space = subcmd.indexOf(' ');
        String countStr = subcmd.substring(space + 1);
        int count = countStr.toInt();
        Wire.requestFrom(currentI2CAddress, (uint8_t)count);
        while (Wire.available()) {
          uint8_t b = Wire.read();
          Serial.print("0x");
          if (b < 16) Serial.print("0");
          Serial.print(b, HEX);
          Serial.print(" ");
        }
        Serial.println();
      }else if (subcmd.equalsIgnoreCase("SCAN?")) {
        Serial.println("Scanning I2C bus...");
        for (uint8_t addr = 1; addr < 127; addr++) {
          Wire.beginTransmission(addr);
          uint8_t error = Wire.endTransmission();
          if (error == 0) {
            Serial.print("Found device at 0x");
            if (addr < 16) Serial.print("0");
            Serial.println(addr, HEX);
          }
        }
        Serial.println();
        Serial.println("Scan complete.");
      } else {
        Serial.println("Unbekannter I2C-Befehl.");
        printHelp();
      }


    // SPI-Befehle
    } else if (root == "SPI") {
      recognized = true;
      if (subcmd.startsWith("WRITE")) {
        int space = subcmd.indexOf(' ');
        String data = subcmd.substring(space + 1);
        while (data.length() > 0) {
          int sep = data.indexOf(' ');
          String byteStr = sep >= 0 ? data.substring(0, sep) : data;
          uint8_t out = strtol(byteStr.c_str(), NULL, 16);
          uint8_t in = SPI.transfer(out);
          Serial.print("-> 0x");
          if (in < 16) Serial.print("0");
          Serial.println(in, HEX);
          if (sep < 0) break;
          data = data.substring(sep + 1);
          data.trim();
        }
      } else if (subcmd.startsWith("READ?")) {
        int space = subcmd.indexOf(' ');
        String countStr = subcmd.substring(space + 1);
        int count = countStr.toInt();
        for (int i = 0; i < count; i++) {
          uint8_t in = SPI.transfer(0x00);
          Serial.print("0x");
          if (in < 16) Serial.print("0");
          Serial.print(in, HEX);
          Serial.print(" ");
        }
        Serial.println();
      }else {
        Serial.println("Unbekannter SPI-Befehl.");
        printHelp();
      }
    }

    start = end + 1;
  }

  if (!recognized) {
    Serial.println("Unbekannter Befehl. Geben Sie 'HELP' ein für eine Liste gültiger Befehle.");
  }
}
