##Deprecated
This version of MungPlex is deprecated. You can find [the new one here](https://github.com/CosmoCortney/MungPlex)

MungPlex is a game hacking, cheating and reverse engineering tool for various emulators! Once a supported emulator has launched a game you can hook this tool to it in order to search for cheats, create cheats as Lua scripts, search for pointers, dump memory and much more!

## Usage

## Main Features
![Main UI features](https://github.com/CosmoCortney/MungPlex/tree/master/readme_files/ui01.png?raw=true)

1. Connection section
   - 1.1 Select an emulator or PC game (by selecting "PC" from the dropbox). Note: Game and emulator are assumed to be running
   - 1.2 Hook MungPlex to the selected process
   - 1.3 if "PC" is selected you will be asked to select a process (game) that is currently running. The selected process' name will be displayed here
   - 1.4 Tells you if the connection attempt to the target process was successful

2. Game Information
3. Features additional tools like Memory Viewer and Data Conversion

4. Tabs of different categories of features to select from
   - 4.1 Memory Search: Allows you to search for cheat codes. PC games not supported yet
   - 4.2 Cheat Codes: Executes cheats in Lua format
   - 4.3 Pointer Search: Allows you to search for pointers. PC games not supported yet
   - 4.4 Process Information: Displays (perhaps) useful information about the emulator/process
   - 4.5 Displays information about your system
   - 4.6 Settings: Settings to define

## Search Tab

![Search Tab](https://github.com/CosmoCortney/MungPlex/tree/master/readme_files/ui02.png?raw=true)

1. Range Options
   - Memory Range: Select the memory range you want to process
   - Start: Begin of desired search-range. Must be expressed as hex number. Can be >= than the begin of the selected memory range.
   - End: End of desired search-range. Must be expressed as hex number. Can be <= than the end of the selected memory range.
   - I recommend using ranges not bigger than 0x10000000 (256MB). MungPlex saves a copy of the range in your system memory (RAM) alongside an address list 8 times as big (if unaligned). A cheat search with a range of 1GB can consume 9GB of RAM before refining.

2. Value Options
   - Value Type: The type of values you want to search for
   - Big Endian: Check this if the emulated system maps its memory in big endian. The recommended option will be selected for you when connecting.
   - Signed: Whether you want to allow the values to be interpreted as signed values.

3. Search Options
   - Start/Next: Starts or continues a memory search with the selected options.
   - Cancel Search: Aborts search.
   - Known: Here you can enter a known value to perform comparisions with.
   - Extensions: Values for extended options (to do)
   - Condition: 
     - Switch between Unkown and Known value searches
     - Select comparision type
     - Accuracy: Set accuracy for floats, doubles and arrays (to do)
     - Aligned: Whether the search is aligned by 4 bytes (2 bytes for Int16) or not
     - Values are HEX: Whether to enter and view results as hex
     - Enable chache: Saves searches on HDD rather than RAM. Performance will be much slower (todo).

4. Results
   - Displays your result's addresses, current values, previous values and differences (max. 1024 results per page).

5. Result testing
   - By clicking on any result the address and current value will be copied here
   - Poke all selected: Whether all selected values should be tested at once or not
   - Poke previous value: When testingall selected you can do this by poking all previous values rather than a custom value.

6. Search results information and view
   - Change page, counts iterations and results   


## Cheats Tab

![Cheats Tab](https://github.com/CosmoCortney/MungPlex/tree/master/readme_files/ui03.png?raw=true)

- Open Cheats Dir: Open a directory with cheat files
  - GroupBox: Select whether you want to use cheats from the list or write down a text cheat yourself.
  - Execute Cheats: Executes cheats
  - Terminate Cheats: terminates cheats
  - Cheat List: Lists all cheat files from the selected directory. Check those you want to execute. Doubleclicking a cheat allows you to edit it.
  - Lua Text: Enter a Lua script
  - Save As...: Save Lua Text to new file or overwrite an existing one.
  - Log: Logs information and errors

- Check out the official Lua documentation: https://www.lua.org/manual/5.3/

- Following functions were implemented that are not part of the Lua documentation:
  - WriteToRAM([DataType], [Address], [Value]): nil
    - Write [Value] of <DataType] to [Address]. Returns nothing (nil)
    - [DataType]: See DataType list below
    - [Value]: Value compatible with [DataType]. Can be a variable or represented as hex or dec
    - [Address]: Logical address to write to. Can be a variable or represented as hex or dec

  - ReadFromRAM([DataType], [Address]): [DataType]
    - Reads and returns a Value of [DataType] from Address

  - LogToGUI([Value]): nil
    - Logs anything to the GUI's log area

  - ClearLog(): nil
    - Clears the log area

- [DataType]
  - BOOL: [Value] is boolean
  - INT8: [Value] is 8-bit integer (value is never negative because it only occupies 8 bits of the Lua integer (64 bits))
  - INT16: [Value] is 16-bit integer (value is never negative because it only occupies 16 bits of the Lua integer (64 bits))
  - INT32: [Value] is 32-bit integer (value is never negative because it only occupies 8 bits of the Lua integer (64 bits))
  - INT64: [Value] is 64-bit integer
  - FLOAT: [Value] is float (the value is read as float and then converted to Lua number to be usable)
  - DOUBLE: [Value] is double
  
- EXAMPLES
  - `WriteToRAM(INT32, 0x80123456, 1337)`
  - `tempVal = ReadFromRAM(FLOAT, 0x80456789)`
  - `LogToGUI(tempVal)`


## Deployment

#### Configuring the project
Compiler: Desktop Qt <your version> MinGW 64-bit
Build step:
-	`qmake: qmake.exe MungPlex.pro -spec win32-g++ "CONFIG+=qtquickcompiler"`
-	`Make: mingw32-make.exe -j4 in <your build folder>`

#### Deployment
1. Set build to 'Release'
2. Click on 'Build' (the hammer icon)
3. Go to the deployment folder where MungPlex.exe is located
4. Create folders named 'css', 'platforms' and 'styles' (without ')
5. Put all theme .css files into the css folder
6. Copy the following files from <your Qt installation>\<your Qt version>\mingw<version>_64\bin\
	  - libgcc_s_seh-1.dll
	  - libstdc++-6.dll
	  - libwinpthread-1.dll
	  - Qt5Core.dll
	  - Qt5Gui.dll
	  - Qt5Widgets.dll to the deployment folder
7. Copy advapi32.dll from C:\Windows\System32\ into your deployment folder. If it's not found there take it from previous MungPlex releases, check SysWOW64 or download it from a trustworthy source
8. Copy the following files from <your Qt installation>\<your Qt version>\mingw<version>_64\bin\platforms
	- qdirect2d.dll
	- qminimal.dll
	- qoffscreen.dll
	- qwebgl.dll
	- qwindows.dll into the 'platforms' folder of your deployment directory
9. Copy the following files from <your Qt installation>\<your Qt version>\mingw<version>_64\bin\styles
	- qwindowsvistastyle.dll into the 'styles' folder of your deployment directory
10. Copy the settings.json (without any settings being set) into the deplyment folder
11. Delete all .o, .h and .cpp files from the deployment folder
12. Update build revision in 'mainwindow.h' at line 3
13. Test if MungPlex can be launched
