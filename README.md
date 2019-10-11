# Nics-mess
A customisation of the original PN5180 and PN5180ISO15693 libraries created by ATrappmman. 
Targeting the ESP32 DEV Kit V1,  When I try to complie my arduino sketch called "nic_PN5180_custom_v1a" the following errors are generated:


**************************************************************************************************************************************
Arduino: 1.8.9 (Windows Store 1.8.21.0) (Windows 10), Board: "DOIT ESP32 DEVKIT V1, 80MHz, 921600, None"

In file included from C:\Users\User\Documents\Arduino\libraries\Nic-master\Nic.cpp:8:0:

C:\Users\User\Documents\Arduino\libraries\PN5180-Library-master/PN5180ISO15693.h:61:9: error: '__FlashStringHelper' does not name a type

   const __FlashStringHelper *strerror(ISO15693ErrorCode errno);

         ^

C:\Users\User\Documents\Arduino\libraries\Nic-master\Nic.cpp: In member function 'void Nic::creatNewLibrary(int&, int&, uint8_t&, int)':

C:\Users\User\Documents\Arduino\libraries\Nic-master\Nic.cpp:85:10: error: 'class EEPROMClass' has no member named 'update'

   EEPROM.update(i, 0xFF);

          ^

C:\Users\User\Documents\Arduino\libraries\Nic-master\Nic.cpp:199:26: error: 'class PN5180ISO15693' has no member named 'strerror'

       Serial.println(nfc.strerror(rc));

                          ^

C:\Users\User\Documents\Arduino\libraries\Nic-master\Nic.cpp: In member function 'void Nic::appLibrary(int&, int&, uint8_t&, int)':

C:\Users\User\Documents\Arduino\libraries\Nic-master\Nic.cpp:457:26: error: 'class PN5180ISO15693' has no member named 'strerror'

       Serial.println(nfc.strerror(rc));

                          ^

exit status 1
Error compiling for board DOIT ESP32 DEVKIT V1.

This report would have more information with
"Show verbose output during compilation"
option enabled in File -> Preferences.
