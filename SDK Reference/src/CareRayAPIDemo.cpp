// Theta5p0Client.cpp : 定义控制台应用程序的入口点。
//
#include "CareRayAPIDemo.h"
#include <string.h>

#define PANORAMA_HEIGHT	64

#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)
#else
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

static struct termios oldt, newt;
int _kbhit()
{
    termios term;
    tcgetattr(0, &term);

    termios term2 = term;
    term2.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &term2);

    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);

    tcsetattr(0, TCSANOW, &term);

    return byteswaiting;
}

//static struct termios oldt, newt;
/* Initialize new terminal i/o settings */
void initTermios(int echo)
{
  tcgetattr(0, &oldt); /* grab old terminal i/o settings */
  newt = oldt; /* make new settings same as old settings */
  newt.c_lflag &= ~ICANON; /* disable buffered i/o */
  if (echo) {
	  newt.c_lflag |= ECHO; /* set echo mode */
  } else {
	  newt.c_lflag &= ~ECHO; /* set no echo mode */
  }
  tcsetattr(0, TCSANOW, &newt); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void)
{
  tcsetattr(0, TCSANOW, &oldt);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo)
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char _getch()
{
  return getch_(0);
}

/* Read 1 character with echo */
char getche(void)
{
  return getch_(1);
}

#endif


CCareRayAPIDemo::CCareRayAPIDemo()
{
	m_nCurrentDetrIndex = 1;//1 is the default index
	for (auto it : m_mapModeInfos)
	{
		it.second = NULL;
	}

	for (auto it : m_mapCallbackPtrs)
	{
		it.second = NULL;
	}

	for (auto it : m_mapFrmBuf)
	{
		it.second = NULL;
	}
}

CCareRayAPIDemo::~CCareRayAPIDemo()
{
	FreeVariants();
}

void CCareRayAPIDemo::PrintModeList(int nDetrIdx)
{
	cout << endl << endl;	
	cout << "----------------- List of Supported Application Modes ----------------" << endl;	                                                               
	for(int i = 0; i < m_mapModeNum[nDetrIdx]; i++)
	{
		if (!m_mapModeInfos.empty())
		{
			cout << " Mode "<< i+1<< ": ";		
			cout <<endl<< "\tMode ID = " << m_mapModeInfos[nDetrIdx][i].nModeID << ";" << endl;
			cout << "\tImage dimension = " << m_mapModeInfos[nDetrIdx][i].nImageWidth <<" x "<<m_mapModeInfos[nDetrIdx][i].nImageHeight<< ";" << endl;
			cout << "\tBinning scheme = " << m_mapModeInfos[nDetrIdx][i].nBinX <<" x "<<m_mapModeInfos[nDetrIdx][i].nBinY<< ";" << endl;
			cout << "\tPixel depth = " << m_mapModeInfos[nDetrIdx][i].nPixelDepth << endl;
			cout << "\tMax frame rate = " << m_mapModeInfos[nDetrIdx][i].fMaxFrmRate <<" fps"<< endl;
			cout << "\tDescription = " << m_mapModeInfos[nDetrIdx][i].szDesc << endl;
		}
	}
	cout << "---------------------------- End of List -----------------------------" << endl;
}

void CCareRayAPIDemo::PrintDetrInfo(int nDetrIdx)
{
	CR_DetrStatus detrStatus;

	int nRet = CR_GetSystemInformation(nDetrIdx, &m_mapSystemInfos[nDetrIdx]);
	if (CR_OK == nRet)
	{
		cout << "System information:"<< endl;
		cout << "\tRaw image width (unit: pixels) = " << m_mapSystemInfos[nDetrIdx].nRawImageWidth << ";" << endl;
		cout << "\tRaw image height (unit: pixels) = " << m_mapSystemInfos[nDetrIdx].nRawImageHeight << ";" << endl;
		cout << "\tHardware version = " << string(m_mapSystemInfos[nDetrIdx].szHardwareVersion) << ";" << endl;
		cout << "\tSerial number = " << string(m_mapSystemInfos[nDetrIdx].szSerialNumber) << ";" << endl;
		cout << "\tSoftware version = " << string(m_mapSystemInfos[nDetrIdx].szSoftwareVersion) << ";" << endl;
		cout << "\tFirmware version = " << string(m_mapSystemInfos[nDetrIdx].szFirmwareVersion) << ";" << endl;
		cout << "\tMachine ID = " << string(m_mapSystemInfos[nDetrIdx].szDetrMachineID) << ";" << endl;
		cout << "\tDetector description  = " << string(m_mapSystemInfos[nDetrIdx].szDetrDesc) << ";" << endl;
		cout << endl;

		nRet = CR_GetDetrStatus(nDetrIdx, &detrStatus);
		if (CR_OK == nRet)
		{
			cout << "Readings from temperature sensors inside the detector:"<< endl;			
			cout << "\tCurrent operating temperature = " << detrStatus.oDetrTemperature.currentTemperature << " degrees Celsius" << endl
				<< "\t                              = " << 32 + 9 * detrStatus.oDetrTemperature.currentTemperature / 5 << " degrees Fahrenheit" << endl;
			cout << "\tOverheat = " << ((1 == detrStatus.oDetrTemperature.overhot_flag)? "true" : "false") << endl;			
		}
		else
		{
			char info[256];
			CR_GetLastIntlMsg(info, 256);
			cout<<"CR_GetDetrStatus failed, "<<string(info)<<endl;
		}
	}
	else
	{
		char info[256];
		CR_GetLastIntlMsg(info, 256);
		cout<<"CR_GetSystemInformation failed, "<<string(info)<<endl;
		cout<<"API will not receive and save images because of failure of CR_GetSystemInformation"<<endl;
	}
	cout << "**********************************************************************" << endl;
}

int CCareRayAPIDemo::RegisterAppModes()
{
	int nRet = 300000;
	map<int, TUPLEOFAPPMODEKEY>* mapRegisteredAppModes = &m_mapRegisteredAppModes[m_nCurrentDetrIndex];
	
	//register new mode
	char chRegisterNewMode = 'y';			
	mapRegisteredAppModes = &m_mapRegisteredAppModes[m_nCurrentDetrIndex];
	if (mapRegisteredAppModes->empty())
	{
		//add a new mode
		char chAddNewMode = 'y';
		cout<<"Add a new mode ? ( y / n )"<<endl;
		cout<<"Your Input >> ";
		cin>>chAddNewMode;
		checkInput(chAddNewMode);
		if ('y' == tolower(chAddNewMode))
		{
			nRet = ModifyAppMode(true);
		}
	}else
	{
		//display registered application modes and the status of the associated calibration files
		mapRegisteredAppModes = &m_mapRegisteredAppModes[m_nCurrentDetrIndex];
		DisplayAppModesInfo(mapRegisteredAppModes, true, true);
		if (m_mapNeedToCalibrateAppModes[m_nCurrentDetrIndex].size() > 0)
		{
			//proceed to calibrate detector
			nRet = Calibration(m_mapNeedToCalibrateAppModes[m_nCurrentDetrIndex]);
			DisplayAppModesInfo(mapRegisteredAppModes, true, true);
		}
	}

	do 
	{
		cout<<"Register a new mode ? ( y / n )"<<endl;
		cout<<"Your Input >> ";
		cin>>chRegisterNewMode;
		checkInput(chRegisterNewMode);
		if ('y' == tolower(chRegisterNewMode))
		{
			//add a new mode
			char chAddNewMode = 'y';
			cout<<"Add a new mode ? ( y / n )"<<endl;
			cout<<"Your Input >> ";
			cin>>chAddNewMode;
			checkInput(chAddNewMode);
			if ('y' == tolower(chAddNewMode))
			{
				nRet = ModifyAppMode(true);
			}else if('n' == tolower(chAddNewMode))
			{
				if (!m_mapRegisteredAppModes[m_nCurrentDetrIndex].empty())
				{
					//replace an existing mode
					char chReplaceMode = 'y';
					cout<<"Replace an existing mode ? ( y / n )"<<endl;
					cout<<"Your Input >> ";
					cin>>chReplaceMode;
					checkInput(chReplaceMode);
					if ('y' == tolower(chReplaceMode))
					{
						nRet = ModifyAppMode(false);
					}else
					{
						break;
					}
				}
			}
		}else
		{
			break;
		}
	} while (GetReturnCode(nRet) <= CR_WARN);

	return nRet;
}

