#ifndef __CARERAY_API_DLL_H__
#define __CARERAY_API_DLL_H__

#include <map>
#include "ICallback.h"
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)

#ifdef _CARERAY_API_
#define CARERAY_API_PORT __declspec(dllexport)
#else
#define CARERAY_API_PORT __declspec(dllimport)
#endif

#else
#define CARERAY_API_PORT
#endif

CARERAY_API_PORT int CR_InitializeLibrary();
CARERAY_API_PORT int CR_DeinitializeLibrary();
CARERAY_API_PORT int CR_GetDetectorIndexAndIPAddress(CR_DetrIdxAndIPAddr* pDetrIdxAndIPAddr, int* pDetrNum);
CARERAY_API_PORT int CR_Connect(int nDetrIndex);
CARERAY_API_PORT int CR_Disconnect(int nDetrIndex);
CARERAY_API_PORT int CR_ResetDetector(int nDetrIndex, bool needReboot);
CARERAY_API_PORT int CR_DoSelfTest(int nDetrIndex);
CARERAY_API_PORT int CR_GetSystemInformation(int nDetrIndex, CR_SystemInfo* pSystemInformation);
CARERAY_API_PORT int CR_GetApplicationMode(int nDetrIndex, CR_ModeInfo* pModeInfo, int* pModeNum);
CARERAY_API_PORT int CR_RegisterApplicationMode(int nDetrIndex, int nAppModeKey, int nModeID, float *pFrameRate, float *pExposureTime, int nTriggType, int nGainIndex, int nRowIdx = 0);
CARERAY_API_PORT int CR_StartAcquisition(int nDetrIndex, int nAppModeKey, void* pBuffer, int nBuffSize, int nNumFrmReqFromDetr = -1); // nNumFrmReqFromDetr == -1 means that detector sends out frames continuously until StopAcquisition() is received.
CARERAY_API_PORT int CR_StartAcquisitionWithCorrOpt(int nDetrIndex, int nAppModeKey, void* pBuffer, int nBuffSize, unsigned int wCorrOpt);
CARERAY_API_PORT int CR_StartDarkAcquisition(int nDetrIndex, int nAppModeKey, int nNumFrmReqFromDetr = -1);
CARERAY_API_PORT int CR_StopAcquisition(int nDetrIndex);
CARERAY_API_PORT int CR_GetDetrStatus(int nDetrIndex, CR_DetrStatus* pDetrStatus);
CARERAY_API_PORT int CR_StartDarkCalibration(int nDetrIndex, int nAppModeKey, bool bIsTradCalib = true, bool bUpdateDefectMap = false);
CARERAY_API_PORT int CR_StartGainCalibration(int nDetrIndex, int nAppModeKey);
CARERAY_API_PORT int CR_StopCalibration(int nDetrIndex);
CARERAY_API_PORT int CR_QueryCalibrationStatus(int nDetrIndex, CR_CalibrationInfo* pCalProgInfo);
CARERAY_API_PORT int CR_QueryReferenceStatus(int nDetrIndex, int nAppModeKey, CR_RefStatusInfo* pRefStatusInfo);
CARERAY_API_PORT int CR_RegisterEventCallback(int nDetrIndex, ICallback* pCallback);
CARERAY_API_PORT int CR_LoadReference(int nDetrIndex, int nAppModeKey);
CARERAY_API_PORT int CR_UnloadReference(int nDetrIndex, int nAppModeKey);
CARERAY_API_PORT int CR_GetConfigItemValue(const char *pItemName, char *pItemValue);
CARERAY_API_PORT int CR_SetConfigItemValue(const char *pItemName, const char *pItemValue);

//radiography mode
CARERAY_API_PORT int CR_QueryAcquisitionStatus(int nDetrIdx, CR_ExpProgress* pExpProg);
CARERAY_API_PORT int CR_PermitExposure(int nDetrIdx);
CARERAY_API_PORT int CR_RequestExposure(int nDetrIdx);
CARERAY_API_PORT int CR_GetImage(int nDetrIdx, void* pImage, size_t nBuffLen);
CARERAY_API_PORT int CR_InPaintImage(int nDetrIdx, void* pImage);
CARERAY_API_PORT int CR_GetConnectionStatus(int nDetrIndex, CR_ConnectionStatus *pStatus);

// CARERAY_API_PORT int CR_ValidateAccount(const int nDetrIndex, const string strName, const string strPassword);
// CARERAY_API_PORT int CR_ChangePassword(const int nDetrIndex, const string strName, const string strOldPassword, const string strNewPassword);
// CARERAY_API_PORT int CR_DeleteProcChain(const int nDetrIndex, const int nChainID);
// CARERAY_API_PORT int CR_QueryAccounts(const int nDetrIndex, vector<string>& accountNames);
// CARERAY_API_PORT int CR_QueryConfItemInfo(const int nDetrIndex, string strModule, int nPermission, vector<CR_ConfItemInfo> &itemsVector);
// CARERAY_API_PORT int CR_ModifyConfItems(const int nDetrIndex, map<string, string>& itemsMap);
// CARERAY_API_PORT int CR_QueryProcChains(const int nDetrIndex, map<int, string>& chainMap);
// CARERAY_API_PORT int CR_QueryProcChainInfo(const int nDetrIndex, int nChainID, CR_ProcessChain& chainInfo);
// CARERAY_API_PORT int CR_CreateProcChain(const int nDetrIndex, string strChainName, CR_ProcessChain& chainInfo);
// CARERAY_API_PORT int CR_ModifyProcChain(const int nDetrIndex, int nChainID, CR_ProcessChain& chainInfo);
// CARERAY_API_PORT int CR_RestoreProcChain(const int nDetrIndex, int nChainID);
CARERAY_API_PORT void CR_GetLastIntlMsg(char* szMsgBuf, int nBufLen, int nErrorCode = -1);
#ifdef __cplusplus
}
#endif

#endif