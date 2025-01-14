#include <MemoryFree.h>
#include <Wire.h>
#include <math.h>

//pin assignments - NOTE: search the pin name to update the hard-coded pins in the code itself. This reduces vars and saves VERY limited microprocessor memory
// oneWirePin = 2;
// luxPin = A1;
// sensorPowerPin = 5;
// RTC_CLK_Pin = 6; RTC_DAT_Pin = 7; RTC_RST_Pin = 8;
// SPI_MOSI_Pin = 11; SPI_MISO_Pin = 12; SPI_CLK_Pin = 13;
// SD_CS_Pin = 4;
// ADXL345_CS = 3;
// I2C_SDA = A4; I2C_SCL = A5 // used for Compass

#include <virtuabotixRTC.h>
// Creation of the Real Time Clock Object. Example from https://miliohm.com/ds1302-rtc-with-arduino-tutorial/
virtuabotixRTC myRTC(6, 7, 8); // RTC_CLK_Pin = 6; RTC_DAT_Pin = 7; RTC_RST_Pin = 8;
String currentDateTimeString = "";
int newDateTimeArray[] = {0, 0, 0, 0, 1, 1, 2020}; //temp var to hold date an times in teh format of seconds, minutes, hours, day of the week, day of the month, month, year

//onewire temp stuff
#include <OneWire.h>
#include <DallasTemperature.h> //tip: if this library is too large try this instead: http://www.cupidcontrols.com/2014/10/moteino-arduino-and-1wire-optimize-your-read-for-speed/
#define ONE_WIRE_BUS 2 //oneWirePin D2
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature

//SD Card stuff
#include <SPI.h>
#include <SD.h>
File myFile;

//ADXL345 stuff  // https://github.com/wollewald/ADXL345_WE/blob/main/examples/ADXL345_SPI_basic_data/ADXL345_SPI_basic_data.ino
#include <ADXL345_WE.h>
//#include <SPI.h>
#define CS_PIN 3  // ADXL345_CS = 3;
/* In case you want to change the default SPI pins (e.g. for ESP32), uncomment and adjust:
  #define MOSI_PIN 22
  #define MISO_PIN 17
  #define SCK_PIN 16
*/
bool spi = true;    // flag indicating that SPI shall be used
/* There are three ways to create your ADXL345 object in SPI mode:
 * ADXL345_WE myAcc = ADXL345_WE(CS_PIN, spi)     -> uses SPI, spi is just a flag, see SPI example
 * ADXL345_WE myAcc = ADXL345_WE(&SPI, CS_PIN, spi) -> uses SPI / passes the SPI object, spi is just a flag, see SPI example
 * ADXL345_WE myAcc = ADXL345_WE(&SPI, CS_PIN, spi, MOSI_PIN, MISO_PIN, SCK_PIN) -> like the latter, but also changes the SPI pins  
 */
ADXL345_WE myAcc = ADXL345_WE(CS_PIN, spi);
xyzFloat raw;
xyzFloat g;

// Digital compass stuff (HMC5883)
#include <MechaQMC5883.h>
MechaQMC5883 qmc;
int magX, magY, magZ, magAzimuth;

//other vars
unsigned long lastSerialDebugUpdateTime = 0; //for remembering the last time an update was sent to the serial port
unsigned long lastUpdateTime = 0; //for determining next time to pull data from sensors
//int avgLightInput = 0;
//int curLightInput = 0;
double currentTemp;
bool tempsRequested = false;
String receivedSerialData = "";

//temp vars
boolean tempBoolean;
char tempChar;
int tempInt;
double tempDouble;
String tempString;
unsigned long tempTime;

