# GuitarAmp
A basic and experimental guitar modeling amp. It models a preamp circuit with four ECC83/12AX7 tubes followed by a 
speaker cabinet and mic simulation.

![](https://raw.githubusercontent.com/apohl79/GuitarAmp/master/manual/screenshot.png "Screenshot")

Features:
---------

- VST2, VST3, AU and Standalone - all OS X only, sorry (feel free to port it, WDL supports windows too)
- Builtin speaker/mic IRs by [Gregor Hennig](http://www.grgr.de/index.html#ir) of [Studio Nord](http://www.studio-nord.net)
- Load your own IRs
- Based on [WDL-OL](https://github.com/olilarkin/wdl-ol)
- Uses [Audio-Toolkit](http://www.audio-tk.com)
- Tube model implemented as audio toolkit filter, see [here](https://github.com/apohl79/AudioTK/blob/Triode3Filter/ATK/Preamplifier/Triode3Filter.h) for the code
- Tube model based on a modefied version of [this](http://www.hs-ulm.de/opus/frontdoor.php?source_opus=114) thesis

 
Windows port:  
-------------  

Porting done by nofish  
https://github.com/nofishonfriday/GuitarAmp  

NOTES:


- Usage:

Requires libsndfile-1.dll (32 or 64 bit) to be present on your computer.  
(otherwise host will probably complain during scanning)  
 
Put the libsndfile-1.dll with the same architecture as your host (32 or 64 bit) either in  
-- install folder of your host/DAW or  
-- Windows system path:  
64 bit version: C:\Windows\System32  
32bit version: C:\Windows\SysWow64  
Get it here:  
http://www.mega-nerd.com/libsndfile/#Download


- Building:

Done with Visual Studio 2015 CE
(only VST2 and VST3 versions are pre-configured)

Set up environment variables:  
AudioTKTriode3Filter_ROOT  
libsndfile_ROOT  
r8brain_ROOT  

Folder structure should look like this:  

$(AudioTKTriode3Filter_ROOT)  
\debug  
\\\32  
\\\\\include  
\\\\\lib  
\\\64  
\\\\\include  
\\\\\lib  

\release  
\\\32  
\\\\\include  
\\\\\lib  
\\\64  
\\\\\include  
\\\\\lib  

$(libsndfile_ROOT)  
\libsndfile   
(libsndfile 32bit installation)  
\libsndfile_64  
(libsndfile 64bit installation)  

$(r8brain_ROOT)  
\r8brain-free-src  
(r8brain source)  

