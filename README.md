# dll-injection
A script injecting DLLs into running programs
## About
Made as part of the Windows project at the end of Architecture course at Magshimim
## How To Run
Using Visual Studio, set 'mydll' as Startup Project by right-clicking on it in the Solution Explorer

Build the DLL using ctrl-shift-b

Set 'usemydll' as Startup Project and run it by clicking on the green arrow on top of the window

Run a desired program (notepad.exe as default, can be changed by changing the #define on top of usemydll.cpp)

Enjoy watching it getting DLL injected!
