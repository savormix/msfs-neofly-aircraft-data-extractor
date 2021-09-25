#ifndef MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_MSFS_PACKAGE_READER_H
#define MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_MSFS_PACKAGE_READER_H

#include <cstdint>

namespace FlightSimulatorFileSystem {
    struct FlightSimulatorReferenceData {
        int32_t maxRange{0};
    };

    bool RetrieveReferenceData(const char *szAircraftCfgPath, FlightSimulatorReferenceData *pReferenceData);
}

#endif //MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_MSFS_PACKAGE_READER_H