void setup() {
  Serial.begin(9600);
  Serial.println(F("SubsurfaceSensorBuoy 0.10"));

  pinMode(5, OUTPUT); //sensorPowerPin=5
  digitalWrite(5, HIGH); //sensorPowerPin=5
  delay(50); //wait for sensors to power on
  
  // Real time clock stuff
  //myRTC.setDS1302Time(0, 0, 0, 0, 1, 1, 2020); // Set the RTC's seconds, minutes, hours, day of the week, day of the month, month, year

  //onewire temp stuff
  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
  sensors.requestTemperatures(); // // Start calculating temperatures to use in next update

  // Digital compass stuff
  Wire.begin(); //for all I2c stuff
  qmc.init();
  
  // SD Card Reader Stuff
  if (!SD.begin(4)) { //SD_CS_Pin = 4;
    Serial.println(F("SD init fail"));
    //while (1);
  }else{
    Serial.println("SD init ok");
  }
  
  //ADXL345 stuff
  if(!myAcc.init()){
    Serial.println("ADXL345 not connected!");
  }
  //myAcc.setSPIClockSpeed(4000000); // You can set the SPI clock speed. Default is 5 MHz
  myAcc.setDataRate(ADXL345_DATA_RATE_12_5); //set data rate to 12.5hz
  myAcc.setRange(ADXL345_RANGE_4G); //Choose the measurement range 16g,8g,4g,2g
  // myAcc.setLowPower(true); //Uncomment to enable Low Power Mode. It saves power but slightly increases noise. LowPower only affetcs Data Rates 12.5 Hz to 400 Hz.

  //pinMode(5, OUTPUT); //sensorPowerPin=5
  //digitalWrite(5, LOW); //sensorPowerPin=5
}


void loop() {

  if (millis() > lastUpdateTime + 1000 && tempsRequested != true) {
    sensors.requestTemperaturesByIndex(0); // Start calculating temperatures now so they are ready by next update
    tempsRequested = true;
  }
  if (millis() > lastUpdateTime + 2000) { //get latest sensor values

    
    
    
    //get latest time
    myRTC.updateTime();
    currentDateTimeString = addLeadingZeroToInt(myRTC.year) + "-" + addLeadingZeroToInt(myRTC.month) + "-" + addLeadingZeroToInt(myRTC.dayofmonth) + " " + addLeadingZeroToInt(myRTC.hours) + ":" + addLeadingZeroToInt(myRTC.minutes) + ":" + addLeadingZeroToInt(myRTC.seconds);
    //Serial.println("Current Date / Time: " + currentDateTimeString);

    currentTemp = (double)sensors.getTempCByIndex(0);
    tempsRequested = false;

    qmc.read(&magX, &magY, &magZ,&magAzimuth); //get compass data

    //get ADXL345 values
    raw = myAcc.getRawValues();
    g = myAcc.getGValues();

    writeToFile(F("output.csv"), currentDateTimeString + "," + currentTemp + "," + magX + "," + magY + "," + magZ + "," + magAzimuth + "," + g.x + "," + g.y + "," + g.z);


    //dataArray[5] = analogRead(1); //dataArray[5]=luxRaw  //luxPin=A1
    //dataArray[6] = (double)(1.5915) * pow(2.7183, (0.0082)*dataArray[5]); //dataArray[6]=lux  //dataArray[5]=luxRaw

    ProcessSerialData();
    updateSerialDebug();
    lastUpdateTime = millis();
  }

  //restart device every 24hrs
  if (millis() >= 86400000) { //86400000 represents 24hrs in ms
    softReset(); //performs a soft reset (note: this method doesn't reset pin states or registers)
  }

} //end of void loop()






