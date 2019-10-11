//Name: Nic.h
//Author: Nic Morgan
//Date:18/09/2019
//DESC: HLibrary of custom functions to be implemented for PN5180 NFC reader on ISO15693 Tags. 

#include <Debug.h>
#include <PN5180.h>
#include <PN5180ISO15693.h>
#include <SPI.h>
#include  <EEPROM.h>
#include "Nic.h"

#define WRITE_ENABLED 1
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_NANO)

#define PN5180_NSS  53
#define PN5180_BUSY 48
#define PN5180_RST  49

#elif defined(ARDUINO_ARCH_ESP32)

#define PN5180_NSS  16
#define PN5180_BUSY 5
#define PN5180_RST  17

#else
#define PN5180_NSS  53
#define PN5180_BUSY 48
#define PN5180_RST  49
#endif

//Nic::Nic(String myString) {
//	_myString = myString;
//}
//
//String Nic::playMyLCD(bool bigorse) {
//	_bigorse = bigorse;
//	if (_bigorse == true){ 
//	  _myStringArray = _myString;}
//		else {_myStringArray = "false alarm";}
//		return _myStringArray;
//}
//*****************************************************************************************************************************************************
//******************************************************************************************************************************************************
void Nic::scanEeprom(int &addyPointer, int &eepromArraySize, uint8_t &numberTags, int &sizeTagUID) {   
  struct tagLibraryInfo tagInfo;
  EEPROM.get(0,tagInfo);
  addyPointer = tagInfo._addyPointer;
  eepromArraySize = tagInfo._eepromArraySize;
  numberTags = tagInfo._numberTags;
  sizeTagUID = tagInfo._sizeTagUID;
  Serial.print("\naddresspointer: ");
  Serial.println(tagInfo._addyPointer, DEC);
  Serial.print("UID array size is: "); Serial.print(tagInfo._eepromArraySize); Serial.println(" Bytes");
  Serial.print("Number of NFC tags in library: "); Serial.println(tagInfo._numberTags);
  Serial.print("Size of each tag UID (bytes): "); Serial.println(tagInfo._sizeTagUID);
  Serial.print('\n');
     Serial.println(F("Tag UID library:"));
	
       for(int j = 0; j<numberTags; j++) {     // Read each of the tag UIDs in the library and compare for match against currently scanned tag
	 uint8_t tempCount = 0;
	 uint8_t uidTemp[8]; // Create temporary array for reading a tag UID from arduino EEPROM
	 struct eepromUIDread read1;
	 EEPROM.get(16+(j*sizeTagUID), read1);			// Read the next UID in the library
	 for(int i=0; i<8; i++){
	 uidTemp[i] = read1.tempUIDread [i];
     }
	 	  for (int k = 0; k < 8; k++) {
      Serial.print(uidTemp[7 - k], HEX); // LSB is first   //print RFID tag 8-character hex ID to serial monitor
      if (k < 2) Serial.print(":");
    }
	Serial.print('\n');
	  }	 
	  Serial.print('\n');
 delay(3500);
}
//*****************************************************************************************************************************************************
//******************************************************************************************************************************************************
void Nic::creatNewLibrary(int &uidPointer, int &size, uint8_t &tags, int sizeTagUID) {

	size = 0; tags = 0; sizeTagUID=8; // Want to update library info as new tags are added but creating new library no so 
		Serial.println("Clearing any existing tag UIDs from the library......");
		delay(2000);
		for (int i=16; i<uidPointer ;i++) { // EEPROM has 4028 address bytes but we only need to erase those already written to
		EEPROM.update(i, 0xFF);
		}
	uidPointer = 16; // Once old UIDs have been cleared bring pointer back to end of configuration bytes in EEPROM

PN5180ISO15693 nfc(PN5180_NSS, PN5180_BUSY, PN5180_RST);

//void setup() { //DON't NOT NEED 'void setup()' AS this is part a source file now not a sketch....and will only run once as required.
  
  Serial.println(F("=================================="));
  Serial.println(F("Uploaded: " __DATE__ " " __TIME__));
  Serial.println(F("PN5180 ISO15693 Demo Sketch"));

  nfc.begin();

  Serial.println(F("----------------------------------"));
  Serial.println(F("PN5180 Hard-Reset..."));
  nfc.reset();

  Serial.println(F("----------------------------------"));
  Serial.println(F("Reading product version..."));
  uint8_t productVersion[2];
  nfc.readEEprom(PRODUCT_VERSION, productVersion, sizeof(productVersion));
  Serial.print(F("Product version="));
  Serial.print(productVersion[1]);
  Serial.print(".");
  Serial.println(productVersion[0]);
 delay(2000);
 
  if (0xff == productVersion[1]) { // if product version 255, the initialization failed
    Serial.println(F("Initialization failed!?"));
    Serial.println(F("Press reset to restart..."));
    Serial.flush();
    exit(-1); // halt
  }
  uint32_t rfStatus;
  nfc.readRegister(RF_STATUS, &rfStatus);
  uint32_t rxStatus;
  nfc.readRegister(0x1E, &rxStatus);
  uint32_t AGCconfig;


  Serial.println(F("----------------------------------"));
  Serial.println(F("Reading firmware version..."));
  uint8_t firmwareVersion[2];
  nfc.readEEprom(FIRMWARE_VERSION, firmwareVersion, sizeof(firmwareVersion));
  Serial.print(F("Firmware version="));
  Serial.print(firmwareVersion[1]);
  Serial.print(".");
  Serial.println(firmwareVersion[0]);

  Serial.println(F("----------------------------------"));
  Serial.println(F("Reading EEPROM version..."));
  uint8_t eepromVersion[2];
  nfc.readEEprom(EEPROM_VERSION, eepromVersion, sizeof(eepromVersion));
  Serial.print(F("EEPROM version="));
  Serial.print(eepromVersion[1]);
  Serial.print(".");
  Serial.println(eepromVersion[0]);
delay(3000);
abort;
  /*
    Serial.println(F("----------------------------------"));
    Serial.println(F("Reading IRQ pin config..."));
    uint8_t irqConfig;
    nfc.readEEprom(IRQ_PIN_CONFIG, &irqConfig, 1));
    Serial.print(F("IRQ_PIN_CONFIG=0x"));
    Serial.println(irqConfig, HEX);
    Serial.println(F("----------------------------------"));
    Serial.println(F("Reading IRQ_ENABLE register..."));
    uint32_t irqEnable;
    nfc.readRegister(IRQ_ENABLE, &irqEnable));
    Serial.print(F("IRQ_ENABLE=0x"));
    Serial.println(irqConfig, HEX);
  */

  Serial.println(F("----------------------------------"));
  Serial.println(F("Enable RF field..."));
  nfc.setupRF();
  // PN5180.readRegister(AGC_CONFIG, uint32_t reg1val);

bool randVar = false ; //create arbitrary boolean flag to allow a while loop to occur below
 uint32_t loopCnt = 0;
//while(randVar == false) { // THIS REPLICATES A MAIN LOOP in a Sketch i pulled this from
 
  Serial.println();
  bool errorFlag = false; //show state of interrupt led for button push
  delay (1500);
  if (errorFlag) {
    uint32_t irqStatus = nfc.getIRQStatus();
    showIRQStatus(irqStatus);
  
    if (0 == (RX_SOF_DET_IRQ_STAT & irqStatus)) { // no card detected
      Serial.println(F("*** No card detected!"));
    }

    nfc.reset();
    nfc.setupRF();

    errorFlag = false;
  }

  Serial.println(F("----------------------------------"));
  Serial.print(F("Loop #"));
  Serial.println(loopCnt++);

  // *******     NIC's addition: Create UID library for comparing future UID addressed commands     ***********************
 uint8_t exitCount = 0;   				// Initiate counter for exit button 
 while (digitalRead(2) == true) { 		// Keep sending UIDs to library if button not pressed 3 times while EEPROM loop is counting
   delay(2000);
  uint8_t uid[8];                                        // uid[8] is an 8 byte array to read an RFID tag's UID
    ISO15693ErrorCode rc = nfc.getInventory(uid);          // Read inventory
 
    if (ISO15693_EC_OK != rc) {                            // Check for read inventory error & return if true
      Serial.print(F("Error in getInventory: "));
      Serial.println(nfc.strerror(rc));
      errorFlag = true;
      continue;
    }
	  Serial.print(F("Inventory successful, UID = "));
    for (int i = 0; i < 8; i++) {
      Serial.print(uid[7 - i], HEX); // LSB is first      //print RFID tag 8-character hex ID to serial monitor
      if (i < 2) Serial.print(":");
    }
	delay(2500);    // Pause to show the scanned UID in the serial monitor
	
//  //******************************************************************************************
 // If a single byte of an 8 byte UID matches the scanned UID byte it will set a flag bit high
 Serial.print('\n');
bool matchFlag = false; 	 
 for(int j = 0; j<tags; j++) {     // Read each of the tag UIDs in the library and compare for match against currently scanned tag
	 uint8_t tempCount = 0;
	 uint8_t uidTemp[8]; // Create temporary array for reading a tag UID from arduino EEPROM
	 struct eepromUIDread read1;
	 EEPROM.get(16+(j*sizeTagUID), read1);			// Read the next UID in the library
	 for(int i=0; i<8; i++){
	 uidTemp[i] = read1.tempUIDread [i];
     }
	 Serial.print(F("UID read from EEPROM is "));
	  for (int k = 0; k < 8; k++) {
      Serial.print(uidTemp[7 - k], HEX); // LSB is first   //print RFID tag 8-character hex ID to serial monitor
      if (k < 2) Serial.print(":");
    }
//	Serial.print('\n');
//	for (int l = 0; l < 8; l++) {
//      Serial.print(uidTemp[7 - l], HEX); // LSB is first      //print RFID tag 8-character hex ID to serial monitor
//      if (l < 2) Serial.print(":");
 //   }
		Serial.print('\n');
	delay(1000);
	
		for (int m = 0;  m < 8; m++) {
		  if( uidTemp[m] != uid[m] ) {
				tempCount ++;
			  Serial.println("non-matching byte exists");
			//set your boolean flag here
			break;
		  }
		}
    if(tempCount == 0) { matchFlag = true; Serial.println(F("UID already exists in library, please present a new tag to the reader " )); delay(1000); break;};
 }
 
 if (matchFlag == true ) {continue;}   
 // If tempCount = 0 then all bytes of compated UIDs are matching and UID already exists...return to top
 // If we get to end and no UID match found then write the new tag UID to the EEPROM library;
 else { 
	Serial.println(F("Writing UID to EEPROM ....  "));
	  Serial.print(F("New UID will be written to library at EEPROM address byte: "));
     Serial.println(uidPointer, DEC);
	struct eepromUIDwrite write2;  // Create structure to enable 8-byte array to be written to EEPROM 
  	 for(int i=0; i<8; i++){
  write2.tempUIDwrite[i] = uid[i];
	 } 										 // Copy the scanned UID into a temporary structure to write to EEPROM
  EEPROM.put(uidPointer, write2); //Write new tag UID to EEPROM 
	
  uint8_t uidTempSingle[8];
  uidPointer += sizeTagUID; tags++; size += sizeTagUID; // Update all parameters to reflect new tag addition
  struct tagLibraryInfo tagInfo;
  tagInfo._addyPointer = uidPointer;
  tagInfo._eepromArraySize = size;
  tagInfo._numberTags = tags;
  tagInfo._sizeTagUID = 8; 
  EEPROM.put(0, tagInfo);   // Update the Tag library info in the first 7 bytes of EEPROM

  Serial.print(F("Number of tags is now: " ));
  Serial.println(tags, DEC);
  Serial.print(F("Size of tag UID library in EEPROM is:"));
  Serial.print(size, DEC);
  Serial.println(F(" bytes"));
    Serial.println('\n');
   for (int i = 0; i < 8; i++) {
      Serial.print(write2.tempUIDwrite[7 - i], HEX); // LSB is first      //print RFID tag 8-character hex ID to serial monitor
      if (i < 2) Serial.print(":");
    }
	Serial.println(F(" UID successfully written to EEPROM library"));
  delay (3000);
 };
	delay(1500);
	if (digitalRead(2) == LOW ) {
		exitCount++;
	};
 loopCnt++;
 }
	// ***********END of While UID array write loop************8
//  
Serial.print('\n');
  Serial.print("Tag UID library creation has finished......" );
   Serial.print('\n');
   Serial.println(F("The Tag library contains the following UIDs:  "));
     for(int j = 0; j<tags; j++) {     // Read each of the tag UIDs in the library and compare for match against currently scanned tag
	 uint8_t tempCount = 0;
	 uint8_t uidTemp[8]; // Create temporary array for reading a tag UID from arduino EEPROM
	 struct eepromUIDread read1;
	 EEPROM.get(16+(j*sizeTagUID), read1);			// Read the next UID in the library
	 for(int i=0; i<8; i++){
	 uidTemp[i] = read1.tempUIDread [i];
     }
	 	  for (int k = 0; k < 8; k++) {
      Serial.print(uidTemp[7 - k], HEX); // LSB is first   //print RFID tag 8-character hex ID to serial monitor
      if (k < 2) Serial.print(":");
    }
	Serial.print('\n');
	  }	  
//	END OF Main Program Loop
 Serial.print('\n');
 }
 

