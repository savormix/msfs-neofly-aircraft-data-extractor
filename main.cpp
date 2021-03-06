#include "version.h"
#include "src/simconnect/simconnect_connection.h"

#include <clocale>
#include <iostream>

int main() {
    setlocale(LC_ALL, ".UTF8");

    const char *szApplicationName = MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_APPNAME;
    std::cout << szApplicationName << std::endl;
    SimConnectLoop2(szApplicationName);
    return 0;
}
