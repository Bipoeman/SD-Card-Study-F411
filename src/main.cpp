#include <Arduino.h>
#include "SdFat.h"
#include <SPI.h>
HardwareSerial Serial1(PB7,PB6);

#define SD_CS_PIN PB12
static SPIClass mySPI2(PB15, PB14, PB13, SD_CS_PIN);
#define SD2_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SD_SCK_MHZ(18), &mySPI2)
 
SdFat SD;
void printDirectory(File dir, int numTabs);
 
void setup() {
  // Open serial communications and wait for port to open:
  Serial1.begin(115200);
  while (!Serial1) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
 
 
  Serial1.print("\nInitializing SD card...");
 
  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!SD.begin(SD2_CONFIG)) {
  // if (!SD.begin(SD_CS_PIN)) {
    Serial1.println("initialization failed. Things to check:");
    Serial1.println("* is a card inserted?");
    Serial1.println("* is your wiring correct?");
    Serial1.println("* did you change the chipSelect pin to match your shield or module?");
    while (1);
  } else {
    Serial1.println("Wiring is correct and a card is present.");
  }
 
  uint32_t cardSize = SD.card()->sectorCount();
 
  // print the type of card
  Serial1.println();
  Serial1.print("Card type:         ");
  switch (SD.card()->type()) {
  case SD_CARD_TYPE_SD1:
    Serial1.println(F("SD1"));
    break;
  case SD_CARD_TYPE_SD2:
    Serial1.println(F("SD2"));
    break;
 
  case SD_CARD_TYPE_SDHC:
    if (cardSize < 70000000) {
      Serial1.println(F("SDHC"));
    } else {
      Serial1.println(F("SDXC"));
    }
    break;
 
  default:
    Serial1.println(F("Unknown"));
  }
 
 
//  print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial1.print("Volume type is:    FAT");
  Serial1.println(int(SD.vol()->fatType()), DEC);
 
  Serial1.print("Card size:  ");
  Serial1.println((float) 0.000512*cardSize);
 
  Serial1.print("Total bytes: ");
  Serial1.println(0.000512*SD.vol()->clusterCount()*SD.sectorsPerCluster());
 
  Serial1.print("Free bytes: ");
  Serial1.println(0.000512*SD.vol()->freeClusterCount()*SD.sectorsPerCluster());
 
  File dir =  SD.open("/");
  printDirectory(dir, 0);
  
  File image = SD.open("Threedom_Small.jpg",FILE_READ);
  if (image){
    
    Serial1.println("Threedom_Small.jpg");
    char hex[] = "0123456789ABCDEF";
    int count = 0;
    char hexPrint[50];
    while (image.available()) {
      int buff = sprintf(hexPrint,"%02X ",image.read());
      Serial1.print(hexPrint);
      count++;
      if (count >= 50){
        Serial1.println();
        count = 0;
      }
      
    }
    // close the file:
    image.close();
    Serial1.println();
    Serial1.println("File Closed");
  }
  else {
    // if the file didn't open, print an error:
    Serial1.println("error opening Threedom_Small.jpg");
  }
}
 
void loop(void) {

}
 
void printDirectory(File dir, int numTabs) {
  while (true) {
 
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial1.print('\t');
    }
    entry.printName(&Serial1);
 
    if (entry.isDirectory()) {
      Serial1.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial1.print("\t\t");
      Serial1.print(entry.size(), DEC);
      uint16_t pdate;
      uint16_t ptime;
      entry.getModifyDateTime(&pdate, &ptime);
 
      Serial1.printf("\tLAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", FS_YEAR(pdate), FS_MONTH(pdate), FS_DAY(pdate), FS_HOUR(ptime), FS_MINUTE(ptime), FS_SECOND(ptime));
    }
    entry.close();
  }
}