# msfs-neofly-aircraft-data-extractor

![Apache-2.0 License](https://img.shields.io/github/license/savormix/msfs-neofly-aircraft-data-extractor)
![MSVC C++](https://img.shields.io/github/languages/top/savormix/msfs-neofly-aircraft-data-extractor)
![Lines of code](https://img.shields.io/tokei/lines/github/savormix/msfs-neofly-aircraft-data-extractor)
![MSFS SDK 0.14.1.0](https://img.shields.io/badge/MSFS%20SDK-0.14.1.0-lightgrey)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/savormix/msfs-neofly-aircraft-data-extractor)


A console application to display aircraft data required to add/update an aircraft in NeoFly in real time (as different aircraft/liveries are selected in MSFS).

## Usage

- Run msfs-neofly-aircraft-data-extractor.exe
- Console window opens (leave it open; aircraft data will be displayed here):

  `MSFS Aircraft Data Extractor for NeoFly v1.0.1`
- Confirm successful connection with MSFS (or start MSFS and wait for it):

  `--- Connected to MSFS`
- When MSFS finishes initial loading (if not already), aircraft data will be displayed, for example:
  ```markdown
  Current aircraft:
  - Name: Top Rudder 103 Solo Asobo Small Wheel
  - Type: Prop
  - Engine(s): 1 (Piston)
  - Category: A
  - Cruise speed (kt): 47, est. 54
  - Empty weight (lbs): 254
  - Max fuel (lbs): 30
  - Max payload (lbs): 296
  - Max takeoff weight (lbs): 550
  - Pax: 0
  ```
- Select a different aircraft/livery and go back to the main menu:
  ```markdown
  Current aircraft:
  - Name: Savage Shock Ultra Asobo Skis
  - Type: Prop
  - Engine(s): 1 (Piston)
  - Category: A
  - Cruise speed (kt): 78, est. 91
  - Empty weight (lbs): 650
  - Max fuel (lbs): 168
  - Max payload (lbs): 670
  - Max takeoff weight (lbs): 1320
  - Pax: 0
    ```
- Repeat as necessary:
  ```markdown
  Current aircraft:
  - Name: B787 10 Xbox Aviators Club Livery
  - Type: Airliner
  - Engine(s): 2 (Jet, Jet)
  - Category: E
  - Cruise speed (kt): 495, est. 444
  - Empty weight (lbs): 298700
  - Max fuel (lbs): 223673
  - Max payload (lbs): 258300
  - Max takeoff weight (lbs): 557000
  - Pax: -1
  ```
- When finished, close the console window

## FAQ

- Q: Where is the range value?

  A: **Planned for version 1.1** (non-marketplace aircraft)

  _Reference values_ (values named `ui_…` in `aircraft.cfg`) are not used during simulation and are not available via SimConnect.
  There is a plan to implement extracting them in the future, but it will not work for any marketplace aircraft
  (including Premium/Deluxe edition aircraft). That would need a different approach, which may or may not be feasible.
- Q: Why two cruise speeds? And why neither matches what is shown when selecting aircraft?

  A: Aircraft selection screen uses _reference values_, which can be set to anything, regardless of the aircraft's own flight model.
  The first cruise speed is the aircraft's design Vc and is what you should use. The second speed, shown after `est.` is a MSFS estimation.
- Q: Why are airliners shown with -1 passengers?
  
  A: There is not enough data available (to SimConnect or in general) to report an accurate number.
- Q: The type/category/pax number for aircraft X is completely wrong!

  A: [Raise an issue](https://github.com/savormix/msfs-neofly-aircraft-data-extractor/issues)

## Changelog

### Version 1.0.1

- Adjusted passenger counting logic
- Removed second cruise speed value (MSFS estimate) from output

## Building the application

### Requirements

- Visual Studio build tools
  - Install the latest if in doubt
  - Make sure to select "Desktop development with C++"
- CMake 3.20 or later

### Build

Execute `cmake` in this directory (directory with `CMakeLists.txt`).

The original releases are optimized for size, which can be done with `cmake -DCMAKE_BUILD_TYPE=MinSizeRel`.
