## A Beginner's Guide to ESP8266

Some time ago, I wrote [a Beginner's Guide to Arduino](http://www.instructables.com/id/A-Beginners-Guide-to-Arduino/) that seems to be quite popular, so I decided to create a follow-up: **A Beginner's Guide to ESP8266**. That's right, a tutorial on how to use the world's most popular $3 WiFi board.

Please note: this Instructable expects basic knowledge of microcontrollers like the Arduino, if that's something you're not already familiar with, I recommend to read my Beginner's Guide to Arduino first, it covers a lot of the basics that I won't go into in this article.

I really want to focus on the ESP8266-specific things, like WiFi, hardware, softwere, IOT, etc ...

### What is an ESP8266?

The ESP8266 is a System on a Chip (SoC), manufactured by the Chinese company [Espressif](https://espressif.com/en/). It consists of a Tensilica L106 32-bit **micro controller** unit (MCU) and a **WiFi transceiver**. It has **11 GPIO pins*** (General Purpose Input/Output pins), and an **analog input** as well. This means that you can program it like any normal Arduino or other microcontroller. And on top of that, you get WiFi communication, so you can use it to connect to your WiFi network, connect to the Internet, host a web server with real web pages, let your smartphone connect to it, etc ... The possibilities are endless! It's no wonder that this chip has become the most popular IOT device available. 

There are many different modules available, standalone modules like the [ESP-## series](http://en.ai-thinker.com/html/Products/WIFI_Module/ESP_01-14Series/) by AI Thinker, or complete development boards like the [NodeMCU DevKit](http://nodemcu.com/index_en.html) or the [WeMos D1](http://www.wemos.cc/). They all have different pins broken out, have different WiFi antennas, or a different amount of flash memory on board.

The easiest way to get one is to buy a board from <span style="color: #d7001d;">Adafruit, but if you want it cheap, you should check out <span style="color: #d7001d;">Ebay and ####

<span style="color: #d7001d;">


<span style="font-size: 13.3333px;">(*) The ESP8266 chip itself has 17 GPIO pins, but 6 of these pins (6-11) are used for communication with the on-board flash memory chip.

### Programming

There are a lot of different ways to program the ESP8266, but I'll only cover the method using the Arduino IDE. This is really easy for beginners, and a very familiar environment if you've used Arduino boards before. 

But know that it's not limited to this option: there's also an official SDK available to program it in real C, this is very useful if you want to optimize your code or do some advanced tricks that aren't supported by the Arduino IDE. Another possibility is to flash it with a <span style="color: #d7001d;">LUA interpreter, so you can upload and run LUA scripts. Or maybe you're more familiar with Python? Then you should check out the <span style="color: #d7001d;">MicroPython firmware to interpret MicroPython scripts. I'm sure there's other languages available as well, so just do a quick Google search and write your code in the language of your choice.

### Requirements

You'll need a couple of things in order to follow this guide:



*   An ESP8266 board
*   A computer that can run the Arduino IDE
*   A USB-to-Serial converter, it is very important that you use a **3.3V** model
*   A USB cable
*   A 3.3V power supply or voltage regulator

### Before you begin ...

There's a few things you have to look out for when using an ESP8266: The most important thing is that it runs at 3.3V, so if you connect it to a 5V power supply, you'll kill it. Unlike some 3.3V Arduino or Teensy boards, **the ESP8266's I/O pins are not 5V tolerant**, so if you use a 5V USB-to-Serial converter, or 5V sensors etc. you'll blow it up. 

A second thing to remember is that the ESP8266 can only supply or sink **11mA per output pin**, compared to 20-40mA for most Arduinos.

The ESP8266 has one **analog to digital converter**, but it has a strange voltage range: **0 - 1V**, voltages above 1V might damage the board.

One last thing to keep in mind is that the ESP8266 has to share the system resources and CPU time between your sketch and the WiFi driver. Also, features like PWM, interrupts or I²C are emulated in software, most Arduinos on the other hand, have dedicated hardware parts for these tasks.

For most applications however, this is not too much of an issue.

## Deciding on what board to buy

ESP8266 is just the name of the chip, many companies have designed their own boards that use this chip, so there are many different ESP8266 boards on the market. If you don't know the difference between all these different models, you might have a hard time deciding on which board to buy.

### Development boards

Some boards have all kinds of features on-board to help developing ESP8266 hardware and software: for example, a USB to Serial converter for programming, a 3.3V regulator for power, on-board LEDs for debugging, a voltage divider for analog inputs ...

If you're a beginner, I would definitely recommend a development board. It's easier to get started if you don't have to worry about all these things.


Here's a table with some of the most popular ESP8266 development boards and their features:

==TODO==

### Bare-bones AI Thinker boards

If you want to add an ESP8266 to a small project, or if you want a cheaper* board, you might want to buy a board that doesn't have these features. In that case, you can buy one of the many boards developed by AI Thinker. They contain just the ESP8266 and the necessary components to run it. Sometimes you get an on-board antenna (PCB or ceramic) and an LED, some boards have just an antenna connector, or no LEDs at all. They also differ in physical size, and flash memory size. An important thing to notice, is that some boards do not <span style="color: #d7001d;">break out all GPIO pins. For example, the ESP-01 only has 1 I/O pin available (apart from the TX and RX pins), while the ESP-07 breaks out all available I/O pins.

## Installation of the required software

The first step is to download and install the Arduino IDE. I explained this in [A Beginner's Guide to Arduino](https://www.instructables.com/id/A-Beginners-Guide-to-Arduino/step3/Software/). (As of February 7th 2017, the latest stable version is 1.8.1.)

To program the ESP8266, you'll need a plugin for the Arduino IDE, it can be manually downloaded from [GitHub](https://github.com/esp8266/Arduino), but it is easier to just add the URL in the Arduino IDE:



1.  Open the Arduino IDE.
2.  Go to File > Preferences.
3.  Paste the URL [http://arduino.esp8266.com/stable/package_esp8266com_index.json](http://arduino.esp8266.com/stable/package_esp8266com_index.json) into the _Additional Board  Manager URLs_ field.
    (You can add multiple URLs, separating them with commas.)
4.  Go to Tools > Board > Board Manager and search for 'esp8266'. Select the newest version, and click install. (As of February 7th 2017, the latest stable version is 2.3.0.)

### Drivers

If you are using a board with the CH340(G) USB-to-Serial chip, like the NodeMCU, you'll probably have to install the USB drivers for it.

They can be found on [GitHub](https://github.com/nodemcu/nodemcu-devkit/tree/master/Drivers). 

If you are using a board with the CP2104 USB-to-Serial chip, like the Adafruit Feather HUZZAH board, you'll probably have to install USB drivers as well. You can find them on the [Silicon Labs website](http://www.silabs.com/products/mcu/pages/usbtouartbridgevcpdrivers.aspx).

Boards with an FTDI chip should work right out of the box, without the need of installing any drivers.




## Getting the hardware ready

There are two main categories of ESP8266 boards: development boards with a USB interface (USB-to-Serial convertor) on-board, and boards without a USB connection.

### Development boards with a USB interface

For example: NodeMCU, SparkFun ESP8266 Thing - Dev Board, SparkFun Blynk Board, Adafruit Feather HUZZAH with ESP8266 WiFi ...

These boards will show up in Device manager (Windows) or in lsusb (Linux) as soon as you plug them in.

They have a 3.3V regulator on-board, and can be programmed over USB directly, so you don't need any external components to get it working.

The only thing you may need to do, is solder on some headers.

### Bare-bone boards and boards without a USB interface

This category has 2 sub-categories: boards with a 3.3V regulator on-board, and boards with just the ESP8266 and a flash memory chip, without 3.3V regulator. If your board doesn't have a 5V to 3.3V regulator, buy one separately. You could use an [LM1117-3.3](http://cds.linear.com/docs/en/datasheet/1117fd.pdf) for example.

To program the board, you'll need a USB-to-Serial converter. The FTDI FT232RL is quite popular, because it can switch between 5V and 3.3V. **It is essential that the USB-to-Serial converter you buy operates at 3.3V. If you buy a 5V model, you will damage the ESP8266.**

#### Connecting the USB-to-Serial converter

1.  Connect the ground (GND) of the USB-to-Serial converter to the ground of the ESP8266.

2.  Connect the RX-pin of the USB-to-Serial converter to the TXD pin of the ESP8266\. (On some board, it's labelled TX instead of TXD, but it's the same pin.)
3.  Connect the TX-pin of the USB-to-Serial converter to the RXD pin of the ESP8266. (On some board, it's labelled RX instead of RXD, but it's the same pin.)
4.  If your ESP8266 board has a DTR pin, connect it to the DTR pin of the USB-to-Serial converter. This enables auto-reset when uploading a sketch, more on that later.



#### Enabling the chip

If you're using a bare-bone ESP-## board by AI Thinker, you have to add some resistors to turn on the ESP8266, and to select the right boot mode. 



1.  Enable the chip by connecting the CH_PD (Chip Power Down, sometimes labeled CH_EN or chip enable) pin to V<sub>CC </sub>through a 10KΩ resistor.
2.  Disable SD-card boot by connecting GPIO15 to ground through a 10KΩ resistor.
3.  Select normal boot mode by connecting GPIO0 to V<sub>CC</sub> through a 10KΩ resistor, and  connect GPIO2 to V<sub>CC</sub> through a 10KΩ resistor as well.
4.  Prevent random resets by connecting the RST (reset) pin to V<sub>CC</sub> through a 10KΩ resistor.

#### Adding reset and program buttons



If your ESP8266 board doesn't have a reset button, you could add one by connecting a push button to between the RST pin and ground.

To put the chip into programming mode, you have to pull GPIO0 low during startup. That's why we also need a program button. Since it's possible to use GPIO0 as an output, we can't directly short it to ground, that would damage the chip. To prevent this, connect 470Ω resistor in series with the switch. It's important that this resistance is low enough, otherwise, it will be pulled high by the 10KΩ resistor we added in the previous paragraph.

#### Connecting the power supply

If the ESP8266 module you have doesn't have a 3.3V voltage regulator on board, you have to add one externally. You could use an [LM1117-3.3](http://cds.linear.com/docs/en/datasheet/1117fd.pdf) for example.



1.  Connect the first pin of the regulator to ground.
2.  Place a 10µF capacitor between pin 2 and ground. Watch the polarity!
3.  Place a 10µF capacitor between pin 3 and ground.
4.  Connect pin 2 to the 3.3V or V<sub>CC </sub>of the ESP8266.
5.  Connect pin 3 to a 5V power source, a USB port, for example.








[![ESP8266.png](/photo.php?id=715&thumb=1)](/photo.php?id=715 "ESP8266.png")[![VReg.png](/photo.php?id=716&thumb=1)](/photo.php?id=716 "VReg.png")