/**
* \file                CallbackImp.cpp
* \date              2017/11/24
* \version          5.2.2
* \author           shuai.li
* \brief              Implement the callback function declared in CallbackImp.h
*  Copyright 2017 CARERAY LTD. All Rights Reserved. 
*/
#include "stdafx.h"
#include "CallbackImp.h"

void CCallbackImp::Process(int nEventID, CR_Event* pEventParam)
{
	switch (nEventID)
	{
	case CR_EVT_NEW_FRAME:
		{
			int nBuffIdx = *(int*)pEventParam->pData;
			int nImgSize = pEventParam->nWidth * pEventParam->nHeight * 2 + m_nFrmHeaderLen;
			char* pCurrFrame = (char*)m_pBuffStartAddr + nBuffIdx * nImgSize;
			int nFrameId = *(int*)(pCurrFrame + 8);

			cout << "Frame id = " << nFrameId << endl;

#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)
			if (m_isImageSaved)
			{
				stringstream ss;
				ss << "./" << pEventParam->nDetrIndex << "/";
				string folder = ss.str();
				_mkdir(folder.c_str());
				
				ss<< pEventParam->nWidth << "x" << pEventParam->nHeight;
				folder = ss.str();
				_mkdir(folder.c_str());

				if (m_pBuffStartAddr != NULL)
				{
					ss.str("");
					ss << folder << "\\ "<< setw(3)<< setfill('0') << nFrameId <<".raw";
					PrintMatrix(ss.str(), string(pCurrFrame, nImgSize));
				}
			}
#endif
		}
		break;
	case CR_EVT_CALIBRATION_IN_PROGRESS:
		{
			if (m_isPrintFlag)
			{
				CR_CalibrationInfo calInfo = *(CR_CalibrationInfo*)pEventParam->pData;
				cout<< "Callback: calibration progress: " <<calInfo.nCurrentFrameNum<<" / "<< calInfo.nTotalFrameNum <<", current frame mean = "<<calInfo.nCurrentFrameMeanValue<< '\r';
				SetProcResult(calInfo.nResult);
			}
		}
		break;
	case CR_EVT_CALIBRATION_FINISHED:
		{
			if (m_isPrintFlag)
			{
				CR_CalibrationInfo calInfo = *(CR_CalibrationInfo*)pEventParam->pData;
				cout<<endl<< "Callback: calibration result: "<<calInfo.nResult<< endl;
				m_oCalInfo = calInfo;
				SetProcResult(calInfo.nResult);
			}
		}
		break;
	case CR_EVT_TEMPERATURE_INFO:
		{
			if (!m_isDisableDebugFlag)
			{
				cout << "Detector ID: " << pEventParam->nDetrIndex<<endl;
				CR_Temperature temperature = *(CR_Temperature*)pEventParam->pData;
				cout<<"Current temperature:"<<temperature.currentTemperature<<endl;
				// 				cout<<"Average temperature: "<<temperature.aveTemperature<<endl;
// 				cout<<"Max temperature:"<<temperature.maxTemperature<<endl;
				cout<<"Over hot flag: "<<temperature.overhot_flag<<endl;
			}
		}
		break;
	case CR_EVT_ACQ_STAT_INFO:
		{
			cout << "Detector ID: " << pEventParam->nDetrIndex;
			CR_AcquisitionStatInfo acqStatInfo = *(CR_AcquisitionStatInfo*)pEventParam->pData;
			stringstream ss;
			ss<<"Statistic of current acquisition:"<<"duration: "<<acqStatInfo.nAcqDuration/1000.0<<" s, "
				<<"frames received: "<<acqStatInfo.nTotalFrameNum<<", frames lost: "<< acqStatInfo.nLostFrameNum <<", "
				<<"frame rate: "<<acqStatInfo.fStatFrameRate<<" fps, speed: "<<acqStatInfo.fTransmissionSpeed<<" MB/s"<<endl;
			cout<<endl<<ss.str()<<endl;
			
		}
		break;
	case CR_EVT_DETR_DISCONNECTED:
		{
			cout<<"Detector disconnected."<<endl;
		}
		break;
	case CR_EVT_SERVER_DISCONNECTED:
		{
			cout<<"Server disconnected"<<endl;
		}
		break;
	case CR_EVT_DETR_RECONNECTED:
		{
			cout<<"Detector reconnected."<<endl;
		}
		break;
	case CR_EVT_SERVER_RECONNECTED:
		{
			cout<<"Server reconnected"<<endl;
		}
		break;
	case CR_EVT_IMAGE_QUEUE_BLOCKED:
		{
			cout<<(char*)pEventParam->pData<<endl;
		}
		break;
	default:
		break;
	}
}
