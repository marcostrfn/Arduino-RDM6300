//
// CONEXION
//                        ARD                  
// RDM6300 P1 
//  PIN4:                 GND
//  PIN5:                 VCC
//  PIN1:                 2
// RDM6300 P2 (ANTENA)
//  PIN1: ANT1 NEGRO
//  PIN2: ANT2 ROJO
//
// LED:                   7
// LED:                   8

#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
//----
// Tags are stored in program flash memory
// 32k minus sketch size determines the amount of tags that can be stored
// Tags include the two CRC bytes (14 bytes total)
const char tag_0[] PROGMEM = "0F00335093FF"; //add your tags here
const char tag_1[] PROGMEM = "0F003344126A";
const char * const tag_table[] PROGMEM = {  tag_0,  tag_1 };
//----

SoftwareSerial rfidReader(2, 3); // Digital pins 2 and 3 connect to pins 1 and 2 of the RMD6300
String tagString;
char tagNumber[14];
boolean receivedTag;
int lockSELECT = -1;
int lockPIN1 = 7; // pin 7 is controls the door 1
int lockPIN2 = 8; // pin 8 is controls the door 2

void setup() {

  pinMode(lockPIN1, OUTPUT);
  pinMode(lockPIN2, OUTPUT);
  Serial.begin(9600);
  rfidReader.begin(9600); // the RDM6300 runs at 9600bps
  Serial.println("\n\n\nRFID Reader...ready!");

}

void loop()
{

  receivedTag = false;
  lockSELECT = -1;
  while (rfidReader.available()) {
    int BytesRead = rfidReader.readBytesUntil(3, tagNumber, 15); //EOT (3) is the last character in tag
    receivedTag = true;
  }

  if (receivedTag) {
    tagString = tagNumber;
    Serial.println();
    Serial.print("Tag Number: ");
    Serial.println(tagString);

    if (checkTag(tagString)) {
      Serial.print("Tag Authorized...");
      switch (lockSELECT) {
        case 0:
          openDoor(lockPIN1);
          break;
        case 1:
          openDoor(lockPIN2);
          break;
        default:
          break;
      }
    }
    else {
      Serial.print("Unauthorized Tag: ");
      Serial.println(tagString);
      delay(1500); // a delay of 1500ms and a flush() seems to stop tag repeats
      rfidReader.flush();
    }
    memset(tagNumber, 0, sizeof(tagNumber)); //erase tagNumber
  }

}

// ----
// checkTag function (give it the tag string complete with SOT and EOT)
// compares with tags in tag_table
// and returns true if the tag is in the list

boolean checkTag(String tag) {

  char nullTag[12] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'};
  if (tag == nullTag) {
    Serial.println("Null Tag....");
    return false;
  }

  char testTag[14];

  for (int i = 0; i < sizeof(tag_table) / 2; i++)
  {
    strcpy_P(testTag, (char*)pgm_read_word(&(tag_table[i])));
    if (tag.substring(1, 13) == testTag) { //substring function removes SOT and EOT
      lockSELECT = i;
      return true;
      break;
    }
  }
  return false;
}
//----

void openDoor(int PIN) {
  Serial.print("Opening door...");
  Serial.print(PIN);
  digitalWrite(PIN, HIGH);
  delay(1000);
  Serial.println("Re-locking door");
  digitalWrite(PIN, LOW);
  delay(1500);// a delay of 1500ms and a flush() seems to stop tag repeats
  rfidReader.flush();
}
