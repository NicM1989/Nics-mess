#include <EEPROM.h>
#include <dummy.h>
#include <PN5180.h>
#include <PN5180ISO15693.h>
#include <Nic.h>
#include <stdio.h>


////uint8_t tagLibrary[64];
      //global pointer variable for EEPROM
      // First 16 bytes in EEPROM reserved for configuration and data info
      //struct proxy {int proxyPointer =16; int proxyLength = 80 ; byte proxyNoTags = 8; int proxySizeUID = 8;}; 
//

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

int addy;  // Declare global variables to return the UID library info from EEPROM (bytes 0-7)
int eeASize; 
uint8_t nTags; 
int sizeUID;
uint8_t uidA[300];      // Create a flash program uid array so not constantly reading from EEPROM uid library
uint8_t uidArrayPulled[0];
bool finalFlag = false; 
bool initiation = true;
bool dontLoop = false; 

uint8_t mongoose;
int c = 0 ; 
//long initMs = millis(); 
//long delayTime = 8000; //Want my delay to be 3 seconds      
//bool flag1= LOW;
volatile uint8_t countB;
uint8_t countBog;

byte currentButtonState;
byte previousButtonState;
int count = 0;
boolean printFinalMessage = true;
unsigned long debounceStartMillis;
unsigned long debouncePeriod = 20;
boolean debouncing = false;
const int button = 2;
unsigned long periodStartMillis;
unsigned long currentMillis;
const unsigned long period = 5000;  //period during which button input is valid
unsigned long cMilli;
unsigned long periodStart;
unsigned long periodHB = 500; // Period of program heartbeat in ms
//int i;
//******************************************************************************************
PN5180ISO15693 nfc(PN5180_NSS, PN5180_BUSY, PN5180_RST);
Nic Nic1;

