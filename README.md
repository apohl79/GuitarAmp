# GuitarAmp
A basic and experimental guitar modeling amp. It models a preamp circuit with four ECC83/12AX7 tubes followed by a 
speaker cabinet and mic simulation.

![](https://raw.githubusercontent.com/apohl79/GuitarAmp/master/manual/screenshot.png "Screenshot")

Features:
---------

- VST2, VST3, AU and Standalone - all OS X only, sorry (feel free to port it, WDL supports windows too)
- 600+ speaker/mic IR responses builtin
- Load your own IR responses
- Based on [WDL-OL](https://github.com/olilarkin/wdl-ol)
- Uses [Audio-Toolkit](http://www.audio-tk.com)
- Tube model implemented as audio toolkit filter, see [here](https://github.com/apohl79/AudioTK/blob/Triode3Filter/ATK/Preamplifier/Triode3Filter.h) for the code
- Tube model based on a modefied version of [this](http://www.hs-ulm.de/opus/frontdoor.php?source_opus=114) thesis