void CCareRayAPIDemo::DisplayAppModesInfo(map<int, TUPLEOFAPPMODEKEY>* mapAppModes, bool bDisplayFluoro, bool bDisplayRad)
{
	int nCellWidth = 10;
	string strSepara = "|", strLine = "-", strCross = "+";
	string strFields[] = {"AppKey", "ModeID", "FrmRate", "ExpTime", "TrigType", "GainLvl", "IsFluoro", "Ref"};

	cout << "Registered application modes: "<<endl;
	for (int nIdx = 0; nIdx < nCellWidth * (strFields->length() + 1) + 3; nIdx++)
	{
		cout << std::left << strLine;
		if (6 == nIdx || 56 == nIdx)
		{
			cout << strCross;
		}
	}
	cout << endl;
	cout << std::left << setw(7) << strFields[0];
	cout << std::left << setw(nCellWidth - 7) << strSepara;
	cout << std::left << setw(nCellWidth) << strFields[1];
	cout << std::left << setw(nCellWidth) << strFields[2];
	cout << std::left << setw(nCellWidth) << strFields[3];
	cout << std::left << setw(nCellWidth) << strFields[4];
	cout << std::left << setw(8) << strFields[5];
	cout << std::left << setw(nCellWidth - 8) << strSepara;
	cout << std::left << setw(nCellWidth) << strFields[6];
	cout << std::left << setw(nCellWidth) << strFields[7];
	for (int nIdx = 0; nIdx < nCellWidth * (strFields->length() + 1) + 3; nIdx++)
	{
		cout << std::left << strLine;
		if (6 == nIdx || 56 == nIdx)
		{
			cout << strCross;
		}
	}
	cout << endl;

/*	m_mapNeedToCalibrateAppModes[m_nCurrentDetrIndex].clear();*/
	for (int nLoopCnt = 0; nLoopCnt < 2; nLoopCnt++)
	{
		for (auto it : *mapAppModes)
		{
			//update the reference status
			string strRefStatus = "CR_OK";
			//int nRet = CR_LoadReference(m_nCurrentDetrIndex, it.first);
			CR_RefStatusInfo oRefStatusInfo;
			memset(&oRefStatusInfo, 0, sizeof(CR_RefStatusInfo));
			int nRet = CR_QueryReferenceStatus(m_nCurrentDetrIndex, it.first, &oRefStatusInfo);
			if (CR_OK == nRet)
			{
				strRefStatus = "Offset: " +GetReferenceFilesStatus(oRefStatusInfo.nDarkStatus) + ", Gain: " + GetReferenceFilesStatus(oRefStatusInfo.nGainStatus) + ", Defect: " + GetReferenceFilesStatus(oRefStatusInfo.nDefectStatus);
			}
			else
			{
				char info[256];
				CR_GetLastIntlMsg(info, 256);
				cout<<"CR_QueryReferenceStatus failed, "<<string(info)<<endl;
			}

			std::get<2>(it.second) = strRefStatus;
			if ((CR_REF_VALID == oRefStatusInfo.nDarkStatus || CR_REF_ABOUT_TO_EXPIRE == oRefStatusInfo.nDarkStatus)
				&& (CR_REF_VALID == oRefStatusInfo.nGainStatus || CR_REF_ABOUT_TO_EXPIRE == oRefStatusInfo.nGainStatus)
				&& (CR_REF_VALID == oRefStatusInfo.nDefectStatus || CR_REF_ABOUT_TO_EXPIRE == oRefStatusInfo.nDefectStatus))
			{
				vector<pair<int, bool>>* vecTmp = &m_mapNeedToCalibrateAppModes[m_nCurrentDetrIndex];
				for (vector<pair<int, bool>>::iterator it2 = vecTmp->begin(); it2 != vecTmp->end(); it2++)
				{
					if (it2->first == it.first)
					{
						vecTmp->erase(it2);
					}
					break;
				}
			}

			TUPLEOFAPPMODEKEY tupleModeParams = it.second;			
			//classify the fluoro and rad modes according to 'IsFluoro'
			if ((bool)std::get<1>(tupleModeParams) && 0 == nLoopCnt && bDisplayFluoro
				|| !(bool)std::get<1>(tupleModeParams) && 1 == nLoopCnt && bDisplayRad)
			{
				cout << std::left << setw(7) << it.first;
				cout << std::left << setw(nCellWidth - 7) << strSepara;
				cout << std::left << setw(nCellWidth) << (int)std::get<0>(std::get<0>(tupleModeParams));
				cout << std::left << setw(nCellWidth) << (float)std::get<1>(std::get<0>(tupleModeParams));
				cout << std::left << setw(nCellWidth) << (int)std::get<2>(std::get<0>(tupleModeParams));
				switch ((int)std::get<3>(std::get<0>(tupleModeParams)))
				{
				case CR_FluReserved: cout << std::left << setw(nCellWidth) << "FluoFree"; break;
				case CR_FluExtSync: cout << std::left << setw(nCellWidth) << "FluoExt"; break;
				case CR_FluIntSync: cout << std::left << setw(nCellWidth) << "FluoInt"; break;
				case CR_RadExtSync: cout << std::left << setw(nCellWidth) << "RadExt"; break;
				case CR_RadSoftSync: cout << std::left << setw(nCellWidth) << "RadSoft"; break;
				case CR_RadManualSync: cout << std::left << setw(nCellWidth) << "RadMan"; break;
				default: cout << std::left << setw(nCellWidth) << (int)std::get<3>(std::get<0>(tupleModeParams)); break;
				}
				cout << std::left << setw(8) << (int)std::get<4>(std::get<0>(tupleModeParams));
				cout << std::left << setw(nCellWidth - 8) << strSepara;

				cout << std::left << setw(10) << (bool)std::get<1>(tupleModeParams);
				cout << std::left << setw(8) << (string)std::get<2>(tupleModeParams);
				cout << endl;
				for (int nIdx = 0; nIdx < nCellWidth * (strFields->length() + 1) + 3; nIdx++)
				{
					cout << std::left << strLine;
					if (6 == nIdx || 56 == nIdx)
					{
						cout << strCross;
					}
				}
				cout << endl;
			}
		}
	}
}

