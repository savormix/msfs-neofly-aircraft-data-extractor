#include "simconnect_connection.h"

#include <Windows.h>
#include "SimConnect.h"
#include "../msfs/msfs_package_reader.h"
#include "../neofly/neofly_aircraft_data.h"

#include <cstring>
#include <iostream>

typedef void (CALLBACK *DispatchLoop)(HANDLE hSimConnect, uint32_t dwDispatchIntervalMillis);

enum MSFS_EVENT_TYPE {
    REQUEST_AIRCRAFT_FILE_ABSOLUTE_PATH
};
enum MSFS_REQUEST_TYPE {
    REQUEST_AIRCRAFT_FILE_RELATIVE_TO_PACKAGE_PATH,
    REQUEST_AIRCRAFT_DATA_FOR_NEOFLY
};
enum MSFS_MESSAGE_TYPE {
    RESPONSE_AIRCRAFT_DATA_FOR_NEOFLY
};

struct NonSimVarData {
    char szAircraftFilePath[260]{};
    FlightSimulatorFileSystem::FlightSimulatorReferenceData referenceData{};
};

struct RecvContext {
    bool bOnline{};
    bool bRecv{};
    NonSimVarData nonSimVarData{};
};

void ReportAircraftData(NeoFly::NeoFlyAircraftData *neoFlyData, FlightSimulatorFileSystem::FlightSimulatorReferenceData *pReferenceData) {
    std::cout << "Current aircraft:" << std::endl;
    std::cout << "- Name: " << neoFlyData->szTitle << std::endl;
    std::cout << "- Type: " << NeoFly::GetType(neoFlyData) << std::endl;
    std::cout << "- Engine(s): " << neoFlyData->dwEngineCount;
    std::cout << " (" << FlightSimulator::GetEngineTypeString(neoFlyData->dwEngineTypes[0]);
    for (size_t i = 1; i < neoFlyData->dwEngineCount; ++i) {
        std::cout << ", " << FlightSimulator::GetEngineTypeString(neoFlyData->dwEngineTypes[i]);
    }
    std::cout << ")" << std::endl;
    std::cout << "- Category: " << NeoFly::GetCategory(neoFlyData) << std::endl;
    std::cout << "- Cruise speed (kt): " << neoFlyData->dwSpeedVCKt << std::endl;
    std::cout << "- Range (NM): ";
    if (pReferenceData->maxRange > 0) {
        std::cout << pReferenceData->maxRange;
    } else {
        std::cout << "Unknown";
    }
    std::cout << std::endl;
    std::cout << "- Empty weight (lbs): " << neoFlyData->dEmptyWeightPounds << std::endl;
    std::cout << "- Max fuel (lbs): " << (neoFlyData->dMaxFuelGallons * neoFlyData->dGallonWeightPounds) << std::endl;
    std::cout << "- Max payload (lbs): " << (neoFlyData->dMaxTakeoffPounds - neoFlyData->dEmptyWeightPounds) << std::endl;
    std::cout << "- Max takeoff weight (lbs): " << neoFlyData->dMaxTakeoffPounds << std::endl;
    std::cout << "- Pax: " << NeoFly::InferPassengerCapacity(neoFlyData) << std::endl;
}

