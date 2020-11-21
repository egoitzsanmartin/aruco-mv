#include <iostream>
#include <windows.h>
#include <conio.h>

// ADS headers
#include "TcAdsDef.h"
#include "TcAdsApi.h"

using namespace std;


void main()
{
    LONG           nErr, nPort;
    AdsVersion     Version;
    AdsVersion* pVersion = &Version;
    char           pDevName[50];
    AmsAddr        Addr;
    PAmsAddr       pAddr = &Addr;

    // Open communication port on the ADS router
    nPort = AdsPortOpen();
    nErr = AdsGetLocalAddress(pAddr);
    if (nErr) cerr << "Error: AdsGetLocalAddress: " << nErr << '\n';

    // TwinCAT2 PLC1 = 801
    pAddr->port = 801;

    nErr = AdsSyncReadDeviceInfoReq(pAddr, pDevName, pVersion);
    if (nErr)
        cerr << "Error: AdsSyncReadDeviceInfoReq: " << nErr << '\n';
    else
    {
        cout << "Name: " << pDevName << '\n';
        cout << "Version: " << (int)pVersion->version << '\n';
        cout << "Revision: " << (int)pVersion->revision << '\n';
        cout << "Build: " << pVersion->build << '\n';
    }
    cout.flush();

    // Close communication port
    nErr = AdsPortClose();
    if (nErr) cerr << "Error: AdsPortClose: " << nErr << '\n';
}