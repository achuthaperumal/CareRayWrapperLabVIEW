#ifndef __STRUCTURE_530_H__	
#define __STRUCTURE_530_H__

#include <iostream>
#include <vector>

using std::vector;
using std::string;

#define CR_DIR_TO_SAVE_CAL_FILES    "DirToSaveCalFiles"

namespace CareRay
{
	//*****************************************************************************************
	// enum starts here.
	//*****************************************************************************************
	enum CR_TrigTypeOpt
	{
		CR_RadExtSync		= 0x01,
		CR_RadSoftSync		= 0x02,
		CR_RadAutoSync		= 0x03,
		CR_RadManualSync	= 0x04,
		CR_RadScanSync		= 0x05,
		CR_RadDaecSync      = 0x06,
		CR_FluReserved		= 0x07,
		CR_FluExtSync		= 0x08,
		CR_FluIntSync	    = 0x09,
	};

	enum CR_GainLevelOpt
	{
		CR_G0 = 0,
		CR_G1 = 1,
		CR_G2 = 2,
		CR_G3 = 3,
		CR_G4 = 4,
		CR_G5 = 5,
		CR_G6 = 6,
		CR_G7 = 7,
	};

	enum CR_PowerModeID
	{
		CR_PWR_STANDBY,				//set front-end into nap mode
		CR_PWR_FULL_RUNNING,			//set front-end into normal mode.
		CR_PWR_SMART_RUNNING,			//set front-end into nap mode at the integration phase.
		CR_PWR_DOWN_FE = 4,	//set front-end into power-down mode.               
		CR_PWR_SLEEPIN,				//unsupported
		CR_PWR_DEEP_SLEEPING,			//unsupported
		CR_PWR_SUSPEND,				//unsupported
	};

	enum CR_AcqModeOpt
	{
		CR_Radiography           = 0x10,
		CR_Fluoroscopy           = 0x11,
		CR_FluoroLag             = 0x12,
		CR_FluoroCustom          = 0x13,
		CR_InnerTest             = 0x14,
		CR_Bin22                 = 0x15,
		CR_NondestructiveTesting = 0x16,
		CR_Preview               = 0x17,
		CR_Bin11                 = 0x18,
	};

	enum CR_EventID
	{
		CR_EVT_SERVER_DISCONNECTED,
		CR_EVT_DETR_DISCONNECTED,
		CR_EVT_EXPOSURE_INFO,
		CR_EVT_TEMPERATURE_INFO,
		CR_EVT_BATTERY_INFO,
		CR_EVT_WIRELESS_INFO,
		CR_EVT_NEW_FRAME,
		CR_EVT_CALIBRATION_IN_PROGRESS,
		CR_EVT_CALIBRATION_FINISHED,
		CR_EVT_NEW_SPOT_FILM,
		CR_EVT_ACQ_STAT_INFO,
		CR_EVT_RAD_ACQ_IN_PROGRESS,
		CR_EVT_SERVER_RECONNECTED,
		CR_EVT_DETR_RECONNECTED,
		CR_EVT_IMAGE_QUEUE_BLOCKED
	};

	enum CR_ExpStatus
	{
		CR_EXP_ERROR = -1,
		CR_EXP_INIT,
		CR_EXP_READY,
		CR_EXP_WAIT_PERMISSION,
		CR_EXP_PERMITTED,
		CR_EXP_EXPOSE,
		CR_EXP_COMPLETE,
	};

	enum CR_ReturnCodeType
	{
		CR_INFOR = 1,
		CR_WARN,
		CR_ERROR,
		CR_FATALERROR
	};

	enum CR_CalFileType
	{
		CR_OFFSET_FILE = 0,
		CR_GAIN_FILE = 1,
		CR_DEFECT_FILE = 2
	};

	enum CR_RefStatus
	{
		CR_REF_NOT_FOUND,
		CR_REF_VALID,
		CR_REF_ABOUT_TO_EXPIRE,
		CR_REF_INVALID
	};

	enum CR_CalibFilterMethod
	{
		CR_CALIB_FILTER_BASE,
		CR_CALIB_FILTER_MEAN,
		CR_CALIB_FILTER_BOUNDARY
	};