bool SetupConnection(HANDLE hSimConnect) {
    if (!SUCCEEDED(SimConnect_RequestSystemState(hSimConnect, REQUEST_AIRCRAFT_FILE_RELATIVE_TO_PACKAGE_PATH, "AircraftLoaded"))) {
        return false;
    }
    if (!SUCCEEDED(SimConnect_SubscribeToSystemEvent(hSimConnect, REQUEST_AIRCRAFT_FILE_ABSOLUTE_PATH, "AircraftLoaded"))) {
        return false;
    }

    char szFStr[256];
    size_t elementCount;
    if (!SUCCEEDED(SimConnect_AddToDataDefinition(hSimConnect, RESPONSE_AIRCRAFT_DATA_FOR_NEOFLY, "TITLE", nullptr, SIMCONNECT_DATATYPE_STRING256))) {
        return false;
    }
    if (!SUCCEEDED(SimConnect_AddToDataDefinition(hSimConnect, RESPONSE_AIRCRAFT_DATA_FOR_NEOFLY, "NUMBER OF ENGINES", "Number", SIMCONNECT_DATATYPE_INT32))) {
        return false;
    }
    elementCount = (sizeof(NeoFly::NeoFlyAircraftData::dwEngineTypes) >> 2);
    for (int i = 1; i <= elementCount; ++i) {
        sprintf_s(szFStr, "ENGINE TYPE:%d", i);
        if (!SUCCEEDED(SimConnect_AddToDataDefinition(hSimConnect, RESPONSE_AIRCRAFT_DATA_FOR_NEOFLY, szFStr, "Enum", SIMCONNECT_DATATYPE_INT32))) {
            return false;
        }
    }
    if (!SUCCEEDED(SimConnect_AddToDataDefinition(hSimConnect, RESPONSE_AIRCRAFT_DATA_FOR_NEOFLY, "DESIGN SPEED VC", "knots", SIMCONNECT_DATATYPE_INT32))) {
        return false;
    }
    if (!SUCCEEDED(SimConnect_AddToDataDefinition(hSimConnect, RESPONSE_AIRCRAFT_DATA_FOR_NEOFLY, "EMPTY WEIGHT", "pounds", SIMCONNECT_DATATYPE_FLOAT64))) {
        return false;
    }
    if (!SUCCEEDED(SimConnect_AddToDataDefinition(hSimConnect, RESPONSE_AIRCRAFT_DATA_FOR_NEOFLY, "FUEL TOTAL CAPACITY", "gallons", SIMCONNECT_DATATYPE_FLOAT64))) {
        return false;
    }
    if (!SUCCEEDED(SimConnect_AddToDataDefinition(hSimConnect, RESPONSE_AIRCRAFT_DATA_FOR_NEOFLY, "FUEL WEIGHT PER GALLON", "pounds", SIMCONNECT_DATATYPE_FLOAT64))) {
        return false;
    }
    if (!SUCCEEDED(SimConnect_AddToDataDefinition(hSimConnect, RESPONSE_AIRCRAFT_DATA_FOR_NEOFLY, "MAX GROSS WEIGHT", "pounds", SIMCONNECT_DATATYPE_FLOAT64))) {
        return false;
    }
    if (!SUCCEEDED(SimConnect_AddToDataDefinition(hSimConnect, RESPONSE_AIRCRAFT_DATA_FOR_NEOFLY, "PAYLOAD STATION COUNT", "Number", SIMCONNECT_DATATYPE_INT32))) {
        return false;
    }
    elementCount = (sizeof(NeoFly::NeoFlyAircraftData::szPayloadStationNames) >> 8);
    for (int i = 1; i <= elementCount; ++i) {
        sprintf_s(szFStr, "PAYLOAD STATION NAME:%d", i);
        if (!SUCCEEDED(SimConnect_AddToDataDefinition(hSimConnect, RESPONSE_AIRCRAFT_DATA_FOR_NEOFLY, szFStr, nullptr, SIMCONNECT_DATATYPE_STRING256))) {
            return false;
        }
    }
    if (!SUCCEEDED(SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_AIRCRAFT_DATA_FOR_NEOFLY, RESPONSE_AIRCRAFT_DATA_FOR_NEOFLY, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND,
                                                     SIMCONNECT_DATA_REQUEST_FLAG_CHANGED))) {
        return false;
    }
    return true;
}

#pragma ide diagnostic ignored "EndlessLoop"

void SimConnectLoop(const char *szName, uint32_t dwConnectIntervalMillis, DispatchLoop pfcnDispatchLoop, uint32_t dwDispatchIntervalMillis) {
    HANDLE hSimConnect = nullptr;
    while (true) {
        if (!SUCCEEDED(SimConnect_Open(&hSimConnect, szName, nullptr, 0, nullptr, SIMCONNECT_OPEN_CONFIGINDEX_LOCAL))) {
            Sleep(dwConnectIntervalMillis);
            continue;
        }
        std::cout << "--- Connected to MSFS" << std::endl;
        if (SetupConnection(hSimConnect)) {
            pfcnDispatchLoop(hSimConnect, dwDispatchIntervalMillis);
        }
        SimConnect_Close(hSimConnect);
        std::cout << "--- Disconnected from MSFS" << std::endl;
        Sleep(dwConnectIntervalMillis);
    }
}

