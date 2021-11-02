#ifndef __CARERAY_API_DLL_530_H__
#define __CARERAY_API_DLL_530_H__

#include "ICallback.h"
#include <map>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CARERAY_LINUX_VERSION

#ifdef _CARERAY_API_
#define CARERAY_API_PORT __declspec(dllexport)
#else
#define CARERAY_API_PORT __declspec(dllimport)
#endif

#else
#define CARERAY_API_PORT
#endif

// Documented APIs (For Details Refer To "Software Interface Specification".)
CARERAY_API_PORT int CR_Connect(int nDetrIndex);
CARERAY_API_PORT int CR_Disconnect(int nDetrIndex);
CARERAY_API_PORT int CR_GetSystemInformation(int nDetrIndex, CR_SystemInfo* pSystemInformation);
CARERAY_API_PORT int CR_GetApplicationMode(int nDetrIndex, CR_ModeInfo* pModeInfo, int* pModeNum);
CARERAY_API_PORT int CR_ResetDetector(int nDetrIndex, bool needReboot);
CARERAY_API_PORT int CR_RegisterApplicationMode(int nDetrIndex, int nAppModeKey, int nModeID, float *pFrameRate, float *pExposureTime, int nTriggType, int nGainIndex, int nColIdx = 0);
CARERAY_API_PORT int CR_RegisterEventCallback(int nDetrIndex, ICallback* pCallback);
CARERAY_API_PORT int CR_LoadReference(int nDetrIndex, int nAppModeKey);
CARERAY_API_PORT int CR_UnloadReference(int nDetrIndex, int nAppModeKey);
CARERAY_API_PORT int CR_StartAcquisition(int nDetrIndex, int nAppModeKey, void* pBuffer, int nBuffSize, int nNumFrmReqFromDetr = -1); 
// nNumFrmReqFromDetr == -1 means that detector sends out frames continuously until StopAcquisition() is received.
CARERAY_API_PORT int CR_StopAcquisition(int nDetrIndex);
CARERAY_API_PORT void CR_GetLastIntlMsg(char* szMsgBuf, int nBufLen, int nErrorCode = -1);
CARERAY_API_PORT int CR_StartDarkCalibration(int nDetrIndex, int nAppModeKey, bool bIsTradCalib = true, bool bUpdateDefectMap = false);
CARERAY_API_PORT int CR_StartGainCalibration(int nDetrIndex, int nAppModeKey);
CARERAY_API_PORT int CR_QueryCalibrationStatus(int nDetrIndex, CR_CalibrationInfo* pCalProgInfo);
CARERAY_API_PORT int CR_GetDetectorIndexAndIPAddress(CR_DetrIdxAndIPAddr* pDetrIdxAndIPAddr, int* pDetrNum);
CARERAY_API_PORT int CR_QueryAcquisitionStatus(int nDetrIdx, CR_ExpProgress* pExpProg);
CARERAY_API_PORT int CR_PermitExposure(int nDetrIdx);
CARERAY_API_PORT int CR_RequestExposure(int nDetrIdx);
CARERAY_API_PORT int CR_GetImage(int nDetrIdx, void* pImage, size_t nBuffLen, int nWaitMs = 9000);
CARERAY_API_PORT int CR_QueryReferenceStatus(int nDetrIndex, int nAppModeKey, CR_RefStatusInfo* pRefStatusInfo);
CARERAY_API_PORT int CR_GetConfigItemValue(const char *pItemName, char *pItemValue);
CARERAY_API_PORT int CR_SetConfigItemValue(const char *pItemName, const char *pItemValue);
CARERAY_API_PORT int CR_GetDaecActiveAreas(int detr_idx, CR_ActiveAreaInfo *active_areas_array, int *active_areas_num, int *threshold);
CARERAY_API_PORT int CR_SetDaecActiveAreas(int detr_idx, CR_ActiveAreaInfo *active_areas_array, int active_areas_num, int threshold);