	enum CR_ProcChainOpt
	{
		CR_PROCCHAIN_SANITYCHECK      = 0x01,
		CR_PROCCHAIN_DARKCORR         = 0x02,
		CR_PROCCHAIN_GAINCORR         = 0x04,
		CR_PROCCHAIN_DEFECTCORR       = 0x08,
		CR_PROCCHAIN_LAGCORR          = 0x10,
		CR_PROCCHAIN_IMGCROPPING      = 0x20,
		CR_PROCCHAIN_RTPIXELCORR      = 0x40,
		CR_PROCCHAIN_DENOISING        = 0x80,
		CR_PROCCHAIN_GAUSSFILTER      = 0x80000000
	};

	enum CR_ProcChainID
	{
		CR_PROCCHAINID_FLUOROSCOPY = 1,
		CR_PROCCHAINID_RADIOGRAPHY = 2
	};

	//*****************************************************************************************
	// struct starts here.
	//*****************************************************************************************
	struct CR_SystemInfo
	{
		unsigned int nRawImageWidth;
		unsigned int nRawImageHeight;
		unsigned int nFrmHeaderLen;
		char szHardwareVersion[64];
		char szSerialNumber[64];
		char szSoftwareVersion[64];
		char szFirmwareVersion[64];
		char szDetrMachineID[64];
		char szDetrDesc[128];
		char szReserved[512];
	};

	struct CR_DefectInfo
	{
		unsigned int unNumBadPixel;
		unsigned int unNumBadRow;
		unsigned int unNumBadCol;
		unsigned short *pBadPixelPtr;
		unsigned short *pBadRowPtr;
		unsigned short *pBadColPtr;
	};

	struct CR_ConnectionStatus
	{
		bool isServerConnected;
		bool isDetrConnected;
	};

	struct CR_Temperature
	{
		float reserved[4];
		float currentTemperature;
		float maxTemperature;
		float aveTemperature;
		unsigned overhot_flag;
	};

	struct CR_DetrStatus
	{
		CR_Temperature oDetrTemperature;
	};

	struct CR_DetrIdxAndIPAddr
	{
		int nIdx;
		char szIPAddr[64];
		char szReserved[128];
	};

	struct CR_RegAttrInfo
	{
		bool		isWritable;
		bool		isFloat;
		unsigned	nVal;
		float		fVal;
		char		czPropertyName[32];
	};

	struct CR_ModeInfo
	{
		int nModeID;
		int nImageWidth;	// ROI dimension along x direction after binning
		int nImageHeight;	// ROI dimension along y direction after binning
		int nCutoffX;	// number of cut-off lines (along x direction) from left after binning
		int nCutoffY;			// number of cut-off lines (along y direction) from top after binning
		int nBinX;				// binning scheme along x direction
		int nBinY;				// binning scheme along y direction
		float fMaxFrmRate;	// in Hz
		float fMaxExpTime;		// in ms
		int nPixelDepth;			// bit depth of each pixel sending out from detector
		int nTrigTypes[16];
		int nTrigTypeNum;
		int nGainLevels[16];
		int nGainLevelNum;
		int nDefaultTrigType;
		int nDefaultGainLevel;
		int nRoiX;					// x coordinate of top left point after binning
		int nRoiY;					// y coordinate of top left point after binning
		char szDesc[256];		// description of this mode	
	};

	struct CR_CalibrationInfo
	{
		int nTotalFrameNum;
		int nCurrentFrameNum;
		int nCurrentFrameMeanValue;
		char szErrorMsg[256];
		int nResult;
		int nExpStatus;
		int nTargetGrayValue;
	};

	struct CR_ConfItemInfo
	{
		int nId;
		string strName;
		string strModule;
		string strValue;
		string strDefaultValue;
		string strRegex;
		int nPermission;
		string strDesc;
		string strDescZH;
		int nReboot;
	};

	struct CR_ProcessModule
	{
		int nModuleID;
		int nOrderID;
		string strName;
		int nEnabled;
		int nDefaultEnabled;
		int nAutoSave;
		int nUpdatePolicy;
	};