void updateSerialDebug() {
  if (millis() >= lastSerialDebugUpdateTime + 2000) {
    Serial.print(F("<freemem:")); Serial.print(freeMemory());
    Serial.print(F("; currentDateTimeString:")); Serial.print(currentDateTimeString);
    Serial.print(F("; currentTemp:")); Serial.print(currentTemp);
    Serial.print(F("; magX:")); Serial.print(magX);
    Serial.print(F("; magY:")); Serial.print(magY);
    Serial.print(F("; magZ:")); Serial.print(magZ);
    Serial.print(F("; magAzimuth:")); Serial.print(magAzimuth);
    Serial.print(F("; raw.x:")); Serial.print(raw.x);
    Serial.print(F("; raw.y:")); Serial.print(raw.y);
    Serial.print(F("; raw.z:")); Serial.print(raw.z);
    Serial.print(F("; g.x:")); Serial.print(g.x);
    Serial.print(F("; g.y:")); Serial.print(g.y);
    Serial.print(F("; g.z:")); Serial.print(g.z);
    //Serial.print(F("; luxRaw:")); Serial.print(dataArray[5]); //dataArray[5]=luxRaw
    Serial.println(F(">"));
    lastSerialDebugUpdateTime = millis();
  }
}


// Get commands received via serial port
void ProcessSerialData() {
  while (Serial.available() > 0) {
    tempChar = Serial.read();
    receivedSerialData += tempChar;
    if (tempChar == '\n') { //means we have received a complete line - time to process...

      if (receivedSerialData.indexOf("|setDT|") > -1) { //example input: "|setDT|0|10|5|6|26|4|2024" this command saves a new date and time using the folowing params: seconds, minutes, hours, day of the week, day of the month, month, year
        newDateTimeArray[0] = getDelimValue(receivedSerialData, '|', 2).toInt(); //seconds
        newDateTimeArray[1] = getDelimValue(receivedSerialData, '|', 3).toInt(); //min
        newDateTimeArray[2] = getDelimValue(receivedSerialData, '|', 4).toInt(); //hrs
        newDateTimeArray[3] = getDelimValue(receivedSerialData, '|', 5).toInt(); //day of week
        newDateTimeArray[4] = getDelimValue(receivedSerialData, '|', 6).toInt(); //day of month
        newDateTimeArray[5] = getDelimValue(receivedSerialData, '|', 7).toInt(); //month
        newDateTimeArray[6] = getDelimValue(receivedSerialData, '|', 8).toInt(); //year
        if (newDateTimeArray[6] != 0) { //for error checking lets just spot check that the last parameter exists and has the right number of digits
          myRTC.setDS1302Time(newDateTimeArray[0], newDateTimeArray[1], newDateTimeArray[2], newDateTimeArray[3], newDateTimeArray[4], newDateTimeArray[5], newDateTimeArray[6]); // Set the RTC's seconds, minutes, hours, day of the week, day of the month, month, year
          Serial.println(F("New DT set"));
        } else {
          Serial.println(F("Format Error"));
        }

      }

      receivedSerialData = "";
    } //end of if (tempChar == '\n')

    if (receivedSerialData.length() > 40) { //Received way more data than expected. lets clear receivedSerialData to avoid memory issues
      Serial.println(F("Excess input, purging"));
      receivedSerialData = "";
    }
  } //end of while (Serial.available()>0)
} //end of ProcessSerialData function





void writeToFile(String fileName, String dataToWrite) {
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open(fileName, FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {
    myFile.println(dataToWrite);
    // close the file:
    myFile.close();
    //Serial.println(fileName + F(" written"));
  } else {
    // if the file didn't open, print an error:
    Serial.println(fileName + F(" error"));
  }
}


String addLeadingZeroToInt(int inputInt) {
  if (inputInt < 10) {
    tempString = "0" + (String)inputInt;
  } else {
    tempString = (String)inputInt;
  }
  return tempString;
}


String getDelimValue(String inputString, char delimChar, int valuePosition) {
  String requestedValue = "";
  int currentPosition = 0;
  char tempChar;
  for (int i = 0; i <= inputString.length(); i++) {
    tempChar = inputString.charAt(i);
    if (tempChar == delimChar) currentPosition++;
    if (tempChar != delimChar && currentPosition == valuePosition) requestedValue = requestedValue + (String)inputString.charAt(i);
  }//end of for
  return requestedValue;
} // end of getDelimValue function





void softReset() { //performs a soft reset (note: this method doesn't reset pin states or registers)
  asm volatile ("  jmp 0");
}