// Undocumented APIs 
CARERAY_API_PORT int CR_GetDaecH1H2(int nDetrIndex, float *pH1, float *pH2 = NULL);
CARERAY_API_PORT int CR_SetDaecH1H2(int nDetrIndex, float H1, float H2 = 0.0);
CARERAY_API_PORT int CR_GetDefectInfo(const int nDetrIndex, const int nAppModeKey, CR_DefectInfo *pDefectInfo);
CARERAY_API_PORT int CR_AddBadLine(const int nDetrIndex, const int nAppModeKey, const int nLineOffset, const bool isRow);
CARERAY_API_PORT int CR_DeleteBadLine(const int nDetrIndex, const int nAppModeKey, const int nLineOffset, const bool isRow);
CARERAY_API_PORT int CR_AddBadPixels(const int nDetrIndex, const int nAppModeKey, const int *pCoordInfo, const size_t unNumBadPixel);
CARERAY_API_PORT int CR_DeleteBadPixels(const int nDetrIndex, const int nAppModeKey, const int *pCoordInfo, const size_t unNumBadPixel);
CARERAY_API_PORT int CR_DoSelfTest(int nDetrIndex);
CARERAY_API_PORT int CR_SetPowerMode(int nDetrIndex, CR_PowerModeID nModeID);
CARERAY_API_PORT int CR_GetDetrStatus(int nDetrIndex, CR_DetrStatus* pDetrStatus);
CARERAY_API_PORT int CR_StopCalibration(int nDetrIndex);
CARERAY_API_PORT int CR_ValidateAccount(const int nDetrIndex, const string strName, const string strPassword);
CARERAY_API_PORT int CR_ChangePassword(const int nDetrIndex, const string strName, const string strOldPassword, const string strNewPassword);
CARERAY_API_PORT int CR_QueryAccounts(const int nDetrIndex, vector<string>& accountNames);
CARERAY_API_PORT int CR_QueryConfItemInfo(const int nDetrIndex, string strModule, int nPermission, vector<CR_ConfItemInfo> &itemsVector);
CARERAY_API_PORT int CR_ModifyConfItems(const int nDetrIndex, map<string, string>& itemsMap);
CARERAY_API_PORT int CR_QueryProcChainInfo(const int nDetrIndex, int nChainID, CR_ProcessChain& chainInfo);
CARERAY_API_PORT int CR_ModifyProcChain(const int nDetrIndex, int nChainID, CR_ProcessChain& chainInfo);
CARERAY_API_PORT int CR_RestoreProcChain(const int nDetrIndex, int nChainID);
CARERAY_API_PORT int CR_StartDarkAcquisition(int nDetrIndex, int nAppModeKey, int nNumFrmReqFromDetr = -1);
CARERAY_API_PORT int CR_StartDarkAcquisitionWithCorrOpt(int nDetrIndex, int nAppModeKey, unsigned int wCorrOpt);
CARERAY_API_PORT int CR_GetConnectionStatus(int nDetrIndex, CR_ConnectionStatus *pStatus);
CARERAY_API_PORT int CR_InitializeLibrary(void);
CARERAY_API_PORT int CR_DeinitializeLibrary(void);
CARERAY_API_PORT int CR_StartAcquisitionWithCorrOpt(int nDetrIndex, int nAppModeKey, void* pBuffer, int nBuffSize, unsigned int wCorrOpt);
CARERAY_API_PORT int CR_GetAcquisitionStatInfo(int nDetrIndex, struct CR_AcquisitionStatInfo* pStatInfo);
// New interface: simplify acquisition and calibration
// Get all support application modes in JSON format
CARERAY_API_PORT void CR_GetApplicationModeInJson(int detr_index, char *dest_string_buffer);
// Register custom modes from configuration file
CARERAY_API_PORT void CR_RegisterCustomModeFromConfigFile(int detr_index, char *result_string_buffer);
// Generate sample configuration file for custom modes
CARERAY_API_PORT void CR_GenerateSampleCustomModeConfigFile(int detr_index);
// Get acquisition progress in JSON format - rad mode only
CARERAY_API_PORT void CR_GetAcquisitionProgressInJson(int detr_index, char *dest_string_buffer);
// Get calibration progress in JSON format
CARERAY_API_PORT void CR_GetCalibrationProgressInJson(int detr_index, char *dest_string_buffer);