int CCareRayAPIDemo::ModifyAppMode(bool bAddNewMode)
{
	set<int> setCandidates;
	int nRet = CR_OK, nIdx = 0;
	int nNewAppModeKey, nReplacedAppModeKey = 0;
	int nModeID = 0, nTrigType = 0, nGainId = 0;
	float fFrameRate = 0.0f, fExpTime = 0.0f;
	bool bIsFluoroMode = true;
	map<int, TUPLEOFAPPMODEKEY> mapRegisteredAppModes = m_mapRegisteredAppModes[m_nCurrentDetrIndex];
	
	if (!bAddNewMode)
	{
		cout <<"Specify the candidate app mode key values as one of [ ";
		for (map<int, TUPLEOFAPPMODEKEY>::iterator it = mapRegisteredAppModes.begin(); it != mapRegisteredAppModes.end(); it++)
		{
			if (it != mapRegisteredAppModes.begin())
			{
				cout << " | ";
			}
			cout <<it->first;
		}
		cout <<  " ]"<< endl;

		cout << "Input the application mode key which needs to be replaced: >> ";				
		while (1)
		{
			cin >> nReplacedAppModeKey;
			checkInput(nReplacedAppModeKey);
			break;
		}
		cout << "Modify the mode params: " <<endl;
	}else
	{
		if (mapRegisteredAppModes.size() > 0)
		{
			cout << "Enter an integer as a new application mode key, other than [ ";
			for (map<int, TUPLEOFAPPMODEKEY>::iterator it = mapRegisteredAppModes.begin(); it != mapRegisteredAppModes.end(); it++)
			{
				if (it != mapRegisteredAppModes.begin())
				{
					cout << " | ";
				}
				cout <<it->first;
			}
			cout <<  " ] >> ";
		}else
		{
			cout << "Enter an integer as a new application mode key >> ";
		}

		while (1)
		{
			cin >> nNewAppModeKey;
			checkInput(nNewAppModeKey);
			break;
		}
	}

	cout <<endl<< "(1/4) Specify the mode ID as one of [ ";
	for(nIdx = 0; nIdx < m_mapModeNum[m_nCurrentDetrIndex]; nIdx++)
	{
		if (nIdx != 0)
		{
			cout << " | ";
		}
		setCandidates.insert(m_mapModeInfos[m_nCurrentDetrIndex][nIdx].nModeID);
		cout<<m_mapModeInfos[m_nCurrentDetrIndex][nIdx].nModeID;
	}
	cout <<  " ]" << endl;
	cout << "Mode ID >> ";
	while (1)
	{
		cin >> nModeID;
		checkInput(nModeID);
		if (setCandidates.end() == setCandidates.find(nModeID))
		{
			cout << "Invalid input, enter again" << endl << "Your input >> ";
			continue;
		}
		break;
	}

	for (nIdx = 0; nIdx < m_mapModeNum[m_nCurrentDetrIndex]; nIdx++)
	{
		if (nModeID == m_mapModeInfos[m_nCurrentDetrIndex][nIdx].nModeID)
		{
			break;
		}
	}

	CR_ModeInfo oCurrentModeInfo;
	m_mapActiveFluoroModes[m_nCurrentDetrIndex] = m_mapModeInfos[m_nCurrentDetrIndex][nIdx];
	oCurrentModeInfo = m_mapModeInfos[m_nCurrentDetrIndex][nIdx];
	string strModeDesc = oCurrentModeInfo.szDesc;	
	bIsFluoroMode = ((strModeDesc.find("fluoroscopic") != std::string::npos || strModeDesc.find("panoramic") != std::string::npos) ? true : false);

	setCandidates.clear();
	cout <<endl<< "(2/4) Specify the trigger type as one of [ ";
	for (size_t rIdx2 = 0; rIdx2 < m_mapModeInfos[m_nCurrentDetrIndex][nIdx].nTrigTypeNum; rIdx2++)
	{
		if (rIdx2 != 0)
		{
			cout << " | ";
		}
		setCandidates.insert(m_mapModeInfos[m_nCurrentDetrIndex][nIdx].nTrigTypes[rIdx2]);
		switch (m_mapModeInfos[m_nCurrentDetrIndex][nIdx].nTrigTypes[rIdx2])
		{
		case CR_RadExtSync:
			cout << "1: rad external trigger"; 
			break;
		case CR_RadSoftSync:
			cout << "2: rad soft trigger"; 
			break;
		case CR_RadAutoSync:
			cout << "3: rad auto trigger"; 
			break;
		case CR_RadManualSync:
			cout << "4: rad manual trigger"; 
			break;
		case CR_RadScanSync:
			cout << "5: rad scan trigger"; 
			break;
		case CR_RadDaecSync:
			cout << "6: rad DAEC trigger"; 
			break;
		case CR_FluReserved: 
			cout << "7: fluo free trigger"; 
			break;
		case CR_FluExtSync: 
			cout << "8: fluo external trigger";	
			break;
		case CR_FluIntSync: 
			cout << "9: fluo internal trigger"; 
			break;
		default: 
			break;
		}
	}
	cout <<  " ]" << endl;
	cout << "Trigger type >> ";
	while (1)
	{
		cin >> nTrigType;
		checkInput(nTrigType);
		if (setCandidates.end() == setCandidates.find(nTrigType))
		{
			cout << "Invalid input, enter again" << endl << "Your input >> ";
			continue;
		}
		break;
	}

	//Set frame or exposure time, set exposure time as default, and the other one as 0
	if (bIsFluoroMode)
	{
		float fMaxFrameRate = oCurrentModeInfo.fMaxFrmRate;
		cout <<endl<< "(3/4) Set frame rate (unit: fps, max frame rate: "<<fMaxFrameRate<< ") >> ";
		while (1)
		{
			cin >> fFrameRate;
			checkInput(fFrameRate);

			if (fFrameRate < 0.0f || fFrameRate > oCurrentModeInfo.fMaxFrmRate)
			{
				cout << "Invalid input, enter again" << endl << "Your input >> ";
				continue;
			}
			break;
		}
	}else
	{
		cout <<endl<< "(3/4) Set exposure time (unit: ms) >> ";				
		cin >> fExpTime;
		checkInput(fExpTime);
	}

	setCandidates.clear();
	cout <<endl<< "(4/4) Specify the gain value as one of [ ";
	for (size_t rIdx2 = 0; rIdx2 < size_t(m_mapModeInfos[m_nCurrentDetrIndex][nIdx].nGainLevelNum); rIdx2++)
	{
		if (rIdx2 != 0)
		{
			cout << " | ";
		}
		setCandidates.insert(m_mapModeInfos[m_nCurrentDetrIndex][nIdx].nGainLevels[rIdx2]);
		cout<<m_mapModeInfos[m_nCurrentDetrIndex][nIdx].nGainLevels[rIdx2];
	}
	setCandidates.insert(0);
	cout <<  " ], or press 0 to use the default gain value of " << m_mapModeInfos[m_nCurrentDetrIndex][nIdx].nDefaultGainLevel << endl;
	cout << "Gain value >> ";
	while (1)
	{
		cin >> nGainId;
		checkInput(nGainId);
		if (setCandidates.end() == setCandidates.find(nGainId))
		{
			cout << "Invalid input, enter again" << endl << "Your input >> ";
			continue;
		}
		break;
	}
	if (nGainId == 0)
	{
		nGainId = m_mapModeInfos[m_nCurrentDetrIndex][nIdx].nDefaultGainLevel;
	}
	int nParonamaRowIdx = 0;
	if (strModeDesc.find("panoramic") != string::npos)
	{
		int nHeightHalfSize = m_mapSystemInfos[m_nCurrentDetrIndex].nRawImageHeight / 2;
		cout << "set the starting row index of panoramic ROI" << endl;
		while (true)
		{
			cout << "valid rage is 1 - " << to_string(nHeightHalfSize - PANORAMA_HEIGHT + 1) << " and " << to_string(nHeightHalfSize + 1) +  " - " + to_string(m_mapSystemInfos[m_nCurrentDetrIndex].nRawImageHeight - PANORAMA_HEIGHT + 1) + ": ";
			cin >> nParonamaRowIdx;
			if (nParonamaRowIdx > 0 && nParonamaRowIdx <= nHeightHalfSize - PANORAMA_HEIGHT + 1 || nParonamaRowIdx > nHeightHalfSize && nParonamaRowIdx <= m_mapSystemInfos[m_nCurrentDetrIndex].nRawImageHeight - PANORAMA_HEIGHT + 1)
			{
				break;
			}
		}
	}
	
	int nCurrentAppModeKey = (bAddNewMode == true ? nNewAppModeKey : nReplacedAppModeKey);
	nRet = CR_RegisterApplicationMode(m_nCurrentDetrIndex, nCurrentAppModeKey, nModeID, &fFrameRate, &fExpTime, nTrigType, nGainId, nParonamaRowIdx);
	
	if (GetReturnCode(nRet) > CR_WARN)
	{
		char info[256];
		CR_GetLastIntlMsg(info, 256);
		cout<<"RegisterApplicationMode failed, "<<string(info)<<endl;
	}
	else if (CR_WARN == GetReturnCode(nRet))
	{
		char info[256];
		CR_GetLastIntlMsg(info, 256);
		cout<<"RegisterApplicationMode Warning, "<<string(info)<<endl;
	}

	if (GetReturnCode(nRet) <= CR_WARN)
	{
		string strRefStatus = "CR_OK";
		//nRet = CR_LoadReference(m_nCurrentDetrIndex, nCurrentAppModeKey);
		m_mapRegisteredAppModes[m_nCurrentDetrIndex][nCurrentAppModeKey] = 
			make_tuple(make_tuple(nModeID, fFrameRate, fExpTime, nTrigType, nGainId), bIsFluoroMode, strRefStatus);

		CR_RefStatusInfo oRefStatusInfo;
		memset(&oRefStatusInfo, 0, sizeof(CR_RefStatusInfo));
		nRet = CR_QueryReferenceStatus(m_nCurrentDetrIndex, nCurrentAppModeKey, &oRefStatusInfo);
		if (CR_OK == nRet)
		{
			if (CR_REF_NOT_FOUND == oRefStatusInfo.nDarkStatus || CR_REF_INVALID == oRefStatusInfo.nDarkStatus
				|| CR_REF_NOT_FOUND == oRefStatusInfo.nGainStatus || CR_REF_INVALID == oRefStatusInfo.nGainStatus
				|| CR_REF_NOT_FOUND == oRefStatusInfo.nDefectStatus || CR_REF_INVALID == oRefStatusInfo.nDefectStatus)
			{
				m_mapNeedToCalibrateAppModes[m_nCurrentDetrIndex].push_back(make_pair(nCurrentAppModeKey, bIsFluoroMode));
				vector<pair<int, bool>> vecTmp;
				vecTmp.push_back(make_pair(nCurrentAppModeKey, bIsFluoroMode));
				Calibration(vecTmp);
			}
		}
		else
		{
			char info[256];
			CR_GetLastIntlMsg(info, 256);
			cout<<"CR_QueryReferenceStatus failed, "<<string(info)<<endl;
		}

		cout<<"Modify application mode successfully"<<endl<<endl;
		DisplayAppModesInfo(&m_mapRegisteredAppModes[m_nCurrentDetrIndex], true, true);
	}
	cout<<endl;

	int nSaveFlag = 0;
	cout<<"Press 1 if you want to save images. Otherwise, press 0" << endl << "Your input >> ";
	cin>>nSaveFlag;
	checkInput(nSaveFlag);
	m_mapCallbackPtrs[m_nCurrentDetrIndex]->SetImageSaveFlag(0 != nSaveFlag ? true : false);

	return nRet;
}

