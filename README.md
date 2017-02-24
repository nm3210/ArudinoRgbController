# Arudino Rgb(w) Project Collection
This repository contains a bunch of projects that I've created over the years for controlling various RGB (and sometimes RGBW) LED projects using arduinos uno's/teensy's. I have primarily been using the Eclipse-based [Sloeber](http://eclipse.baeyens.it/stable.php?OS=Windows) Arduino IDE which makes programming in C++ for the arduino platform much eaiser (especially if you're used to Eclipse).

## Main Projects:
* [ArudinoRgbController](ArudinoRgbController): This contains the original Atmega328 arduino (duo/uno) code that was used on a project to control some RGB LEDs (the simple four-lead ones) based on the time from a real-time clock (RTC) input. 

    The idea was to be a 'Visual Alarm', inasmuch that the LEDs would change color based on the time during the day. For this project, I also designed a circuit board with all the necessary RTC inputs and power for the LEDs which can be found on OSH Park [here](https://oshpark.com/shared_projects/ZrMcpgAp) (images of the circuit board below). 
    
    [![Visual Alarm Rev3](https://644db4de3505c40a0444-327723bce298e3ff5813fb42baeefbaa.ssl.cf1.rackcdn.com/uploads/project/bottom_image/ZrMcpgAp/thumb_i.png)](https://644db4de3505c40a0444-327723bce298e3ff5813fb42baeefbaa.ssl.cf1.rackcdn.com/uploads/project/bottom_image/ZrMcpgAp/i.png) [![Visual Alarm Rev3](https://644db4de3505c40a0444-327723bce298e3ff5813fb42baeefbaa.ssl.cf1.rackcdn.com/uploads/project/top_image/ZrMcpgAp/thumb_i.png)](https://644db4de3505c40a0444-327723bce298e3ff5813fb42baeefbaa.ssl.cf1.rackcdn.com/uploads/project/top_image/ZrMcpgAp/i.png)
    
    
* [RgbControllerWithIR](RgbControllerWithIR): This is an extension of the above ArudinoRgbController project that adds in the [IRremote library](https://github.com/z3t0/Arduino-IRremote) in order to control the colors a little easier.

    A *big* note: it was necessary to move the output pin for the blue RGB from pin 11 to pin 5 because of a conflict with the internal timer libraries and the IRremote ([see issue here](https://github.com/z3t0/Arduino-IRremote/issues/251)). This change required me to physically ripup the connection between pin 11 and the blue led transistor and then white wire a new connection to pin 5. Then the IR receiver was added to pin 2.
    
    <img src="http://i.imgur.com/oDfzFbK.png" alt="Circuit board ripped up and IR added" width=40% height=40% />


## Testing & Prototyping Projects:
### NeoPixel (WS2812B/SK6812, specifically RGBW strips)
* [NeoPixelTesting](NeoPixelTesting): This project plays around with the NeoPixel RGBW LED strips. I initially bought the [AdaFruit](https://www.adafruit.com/products/2868) stick of eight but eventually bought a roll of them off ebay (5m of 30LEDs/m).
* [MultiSegmentTesting](MultiSegmentTesting): This is an extension of the NeoPixelTesting project that adds in the ability to split up the strip into differnt arbitrary-length segments.

### RTC (DS1307)
* [RTC_SetTime](RTC_SetTime): Set/initialize the time of the DS1307 to the time at compile.
* [RTC_ReadTime](RTC_ReadTime): Read back the time on the DS1307 with the ability to add in extra seconds with a button press.
* [RTC_AlarmTesting](RTC_AlarmTesting): Test the [TimeAlarm](https://github.com/PaulStoffregen/TimeAlarms) library with the DS1307.

### Other/Misc
* [IrRemoteTesting](IrRemoteTesting): Test the incorporation of an IR receiver on an arduino.
* [ButtonTesting](ButtonTesting): Test the use of the AIN0 comparator on the atmega328 for use as a button.
* [HsiColor](HsiColor): Uses the 'HSI' colorspace instead of RGB, making it easier to do crossfading, etc.
