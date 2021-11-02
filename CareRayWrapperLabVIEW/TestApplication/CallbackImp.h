/**
* \file               CallbackImp.h
* \date             2017/11/24
* \version         5.2.2
* \author          Shuai Li
* \brief             Declaration of Callback function which would be implemented by integrator
*  Copyright 2017 CARERAY LTD. All Rights Reserved. 
*/

#ifndef __CALLBACK_IMP_H__
#define __CALLBACK_IMP_H__

#include "ICallback.h"
#include <iostream>
#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)
#include <direct.h>
#include <conio.h>
#endif
#include <fstream>
#include <sstream>
#include <iomanip>
#include "IntlMsgAliasID.h"
#include "CareRayAPIDll.h"

using namespace std;

/** 
* \class CCallbackImp
* \brief methods  and variables declare for callback function
*/
class CCallbackImp : public ICallback
{
public:

	CCallbackImp() : ICallback()
	{
		m_isDisableDebugFlag = true;
		m_pBuffStartAddr = NULL;
		m_isImageSaved = false;
		m_isPrintFlag = false;
		m_nFrmHeaderLen = 256;
	};

	/**
	* @brief set flag of processing result
	* @param[in] nRet  ( passed in parameter )
	*/
	void SetProcResult(int nRet) { m_nRet = nRet; };

	/**
	* @brief get flag of processing result
	*/
	int GetProcResult() { return m_nRet; };

	/**
	* @brief set flag of printing calibration progress information
	* @param[in] nRet  ( passed in parameter )
	*/
	void SetPrintFlag(bool isPrint) { m_isPrintFlag = isPrint; };

	CR_CalibrationInfo* GetCalInfo() { return &m_oCalInfo; };

	/**
	* @brief virtual function which would be override written used to process each event
	* @param[in] nEventID  ( trigger event id )
	* @param[in] pEventParam  ( the event data attached to the event id )
	*/
	virtual void Process(int nEventID, CR_Event* pEventParam);

	void SetImageSaveFlag(bool isImageSaved) { m_isImageSaved = isImageSaved; };
	
	void SetBufferStartAddr(void* pBuffStartAddr)	{ m_pBuffStartAddr = pBuffStartAddr;};

	void SetFrmHeaderLen(int nLen)	{ m_nFrmHeaderLen = nLen;};

private:
	
	bool m_isImageSaved;
	void* m_pBuffStartAddr;

	int m_nRet;
	int m_nFrmHeaderLen;
	bool m_isPrintFlag;
	bool m_isDisableDebugFlag;
	CR_CalibrationInfo m_oCalInfo;

	void PrintMatrix(string strName, string imgDataStr)
	{
		ofstream fout;
		fout.open(strName.c_str(), ios::binary);
		if(true != fout.good())
		{
			//throw CException(0,"Failed to open file.");
			cout<<"Failed to save frame to ."<< strName.c_str() << endl;
		}
		else
		{
			cout << "Save file path: " << strName << endl;
		}

		fout.write(imgDataStr.c_str(), imgDataStr.length());			
		fout.close();
	}
};

#endif