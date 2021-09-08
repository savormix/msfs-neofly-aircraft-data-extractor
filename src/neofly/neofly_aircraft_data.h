#ifndef MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_NEOFLY_AIRCRAFT_DATA_H
#define MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_NEOFLY_AIRCRAFT_DATA_H

#include <cstdint>

namespace NeoFly {
#pragma pack(push, 1)
    struct NeoFlyAircraftData {
        char szTitle[256]; // SIMCONNECT_DATATYPE_STRING256
        uint32_t dwEngineCount; // SIMCONNECT_DATATYPE_INT32
        uint32_t dwEngineTypes[4]; // SIMCONNECT_DATATYPE_INT32
        uint32_t dwCruiseSpeedKt; // SIMCONNECT_DATATYPE_INT32
        uint32_t dwSpeedVCKt; // SIMCONNECT_DATATYPE_INT32
        double dEmptyWeightPounds; // SIMCONNECT_DATATYPE_FLOAT64
        double dMaxFuelGallons; // SIMCONNECT_DATATYPE_FLOAT64
        double dGallonWeightPounds; // SIMCONNECT_DATATYPE_FLOAT64
        double dMaxTakeoffPounds; // SIMCONNECT_DATATYPE_FLOAT64
        uint32_t dwPayloadStationCount; // SIMCONNECT_DATATYPE_INT32
        char szPayloadStationNames[50][256]; // SIMCONNECT_DATATYPE_STRING256
    };
#pragma pack(pop)

    const char* GetCategory(NeoFlyAircraftData*);
    const char* GetType(NeoFlyAircraftData*);
    int32_t InferPassengerCapacity(NeoFlyAircraftData*);
}

namespace FlightSimulator {
    const char* GetEngineTypeString(uint32_t engineType);
}

#endif //MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_NEOFLY_AIRCRAFT_DATA_H
