# msfs-neofly-aircraft-data-extractor

![Apache-2.0 License](https://img.shields.io/github/license/savormix/msfs-neofly-aircraft-data-extractor)
![MSVC C++](https://img.shields.io/github/languages/top/savormix/msfs-neofly-aircraft-data-extractor)
![Lines of code](https://img.shields.io/tokei/lines/github/savormix/msfs-neofly-aircraft-data-extractor)
![MSFS SDK 0.15.0.0](https://img.shields.io/badge/MSFS%20SDK-0.15.0.0-lightgrey)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/savormix/msfs-neofly-aircraft-data-extractor)


A console application to display aircraft data required to add/update an aircraft in NeoFly in real time (as different aircraft/liveries are selected in MSFS).

## Usage

- Run msfs-neofly-aircraft-data-extractor.exe
- Console window opens (leave it open; aircraft data will be displayed here):

  `MSFS Aircraft Data Extractor for NeoFly v1.1.2`
- Confirm successful connection with MSFS (or start MSFS and wait for it):

  `--- Connected to MSFS`
- When MSFS finishes initial loading (if not already), aircraft data will be displayed, for example:
  ```markdown
  Current aircraft:
  - Name: 103 Solo Aviators Club Livery
  - Type: Prop
  - Engine(s): 1 (Piston)
  - Category: A
  - Cruise speed (kt): 47
  - Range (NM): Unknown
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
  - Cruise speed (kt): 78
  - Range (NM): Unknown
  - Empty weight (lbs): 650
  - Max fuel (lbs): 168
  - Max payload (lbs): 670
  - Max takeoff weight (lbs): 1320
  - Pax: 0
  ```
- Repeat as necessary:
  ```markdown
  Current aircraft:
  - Name: B787 10 World Travel Livery
  - Type: Airliner
  - Engine(s): 2 (Jet, Jet)
  - Category: E
  - Cruise speed (kt): 495
  - Range (NM): Unknown
  - Empty weight (lbs): 298700
  - Max fuel (lbs): 223673
  - Max payload (lbs): 258300
  - Max takeoff weight (lbs): 557000
  - Pax: -1
    ```
  ```markdown
  Current aircraft:
  - Name: TBM 930 01 Livery
  - Type: Turbo Prop
  - Engine(s): 1 (Turboprop)
  - Category: C
  - Cruise speed (kt): 330
  - Range (NM): 1730
  - Empty weight (lbs): 4629
  - Max fuel (lbs): 1956.4
  - Max payload (lbs): 2765
  - Max takeoff weight (lbs): 7394
  - Pax: 5
  ```
  ```markdown
  - Current aircraft:
  - Name: Just Flight PA28 Warrior II Custom
  - Type: Prop
  - Engine(s): 1 (Piston)
  - Category: A
  - Cruise speed (kt): 111
  - Range (NM): 637
  - Empty weight (lbs): 1300
  - Max fuel (lbs): 288
  - Max payload (lbs): 1025
  - Max takeoff weight (lbs): 2325
  - Pax: 3
  ```
- When finished, close the console window

## FAQ

- **Q: Why is aircraft range "Unknown"?**

  A: _Reference values_ (values named `ui_…` in `aircraft.cfg`) are not used during simulation and are not available via SimConnect.
  This application will extract the range value from file for non-marketplace aircraft, but there is **no EULA compliant way to do this for marketplace-locked aircraft**.

  If you receive an `Unknown` value for non-marketplace aircraft, [raise an issue](https://github.com/savormix/msfs-neofly-aircraft-data-extractor/issues).

- **Q: Why does the cruise speed not match what is shown at aircraft selection screen?**

  A: This application provides the _Aircraft Design Vc_ value. Some aircraft are known to specify their afterburner speed in the selection menu instead.

- **Q: Why are airliners shown with -1 passengers?**
  
  A: Not enough data available (to SimConnect or in general) to report an accurate number.
- **Q: The type/category/pax number/max range for aircraft X is completely wrong!**

  A: [Raise an issue](https://github.com/savormix/msfs-neofly-aircraft-data-extractor/issues)

## Changelog

### Version 1.1.2

- Classify instructor as pax

### Version 1.1.1

- Correction for Airliner vs Jet classification with regards to pax

### Version 1.1.0

- Added Range value to output

  (reports `Unknown` for marketplace-locked aircraft)

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
