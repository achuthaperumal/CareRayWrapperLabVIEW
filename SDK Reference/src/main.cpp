#include "CareRayAPIDemo.h"

int main()
{
	cout << "**********************************************************************" << endl;
	cout << "**************************** CareRay API Demo ************************" << endl;
	cout << "**********************************************************************" << endl;

	char szMenu1[] = {'L', 'R', 'G', 'E', 'U', 'M', 'N', 'S', 'F' };
	char szMenu2[] = {'L', 'R', 'A', 'G', 'E', 'I', 'U', 'M', 'N', 'S', 'F' };
	char szMenu3[] = {'L', 'R', 'A', 'C', 'D', 'G', 'E', 'M', 'N', 'S', 'F' };
	vector<char> vecMenuItemIdx1(szMenu1, szMenu1 + sizeof(szMenu1)/sizeof(szMenu1[0]));
	vector<char> vecMenuItemIdx2(szMenu2, szMenu2 + sizeof(szMenu2)/sizeof(szMenu2[0]));
	vector<char> vecMenuItemIdx3(szMenu3, szMenu3 + sizeof(szMenu3)/sizeof(szMenu3[0]));

	map<char, string> mapMenu;
	mapMenu['I'] = "Initialize Library";
	mapMenu['U'] = "DeInitialize Library";
	mapMenu['L'] = "List supported application modes";
	mapMenu['R'] = "Register application modes and manage calibration files";
	mapMenu['A'] = "Acquire fluoroscopic images";
	mapMenu['C'] = "Acquire a radiographic image";
	mapMenu['D'] = "Acquire a dark radiographic image";
	mapMenu['G'] = "Get connection status";
	mapMenu['M'] = "Get DAEC parameters";
	mapMenu['N'] = "Set DAEC parameters";
	mapMenu['S'] = "Generate sample custom mode config file";
	mapMenu['F'] = "Register custom mode from config file";
	mapMenu['E'] = "Exit";

	CCareRayAPIDemo* pCareRayAPIDemo = new CCareRayAPIDemo();
	if (NULL != pCareRayAPIDemo)
	{
		int nRet = pCareRayAPIDemo->Initialize();
		vector<int> vecDetrIdxs;
		pCareRayAPIDemo->GetDetrIdxs(&vecDetrIdxs);

		if (CR_OK == nRet)
		{
			for(vector<int>::iterator iter = vecDetrIdxs.begin(); iter != vecDetrIdxs.end(); ++iter)
			{
				pCareRayAPIDemo->SetCurrDetrIdx(*iter);
				cout << "**********************************************************************" << endl << endl;
			}

			int nControlFlag = 0;
			bool bFlag = true;
			do 
			{
				char chOption = 0;
				cout << endl << "Manipulating detector " << pCareRayAPIDemo->GetCurrDetrIdx() << ".";
				cout << endl << "\t=============== Main Menu ===============" << endl;					

				if (0 == nControlFlag)
				{
					for (vector<char>::iterator it = vecMenuItemIdx1.begin(); it != vecMenuItemIdx1.end(); ++it)
					{
						cout <<"\t"<<*it<<": "<<mapMenu[*it]<<endl;
					}
				}
				else
				{
					vector<char> vecMenuItemIdx = (pCareRayAPIDemo->IsRadiographicModeRegistered() ? vecMenuItemIdx3 : vecMenuItemIdx2)	;
					for (vector<char>::iterator it = vecMenuItemIdx.begin(); it != vecMenuItemIdx.end(); ++it)
					{
						cout <<"\t"<<*it<<": "<<mapMenu[*it]<<endl;
					}
				}

				cout <<endl<< "Your selection is >> ";
				cin >> chOption;
				cout << endl;
				pCareRayAPIDemo->checkInput(chOption);
				chOption = toupper(chOption);
				if (0 == nControlFlag && vecMenuItemIdx1.end() == std::find(vecMenuItemIdx1.begin(), vecMenuItemIdx1.end(), chOption))
				{
					cout<<"Invalid input"<<endl;
					continue;
				}

				switch (chOption)
				{
				case 'L':
					pCareRayAPIDemo->PrintModeList(pCareRayAPIDemo->GetCurrDetrIdx());
					break;
				case 'R':
					nRet = pCareRayAPIDemo->RegisterAppModes();
					if (GetReturnCode(nRet) <= CR_WARN)
					{
						nControlFlag++;
					}
					break;
				case 'G':
					nRet = pCareRayAPIDemo->GetConnectionStatus();
					break;
				case 'A':
					nRet = pCareRayAPIDemo->Acquisition();
					break;
				case 'E':
					bFlag = false;
					break;
				case 'C':
					nRet = pCareRayAPIDemo->AcquireRadImage();
					break;
				case 'D':
					nRet = pCareRayAPIDemo->AcquireDarkImage();
					break;
				case 'I':
					nRet = pCareRayAPIDemo->InitializeLibrary();
					if (GetReturnCode(nRet) <= CR_WARN)
					{
						nRet = pCareRayAPIDemo->Initialize();
					}
					break;
				case 'U':
					nRet = pCareRayAPIDemo->DeInitialize();
					if (GetReturnCode(nRet) <= CR_WARN)
					{
						nRet = pCareRayAPIDemo->DeInitializeLibrary();
					}
					break;
				case 'M':
					nRet = pCareRayAPIDemo->GetDaecParams();
					break;
				case 'N':
					nRet = pCareRayAPIDemo->SetDaecParams();
					break;
				case 'S':
					pCareRayAPIDemo->GenerateSampleCustomModeConfigFile();
					break;
				case 'F':
					pCareRayAPIDemo->RegisterCustomModeFromConfigFile();
					break;
				default:
					cout<<"Invalid input"<<endl;
					break;
				}
			} while (bFlag);

			if (CR_OK == nRet)
			{
				nRet = pCareRayAPIDemo->DeInitialize();
			}

			delete pCareRayAPIDemo;
			pCareRayAPIDemo = NULL;
		}
		getchar();
	}

	return 0;
}