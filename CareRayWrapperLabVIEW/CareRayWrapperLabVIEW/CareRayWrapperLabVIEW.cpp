#include "pch.h"
#include "CareRayWrapperLabVIEW.h"


static int nRet;


int __stdcall Initialize(int nDetrIndex)
{
	nRet = CR_InitializeLibrary();
}


int __stdcall Connect(int nDetrIndex)
{
	nRet = CR_Connect(nDetrIndex);
}


int __stdcall GetSystemInfo(int nDetrIndex, CR_SystemInfo *pSystemInfo)
{
	nRet = CR_GetSystemInformation(nDetrIndex, pSystemInfo);
}


int __stdcall StartAcquisition(int nDetrIndex, int nAppModeKey, void* pBuffer, int nBuffSize, int nNumFrmReqFromDetr = -1)
{

}

int __stdcall StopAcquisition(int nDetrIndex)
{

}


int __stdcall Disconnect(int nDetrIndex)
{
	nRet = CR_Disconnect(nDetrIndex);
}

int __stdcall DeInitialize(int nDetrIndex)
{
	nRet = CR_DeinitializeLibrary();
}

