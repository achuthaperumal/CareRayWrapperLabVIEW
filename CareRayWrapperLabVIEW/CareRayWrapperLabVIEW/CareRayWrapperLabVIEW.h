#pragma once

#include "CareRayAPIDll.h"
#include "CallbackImp.h"

#ifndef CARERAYLIBRARY_EXPORTS
#define CARERAYLIBRARY_EXPORTS
#endif // !CARERAYLIBRARY_EXPORTS

#ifdef CARERAYLIBRARY_EXPORTS
#define CARERAYLIBRARY_API __declspec(dllexport)
#else
#define CARERAYLIBRARY_API __declspec(dllimport)
#endif

extern "C" CARERAYLIBRARY_API int __stdcall Initialize(void);

extern "C" CARERAYLIBRARY_API int __stdcall Connect(int nDetrIndex);

extern "C" CARERAYLIBRARY_API int __stdcall GetSystemInformation(int nDetrIndex, CR_SystemInfo * pSystemInfo);

extern "C" CARERAYLIBRARY_API int __stdcall GetApplicationMode(int nDetrIndex, CR_ModeInfo * pModeInfo, int* pModeNum);

extern "C" CARERAYLIBRARY_API int __stdcall RegisterApplicationMode(int nDetrIndex, int nAppModeKey, int nModeID, float* pFrameRate, float* pExposureTime, int nTriggType, int nGainIndex, int nColIdx = 0);

extern "C" CARERAYLIBRARY_API int __stdcall StartAcquisition(int nDetrIndex, int nAppModeKey, void* pBuffer, int nBuffSize, int nNumFrmReqFromDetr = -1);

extern "C" CARERAYLIBRARY_API int __stdcall StopAcquisition(int nDetrIndex);

extern "C" CARERAYLIBRARY_API int __stdcall Disconnect(int nDetrIndex);

extern "C" CARERAYLIBRARY_API int __stdcall Deinitialize(void);