int CCareRayAPIDemo::Initialize()
{
	cout << "Initializing ..." << endl << endl;
	int nRet = CR_OK;
	int nDetrNum = 0;
	CR_DetrIdxAndIPAddr oDetrIdxAndIPAddr[16];
	nRet = CR_GetDetectorIndexAndIPAddress(oDetrIdxAndIPAddr, &nDetrNum);
	if (GetReturnCode(nRet) > CR_WARN)
	{
		char info[256];
		CR_GetLastIntlMsg(info, 256);
		cout<<"GetDetectorIndexAndIPAddress failed, "<<string(info)<<endl;
		return nRet;
	}

	for (int i = 0; i < nDetrNum; i++)
	{
		m_mapDetrIdxnIPAddr.insert(make_pair(oDetrIdxAndIPAddr[i].nIdx, string(oDetrIdxAndIPAddr[i].szIPAddr)));
		m_mapModeInfos[oDetrIdxAndIPAddr[i].nIdx] = new CR_ModeInfo[16];
	}

	cout << "Establishing a system with " << m_mapDetrIdxnIPAddr.size() << ((m_mapDetrIdxnIPAddr.size() > 1)? " detectors ..." : " detector ...") << endl;
	for(auto it : m_mapDetrIdxnIPAddr)
	{
		cout << "**************************** Detector "<< it.first << " ******************************" << endl;
		CCallbackImp *pCallBack = new CCallbackImp();
		cout << "IP: " << it.second << endl;

		m_mapCallbackPtrs[it.first] = pCallBack;
		CR_RegisterEventCallback(it.first, pCallBack);

		//m_pCallback = pCallBack;

		nRet = CR_Connect(it.first);	
		if (GetReturnCode(nRet) > CR_WARN)
		{
			char info[256];
			CR_GetLastIntlMsg(info, 256);
			cout<<"Open failed, "<<string(info)<<endl;
		}
		else
		{
			nRet = CR_ResetDetector(it.first, false);
			if (GetReturnCode(nRet) > CR_WARN)
			{
				char info[256];
				CR_GetLastIntlMsg(info, 256);
				cout<<"Reset failed, "<<string(info)<<endl;
			}
			else
			{
				nRet = CR_GetApplicationMode(it.first, m_mapModeInfos[it.first], &m_mapModeNum[it.first]);
				if (GetReturnCode(nRet) > CR_WARN)
				{
					char info[256];
					CR_GetLastIntlMsg(info, 256);
					cout<<"GetApplicationMode failed, "<<string(info)<<endl;
				}
			}
		}

		if (CR_OK == nRet)
		{
			PrintDetrInfo(it.first);
			m_mapCallbackPtrs[it.first]->SetFrmHeaderLen(m_mapSystemInfos[it.first].nFrmHeaderLen);
		}
	}

	cout << "**********************************************************************" << endl;
	return nRet;
}

void CCareRayAPIDemo::FreeVariants()
{
	m_mapDetrIdxnIPAddr.erase(m_mapDetrIdxnIPAddr.begin(), m_mapDetrIdxnIPAddr.end());
	m_mapActiveFluoroModes.erase(m_mapActiveFluoroModes.begin(), m_mapActiveFluoroModes.end());
	m_mapSystemInfos.erase(m_mapSystemInfos.begin(), m_mapSystemInfos.end());
	m_mapRegisteredAppModes.erase(m_mapRegisteredAppModes.begin(), m_mapRegisteredAppModes.end());
	m_mapNeedToCalibrateAppModes.erase(m_mapNeedToCalibrateAppModes.begin(), m_mapNeedToCalibrateAppModes.end());
	m_mapModeNum.erase(m_mapModeNum.begin(), m_mapModeNum.end());
	
	for (auto it : m_mapModeInfos)
	{
		if (NULL != it.second)
		{
			delete[] it.second;
			it.second = NULL;
		}
	}
	m_mapModeInfos.erase(m_mapModeInfos.begin(), m_mapModeInfos.end());

	for (auto it : m_mapCallbackPtrs)
	{
		if (NULL != it.second)
		{
			delete it.second;
			it.second = NULL;
		}
	}
	m_mapCallbackPtrs.erase(m_mapCallbackPtrs.begin(), m_mapCallbackPtrs.end());

	for (auto it : m_mapFrmBuf)
	{
		if (NULL != it.second)
		{
			delete[] it.second;
			it.second = NULL;
		}
	}
	m_mapFrmBuf.erase(m_mapFrmBuf.begin(), m_mapFrmBuf.end());
}

int CCareRayAPIDemo::InitializeLibrary()
{
	return CR_InitializeLibrary();
}

int CCareRayAPIDemo::DeInitializeLibrary()
{
	FreeVariants();
	int nRet = CR_DeinitializeLibrary();
	cout<<"To keep working, \'I\' menu must be selected"<<endl;
	return nRet;
}

int CCareRayAPIDemo::DeInitialize()
{
	int nRet = CR_OK;
	nRet = CR_Disconnect(m_nCurrentDetrIndex);
	if (GetReturnCode(nRet) > CR_WARN)
	{
		char info[256];
		CR_GetLastIntlMsg(info, 256);
		cout<<"Close failed, "<<string(info)<<endl;
	}
	return nRet;
}

int CCareRayAPIDemo::GetConnectionStatus()
{
	CR_ConnectionStatus oStatus;
	CR_GetConnectionStatus(m_nCurrentDetrIndex, &oStatus);

	if (oStatus.isServerConnected)
	{
		cout << "\tServer connected" << endl;
	}
	else
	{
		cout << "\tServer not connected" << endl;
	}

	if (oStatus.isDetrConnected)
	{
		cout << "\tDetector connected" << endl;
	}
	else
	{
		cout << "\tDetector not connected" << endl;
	}

	return CR_OK;
}

int CCareRayAPIDemo::DoOffsetCalibration(int nAppModeKey)
{
	int nRet = CR_OK, nCurrFrmNum = 0, nPrevFrmNum = nCurrFrmNum - 1, nKeyToContinue = 0;
	CR_CalibrationInfo calProgInfo;
	m_mapCallbackPtrs[m_nCurrentDetrIndex]->SetPrintFlag(false);

	//////////////////////////////////////////////////////////////////////////
	//1. Dark Calibration, the X-ray shouldn't expose the detector
	//////////////////////////////////////////////////////////////////////////
	int nDefectOption = 0;
	cout << "Whether update defect file (0: false, 1 true)" << endl;
	cin >> nDefectOption;
	
	cout<<"Dark calibration starts"<<endl;
	if (0 != nDefectOption)
	{
		nRet = CR_StartDarkCalibration(m_nCurrentDetrIndex, nAppModeKey, true, true);
	}
	else
	{
		nRet = CR_StartDarkCalibration(m_nCurrentDetrIndex, nAppModeKey);
	}
		
	if (GetReturnCode(nRet) > CR_WARN)
	{
		/*string strIntlMsg = GetLastIntlMsg();
		cout<<"StartDarkCalibration failed, "<<strIntlMsg.c_str()<<endl;*/
		char info[256];
		CR_GetLastIntlMsg(info, 256);
		cout<<"StartDarkCalibration failed, "<<string(info)<<endl;
		return nRet;
	}

	nCurrFrmNum = 0;
	nPrevFrmNum = nCurrFrmNum;
	do
	{
		if (_kbhit())
		{
			nRet = CR_StopCalibration(m_nCurrentDetrIndex);
			return nRet;
		}
		
		CR_QueryCalibrationStatus(m_nCurrentDetrIndex, &calProgInfo);
		nCurrFrmNum = calProgInfo.nCurrentFrameNum;
		
		if(nCurrFrmNum > nPrevFrmNum)
		{
			nPrevFrmNum = nCurrFrmNum;
			cout<< "Dark calibration progress: " <<calProgInfo.nCurrentFrameNum<<" / "<< calProgInfo.nTotalFrameNum<< '\r';
			cout.flush();
		}
#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)
		Sleep(20);
#else
		sleep(20);
#endif
	}while(-1 == calProgInfo.nResult);
	cout << endl;

	nRet = calProgInfo.nResult;
	if (CR_OK == nRet)
	{
		cout<<"Offset calibration completes successfully."<<endl;
	}else if (CR_WARN == GetReturnCode(nRet))
	{
		cout<<"StartDarkCalibration warns, "<<string(calProgInfo.szErrorMsg)<<endl;
		return nRet;
	}else if (GetReturnCode(nRet) > CR_WARN)
	{
		cout<<"StartDarkCalibration failed, "<<string(calProgInfo.szErrorMsg)<<endl;
		return nRet;
	}

	return nRet;
}

