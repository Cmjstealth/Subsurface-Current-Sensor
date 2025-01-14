# Subsurface-Current-Sensor
Deployable low cost DIY sensor buoy for logging subsurface current speed, direction, water temperature, etc at common recreational diving depths (0-130ft). Note this project is still an incomplete and untested work in progress. Several components still need to be developed before an initial test.

The goal is to develop an automated sensor and data logger device that could be cheaply ($60?) built in with common components and 3d printable parts. A device that could be deployed via scuba for several months to collect automated time series data such as water current speeds, direction, temperature, salinity, etc. 

Similar to the open source CTD strategy of oceanographyforeveryone.com, this project aims to enable researchers and marine enthusiasts by reduce the financial barrier to collecting key data about our underwater world at scale. The initial design is focused on an negatively buoyant, entirely subsurface tilt sensor. This would allow it to be “hung” from existing structures (with owner permission) that are already permitted and avoid often length regulatory processes often faced by new deployments. In areas where no such structure exists a simple subsurface float could be attached to a temporary anchor.

My pilot project is in the Salish sea of Washington state where there is an opportunity to model currents and temperatures at the highly-specific locations used for recreational scuba diving. This should allow the development of better current prediction guidelines for the local dive community. Site specific water quality data would also be valuable for modeling and better understanding trends observed in existing volunteer marine species monitoring projects at these same locations such as the Reef.org fish survey project, reefcheck.org kelp monitoring surveys, the YOY Rockfish survey, etc.

<img src=https://github.com/Cmjstealth/Subsurface-Current-Sensor/blob/main/Pictures/4in%20Flange%20Housing%20-%2020240509_190037.jpg width=400></img><img src=https://github.com/Cmjstealth/Subsurface-Current-Sensor/blob/main/Pictures/4in%20PVC%20housing%20-%2020240606_170434.jpg width=400></img>

<u>Some project notes</u>:
- Initial version powered by 3 D batteries in series without any voltage regulator (3.6v-4.5v).
- Data is saved on an internal SD card that can be removed after sensor recovery
- Operated with a 3 position magnetic switch
  - Off
  - On (continuous operation): Takes measurements every few seconds useful for collecting calibration data
  - On (power save): the mode used when deployed for (hopefully) several months at a time

Water current speed calibration idea so far (untested):
  - Hang sensor from a surface float. Need gps with tracking turned on and the clock synchronized with the sensor's RTC. Line attached from surface float to a boat somewhere well clear of paddle or prop wash effects.
  - Boat can then tow the float at a slow continuous speed holding a known compass heading on a calm day for a minute or so. Repeat at perhaps 2 other speeds.
  - Then repeat all those same known speeds on a reciprocal compass heading to cancel out potential differences in speed through water vs speed over ground (due to current or wind).
  - This data could be used for a regression analysis so the sensor's raw tilt values can be used to predict current speed values.

This repository includes files for several variants of the housing design
  - **"4in PVC housing cap"** is what I'm currently testing. It uses 6" of standard 4" PVC purchased from home depot. This way only the end caps need to be 3d printed reducing resin cost, print time, and post-print finishing workload. A 4" Diameter was chosen as it is big enough to accommodate the cheapest rechargeable lead acid batteries. This could be desirable over alkaline D batteries in a future turbine style current sensor capable of energy harvesting and powering higher current sensors, microprocessors, or radio transceivers.
  - **"4in Flange Housing"** is included as I built one and successfully pressure tested it however it requires a significant amount of resin to print. The flange design that compresses the O ring doesn't seem as robust either. It also requires a lot of stainless steel hardware.
  - **(2" design coming soon)** Since V1 will likely just be powered by disposable D batteries it probably makes sense to modify the PVC and resin printed housing cap design with something closer to a 2" diameter. It would make all the materials substantially cheaper (resin, pvc pipe, and o-rings). it should also be substantially easier to slide smaller end caps into the pipe. The 4" version requires lots of silicon lube and force to put on.



Tentative Component list:
|Component|Price $USD|Notes|
|--------------------|----------|----------------------------------------------------------------------------------------------------|
|Microprocessor: ATMEGA328P DIP-28 (chip only)|2.16|Low power processor|
|DIP-28 wide pin socket|0.78|To ease assembly and so the processor can be removed from pcb for programming bootloader|
|Micro SD Storage module|0.73|Needed to save data to a sd card|
|Small 16GB Micro SD Card|2.2|Don't need much space but I've had trouble with generic brand cards.Microdrive brand availible on aliexpress|
|GY-273 HMC5883L Triple Axis Compass Magnetometer Sensor Module|2.16|To determine magnetic direction of tidal current|
|PCF8563T IIC Real Time Clock Module|1.11|microoprocessor doesn't have any persistant way of knowing the date/time to log with observations.|
|DS18B20 Digital Temperature Sensor|0.688|Accurate within 0.5 degree C. Low power. Pre-calibrated.|
|ADXL362 3-Axis Digital Accelerometer Sensor|2.87|for tilt sensor design|
|LM1117 3.3v voltage regulator|0.43|Draws 5mA when not in use so will need to be powered down when not in use|
|2N3904 and 2N3906 transistors|0.4|for switching on/off sensors, buzzers, etc|
|5v Buzzer|0.37|To give indication of sucessful power on|
|2x Magnetic reed switches|0.3||
|Single sided copper pcb board|1|Est cost each… would likely CNC multiple boards at once from a single large sheet|
|Other assorted electrical components (resistors, diodes, solder, etc)|2|Just a ballpark cost estimate for now|
|3x D Battery holders|2.91||
|PVC Pipe|5||
|4x O rings 100mm OD 4mm dia|2.72||
|Assorted stainless screws, stainless swivels, zip ties, mounting hardware,etc|5||
|3d printing liquid resin|9||
|3d printing filiment|3||
|Tax|3.2828||
|Total|48.1108||

<img src=https://github.com/Cmjstealth/Subsurface-Current-Sensor/blob/main/Pictures/20240610_233220%20Breadboard%20prototype.jpg width=400></img>
