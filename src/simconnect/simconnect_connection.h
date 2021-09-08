#ifndef MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_SIMCONNECT_CONNECTION_H
#define MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_SIMCONNECT_CONNECTION_H

#include <cstdint>

void SimConnectLoop1(const char *szName, uint32_t dwConnectIntervalMillis = 5000, uint32_t dwDispatchIntervalMillis = 750);
void SimConnectLoop2(const char *szName, uint32_t dwConnectIntervalMillis = 5000, uint32_t dwDispatchIntervalMillis = 750);

#endif //MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_SIMCONNECT_CONNECTION_H