int CCareRayAPIDemo::DoGainCalibration(pair<int, bool> pairCalType)
{
	int nRet = CR_OK, nCurrFrmNum = 0, nPrevFrmNum = nCurrFrmNum - 1, nKeyToContinue = 0;
	int nExpPermittedHint = 0, nExpForbiddenHint = 0, nSoftExposeReqHint = 0;
	CR_CalibrationInfo calProgInfo;
	memset(&calProgInfo, 0x0, sizeof(CR_CalibrationInfo));
	m_mapCallbackPtrs[m_nCurrentDetrIndex]->SetPrintFlag(false);

	map<int, TUPLEOFAPPMODEKEY>::iterator iter = m_mapRegisteredAppModes[m_nCurrentDetrIndex].find(pairCalType.first);
	if (m_mapRegisteredAppModes[m_nCurrentDetrIndex].end() == iter)
	{
		cout << "Find trigger type failed from map" << endl;
	}
	TUPLEOFAPPMODEKEY tupleModeParams = iter->second;
	int nTriggType = (int)std::get<3>(std::get<0>(tupleModeParams));

	//////////////////////////////////////////////////////////////////////////
	//2. Gain Calibration, the X-ray should expose the detector
	//////////////////////////////////////////////////////////////////////////
	cout << "Make sure that the detector receives X-ray before gain calibration starts" << endl;
	cout << "Enter any integer to continue" << endl << "Your input >> ";
	cin >> nKeyToContinue;
	checkInput(nKeyToContinue);
	nCurrFrmNum = 0;
	nPrevFrmNum = nCurrFrmNum;
	nRet = CR_StartGainCalibration(m_nCurrentDetrIndex, pairCalType.first);
	if (GetReturnCode(nRet) > CR_WARN)
	{
		/*string strIntlMsg = GetLastIntlMsg();
		cout<<"StartGainCalibration failed, "<<strIntlMsg.c_str()<<endl;*/
		char info[256];
		CR_GetLastIntlMsg(info, 256);
		cout<<"StartGainCalibration failed, "<<string(info)<<endl;
		return nRet;
	}

	do
	{
		if (_kbhit())
		{
			nRet = CR_StopCalibration(m_nCurrentDetrIndex);
			return nRet;
		}

		//radiographic mode calibration flow
		if (!pairCalType.second)
		{
			if (CR_EXP_READY == calProgInfo.nExpStatus)
			{
				if (0 == nExpPermittedHint)
				{
					cout<< "Target value = "<<calProgInfo.nTargetGrayValue<<endl;
					cout<<"Now you can press x-ray hand switch..."<<endl;
					nExpPermittedHint++;
					nExpForbiddenHint = 0;

					if (CR_RadSoftSync == nTriggType)
					{
#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)
						Sleep(500);
#else
						sleep(500);
#endif
						if (0 == nSoftExposeReqHint)
						{
							nSoftExposeReqHint++;
							nRet = CR_RequestExposure(m_nCurrentDetrIndex);
							if (nRet != CR_OK)
								break;
						}
					}
				}
			}
			else if (CR_EXP_EXPOSE == calProgInfo.nExpStatus)
			{
				if (CR_RadSoftSync == nTriggType)
				{
					cout<<"Status Polled>> Detector is in EXPOSE state, emit X-Ray now..."<<endl;
				}
			}
			else
			{
				if (0 == nExpForbiddenHint)
				{
					cout<<"Now you can't press x-ray hand switch..."<<endl;
					nExpForbiddenHint++;
					nExpPermittedHint = 0;
				}
			}
		}

		CR_QueryCalibrationStatus(m_nCurrentDetrIndex, &calProgInfo);
		nCurrFrmNum = calProgInfo.nCurrentFrameNum;
		if(nCurrFrmNum > nPrevFrmNum)
		{
			nPrevFrmNum = nCurrFrmNum;
			cout<< "Gain calibration progress: " <<calProgInfo.nCurrentFrameNum<<
				" / "<< calProgInfo.nTotalFrameNum <<", current frame mean = "<<calProgInfo.nCurrentFrameMeanValue << '\r';
			cout.flush();
			if (!pairCalType.second)
			{
				nExpPermittedHint = 0;
				nSoftExposeReqHint = 0;
			}
		}
#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)
		Sleep(20);
#else
		sleep(20);
#endif
	}while(-1 == calProgInfo.nResult);
	cout << endl;

	nRet = calProgInfo.nResult;
	if(CR_OK == nRet)
	{
		cout<<"Gain calibration completes successfully."<<endl;
	}else if(GetReturnCode(nRet) > CR_WARN)
	{
		cout<<"StartGainCalibration failed, "<<string(calProgInfo.szErrorMsg)<<endl;
		CR_StopCalibration(m_nCurrentDetrIndex);
		return nRet;
	}else if (CR_WARN == GetReturnCode(nRet))
	{
		cout<<"StartGainCalibration warns, "<<string(calProgInfo.szErrorMsg)<<endl;
	}
	cout << endl;
	return nRet;
}

int CCareRayAPIDemo::Calibration(vector<pair<int, bool>> vecAppModeKeys)
{
	int nRet = CR_OK;
	char chProceedToCalibrate = 'y';
	cout<<"Proceed to calibrate ? ( y / n )"<<endl;
	cout<<"Your Input >> ";
	cin>>chProceedToCalibrate;
	checkInput(chProceedToCalibrate);

	if ('y' == tolower(chProceedToCalibrate))
	{
		//proceed to calibrate detector
		for (auto it : vecAppModeKeys)
		{
			cout << "Do calibration for application mode key: " << it.first <<endl;
			nRet = DoOffsetCalibration(it.first);
			if (CR_OK == nRet)
			{
				nRet = DoGainCalibration(it);
			}else
			{
				return nRet;
			}
		}
	}

	return nRet;
}

// void CCareRayAPIDemo::CalibrationUsingInterrupts()
// {
// 	m_pCallback->SetPrintFlag(true);
// 	thread t1(&CCareRayAPIDemo::CalibrationThread, m_pCallback);
// 	t1.join();
// }

// int CCareRayAPIDemo::CalibrationThread(CCallbackImp* pCallbackImp)
// {
// 	//////////////////////////////////////////////////////////////////////////
// 	//1. Dark Calibration, the X-ray shouldn't expose the detector
// 	//////////////////////////////////////////////////////////////////////////
// 	cout<<"Dark calibration starts ..."<<endl;
// 	int nRet = StartDarkCalibration();
// 	if (GetReturnCode(nRet) > WARN)
// 	{
// 		string strIntlMsg = GetLastIntlMsg();
// 		cout<<"StartDarkCalibration failed, "<<strIntlMsg.c_str()<<endl;
// 		return nRet;
// 	}
// 
// 	unique_lock<std::mutex> uniqLock(pCallbackImp->m_mtx);
// 	pCallbackImp->m_cndVar.wait(uniqLock);
// 	nRet = pCallbackImp->GetProcResult();
// 	if (GetReturnCode(nRet) > WARN)
// 	{
// 		cout<<"StartDarkCalibration failed, "<<pCallbackImp->GetCalInfo()->strReason.c_str()<<endl;
// 		return nRet;
// 	}
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	//2. Gain Calibration, the X-ray should expose the detector
// 	//////////////////////////////////////////////////////////////////////////
// 	int nKeyToContinue = 0;
// 	cout<<"Gain calibration starts. Make sure that the detector receives X-ray, then press any number key (0-9) to continue..."<<endl;
// 	cin>> nKeyToContinue;
// 	nRet = StartGainCalibration();
// 	if (GetReturnCode(nRet) > WARN)
// 	{
// 		string strIntlMsg = GetLastIntlMsg();
// 		cout<<"StartGainCalibration failed, "<<strIntlMsg.c_str()<<endl;
// 		return nRet;
// 	}
// 
// 	pCallbackImp->m_cndVar.wait(uniqLock);
// 	nRet = pCallbackImp->GetProcResult();
// 	if (GetReturnCode(nRet) > WARN)
// 	{
// 		cout<<"StartGainCalibration failed, "<<pCallbackImp->GetCalInfo()->strReason.c_str()<<endl;
// 	}else if (WARN == GetReturnCode(nRet))
// 	{
// 		cout<<"Calibration warns, "<<pCallbackImp->GetCalInfo()->strReason.c_str()<<endl;
// 	}
// 
// 	return nRet;
// }

