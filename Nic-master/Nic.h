
//Name: Nic.h
//Author: Nic Morgan
//Date:18/09/2019
//DESC: Header file for custom functions to be implemented for PN5180 NFC reader on ISO15693 Tags.
 
#ifndef Nic_H	// Default code to include in every library header file in case of duplication 
#define Nic_H

#include <Debug.h>			//List of libraries to include to ensure function calls are valid
#include <PN5180.h>
#include <PN5180ISO15693.h>
#include <SPI.h>
#include  <EEPROM.h>

class Nic : public PN5180ISO15693 { // Define class
 	
public:
void scanEeprom(int &addyPointer, int &eepromArraySize, uint8_t &numberTags, int &sizeTagUID);
//	Nic(String myString);
//	String playMyLCD(bool bigorse);								//The firest two bytes of EEPROM  are used to store an EEPROM address pointer
//   															//so that data already stored isn't overwritten or erased.
//	void createTagLibrary(int tagSize, int noTags);
	
void getInfo (uint8_t &camel); 
// uint8_t _fontWidth; 

// The following functions add new tag UIDs to the EEPROM library and require
// library info to be updated as new data is written:

void showIRQStatus(uint32_t irqStatus);

void creatNewLibrary(int &uidPointer, int &size, uint8_t &tags, int sizeTagUID);

void appLibrary(int &uidPointer, int &size, uint8_t &tags, int sizeTagUID);

private:

// Strucures are needed to read/write arrays to arduino EEPROM
struct tagLibraryInfo { int _addyPointer; int _eepromArraySize; uint8_t _numberTags; int _sizeTagUID;};
struct eepromUIDread { uint8_t tempUIDread[8];};   // Create temp UID 8 byte array for reading UID from EEPROM tag library
struct eepromUIDwrite { uint8_t tempUIDwrite[8];};  // Create temp UID 8-byte array for writing UID to EEPROM tag library
struct myObject { uint8_t tempStru[8];};

//***** EEprom scan privates ********
			// Size of existing array in EEPROM (doesn't exist if EEPROM 0x0001 = FFFF or 0)
			// Number of tags in existing library
			// ISO15693 UID = 8 bytes so typically won't change but may need to adapt for a different protocol

//	uint8_t tagArray[];    // Create NFC tag array for storins tag UID's
//	int arraySize;			// array size will depend on #tags & size of UIDs for each tag
//	static uint8_t ind_arr =0;	// Need to index/point array addresses to read/write UIDs in array memory
	
};

#endif