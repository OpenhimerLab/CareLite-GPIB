# CareLite-GPIB
CareLite is an open-source, tiny, small USB-GPIB adaptor based on STM32 with temperature sensors. 
It uses VCP (Virtual COM Port) and USB Type-C connector, without the need for external power; 
also with dedicated PC software (but not limited to), 
easy GPIB connection can be established and Prologix-like command protocol makes the compatiblity better, and real-time temp logging can be done by DS18B20 or so.

![image](https://github.com/OpenhimerLab/CareLite-GPIB/assets/173666084/5ae77768-cfe6-4e89-9caf-9cfe6163c80b)
![image](https://github.com/OpenhimerLab/CareLite-GPIB/assets/173666084/ffd2a93a-457e-4541-ae86-d95882501d05)


****I am NOT the author with no any relationship or benefit conflict with the author. I have no commercial intention to introdue this project and I have no intention to make benefits from this. 
All responsibility and risk or financial loss should be taken by the DIYers yourselves. The project is introduced and shared "AS IS".
Many functions, issues may exisit so use it at your own risk.
![image](https://github.com/OpenhimerLab/CareLite-GPIB/assets/173666084/7368e382-3808-4992-828d-67f7ef176b9a)

There are 2 version of MeterCare USB-GPIB converter, while sharing the same PC host Software, the firmware is unclear that both version could be applied.
We choose to introduce and share the "Lite" version firstly, as it is a simple, costless design somehow like AR488, 
but the designer reserves potentials to expand other functionality like temperature, humidity sensing ability (there is an UART+DS18B20 port, with proper firmware, it can be amazing for its various functionality!).
![Schematic_CareLite_V1 21](https://github.com/OpenhimerLab/CareLite-GPIB/assets/173666084/042a22eb-3862-49f5-8cae-2c56f9fd99a2)

By the STM32F401, the CareLite can perform GPIB-USB conversion. 
In addition, there is a connector called MeterLink that provides the integration of multiple sensors(1 DS18B20 is confirmed to be functional).

![PCB4](https://github.com/OpenhimerLab/CareLite-GPIB/assets/173666084/2ef71d04-2e4b-4291-a491-d7c86edff05d)

![PCB3](https://github.com/OpenhimerLab/CareLite-GPIB/assets/173666084/37e7a7e5-7d33-4c86-88cd-dc49e2917931)
![PCB2](https://github.com/OpenhimerLab/CareLite-GPIB/assets/173666084/93ca5840-2506-44d1-a392-49162cc71ac6)

Till now, at least 1 DS18B20 OneWire Temperature sensor can be used for logging temperature (by using MeterLink).
![image](https://github.com/OpenhimerLab/CareLite-GPIB/assets/173666084/9c236be9-46f2-42c3-8fd8-f2605ef396dd)
![image](https://github.com/OpenhimerLab/CareLite-GPIB/assets/173666084/b4843e3a-a66e-4241-a5d9-2b3303601551)


![PC Software 2](https://github.com/OpenhimerLab/CareLite-GPIB/assets/173666084/7ec9ccb6-8c17-49b2-b5b0-b78213cea740)

With the software written by "ZDD", you can connect a DMM and do a logging of both volt/ohm and temperature, and it will simutaneously calculate the math like temp.Co, or the deviation of logged data.
This makes it very clear to understand the dependency of multiple variables and its statistical property.
![image](https://github.com/OpenhimerLab/CareLite-GPIB/assets/173666084/303df12f-88a0-44c1-9caf-f68441fe487d)



Also note that the software by ZDD is Labeview-based, you need to install:
1. NI Labview runtime
2. Local emulator to deal with encoding problems, as it is originally based on GBK simplified chinese, not UTF-8.
![MultiTemp](https://github.com/OpenhimerLab/CareLite-GPIB/assets/173666084/09566bb3-3c2e-4cfa-8eeb-87d3c46a3c52)



A sensor board with BMP180 and DS18B20, SHT2X is also possible for environment data logging, there is UART1 definition in MeterLink;
so it might be possible functional. This expansion board is mainly for the "Care" USB-GPIB, not sure if it is functional on CareLite.
![PC Software 1](https://github.com/OpenhimerLab/CareLite-GPIB/assets/173666084/e11030d0-5c59-434f-b434-6bad4b4df803)
![SW in USE 2](https://github.com/OpenhimerLab/CareLite-GPIB/assets/173666084/07b18534-78ac-415f-a746-fecc6fae5177)
![SW in USE 1](https://github.com/OpenhimerLab/CareLite-GPIB/assets/173666084/35fcec1d-972c-46ea-a503-4f0cffe23b97)

The firmware is a bit messy, so try and find the best one for yourself. And explore the function, the Care ecosystem is very potential.
The protocol and communication is very like Prologix, using "++" command system, there are some explaination files, but it's in simplified Chinese,
for now I have no time to translate it, so please try by your own self. 
All in all, this is a tiny, easy to use GPIB adaptor for many devices, somthing like a STM32-based AR488 but with dedicated PC sotware!

For COPYright:
![image](https://github.com/OpenhimerLab/CareLite-GPIB/assets/173666084/d71564c3-2b55-4639-9fdd-ffe5f014cbc6)