// **************END OF MAIN LOOP ************** USE WHILE LOOP ???******************
//}  Loop end position prtevious test

//*****************************************************************************************************************************************************
//******************************************************************************************************************************************************	
void Nic::showIRQStatus(uint32_t irqStatus) {
  Serial.print(F("IRQ-Status 0x"));
  Serial.print(irqStatus, HEX);
  Serial.print(": [ ");
  if (irqStatus & (1 << 0)) Serial.print(F("RQ "));
  if (irqStatus & (1 << 1)) Serial.print(F("TX "));
  if (irqStatus & (1 << 2)) Serial.print(F("IDLE "));
  if (irqStatus & (1 << 3)) Serial.print(F("MODE_DETECTED "));
  if (irqStatus & (1 << 4)) Serial.print(F("CARD_ACTIVATED "));
  if (irqStatus & (1 << 5)) Serial.print(F("STATE_CHANGE "));
  if (irqStatus & (1 << 6)) Serial.print(F("RFOFF_DET "));
  if (irqStatus & (1 << 7)) Serial.print(F("RFON_DET "));
  if (irqStatus & (1 << 8)) Serial.print(F("TX_RFOFF "));
  if (irqStatus & (1 << 9)) Serial.print(F("TX_RFON "));
  if (irqStatus & (1 << 10)) Serial.print(F("RF_ACTIVE_ERROR "));
  if (irqStatus & (1 << 11)) Serial.print(F("TIMER0 "));
  if (irqStatus & (1 << 12)) Serial.print(F("TIMER1 "));
  if (irqStatus & (1 << 13)) Serial.print(F("TIMER2 "));
  if (irqStatus & (1 << 14)) Serial.print(F("RX_SOF_DET "));
  if (irqStatus & (1 << 15)) Serial.print(F("RX_SC_DET "));
  if (irqStatus & (1 << 16)) Serial.print(F("TEMPSENS_ERROR "));
  if (irqStatus & (1 << 17)) Serial.print(F("GENERAL_ERROR "));
  if (irqStatus & (1 << 18)) Serial.print(F("HV_ERROR "));
  if (irqStatus & (1 << 19)) Serial.print(F("LPCD "));
  Serial.println("]");
}


