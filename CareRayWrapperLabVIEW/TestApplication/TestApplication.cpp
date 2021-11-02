// TestApplication.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include"CareRayAPIDll.h"

int main()
{
    CR_InitializeLibrary();
    const int cDETR_IDX = 1;
    CR_Connect(cDETR_IDX);
    CR_SystemInfo stSysInfo;

    CR_GetSystemInformation(cDETR_IDX, &stSysInfo);

    cout << "S/W ver: " << stSysInfo.szSoftwareVersion << endl;
    cout << "F/W ver: " << stSysInfo.szFirmwareVersion << endl;
    cout << "Machine ID: " << stSysInfo.szDetrMachineID << endl;
    cout << "Frame Header Length: " << stSysInfo.nFrmHeaderLen << endl;
    long long int nBuffSize = stSysInfo.nRawImageHeight * stSysInfo.nRawImageWidth * sizeof(unsigned short) + stSysInfo.nFrmHeaderLen;
    cout << "Allocated Memory in bytes: " << nBuffSize << endl;
    CR_Disconnect(cDETR_IDX);
    //CR_DeinitializeLibrary();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
