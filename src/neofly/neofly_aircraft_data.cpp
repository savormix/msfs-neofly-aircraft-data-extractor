#include "neofly_aircraft_data.h"

#include <algorithm>
#include <cstring>

// this will obviously not work for anything that is bypassing localization files
const char *CARGO_SUBSTRINGS[] = {"BAGGAGE", "CARGO", "CABINET", "LUGGAGE"};
const char *PASSENGER_SUBSTRINGS[] = {"ATTENDANT", "CABIN", "COPILOT", "INSTRUCTOR", "P1", "P2", "PASSENGER", "PASS", "PAX"};
const char *UNKNOWN_SUBSTRINGS[] = {"BUSINESS", "CLASS", "CREW", "ECONOMY", "GROUP", "PILOTS"};

void uppercase(const char *src, char *dst) {
    size_t length = strlen(src);
    for (size_t i = 0; i < length; ++i) {
        if (src[i] >= 'a' && src[i] <= 'z') {
            dst[i] = (char)(src[i] - 32);
        } else {
            dst[i] = src[i];
        }
    }
    dst[length] = 0;
}

namespace NeoFly {
    const char *GetCategory(NeoFlyAircraftData *neoFlyData) {
        uint32_t pistons = 0;
        uint32_t turboprops = 0;
        uint32_t jets = 0;
        for (uint32_t i = 0; i < neoFlyData->dwEngineCount; ++i) {
            switch (neoFlyData->dwEngineTypes[i]) {
                case 0:
                    ++pistons;
                    break;
                case 1:
                    ++jets;
                    break;
                case 5:
                    ++turboprops;
                    break;
            }
        }
        if (jets == 4) {
            return "F";
        }
        if (jets > 0) {
            return "E";
        }
        if (turboprops >= 2) {
            return "D";
        }
        if (turboprops >= 1) {
            return "C";
        }
        if (pistons >= 2) {
            return "B";
        }
        return "A";
    }

    const char *GetType(NeoFlyAircraftData *neoFlyData) {
        const char category = GetCategory(neoFlyData)[0];
        switch (category) {
            case 'A':
            case 'B':
                return "Prop";
            case 'C':
            case 'D':
                return "Turbo Prop";
            case 'E':
                // a pretty arbitrary condition here
                if (neoFlyData->dEmptyWeightPounds >= 50000) {
                    return "Airliner";
                }
                return InferPassengerCapacity(neoFlyData) >= 0 ? "Jet" : "Airliner";
            case 'F':
                return "Airliner";
            default:
                return "Unknown";
        }
    }

    int32_t InferPassengerCapacity(NeoFlyAircraftData *neoFlyData) {
        if (neoFlyData->dwPayloadStationCount <= 1) {
            return 0;
        }
        char uppercased[256];
        bool pilotExcluded = false;
        int32_t result = 0;
        for (uint32_t i = 0; i < neoFlyData->dwPayloadStationCount; ++i) {
            uppercase(neoFlyData->szPayloadStationNames[i], uppercased);
            for (auto &sub: UNKNOWN_SUBSTRINGS) {
                if (strstr(uppercased, sub) != nullptr) {
                    return -1;
                }
            }
            bool cargo = false;
            for (auto &sub: CARGO_SUBSTRINGS) {
                if (strstr(uppercased, sub) != nullptr) {
                    cargo = true;
                    break;
                }
            }
            if (cargo) {
                continue;
            }
            for (auto &sub: PASSENGER_SUBSTRINGS) {
                if (strstr(uppercased, sub) != nullptr) {
                    ++result;
                    break;
                }
            }
            if (strstr(uppercased, "PILOT") != nullptr) {
                pilotExcluded = true;
            }
        }
        return pilotExcluded ? result : std::max(0, result - 1);
    }
}

namespace FlightSimulator {
    const char *GetEngineTypeString(uint32_t engineType) {
        switch (engineType) {
            case 0:
                return "Piston";
            case 1:
                return "Jet";
            case 2:
                return "None";
            case 3:
                return "Turbine";
            case 4:
                return "Unsupported";
            case 5:
                return "Turboprop";
            default:
                return "Unknown";
        }
    }
}