void Nic::appLibrary(int &uidPointer, int &size, uint8_t &tags, int sizeTagUID){

PN5180ISO15693 nfc(PN5180_NSS, PN5180_BUSY, PN5180_RST);

//void setup() { //DON't NOT NEED 'void setup()' AS this is part a source file now not a sketch....and will only run once as required.
  
  Serial.println(F("=================================="));
  Serial.println(F("Uploaded: " __DATE__ " " __TIME__));
  Serial.println(F("PN5180 ISO15693 Demo Sketch"));

  nfc.begin();

  Serial.println(F("----------------------------------"));
  Serial.println(F("PN5180 Hard-Reset..."));
  nfc.reset();

  Serial.println(F("----------------------------------"));
  Serial.println(F("Reading product version..."));
  uint8_t productVersion[2];
  nfc.readEEprom(PRODUCT_VERSION, productVersion, sizeof(productVersion));
  Serial.print(F("Product version="));
  Serial.print(productVersion[1]);
  Serial.print(".");
  Serial.println(productVersion[0]);
 delay(2000);
 
  if (0xff == productVersion[1]) { // if product version 255, the initialization failed
    Serial.println(F("Initialization failed!?"));
    Serial.println(F("Press reset to restart..."));
    Serial.flush();
    exit(-1); // halt
  }
  uint32_t rfStatus;
  nfc.readRegister(RF_STATUS, &rfStatus);
  uint32_t rxStatus;
  nfc.readRegister(0x1E, &rxStatus);
  uint32_t AGCconfig;


  Serial.println(F("----------------------------------"));
  Serial.println(F("Reading firmware version..."));
  uint8_t firmwareVersion[2];
  nfc.readEEprom(FIRMWARE_VERSION, firmwareVersion, sizeof(firmwareVersion));
  Serial.print(F("Firmware version="));
  Serial.print(firmwareVersion[1]);
  Serial.print(".");
  Serial.println(firmwareVersion[0]);

  Serial.println(F("----------------------------------"));
  Serial.println(F("Reading EEPROM version..."));
  uint8_t eepromVersion[2];
  nfc.readEEprom(EEPROM_VERSION, eepromVersion, sizeof(eepromVersion));
  Serial.print(F("EEPROM version="));
  Serial.print(eepromVersion[1]);
  Serial.print(".");
  Serial.println(eepromVersion[0]);
delay(3000);
abort;
  /*
    Serial.println(F("----------------------------------"));
    Serial.println(F("Reading IRQ pin config..."));
    uint8_t irqConfig;
    nfc.readEEprom(IRQ_PIN_CONFIG, &irqConfig, 1));
    Serial.print(F("IRQ_PIN_CONFIG=0x"));
    Serial.println(irqConfig, HEX);
    Serial.println(F("----------------------------------"));
    Serial.println(F("Reading IRQ_ENABLE register..."));
    uint32_t irqEnable;
    nfc.readRegister(IRQ_ENABLE, &irqEnable));
    Serial.print(F("IRQ_ENABLE=0x"));
    Serial.println(irqConfig, HEX);
  */

  Serial.println(F("----------------------------------"));
  Serial.println(F("Enable RF field..."));
  nfc.setupRF();
  // PN5180.readRegister(AGC_CONFIG, uint32_t reg1val);

bool randVar = false ; //create arbitrary boolean flag to allow a while loop to occur below
 uint32_t loopCnt = 0;
//while(randVar == false) { // THIS REPLICATES A MAIN LOOP in a Sketch i pulled this from
 
  Serial.println();
  bool errorFlag = false; //show state of interrupt led for button push
  delay (1500);
  if (errorFlag) {
    uint32_t irqStatus = nfc.getIRQStatus();
    showIRQStatus(irqStatus);
  
    if (0 == (RX_SOF_DET_IRQ_STAT & irqStatus)) { // no card detected
      Serial.println(F("*** No card detected!"));
    }

    nfc.reset();
    nfc.setupRF();

    errorFlag = false;
  }

  Serial.println(F("----------------------------------"));
  Serial.print(F("Loop #"));
  Serial.println(loopCnt++);

  // *******     NIC's addition: Create UID library for comparing future UID addressed commands     ***********************
 uint8_t exitCount = 0;   				// Initiate counter for exit button 
 while (digitalRead(2) == true) { 		// Keep sending UIDs to library if button not pressed 3 times while EEPROM loop is counting
   delay(2000);
  uint8_t uid[8];                                        // uid[8] is an 8 byte array to read an RFID tag's UID
    ISO15693ErrorCode rc = nfc.getInventory(uid);          // Read inventory
 
    if (ISO15693_EC_OK != rc) {                            // Check for read inventory error & return if true
      Serial.print(F("Error in getInventory: "));
      Serial.println(nfc.strerror(rc));
      errorFlag = true;
      continue;
    }
	 Serial.print('\n');
	  Serial.print(F("Inventory successful, UID = "));
    for (int i = 0; i < 8; i++) {
      Serial.print(uid[7 - i], HEX); // LSB is first      //print RFID tag 8-character hex ID to serial monitor
      if (i < 2) Serial.print(":");
    }
	
	
//  //******************************************************************************************
 // If a single byte of an 8 byte UID matches the scanned UID byte it will set a flag bit high
 Serial.print('\n');
 Serial.print(F("Comparing scanned tag UID with exisitng UIDs in EEPROM library........."));
 Serial.print('\n');
 delay(2500);    // Pause to show the scanned UID in the serial monitor
bool matchFlag = false; 	 
 for(int j = 0; j<tags; j++) {     // Read each of the tag UIDs in the library and compare for match against currently scanned tag
	 uint8_t tempCount = 0;
	 uint8_t uidTemp[8]; // Create temporary array for reading a tag UID from arduino EEPROM
	 struct eepromUIDread read1;
	 EEPROM.get(16+(j*sizeTagUID), read1);			// Read the next UID in the library
	 for(int i=0; i<8; i++){
	 uidTemp[i] = read1.tempUIDread [i];
     }
	 Serial.print(F("UID read from EEPROM is "));
	  for (int k = 0; k < 8; k++) {
      Serial.print(uidTemp[7 - k], HEX); // LSB is first   //print RFID tag 8-character hex ID to serial monitor
      if (k < 2) Serial.print(":");
    }
//	Serial.print('\n');
//	for (int l = 0; l < 8; l++) {
//      Serial.print(uidTemp[7 - l], HEX); // LSB is first      //print RFID tag 8-character hex ID to serial monitor
//      if (l < 2) Serial.print(":");
 //   }
		Serial.print('\n');
	delay(500);
	
		for (int m = 0;  m < 8; m++) {
		  if( uidTemp[m] != uid[m] ) {
				tempCount ++;
			  Serial.println("non-matching byte exists");
			//set your boolean flag here
			break;
		  }
		}
    if(tempCount == 0) { matchFlag = true; Serial.println(F("UID already exists in library, please present a new tag to the reader " )); delay(1000); break;};
 }
 
 if (matchFlag == true ) {continue;}   
 // If tempCount = 0 then all bytes of compated UIDs are matching and UID already exists...return to top
 // If we get to end and no UID match found then write the new tag UID to the EEPROM library;
 else { 
	Serial.println(F("Writing UID to EEPROM ....  "));
	  Serial.print(F("New UID will be written to library at EEPROM address byte: "));
     Serial.println(uidPointer, DEC);
	struct eepromUIDwrite write2;  // Create structure to enable 8-byte array to be written to EEPROM 
  	 for(int i=0; i<8; i++){
  write2.tempUIDwrite[i] = uid[i];
	 } 										 // Copy the scanned UID into a temporary structure to write to EEPROM
  EEPROM.put(uidPointer, write2); //Write new tag UID to EEPROM 
	
  uint8_t uidTempSingle[8];
  uidPointer += sizeTagUID; tags++; size += sizeTagUID; // Update all parameters to reflect new tag addition
  struct tagLibraryInfo tagInfo;
  tagInfo._addyPointer = uidPointer;
  tagInfo._eepromArraySize = size;
  tagInfo._numberTags = tags;
  tagInfo._sizeTagUID = 8; 
  EEPROM.put(0, tagInfo);   // Update the Tag library info in the first 7 bytes of EEPROM

  Serial.print(F("Number of tags is now: " ));
  Serial.println(tags, DEC);
  Serial.print(F("Size of tag UID library in EEPROM is:"));
  Serial.print(size, DEC);
  Serial.println(F(" bytes"));
    Serial.println('\n');
   for (int i = 0; i < 8; i++) {
      Serial.print(write2.tempUIDwrite[7 - i], HEX); // LSB is first      //print RFID tag 8-character hex ID to serial monitor
      if (i < 2) Serial.print(":");
    }
	Serial.println(F(" UID successfully written to EEPROM library"));
  delay (3000);
 };
	delay(1500);
	if (digitalRead(2) == LOW ) {
		exitCount++;
	};
 loopCnt++;
 }
// ***********END of While UID array write loop************8
//  
   Serial.print("\n");
  Serial.print("Tag UID library additions have completed......" );
   Serial.print("\n");
   Serial.println(F("The Tag library now contains the following UIDs:  "));
     for(int j = 0; j<tags; j++) {     // Read each of the tag UIDs in the library and compare for match against currently scanned tag
	 uint8_t tempCount = 0;
	 uint8_t uidTemp[8]; // Create temporary array for reading a tag UID from arduino EEPROM
	 struct eepromUIDread read1;
	 EEPROM.get(16+(j*sizeTagUID), read1);			// Read the next UID in the library
	 for(int i=0; i<8; i++){
	 uidTemp[i] = read1.tempUIDread [i];
     }
	 	  for (int k = 0; k < 8; k++) {
      Serial.print(uidTemp[7 - k], HEX); // LSB is first   //print RFID tag 8-character hex ID to serial monitor
      if (k < 2) Serial.print(":");
    }
	Serial.print('\n');
	  }	  

//	END OF Main Program Loop
 }
 