// int CCareRayAPIDemo::Acquisition()
// {
// 	for (map<int, unsigned short*>::iterator iter = m_mapFrmBuf.begin(); iter != m_mapFrmBuf.end();)
// 	{
// 		delete[] iter->second;
// 		iter->second = NULL;
// 		iter = m_mapFrmBuf.erase(iter);
// 	}
// 
// 	int nRet = CR_OK, nFrameWidth = 0, nFrameHeight = 0;
// 	const int nFrameHeaderSize = 128, nFrameNum = 20;	
// 
// 	int nStartAcq = 0;
// 	cout <<endl <<"Press any key to quit anytime during the course of image acquisition." << endl;
// 	cout << "Enter any integer to start fluoroscopic image acquisition" << endl << "Your input >> ";
// 	cin >> nStartAcq;
// 	while (cin.fail())
// 	{
// 		cin.clear();
// 		cin.ignore();
// 		cout << "Invalid input, enter again" << endl << "Your input >> ";
// 		cin >> nStartAcq;
// 	}
// 
// 	nFrameWidth = m_mapActiveFluoroModes[m_nCurrentDetrIndex].nImageWidth, nFrameHeight = m_mapActiveFluoroModes[m_nCurrentDetrIndex].nImageHeight;
// 	if (NULL == m_mapFrmBuf[m_nCurrentDetrIndex])
// 	{
// 		m_mapFrmBuf[m_nCurrentDetrIndex] = (unsigned short*)malloc(((nFrameWidth * nFrameHeight + nFrameHeaderSize) * nFrameNum) * sizeof(unsigned short));
// 		if (NULL == m_mapFrmBuf[m_nCurrentDetrIndex])
// 		{
// 			return ALLOCATE_BUFF_FAIL;
// 		}
// 	}
// 
// 	m_mapCallbackPtrs[m_nCurrentDetrIndex]->SetBufferStartAddr(m_mapFrmBuf[m_nCurrentDetrIndex]);
// 
// 	nRet = CR_StartAcquisition(m_nCurrentDetrIndex, nFrameNum, m_mapFrmBuf[m_nCurrentDetrIndex]);
// 	if (GetReturnCode(nRet) > CR_WARN)
// 	{
// 		/*string strIntlMsg = GetLastIntlMsg();
// 		cout<<"StartAcquisition failed, "<<strIntlMsg.c_str()<<endl;*/
// 		char info[256];
// 		CR_GetLastIntlMsg(info, 256);
// 		cout<<"StartAcquisition failed, "<<string(info)<<endl;
// 		return nRet;
// 	}
// 
// 	//Press any key to stop the acquisition process
// 	while (!_kbhit())
// 	{
// 		Sleep(10);
// 	}
// 
// 	nRet = CR_StopAcquisition(m_nCurrentDetrIndex);
// 	if (GetReturnCode(nRet) > CR_WARN)
// 	{
// 		char info[256];
// 		CR_GetLastIntlMsg(info, 256);
// 		cout<<"StopAcquisition failed, "<<string(info)<<endl;
// 		return nRet;
// 	}
// 
// 	m_mapCallbackPtrs[m_nCurrentDetrIndex]->SetBufferStartAddr(NULL);
// 
// 	return nRet;
// }

int CCareRayAPIDemo::AcqAllDetrImgs()
{
// 	for (map<int, unsigned short*>::iterator iter = m_mapFrmBuf.begin(); iter != m_mapFrmBuf.end();)
// 	{
// 		delete[] iter->second;
// 		iter->second = NULL;
// 		iter = m_mapFrmBuf.erase(iter);
// 	}
// 
// 	const int nFrameHeaderSize = 128, nFrameNum = 20;
// 	int nRet = CR_OK, nFrameWidth = 0, nFrameHeight = 0;
// 	for(map<int, CR_ModeInfo>::iterator iterMode = m_mapActiveFluoroModes.begin(); iterMode != m_mapActiveFluoroModes.end(); ++iterMode)
// 	{
// 		m_mapCallbackPtrs[iterMode->first]->SetImageSaveFlag(false);
// 		nFrameWidth = iterMode->second.nImageWidth;
// 		nFrameHeight = iterMode->second.nImageHeight;
// 
// 		if (NULL == m_mapFrmBuf[iterMode->first])
// 		{
// 			m_mapFrmBuf[iterMode->first] = (unsigned short*)malloc(((nFrameWidth * nFrameHeight + nFrameHeaderSize) * nFrameNum) * sizeof(unsigned short));
// 			m_mapCallbackPtrs[iterMode->first]->SetBufferStartAddr(m_mapFrmBuf[iterMode->first]);
// 		}
// 
// 		nRet = CR_StartAcquisition(iterMode->first, nFrameNum, m_mapFrmBuf[iterMode->first]);
// 		if (GetReturnCode(nRet) > CR_WARN)
// 		{
// 			char info[256];
// 			CR_GetLastIntlMsg(info, 256);
// 			cout<<"StartAcquisition failed, "<<string(info)<<endl;
// 			return nRet;
// 		}
// 		else
// 		{
// 			cout<<"StartAcquisition for detector "<< iterMode->first <<endl;
// 		}
// 	}
// 
// 	//Press any key to stop the acquisition process
// 	while (!_kbhit())
// 	{
// 		Sleep(10);
// 	}
// 
// 	for(map<int, CCallbackImp*>::iterator iterCallBack = m_mapCallbackPtrs.begin(); iterCallBack != m_mapCallbackPtrs.end(); ++iterCallBack)
// 	{
// 		nRet = CR_StopAcquisition(iterCallBack->first);
// 		if (GetReturnCode(nRet) > CR_WARN)
// 		{
// 			char info[256];
// 			CR_GetLastIntlMsg(info, 256);
// 			cout<<"StopAcquisition failed, "<<string(info)<<endl;
// 			return nRet;
// 		}
// 		else
// 		{
// 			cout<<"StopAcquisition for detector "<< iterCallBack->first <<endl;
// 		}
// 
// 		iterCallBack->second->SetBufferStartAddr(NULL);		
// 	}
// 
 	return CR_OK;
}

//////////////////////////////////////////////////////////////////////////
// This function describes the normal fluoroscopic acquisition for each registered mode.
// For developing rapid application mode switching (such as spot film), refer to the 
// document of "Software Interface Specification.pdf"
//////////////////////////////////////////////////////////////////////////
int CCareRayAPIDemo::Acquisition()
{
	int nRet = CR_OK, nMaxFrameWidth = 0, nMaxFrameHeight = 0, nBuffSize = 0;
	const int nFrameHeaderSize = m_mapSystemInfos[m_nCurrentDetrIndex].nFrmHeaderLen, nFrameNum= 20;
	bool bRegisteredFluoroMode = false;

	for (auto it : m_mapFrmBuf)
	{
		if (NULL != it.second)
		{
			free(it.second);
			it.second = NULL;
		}
	}
	m_mapFrmBuf.erase(m_mapFrmBuf.begin(), m_mapFrmBuf.end());

 	int nStartAcq = 0;
	nMaxFrameWidth = m_mapSystemInfos[m_nCurrentDetrIndex].nRawImageWidth;
	nMaxFrameHeight = m_mapSystemInfos[m_nCurrentDetrIndex].nRawImageHeight;

	if (NULL == m_mapFrmBuf[m_nCurrentDetrIndex])
	{
		nBuffSize = (nMaxFrameWidth * nMaxFrameHeight * sizeof(unsigned short) + nFrameHeaderSize) * nFrameNum;
		m_mapFrmBuf[m_nCurrentDetrIndex] = (unsigned short*)malloc(nBuffSize);
		if (NULL == m_mapFrmBuf[m_nCurrentDetrIndex])
		{
			return ALLOCATE_BUFF_FAIL;
		}
	}
	m_mapCallbackPtrs[m_nCurrentDetrIndex]->SetBufferStartAddr(m_mapFrmBuf[m_nCurrentDetrIndex]);

	cout <<"Press any key to acquire images at next registered mode anytime during the course of image acquisition." << endl;
	cout << "Enter any integer to start fluoroscopic image acquisition" << endl << "Your input >> ";
	cin >> nStartAcq;
	checkInput(nStartAcq);

	map<int, TUPLEOFAPPMODEKEY> mapRegisteredAppModes = m_mapRegisteredAppModes[m_nCurrentDetrIndex];
	for (auto it : mapRegisteredAppModes)
	{
		TUPLEOFAPPMODEKEY tupleModeParams = it.second;
		if (!(bool)std::get<1>(tupleModeParams))
		{
			//skip the registered radiographic modes
			continue;
		}
		else
		{
			bRegisteredFluoroMode = true;
		}

		nRet = CR_LoadReference(m_nCurrentDetrIndex, it.first);
		if (GetReturnCode(nRet) > CR_WARN)
		{
			char info[256];
			CR_GetLastIntlMsg(info, 256);
			cout<<"LoadReference failed, "<<string(info)<<endl;
			//return nRet;
		}

		nRet = CR_StartAcquisition(m_nCurrentDetrIndex, it.first, m_mapFrmBuf[m_nCurrentDetrIndex], nBuffSize);
		if (GetReturnCode(nRet) > CR_WARN)
		{
			char info[256];
			CR_GetLastIntlMsg(info, 256);
			cout<<"StartAcquisition failed, "<<string(info)<<endl;
			return nRet;
		}

		while (1)
		{
			if (_kbhit())
			{
				_getch();
				break;
			}
		}

		nRet = CR_StopAcquisition(m_nCurrentDetrIndex);
		if (GetReturnCode(nRet) > CR_WARN)
		{
			char info[256];
			CR_GetLastIntlMsg(info, 256);
			cout<<"StopAcquisition failed, "<<string(info)<<endl;
			return nRet;
		}
	}

	if (!bRegisteredFluoroMode)
	{
		cout <<"No fluoroscopic modes registered yet"<< endl;
	}

	m_mapCallbackPtrs[m_nCurrentDetrIndex]->SetBufferStartAddr(NULL);

	return nRet;
}

