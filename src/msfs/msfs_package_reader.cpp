#include "msfs_package_reader.h"

#include "cond_comp.h"

#include <cstdio>
#include <cstring>
#ifdef MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_MSFS_PACKAGE_READER_VERBOSE
#include <iostream>
#endif
#include <Windows.h>

#ifdef MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_MSFS_PACKAGE_READER_VERBOSE
class UTF8CodePage {
public:
    UTF8CodePage() : m_old_code_page(::GetConsoleOutputCP()) {
        ::SetConsoleOutputCP(CP_UTF8);
    }
    ~UTF8CodePage() { ::SetConsoleOutputCP(m_old_code_page); }

private:
    UINT m_old_code_page;
};
#endif

namespace FlightSimulatorFileSystem {
    const wchar_t *szStorePrefixEnv = L"LOCALAPPDATA";
    const wchar_t *szNonStorePrefixEnv = L"APPDATA";
    const wchar_t *szStoreSuffix = L"\\Packages\\Microsoft.FlightSimulator_8wekyb3d8bbwe\\LocalCache\\UserCfg.opt";
    const wchar_t *szNonStoreSuffix = L"\\Microsoft Flight Simulator\\UserCfg.opt";
    const wchar_t *szPackagePathOption = L"InstalledPackagesPath";

    bool GetUserCfgPath(const wchar_t *szPrefixEnv, const wchar_t *szSuffix, wchar_t *szResult) {
        szResult[0] = L'\0';
        size_t length;
        if (_wgetenv_s(&length, szResult, 259, szPrefixEnv) != 0) {
            return false;
        }
        wcscat_s(szResult, 259, szSuffix);
        return true;
    }

    bool GetInstalledPackagesPath(wchar_t *wszResult) {
        wszResult[0] = L'\0';

        FILE *pUserCfgFile = nullptr;
        wchar_t wszPathToUserCfg[260];
        if (GetUserCfgPath(szStorePrefixEnv, szStoreSuffix, wszPathToUserCfg)) {
            _wfopen_s(&pUserCfgFile, wszPathToUserCfg, L"r, ccs=UTF-8");
            if (pUserCfgFile == nullptr) {
#ifdef MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_MSFS_PACKAGE_READER_VERBOSE
                std::wcout << L"Configuration file not found: " << wszPathToUserCfg << std::endl;
#endif
            }
        }
        if (pUserCfgFile == nullptr) {
            if (!GetUserCfgPath(szNonStorePrefixEnv, szNonStoreSuffix, wszPathToUserCfg)) {
                return false;
            }
            _wfopen_s(&pUserCfgFile, wszPathToUserCfg, L"r, ccs=UTF-8");
            if (pUserCfgFile == nullptr) {
#ifdef MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_MSFS_PACKAGE_READER_VERBOSE
                std::wcout << L"Configuration file not found: " << wszPathToUserCfg << std::endl;
#endif
                return false;
            }
        }

#ifdef MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_MSFS_PACKAGE_READER_VERBOSE
        std::wcout << L"Looking for " << szPackagePathOption << " in " << wszPathToUserCfg << std::endl;
#endif
        wchar_t wszLine[300];
        while (fgetws(wszLine, 299, pUserCfgFile) != nullptr) {
            if (wcsstr(wszLine, szPackagePathOption) != nullptr) {
                wchar_t *pStart = wcschr(wszLine, L'"') + 1;
                size_t length = wcschr(pStart, L'"') - pStart;
                pStart[length] = L'\0';
                wcscpy_s(wszResult, 259, pStart);
#ifdef MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_MSFS_PACKAGE_READER_VERBOSE
                std::wcout << L"Path to MSFS packages: " << wszResult << std::endl;
#endif
                break;
            }
        }
        fclose(pUserCfgFile);
        return wcslen(wszResult) > 0;
    }

    bool AppendOfficialPath(wchar_t *szPackagesPath) {
        size_t originalLength = wcslen(szPackagesPath);
        wcscat_s(szPackagesPath, 259, L"\\Official\\*");
        WIN32_FIND_DATAW findFileData;
        HANDLE hFindFile = FindFirstFileW(szPackagesPath, &findFileData);
        if (hFindFile == INVALID_HANDLE_VALUE) {
            szPackagesPath[originalLength] = L'\0';
            return false;
        }
        while (findFileData.cFileName[0] == L'.') {
            if (!FindNextFileW(hFindFile, &findFileData)) {
                FindClose(hFindFile);
                return false;
            }
        }
        FindClose(hFindFile);
        szPackagesPath[wcslen(szPackagesPath) - 1] = L'\0';
        wcscat_s(szPackagesPath, 259, findFileData.cFileName);
        return true;
    }