CARERAY_API_PORT int INTERNAL_RegisterApplicationMode(int nDetrIndex, int nAppModeKey, int nModeID, int nColIdx = 0);
CARERAY_API_PORT int INTERNAL_UploadFile(int nDetrIndex, string strLocalPath, string strRemotePath, string strFileName, bool isFileNewerIgnored = false);
CARERAY_API_PORT int INTERNAL_DownloadFile(int nDetrIndex, string strLocalPath, string strRemotePath, string strFileName);
CARERAY_API_PORT int INTERNAL_SwitchIntlMsgLanguage(int nLang);
CARERAY_API_PORT int INTERNAL_ForceDetrNotRepCmd(int nDetrIndex);
CARERAY_API_PORT int INTERNAL_CR_QueryCalFileInfo(const int nDetrIndex, CR_CalFileInfo* pCalFileInfo, int* pCalNum);
CARERAY_API_PORT int INTERNAL_CR_GetRegisterInfo(int nDetrIndex, int nByteShift, map<unsigned short, CR_RegAttrInfo> *mapRegInfo);
CARERAY_API_PORT int INTERNAL_CR_SetRegisterValue(int nDetrIndex, int nByteShift, bool isFloat, int nIntVal, float fFloatVal);
CARERAY_API_PORT int INTERNAL_CR_StartSimulatorAbnormalMode(int nDetrIndex, unsigned unAbnormalModeFlag);
CARERAY_API_PORT int INTERNAL_CR_StopSimulatorAbnormalMode(int nDetrIndex);
CARERAY_API_PORT int INTERNAL_CR_GetAcquisitionStatInfo(int nDetrIndex, struct CR_AcquisitionStatInfo* pStatInfo);
CARERAY_API_PORT int INTERNAL_CR_GetDefectFileInfo(int nDetrIndex, CR_ModeInfo *pModeInfo, string strSuffix, int nGainLevel, int nRoiLength, string &strDefectInfo);
CARERAY_API_PORT int INTERNAL_CR_QueryProcessChainSetting(int nDetrIdx, int nChainIdx, unsigned int* pwCorr);
CARERAY_API_PORT int INTERNAL_CR_GetModeInfoByAppModeKey(int nDetrIdx, int nAppModeKey, CR_ModeInfo *pModeInfo);
CARERAY_API_PORT int INTERNAL_CR_TransDarkFileByRegister(int nDetrIdx, char* pData, int nLen);
CARERAY_API_PORT int INTERNAL_CR_TransGainFileByRegister(int nDetrIdx, char* pData, int nLen);
CARERAY_API_PORT int INTERNAL_CR_TransDataByRegister(int nDetrIdx, unsigned long ulCmd, char* pData, int nLen);
CARERAY_API_PORT int INTERNAL_CR_TransDarkFileByMemory(int nDetrIdx, char* pData, int nLen);
CARERAY_API_PORT int INTERNAL_CR_TransGainFileByMemory(int nDetrIdx, char* pData, int nLen);
CARERAY_API_PORT int INTERNAL_CR_TransDataByMemory(int nDetrIdx, unsigned long ulCmd, char* pData, int nLen);

CARERAY_API_PORT int INTERNAL_CR_GenerateTargetBinDefect(char* pDstFullDirectory, char* pSrcFullpath, int nBinSize);

#ifdef __cplusplus
}
#endif

#endif