int CCareRayAPIDemo::AcquireDarkImage()
{
	unsigned short* pImage = NULL;
	int nFrmWidth = 0, nFrmHeight = 0, nFrmSize = 0, nRet = CR_OK;
	int nExpPermittedHint = 0, nWaitPermissionHint = 0;
	int nAppModeKey = 0, nTriggType = 0;
	map<int, TUPLEOFAPPMODEKEY>::iterator iter;
	
	DisplayAppModesInfo(&m_mapRegisteredAppModes[m_nCurrentDetrIndex], false, true);

	cout << "Enter an application mode key to acquire a dark radiographic image >> ";
	while (1)
	{
		cin >> nAppModeKey;
		checkInput(nAppModeKey);
		iter = m_mapRegisteredAppModes[m_nCurrentDetrIndex].find(nAppModeKey);
		if (m_mapRegisteredAppModes[m_nCurrentDetrIndex].end() == iter)
		{
			cout << "Invalid input, enter again" << endl << "Your input >> ";
			continue;
		}
		break;
	}

	nRet = CR_LoadReference(m_nCurrentDetrIndex, nAppModeKey);
	if(GetReturnCode(nRet) > CR_WARN)
	{
		char info[256];
		CR_GetLastIntlMsg(info, 256);
		cout<<"LoadReference failed, "<<string(info)<<endl;
		return nRet;
	}

	nRet = CR_StartDarkAcquisition(m_nCurrentDetrIndex, nAppModeKey, NULL);
	if (GetReturnCode(nRet) > CR_WARN)
	{
		char info[256];
		CR_GetLastIntlMsg(info, 256);
		cout<<"StartDarkAcquisition failed, "<<string(info)<<endl;
		return nRet;
	}

	CR_ExpProgress oExpProg;
	memset(&oExpProg, 0x0, sizeof(CR_ExpProgress));
	TUPLEOFAPPMODEKEY tupleModeParams = iter->second;
	nTriggType = (int)std::get<3>(std::get<0>(tupleModeParams));

	do
	{
		nRet = CR_QueryAcquisitionStatus(m_nCurrentDetrIndex, &oExpProg);
		if (nRet != CR_OK)
			break;
		//cout<<"status:"<<oExpProg.nExpStatus<<endl;
		if (oExpProg.bIsFetchable)
		{
			cout<<"Image is fetchable now..."<<endl;
			break;
		}
#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)
		Sleep(50);
#else
		sleep(50);
#endif
	}while(true);

	if (oExpProg.bIsFetchable)
	{
		for (int i = 0; i < m_mapModeNum[m_nCurrentDetrIndex]; i++)
		{
			if (m_mapModeInfos[m_nCurrentDetrIndex][i].nModeID == (int)std::get<0>(std::get<0>(tupleModeParams)))
			{
				nFrmWidth = m_mapModeInfos[m_nCurrentDetrIndex][i].nImageWidth;
				nFrmHeight = m_mapModeInfos[m_nCurrentDetrIndex][i].nImageHeight;
				nFrmSize = (nFrmWidth * nFrmHeight) * sizeof(unsigned short) + RAD_HEADER_SIZE;
				pImage = (unsigned short*)malloc(nFrmSize);
				if (NULL != pImage)
				{
					nRet = CR_GetImage(m_nCurrentDetrIndex, pImage, nFrmSize);
					if (GetReturnCode(nRet) > CR_WARN)
					{
						char info[256];
						CR_GetLastIntlMsg(info, 256);
						cout<<"GetImage failed, "<<string(info)<<endl;				
					}
				}
				break;
			}
		}

#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)
		stringstream ss;
		ss << ".\\" << m_nCurrentDetrIndex << "\\";
		string folder = ss.str();
		_mkdir(folder.c_str());

		ss.str("");
		ss << folder << "darkImage.raw";
#else
		stringstream ss;
		ss << "./" << m_nCurrentDetrIndex << "\\";
		string folder = ss.str();

		ss.str("");
		ss << folder << "darkImage.raw";
#endif
		ofstream fos(ss.str().c_str(), ios::binary);
		fos.write((char*)pImage, nFrmSize);
		fos.close();

		free(pImage);
		pImage = NULL;
	}else
	{
		nRet = CR_StopAcquisition(m_nCurrentDetrIndex);
		if (GetReturnCode(nRet) > CR_WARN)
		{
			char info[256];
			CR_GetLastIntlMsg(info, 256);
			cout<<"StopAcquisition failed, "<<string(info)<<endl;
			return nRet;
		}
	}

	return CR_OK;
}