	struct CR_ProcessChain
	{
		int nChainID;
		vector<CR_ProcessModule> vecModule;
	};

	struct CR_Event
	{
		int nDetrIndex;
		int nWidth;
		int nHeight;
		int nPixelDepth;
		void* pData;
	};

	struct CR_AcquisitionStatInfo
	{
		int nTotalFrameNum;
		int nLostFrameNum;
		float fStatFrameRate;
		float fTransmissionSpeed;
		long long nAcqDuration;
	};

	// Image quality statistics
	struct CR_SQualityStatistics
	{
		// Passed type count
		int m_nQualityPassedCount;
		// Failed type count
		int m_nQualityFailedCount;
	};

	struct CR_ExpProgress
	{
		int nExpStatus;
		int bIsFetchable;
		int nResult;
	};

	struct CR_CalFileInfo
	{
		int		nCalType;
		float	fFrameRate;
		int		nGainId;
		float	fTemperature;
		int		nStatus;
		int		nTrigType;
		char	szModeName[256];
		char	szDatetime[64];
		int		nModeId;
		float	fExpTime;
	};

	//Reference status and expiration duration (unit: minute) of special application mode
	struct CR_RefStatusInfo
	{
		int nDarkStatus;
		int nGainStatus;
		int nDefectStatus;
		int nDarkMinsLeft;
		int nGainMinsLeft;
		int nDefectMinsLeft;
	};


	//start-by-ts.wang
	/*
	 * Structure used to read detector file header
	 *
	 */

	struct CR_RAD_DetectorFileHeader
	{
		char typeCode[8];
		int imageSize;
		int imgxSize;
		int imgySize;
		int pixelBits;
		int compressType;
		int frameNum;
		int correctWay;
		int correctPara;
		int isolatedPixels;
		int isolatedPixelspos;
		int horibadlines;
		int horibadlinespos;
		int vertbadlines;
		int vertbadlinespos;
		int clusterpixels;
		int clusterpixelspos;
	};

	struct CR_RAD_DetectorInfoHeader
	{
		int startflag;
		int endiantype;
		char dttver[32];
		int dtttype;
		int dttnum;
		int modeid;
		int imgprop;
		char buildtime[24];
		float vbias;// 4 bytes float
		float vcmp;// 4 bytes floatdynam_range
		float vtft_on;// 4 bytes float
		float vtft_off;// 4 bytes float
		float dynam_range;
		float pga_dac;
		float expos_time;
		float delay_time;
		float wait_time;
		float tempLT;
		float tempRT;
		float tempLB;
		float tempRB;
		float tempM;
		float reserver11[3];
		char machineID[32];
		float Reserved12[49];
		int fpga_reg_version[4];
		int build_date;
		int res000;
		int int_reg_copy;
		int int_mask;
		int expos_control;
		int fpga_status;
		int cycle_count;
		int adc_rsl;

		int	res024[15];
		float fpga_dynam_range;
		float fpga_pga_dac;
		float fpga_pga_dac_control;
		float fpga_vbias;
		float fpga_vcmp;
		float fpga_vtft_on;
		float fpga_vtft_off;
		float fpga_expos_time;
		int res084;
		float fpga_delay_time;
		int res08c;
		float fpga_wait_time;
		int res094;
		int pixel_num;
		int line_num;
		int readout_cntr;
		int scan_line;
		int res0a8[2];
		int x_start;
		int x_end;
		int y_start;
		int y_end;
		int res0c0[16];
		int fifo_max;
		int fifo_thrsh;
		int fifo_count;
		int endflag;
	};

	typedef struct CR_ActiveAreaInfo
	{
		int x;
		int y;
		int width;
		int height;
		float weight;
	}CR_ActiveAreaInfo;

#define DTT_INFO_HEADER_OFFSET 16384
#define PCI_IMG_HEAD_OFFSET	32768
	//end-by-ts.wang

	//typedef void(*CR_CallbackFunPtr)(int, CR_Event *);
}
#endif