void OnAircraftFilePath(const char *szPath, NonSimVarData *pNonSimVarData) {
    strcpy_s(pNonSimVarData->szAircraftFilePath, szPath);
    FlightSimulatorFileSystem::RetrieveReferenceData(szPath, &pNonSimVarData->referenceData);
}

void CALLBACK RecvHandler(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext) {
    auto *context = (RecvContext *) pContext;
    context->bRecv = true;
    switch (pData->dwID) {
        case SIMCONNECT_RECV_ID_EVENT_FILENAME: {
            auto *evtFile = (SIMCONNECT_RECV_EVENT_FILENAME *) pData;
            OnAircraftFilePath(evtFile->szFileName, &context->nonSimVarData);
            break;
        }
        case SIMCONNECT_RECV_ID_SIMOBJECT_DATA: {
            auto *data = (SIMCONNECT_RECV_SIMOBJECT_DATA *) pData;
            auto neoFlyData = (NeoFly::NeoFlyAircraftData *) &data->dwData;
            ReportAircraftData(neoFlyData, &context->nonSimVarData.referenceData);
            break;
        }
        case SIMCONNECT_RECV_ID_QUIT: {
            context->bOnline = false;
            std::cout << "*** MSFS is shutting down" << std::endl;
            return;
        }
    }
}

void CALLBACK DispatchLoop1(HANDLE hSimConnect, uint32_t dwDispatchIntervalMillis) {
    RecvContext context{};
    context.bOnline = true;
    while (context.bOnline) {
        context.bRecv = false;
        if (!SUCCEEDED(SimConnect_CallDispatch(hSimConnect, RecvHandler, &context))) {
            return; // occurs not only on MSFS CTD, but also when dispatching SIMCONNECT_RECV_ID_QUIT
        }
        if (!context.bRecv) {
            Sleep(dwDispatchIntervalMillis);
        }
    }
}

void SimConnectLoop1(const char *szName, uint32_t dwConnectIntervalMillis, uint32_t dwDispatchIntervalMillis) {
    SimConnectLoop(szName, dwConnectIntervalMillis, DispatchLoop1, dwDispatchIntervalMillis);
}

void CALLBACK DispatchLoop2(HANDLE hSimConnect, uint32_t dwDispatchIntervalMillis) {
    SIMCONNECT_RECV* pData;
    DWORD cbData;
    NonSimVarData nonSimVarData{};
    while (true) {
        if (!SUCCEEDED(SimConnect_GetNextDispatch(hSimConnect, &pData, &cbData))) {
            Sleep(dwDispatchIntervalMillis);
            continue;
        }
        switch (pData->dwID) {
            case SIMCONNECT_RECV_ID_SYSTEM_STATE: {
                auto *sysState = (SIMCONNECT_RECV_SYSTEM_STATE *) pData;
                OnAircraftFilePath(sysState->szString, &nonSimVarData);
                break;
            }
            case SIMCONNECT_RECV_ID_EVENT_FILENAME: {
                auto *evtFile = (SIMCONNECT_RECV_EVENT_FILENAME *) pData;
                OnAircraftFilePath(evtFile->szFileName, &nonSimVarData);
                break;
            }
            case SIMCONNECT_RECV_ID_SIMOBJECT_DATA: {
                auto *data = (SIMCONNECT_RECV_SIMOBJECT_DATA *) pData;
                auto neoFlyData = (NeoFly::NeoFlyAircraftData *) &data->dwData;
                ReportAircraftData(neoFlyData, &nonSimVarData.referenceData);
                break;
            }
            case SIMCONNECT_RECV_ID_QUIT: {
                std::cout << "*** MSFS is shutting down" << std::endl;
                return;
            }
        }
    }
}

void SimConnectLoop2(const char *szName, uint32_t dwConnectIntervalMillis, uint32_t dwDispatchIntervalMillis) {
    SimConnectLoop(szName, dwConnectIntervalMillis, DispatchLoop2, dwDispatchIntervalMillis);
}