    bool FindAircraftCfgIn(const wchar_t *szAicraftCfgPath, const wchar_t* szPackageDirPath, wchar_t *absolutePathBuffer) {
#ifdef MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_MSFS_PACKAGE_READER_VERBOSE
        std::wcout << L"Search path: " << szPackageDirPath << std::endl;
#endif
        absolutePathBuffer[0] = L'\0';
        wchar_t szFindPathPackage[260];
        wcscpy_s(szFindPathPackage, 259, szPackageDirPath);
        wcscat_s(szFindPathPackage, 259, L"\\*");
        wchar_t szAircraftCfgPath[260];
        wcscpy_s(szAircraftCfgPath, 259, szPackageDirPath);
        wcscat_s(szAircraftCfgPath, 259, L"\\");
        const size_t bareAirplanePathLength = wcslen(szAircraftCfgPath);

        WIN32_FIND_DATAW findFileDataPackage;
        HANDLE hFindFilePackage = FindFirstFileW(szFindPathPackage, &findFileDataPackage);
        if (hFindFilePackage == INVALID_HANDLE_VALUE) {
            return false;
        }
        while (true) {
            if (findFileDataPackage.cFileName[0] != L'.') {
                szAircraftCfgPath[bareAirplanePathLength] = L'\0';
                wcscat_s(szAircraftCfgPath, 259, findFileDataPackage.cFileName);
                wcscat_s(szAircraftCfgPath, 259, L"\\");
                wcscat_s(szAircraftCfgPath, 259, szAicraftCfgPath);
                if (GetFileAttributesW(szAircraftCfgPath) != INVALID_FILE_ATTRIBUTES) {
                    wcscpy_s(absolutePathBuffer, 259, szAircraftCfgPath);
                    FindClose(hFindFilePackage);
#ifdef MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_MSFS_PACKAGE_READER_VERBOSE
                    std::wcout << L"Found aircraft.cfg: " << absolutePathBuffer << std::endl;
#endif
                    return true;
                }
            }
            if (!FindNextFileW(hFindFilePackage, &findFileDataPackage)) {
                FindClose(hFindFilePackage);
                return false;
            }
        }
    }

    bool RetrieveReferenceData(const char *szAircraftCfgPath, FlightSimulatorReferenceData *pReferenceData) {
#ifdef MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_MSFS_PACKAGE_READER_VERBOSE
        UTF8CodePage use_utf8;
#endif
        wchar_t wszAircraftCfgPath[260];
        size_t length;
        mbstowcs_s(&length, wszAircraftCfgPath, 259, szAircraftCfgPath, strlen(szAircraftCfgPath));
        wchar_t szPackagesPath[260];
        if (strchr(szAircraftCfgPath, ':') == nullptr) { // package-relative path was provided (GetSystemState)
#ifdef MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_MSFS_PACKAGE_READER_VERBOSE
            std::wcout << L"Received package-relative path: " << wszAircraftCfgPath << std::endl;
#endif
            GetInstalledPackagesPath(szPackagesPath);
            length = wcslen(szPackagesPath);

            wchar_t szAbsolutePath[300];
            szAbsolutePath[0] = L'\0';
            if (AppendOfficialPath(szPackagesPath)) {
                FindAircraftCfgIn(wszAircraftCfgPath, szPackagesPath, szAbsolutePath);
            }
            if (wcslen(szAbsolutePath) < 1) {
                szPackagesPath[length] = L'\0';
                wcscat_s(szPackagesPath, 259, L"\\Community");
                if (!FindAircraftCfgIn(wszAircraftCfgPath, szPackagesPath, szAbsolutePath)) {
                    return false;
                }
            }
            wcscpy_s(wszAircraftCfgPath, 259, szAbsolutePath);
        } else {
#ifdef MSFS_NEOFLY_AIRCRAFT_DATA_EXTRACTOR_MSFS_PACKAGE_READER_VERBOSE
            std::wcout << L"Received absolute path: " << wszAircraftCfgPath << std::endl;
#endif
        }

        wchar_t szValueBuffer[11];
        GetPrivateProfileStringW(L"FLTSIM.0", L"ui_max_range", L"0", szValueBuffer, 10, wszAircraftCfgPath);
        pReferenceData->maxRange = wcstol(szValueBuffer, nullptr, 10);
        return true;
    }
}