void setup() {
 Serial.begin(115200);
  /*
    Decalre No. of ID tags to go in library array for future comparison
    Each tag UID is 64 bits
    Declare number of tags and intialise an empty array to fit them in
  */
// The below function scans the first several bytes of EEPROM in arduino to return the UID library parameters
//  // or returns only a message if no UID Tag Library exists yet. From here can overwrite/append/create new/continue with existing
//  // UID library
Nic1.scanEeprom(addy,eeASize,nTags,sizeUID); 
Serial.print(F("Size of eeprom array is: "));
Serial.print(addy, DEC);
Serial.print("\n");

// Serial.println(addy, DEC); Serial.println(eeASize,DEC ); Serial.println(nTags,DEC ); Serial.println(sizeUID,DEC );
// Set parameters for delay and button press counting
countB = 0 ;
pinMode(40, OUTPUT);
pinMode(button, INPUT_PULLUP);
periodStart = millis();
periodStartMillis = millis();
if (nTags ==0 || nTags == 0xFFFF) {
   Serial.print("NO UID Tags Exist in the UID Library.... Press button1 to create new library");
   Serial.print("\n");
   while(digitalRead(button)== true){};
   Nic1.creatNewLibrary(addy, eeASize, nTags, sizeUID); // Create new UID library in EEPROMs
 } //end message display for 'No UID's detected'
 else { //UID library already exists; display info
  Serial.print("Current UID library exists");
  Serial.print("\n");
  Serial.print("You have 10 seconds to select one of the follwing alternative options else will continue with existing library");
  Serial.print("    CREATE NEW LIBRARY (overwrite old library):  1 presses");
   Serial.print("\n");
  Serial.print("    ADD NEW TAGS TO EXISTING LIBRARY:            2 presses");   
   Serial.print("\n");
  Serial.print(" ABORT PROGRAM:                               3 presses");
   Serial.print("\n");
 }; //end message display for library options

}
//******************************************************************************************
uint32_t loopCnt = 0;
bool errorFlag = false;
bool printOnce = false;
int countTags = 0;
void loop() {
  cMilli = millis();
  if (cMilli - periodStart >= periodHB){  //true until the period elapses
  periodStart += periodHB; 
  bool oldStateLED = digitalRead(40);
  bool newStateLED = !oldStateLED;
  digitalWrite(40, newStateLED); 
 } // End heartbeat loop
if(dontLoop == false){
 currentMillis = millis();
  if (currentMillis - periodStartMillis <= period)  //true until the period elapses
  {
    previousButtonState = currentButtonState;    //save the previous button state
    currentButtonState = digitalRead(button);  //read the current state of the input
    if (currentButtonState != previousButtonState) //if the button state has changed
    {
      debounceStartMillis = currentMillis;  //save the time that the state change occured
      debouncing = true;  //flag that debouncing in progress
    }    //end state change check
    
    if (currentMillis - debounceStartMillis >= debouncePeriod)  //if the debounce period has elapsed
    {
      if (debouncing == true)    //debouncing taking place
      {
        if (currentButtonState == LOW)  //if the button is currently pressed
        {
          debouncing = false;    //debouncing is finished
          count++;               //increment the count
          Serial.println(count);
        }    //end count increment
      }  //end debouncing in progress check
    }    //end debounce time elapsed check
  }  //end timing period check
  else  //period has ended
  {
    if (printFinalMessage == true)
    {
      Serial.println("Time is up");
      Serial.print("Button pressed count : ");
      Serial.println(count);
      printFinalMessage = false;
      switch (count){ 
  case (int)0: 
   Serial.println("Proceeding with the existing UID library.......");
       dontLoop = true;
       printOnce =true;
    //continue with current library 0
    break;
  case (int)1:
    // create new library
    Serial.println("You have chosen to overwrite the exisitng tag library in EEPROM");
    Serial.println("                                        Press button to confirm");
    while (digitalRead(button)== true){};      // Don't proceed with EEPROM library erase unless button pressed
    Nic1.creatNewLibrary(addy, eeASize, nTags, sizeUID); 
    dontLoop = true;
           printOnce =true;
    break; 
  case (int)2: 
      Serial.print("  You've chosen to add tags to the existing library");
       dontLoop = true; 
              printOnce = true;
      Serial.println("                          Press button to confirm");
    while (digitalRead(button)== true){};      // Don't proceed with EEPROM library erase unless button pressed
      Nic1.appLibrary(addy, eeASize, nTags, sizeUID);       // Add new tags to UID library in EEPROM
       break;
  case (int)3:     //abort 
  Serial.print("Program aborted");
   dontLoop = true;             
   printOnce = true;
   break;
  default: break;
}//prevent the final message being displayed again
    }    //end printing final message
  }    //end final message check
};
if (printOnce == true) {       // Display updated EEPROM library info and the program memory library in flash
Serial.print('\n');
Serial.print(F("EEPROM tag library size: "));
Serial.println(addy, DEC);
Serial.print(F("Number of tags in library: "));
Serial.println(nTags,DEC);
Serial.print(F("Size of library(bytes):  "));
Serial.println(eeASize, DEC);
Serial.print(F("UID size(bytes): "));
Serial.println(sizeUID, DEC);
  Serial.print('\n');
     for(int j = 0; j<addy; j+=8) {     // Read each of the tag UIDs in the library and compare for match against currently scanned tag
   uint8_t tempCount = 0;
   uint8_t uidTemp[8]; // Create temporary array for reading a tag UID from arduino EEPROM
  struct eepromRed { uint8_t tempUIDread[8];};  
  struct eepromRed readbyte;
   EEPROM.get(j, readbyte);      // Read the next UID in the library
  for (int i=0; i<8; i++) {
  uidA[j+i] = readbyte.tempUIDread[i]; // Copy each byte from EEPROM library into program UID Array
  };
      }  
      
  for(int j = 0; j<nTags; j++) {     // Read each of the tag UIDs in the library and compare for match against currently scanned tag
   uint8_t uidTp[8]; // Create temporary array for reading a tag UID from arduino EEPROM
     // Read the next UID in the library
   for(int i=0; i<8; i++){
   uidTp[i] = uidA[(16+8*j+i)];
     }
      for (int k = 0; k < 8; k++) {
      Serial.print(uidTp[7 - k], HEX); // LSB is first   //print RFID tag 8-character hex ID to serial monitor
      if (k < 2) Serial.print(":");
    }
  Serial.print('\n');
    }
  printOnce = false; 
  finalFlag = true; }; // Set flag bit true so final messgae only print once and allows main program loop to continue

// *****************************************************************************************************************
 if (dontLoop == true) {
     delay(4500); 
   // ISO15693ErrorCode what = nfc.sendQuiet(); 

    for(int j=0; j<nTags; j++) {
      uint8_t uidTEST[8];  
      for (int k=0; k<8; k++) {  
     uidTEST[k] = uidA[(16 + 8*j + k)];  
      }                                         //Testing to see if addressed commands don't engage other tags    
           Serial.println(F("----------------------------------"));
    uint8_t blockSize, numBlocks;
    ISO15693ErrorCode  qc = nfc.getSystemInfo(uidTEST, &blockSize, &numBlocks);
    if (ISO15693_EC_OK != qc) {
      Serial.print(F("Error in getSystemInfo: "));
      Serial.println(nfc.strerror(qc));
      errorFlag = true;
    }
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
  else { Serial.println ("TAG HAS BEEN DETECTED!!!!!!!!!!!!!!!!!!!!!!!!");
         countTags++; }
  Serial.println(F("----------------------------------"));
  Serial.print(F("Loop #"));
  Serial.println(loopCnt++);
   delay(2000); 
   
   } // End loop to address each card in the library
    Serial.println("\n");
    Serial.println (countTags, DEC);
 return ;
//      uint8_t uid[8];                                        // uid[8] is an 8 byte array to read an RFID tag's UID
//    ISO15693ErrorCode rc = nfc.getInventory(uid);
//    if (ISO15693_EC_OK != rc) {
//      Serial.print(F("Error in getInventory: "));
//      Serial.println(nfc.strerror(rc));
//      errorFlag = true;
//      return;
//    }
//    Serial.print(F("Inventory successful, UID="));
//    for (int i=0; i<8; i++) {
//      Serial.print(uid[7-i], HEX); // LSB is first      //print RFID tag 8-character hex ID to serial monitor
//      if (i < 2) Serial.print(":");
//    }
//    delay(10);
//    Serial.println(" ");
//    delay(10);
      //  Serial.println(F("----------------------------------"));
  //  uint8_t blockSize, numBlocks;
  //  rc = nfc.getSystemInfo(uid, &blockSize, &numBlocks);
  //  if (ISO15693_EC_OK != rc) {
  //    Serial.print(F("Error in getSystemInfo: "));
  //    Serial.println(nfc.strerror(rc));
  //    errorFlag = true;
  //    return;
  //  }
  //  Serial.print(F("System Info retrieved: blockSize="));
  //  Serial.print(blockSize);
  //  Serial.print(F(", numBlocks="));
  //  Serial.println(numBlocks);
  //
  //  Serial.println(F("----------------------------------"));
  //  uint8_t readBuffer[blockSize];
  //  for (int no=0; no<numBlocks; no++) {
  //    rc = nfc.readSingleBlock(uid, no, readBuffer, blockSize);
  //    if (ISO15693_EC_OK != rc) {
  //      Serial.print(F("Error in readSingleBlock #"));
  //      Serial.print(no);
  //      Serial.print(": ");
  //      Serial.println(nfc.strerror(rc));
  //      errorFlag = true;
  //      return;
  //    }
  //    Serial.print(F("Read block #"));
  //    Serial.print(no);
  //    Serial.print(": ");
  //    for (int i=0; i<blockSize; i++) {
  //      if (readBuffer[i] < 16) Serial.print("0");
  //      Serial.print(readBuffer[i], HEX);
  //      Serial.print(" ");
  //    }
  //    Serial.print(" ");
  //    for (int i=0; i<blockSize; i++) {
  //      if (isprint(readBuffer[i])) {
  //        Serial.print((char)readBuffer[i]);
  //      }
  //      else Serial.print(".");
  //    }
  //    Serial.println();
  //  }
  //
  //#ifdef WRITE_ENABLED
  //  Serial.println(F("----------------------------------"));
  //
  //
  //  for (int no=0; no<numBlocks; no++) {
  //   uint8_t *writeBuffer = malloc(blockSize);
  //  for (int i=0; i<blockSize; i++) {
  //    uint8_t blockcode = (no+5);
  //    writeBuffer[i] = blockcode + i;
  //    rc = nfc.writeSingleBlock(uid, no, writeBuffer, blockSize);
  //  }
  //    if (ISO15693_EC_OK == rc) {
  //      Serial.print(F("Wrote block #"));
  //      Serial.println(no);         //  Add a break here to print the 1st block only (not all 28)
  //    }
  //    else {
  //      Serial.print(F("Error in writeSingleBlock #"));
  //      Serial.print(no);
  //      Serial.print(": ");
  //      Serial.println(nfc.strerror(rc));
  //      errorFlag = true;
  //      return;
  //    }
  //  }
  //#endif /* WRITE_ENABLED */
// 
 };
//********************************************************************************************************************
}

void showIRQStatus(uint32_t irqStatus) {
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