int CCareRayAPIDemo::AcquireRadImage()
{
	// This test is specific to Radiography mode
	unsigned short* pImage = NULL;
	int nFrmWidth = 0, nFrmHeight = 0, nFrmSize = 0, nRet = CR_OK;
	int nExpPermittedHint = 0, nWaitPermissionHint = 0, nSoftExposeReqHint = 0;
	int nAppModeKey = 0, nTriggType = 0;
	map<int, TUPLEOFAPPMODEKEY>::iterator iter;
	
	DisplayAppModesInfo(&m_mapRegisteredAppModes[m_nCurrentDetrIndex], false, true);

	cout << "Enter an application mode key to acquire a radiographic image >> ";
	while (1)
	{
		cin >> nAppModeKey;
		checkInput(nAppModeKey);
		iter = m_mapRegisteredAppModes[m_nCurrentDetrIndex].find(nAppModeKey);
		if (m_mapRegisteredAppModes[m_nCurrentDetrIndex].end() == iter)
		{
			cout << "Invalid input, enter again" << endl << "Your input >> ";
			continue;
		}
		break;
	}

	nRet = CR_LoadReference(m_nCurrentDetrIndex, nAppModeKey);
	if(GetReturnCode(nRet) > CR_WARN)
	{
		char info[256];
		CR_GetLastIntlMsg(info, 256);
		cout<<"LoadReference failed, "<<string(info)<<endl;
		return nRet;
	}

	nRet = CR_StartAcquisition(m_nCurrentDetrIndex, nAppModeKey, NULL, 0);
	if (GetReturnCode(nRet) > CR_WARN)
	{
		char info[256];
		CR_GetLastIntlMsg(info, 256);
		cout<<"StartAcquisition failed, "<<string(info)<<endl;
		return nRet;
	}

	CR_ExpProgress oExpProg;
	memset(&oExpProg, 0x0, sizeof(CR_ExpProgress));
	TUPLEOFAPPMODEKEY tupleModeParams = iter->second;
	nTriggType = (int)std::get<3>(std::get<0>(tupleModeParams));
	
	do
	{
		nRet = CR_QueryAcquisitionStatus(m_nCurrentDetrIndex, &oExpProg);
		if (nRet != CR_OK)
			break;
		//cout<<"status:"<<oExpProg.nExpStatus<<endl;
		switch (oExpProg.nExpStatus)
		{
		case CR_EXP_READY:
			//press 2nd gear of handswitch to send EXP_REQ signal to detector;
			//To simulate the interactive progress, we could add some interval command in the future. 
			if (0 == nExpPermittedHint)
			{
				cout<<"Status Polled>> Detector is ready for Rad Acquisition, press x-ray hand switch now..."<<endl;
				nExpPermittedHint++;
				if (CR_RadSoftSync == nTriggType)
				{
#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)
					Sleep(500);
#else
					sleep(500);
#endif
					if (0 == nSoftExposeReqHint)
					{
						nSoftExposeReqHint++;
						nRet = CR_RequestExposure(m_nCurrentDetrIndex);
						if (nRet != CR_OK)
							break;
					}
				}
			}
			break;
		case CR_EXP_WAIT_PERMISSION:
			if (0 == nWaitPermissionHint)
			{
				cout<<"Status Polled>> Detector is waiting for exposure permission..."<<endl;
				nRet = CR_PermitExposure(m_nCurrentDetrIndex);
				if (nRet != CR_OK)
					break;
				nWaitPermissionHint++;
			}
		case CR_EXP_EXPOSE:
			{
				if (CR_RadSoftSync == nTriggType)
				{
					cout<<"Status Polled>> Detector is in EXPOSE state, emit X-Ray now..."<<endl;
				}
			}
			break;
		default:
			cout<<"Status Polled>> Don't press x-ray hand switch now..."<<endl;
			nExpPermittedHint = 0;
			break;
		}
		if (oExpProg.bIsFetchable)
		{
			cout<<"Image is fetchable now..."<<endl;
			break;
		}
#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)
		Sleep(50);
#else
		sleep(50);
#endif
	}while(true);

	if (oExpProg.bIsFetchable)
	{
		for (int i = 0; i < m_mapModeNum[m_nCurrentDetrIndex]; i++)
		{
			if (m_mapModeInfos[m_nCurrentDetrIndex][i].nModeID == (int)std::get<0>(std::get<0>(tupleModeParams)))
			{
				nFrmWidth = m_mapModeInfos[m_nCurrentDetrIndex][i].nImageWidth;
				nFrmHeight = m_mapModeInfos[m_nCurrentDetrIndex][i].nImageHeight;
				nFrmSize = (nFrmWidth * nFrmHeight) * sizeof(unsigned short) + RAD_HEADER_SIZE;
				pImage = (unsigned short*)malloc(nFrmSize);
				if (NULL != pImage)
				{
					nRet = CR_GetImage(m_nCurrentDetrIndex, pImage, nFrmSize);
					if (GetReturnCode(nRet) > CR_WARN)
					{
						char info[256];
						CR_GetLastIntlMsg(info, 256);
						cout<<"GetImage failed, "<<string(info)<<endl;				
					}
				}
				break;
			}
		}

#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)
		stringstream ss;
		ss << ".\\" << m_nCurrentDetrIndex << "\\";
		string folder = ss.str();
		_mkdir(folder.c_str());

		ss.str("");
		ss << folder << "radImage.raw";
#else
		stringstream ss;
		ss << "./" << m_nCurrentDetrIndex << "\\";
		string folder = ss.str();

		ss.str("");
		ss << folder << "radImage.raw";
#endif
		ofstream fos(ss.str().c_str(), ios::binary);
		fos.write((char*)pImage, nFrmSize);
		fos.close();

		free(pImage);
		pImage = NULL;
	}else
	{
		nRet = CR_StopAcquisition(m_nCurrentDetrIndex);
		if (GetReturnCode(nRet) > CR_WARN)
		{
			char info[256];
			CR_GetLastIntlMsg(info, 256);
			cout<<"StopAcquisition failed, "<<string(info)<<endl;
			return nRet;
		}
	}

	return CR_OK;
}

// bool CCareRayAPIDemo::hIsRadandSyncSupported(CR_ModeInfo* szModeInfo, int nModeNum, int& nModeIdx, int nSyncType)
// {   
// 	nModeIdx = -1;
// 	for (int i = 0; i < nModeNum; i++)
// 	{
// 		if (CR_Radiography == szModeInfo[i].nModeID)
// 		{
// 			nModeIdx = i;
// 			break;
// 		}
// 	}
// 	if (nModeIdx >= 0)
// 	{
// 		if (nSyncType == -1) // only check the radiography mode
// 			return true;
// 		else
// 		{
// 			bool syncTypeExists = false;
// 			for (int i=0; i < szModeInfo[nModeIdx].nTrigTypeNum; i++)
// 			{
// 				if (nSyncType == szModeInfo[nModeIdx].nTrigTypes[i])
// 				{
// 					syncTypeExists = true;
// 					break;
// 				}
// 			}
// 			if (syncTypeExists)
// 				return true;
// 			else
// 				return false;
// 		}
// 	}
// 	return false;
// }

bool CCareRayAPIDemo::IsRadiographicModeRegistered()
{
	map<int, TUPLEOFAPPMODEKEY> mapRegisteredAppModes = m_mapRegisteredAppModes[m_nCurrentDetrIndex];
	for (auto it : mapRegisteredAppModes)
	{
		TUPLEOFAPPMODEKEY tupleModeParams = it.second;
		if (!(bool)std::get<1>(tupleModeParams))
		{
			return true;
		}
	}
	return false;
}

string CCareRayAPIDemo::GetReferenceFilesStatus(int nRefStatus)
{
	string strRefStatus("");
	switch (nRefStatus)
	{
	case CR_REF_NOT_FOUND:
		strRefStatus = "Not found";
		break;
	case CR_REF_VALID:
		strRefStatus = "Valid";
		break;
	case CR_REF_ABOUT_TO_EXPIRE:
		strRefStatus = "About to expire";
		break;
	case CR_REF_INVALID:
		strRefStatus = "Invalid";
		break;
	default:
		strRefStatus = "Unknown";
		break;
	}

	return strRefStatus;
}

int CCareRayAPIDemo::GetDaecParams()
{
	CR_ActiveAreaInfo oActiveAreas[5];
	int nActiveAreasNum = 0;
	int nThreshold = 0;
	int nRet = CR_GetDaecActiveAreas(m_nCurrentDetrIndex, oActiveAreas, &nActiveAreasNum, &nThreshold);

	if (GetReturnCode(nRet) <= CR_WARN)
	{
		printf("DAEC parameters :\n threshold = %d, active_areas_num = d\n", nThreshold, nActiveAreasNum);
		for (int nAreaIdx = 0; nAreaIdx < nActiveAreasNum; ++nAreaIdx)
		{
			printf("\t active_area_%d : x = %d, y = %d, width = %d, height =%d, weight = %.02f\n",
				nAreaIdx, oActiveAreas[nAreaIdx].x, oActiveAreas[nAreaIdx].y, oActiveAreas[nAreaIdx].width, oActiveAreas[nAreaIdx].height, oActiveAreas[nAreaIdx].weight);
		}
	}
	else
	{
		printf("Get DAEC parameters failed!\n");
	}

	return nRet;
}

int CCareRayAPIDemo::SetDaecParams()
{	
	CR_ActiveAreaInfo oActiveAreas[8];
	int nActiveAreasNum = 0;
	int nThreshold = 0;

	printf("Please enter an integer for Threshold\n");
	scanf("%d", &nThreshold);

	printf("Please enter an integer for total active areas number\n");
	scanf("%d", &nActiveAreasNum);

	for (int nAreaIdx = 0; nAreaIdx < nActiveAreasNum; ++nAreaIdx)
	{
		printf("Please enter an integer for x\n");
		scanf("%d", &(oActiveAreas[nAreaIdx].x));
		printf("Please enter an integer for y\n");
		scanf("%d", &(oActiveAreas[nAreaIdx].y));
		printf("Please enter an integer for width\n");
		scanf("%d", &(oActiveAreas[nAreaIdx].width));
		printf("Please enter an integer for height\n");
		scanf("%d", &(oActiveAreas[nAreaIdx].height));
		printf("Please enter an float for weight\n");
		scanf("%f", &(oActiveAreas[nAreaIdx].weight));
	}

	int nRet = CR_SetDaecActiveAreas(m_nCurrentDetrIndex, oActiveAreas, nActiveAreasNum, nThreshold);
	printf("Set DAEC parameters result code = %d\n", nRet);

	return nRet;
}

void CCareRayAPIDemo::GenerateSampleCustomModeConfigFile()
{
	CR_GenerateSampleCustomModeConfigFile(m_nCurrentDetrIndex);
	printf("Sample custom mode config file save to \"./Conf/CustomMode.ini\"\n");
}

void CCareRayAPIDemo::RegisterCustomModeFromConfigFile()
{
	char szRegistResult[4096];
	CR_RegisterCustomModeFromConfigFile(m_nCurrentDetrIndex, szRegistResult);
	printf("%s\n", szRegistResult);
}