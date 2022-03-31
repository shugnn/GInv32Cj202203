#include"Changjiang.h"
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
using namespace std;
using namespace cv;
extern ACCOUNT account;
CChangjiang *pCCJ;
VOID GetWinInfo(HWND hwnd, LPWNDINFO lpWndInfo)
{
	lpWndInfo->hwnd = hwnd;
	GetWindowText(hwnd, lpWndInfo->winText, sizeof(lpWndInfo->winText));
	GetClassName(hwnd, lpWndInfo->clsName, sizeof(lpWndInfo->clsName));
	lpWndInfo->threadId = GetWindowThreadProcessId(hwnd, &lpWndInfo->processId);
	lpWndInfo->parentHwnd = GetParent(hwnd);
	lpWndInfo->ctrlId = GetWindowLong(hwnd, GWL_ID); 
	lpWndInfo->style = GetWindowLong(hwnd, GWL_STYLE);
}
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	WNDINFO info;
	GetWinInfo(hwnd, &info);
	vector<WNDINFO> *vinfos = (vector<WNDINFO>*)lParam;
	vinfos->push_back(info);
	return true;
}
CChangjiang::CChangjiang()
{
	LPWNDINFOChangjiang = new WNDINFO();
}
CChangjiang::~CChangjiang()
{
	delete LPWNDINFOChangjiang;
	LPWNDINFOChangjiang = NULL;
}
BOOL CChangjiang::Login()
{
	ClearTradingApp();
	//ShellExecute(NULL, "open", exeDirChangjiangXiadan.c_str(), NULL, NULL, SW_SHOWNORMAL);
	TCHAR pathExeCJxiadan[MAX_PATH]; ResolveShortcut(L"xiadanLink.lnk", pathExeCJxiadan);
	WinExec(pathExeCJxiadan, SW_NORMAL);
	HWND loginHwnd = NULL;
	short trys = 0;
	while (!(loginHwnd = FindWin("#32770", "�û���¼", 0, "��ӭ���ʳ���֤ȯ��վ", false, false, NULL)))
	{
		if (++trys >= 10)return false;
		Sleep(2 * 1000);
	}
	GetWinInfo(loginHwnd, LPWNDINFOChangjiang);
	HWND ctrlHwnd = NULL;
	if (ctrlHwnd = FindCtrl("Edit", NULL, 0x3eb, loginHwnd))
	{
		HWND boxHwnd = FindCtrl("Static", NULL, 0x05db, loginHwnd);
		RECT box = { 0, 0, 0, 0 };
		if (GetWindowRect(boxHwnd, &box))
		{
			CHAR text[16];
			if (CallRecImage(&box, text))
				SetText(ctrlHwnd, text);
		}			
	}
	if (ctrlHwnd = FindCtrl("Edit", NULL, 0x3f4, loginHwnd))
		SetText(ctrlHwnd, "237603"); Sleep(100);
	if (ctrlHwnd = FindCtrl("ComboBox", NULL, 0x3f3, loginHwnd))
		SetText(ctrlHwnd, "48024137"); Sleep(100);
	if (ctrlHwnd = FindCtrl("Button", NULL, 0x3ee, loginHwnd))
		ClickButton(ctrlHwnd); Sleep(100);
	HWND traderHwnd = NULL; 
	trys = 0;
	while (!(traderHwnd = FindWin("Afx:400000", "���Ϲ�Ʊ����ϵͳ5.0", 0, NULL, true, false, NULL)))
	{
		if (++trys >= 10)return false;
		Sleep(2 * 1000);
	}
	Sleep(10 * 1000);
	GetWinInfo(traderHwnd, LPWNDINFOChangjiang);
	HWND popupHwnd = NULL;
	while (popupHwnd = FindPopup("#32770", NULL, "��ʾ", 0x0555, true, traderHwnd))
	{
		PostMessage(popupHwnd, WM_CLOSE, 0, 0);
		Sleep(1 * 1000);
	}	
	ShowWindow(traderHwnd, SW_HIDE);
	return true;
}
VOID CChangjiang::Logout()
{
	if (LPWNDINFOChangjiang)
		PostMessage(LPWNDINFOChangjiang->hwnd, WM_CLOSE, 0, 0);
	Sleep(3 * 1000);
}
VOID CChangjiang::ClearTradingApp()
{
	HWND hwnd = NULL;
	while (hwnd = FindWin("Afx:400000", "���Ϲ�Ʊ����ϵͳ5.0", 0, NULL, false, false, NULL))
	{
		PostMessage(hwnd, WM_CLOSE, 0, 0); 
		Sleep(3 * 1000);
	}
	while (hwnd = FindWin("#32770", "�û���¼", 0, "��ӭ���ʳ���֤ȯ��վ", false, false, NULL))
	{
		PostMessage(hwnd, WM_CLOSE, 0, 0);
		Sleep(3 * 1000);
	}
}
BOOL CChangjiang::Inqury(INQURYMODE inqryMode)
{
	HWND hwnd = LPWNDINFOChangjiang->hwnd;
	ShowWindow(hwnd, SW_NORMAL); SetForegroundWindow(hwnd); Sleep(100);
	keybd_event(VK_F4, 0, 0, 0);
	keybd_event(VK_F4, 0, KEYEVENTF_KEYUP, 0);
	Sleep(1 * 1000);
	GetMoney(hwnd, account.money);
	GetStock(hwnd, account.mstocks);
	if (Inqry_Morning == inqryMode)
	{
		if (!account.mstocks.empty())
		{
			GetTodTraded(hwnd, account.mtodtradeds);
			map<string, STOCK>mtempinqrys(account.mstocks);
			for (auto itodtraded = account.mtodtradeds.begin(); itodtraded != account.mtodtradeds.end(); ++itodtraded)
			{
				auto itemp = mtempinqrys.find(itodtraded->first);
				if (mtempinqrys.end() != itemp)
				{
					if (itodtraded->second.operate == "֤ȯ����")
						itemp->second.stkBalance -= itodtraded->second.trdVolume;
					else if (itodtraded->second.operate == "֤ȯ����")
						itemp->second.stkBalance += itodtraded->second.trdVolume;
				}
			}
			for (auto itempinqry = mtempinqrys.begin(); itempinqry != mtempinqrys.end();)
			{
				if (itempinqry->second.stkBalance <= 0)
					itempinqry = mtempinqrys.erase(itempinqry);
				else ++itempinqry;
			}
			if (!mtempinqrys.empty())
				GetHisTraded(hwnd, mtempinqrys, account.mhistradeds);
		}
	}
	else if (Inqry_Midtime == inqryMode)
	{
		GetTodEntrust(hwnd, account.mtodentrusts);
	}
	else if (Inqry_Afternoon == inqryMode)
	{
		GetTodTraded(hwnd, account.mtodtradeds);
	}
	ShowWindow(hwnd, SW_HIDE);
	return true;
}
BOOL CChangjiang::Declare(vector<DECLARE> &vDeclares)
{
	HWND hwnd = LPWNDINFOChangjiang->hwnd;
	ShowWindow(hwnd, SW_NORMAL); SetForegroundWindow(hwnd); Sleep(100);
	for (auto iDeclare = vDeclares.begin(); iDeclare != vDeclares.end(); ++iDeclare)
	{
		string soperate, scode, sprice, svolume;
		soperate = iDeclare->operate;
		scode = iDeclare->codes;
		stringstream ss;
		ss.clear(); ss << iDeclare->declarePrice; sprice = ss.str(); ss.str("");
		ss.clear(); ss << iDeclare->declareVolume; svolume = ss.str(); ss.str("");
		HWND panelHwnd = NULL, editCtrlCodeHwnd = NULL, editCtrlPriceHwnd = NULL, editCtrlVolumeHwnd = NULL, buttonCtrlHwnd = NULL,
			popupHwnd = NULL, staticCtrlHwnd = NULL;
		if (soperate == string("��") || soperate == string("BUY"))
		{
			keybd_event(VK_F1, 0, 0, 0);
			keybd_event(VK_F1, 0, KEYEVENTF_KEYUP, 0);
			Sleep(1 * 1000);
			if (panelHwnd = FindWin("#32770", NULL, 0xe901, "�����Ʊ", true, true, hwnd))
			{
				if ((editCtrlCodeHwnd = FindCtrl("Edit", NULL, 0x0408, panelHwnd)) &&
					(editCtrlPriceHwnd = FindCtrl("Edit", NULL, 0x0409, panelHwnd)) &&
					(editCtrlVolumeHwnd = FindCtrl("Edit", NULL, 0x040a, panelHwnd)) &&
					(buttonCtrlHwnd = FindCtrl("Button", "����[B]", 0x03ee, panelHwnd)))
				{
					SetText(editCtrlCodeHwnd, scode.c_str()); Sleep(500);
					SetText(editCtrlPriceHwnd, sprice.c_str()); Sleep(500);
					SetText(editCtrlVolumeHwnd, svolume.c_str()); Sleep(500);
					keybd_event(VK_CONTROL, 0, 0, 0); Sleep(100);
					keybd_event('B', 0, 0, 0); Sleep(200);
					keybd_event('B', 0, KEYEVENTF_KEYUP, 0); Sleep(100);
					keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
					Sleep(3 * 1000);
				}
			}
		}
		else if (soperate == string("��") || soperate == string("SEL"))
		{
			keybd_event(VK_F2, 0, 0, 0);
			keybd_event(VK_F2, 0, KEYEVENTF_KEYUP, 0);
			Sleep(1 * 1000);
			if (panelHwnd = FindWin("#32770", NULL, 0xe901, "������Ʊ", true, true, hwnd))
			{
				if ((editCtrlCodeHwnd = FindCtrl("Edit", NULL, 0x0408, panelHwnd)) &&
					(editCtrlPriceHwnd = FindCtrl("Edit", NULL, 0x0409, panelHwnd)) &&
					(editCtrlVolumeHwnd = FindCtrl("Edit", NULL, 0x040a, panelHwnd)) &&
					(buttonCtrlHwnd = FindCtrl("Button", "����[S]", 0x03ee, panelHwnd)))
				{
					SetText(editCtrlCodeHwnd, scode.c_str()); Sleep(500);
					SetText(editCtrlPriceHwnd, sprice.c_str()); Sleep(500);
					SetText(editCtrlVolumeHwnd, svolume.c_str()); Sleep(500);
					keybd_event(VK_CONTROL, 0, 0, 0); Sleep(100);
					keybd_event('S', 0, 0, 0); Sleep(200);
					keybd_event('S', 0, KEYEVENTF_KEYUP, 0); Sleep(100);
					keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0); 
					Sleep(3 * 1000);
				}
			}
		}
		while (popupHwnd = FindPopup("#32770", NULL, NULL, 0, true, hwnd))
		{
			if (FindCtrl("Static", "ί��ȷ��", 0x0555, popupHwnd) &&
				(buttonCtrlHwnd = FindCtrl("Button", "��(&Y)", 0x0006, popupHwnd)))
			{
				ClickButton(buttonCtrlHwnd);
			}
			else if (FindCtrl("Static", "��ʾ", 0x0555, popupHwnd) &&
				(staticCtrlHwnd = FindCtrl("Static", NULL, 0x03ec, popupHwnd)) &&
				(buttonCtrlHwnd = FindCtrl("Button", "ȷ��", 0x0002, popupHwnd)))
			{
				CHAR text[1024];
				//GetInfoByProcess(staticCtrlHwnd, LPWNDINFOChangjiang->processId, (WPARAM)&len, (LPARAM)text);			
				SendMessage(staticCtrlHwnd, WM_GETTEXT, sizeof(text), (LPARAM)text);
				if (GetCoding((unsigned char*)text, strlen(text)) == UTF8)
					strcpy(text, UTF8ToGB(text).c_str());
				Strip(text, "\n\r");
				ss.clear(); ss << soperate << scode << "Prc" << sprice << "Vol" << svolume << "��ʾ:" << text;
				cout << ss.str() << endl;
				strcpy(iDeclare->rslt, ss.str().c_str()); ss.str("");
				ClickButton(buttonCtrlHwnd);
			}
			else if (FindCtrl("Static", "��ʾ��Ϣ", 0x0555, popupHwnd) &&
				(staticCtrlHwnd = FindCtrl("Static", "�Ƿ����", 0x0410, popupHwnd)) &&
				(buttonCtrlHwnd = FindCtrl("Button", "��(&N)", 0x0007, popupHwnd)))
			{
				CHAR text[1024];
				//GetWindowText(staticCtrlHwnd, text, sizeof(text));				
				SendMessage(staticCtrlHwnd, WM_GETTEXT, sizeof(text), (LPARAM)text);
				if (GetCoding((unsigned char*)text, strlen(text)) == UTF8)
					strcpy(text, UTF8ToGB(text).c_str());
				Strip(text, "\n\r");
				ss.clear(); ss << soperate << scode << "Prc" << sprice << "Vol" << svolume << "��ʾ��Ϣ:" << text;
				cout << ss.str() << endl;
				strcpy(iDeclare->rslt, ss.str().c_str()); ss.str("");
				ClickButton(buttonCtrlHwnd);
			}
			//Sleep(1 * 1000);
			//PostMessage(popupHwnd, WM_CLOSE, 0, 0);
			Sleep(5 * 1000);
		}
	}
	ShowWindow(hwnd, SW_HIDE);
	return true;
}
HWND CChangjiang::FindWin(PCHAR clsName, PCHAR winText, LONG ctrlId, PCHAR subWinText, BOOL byTidPid, BOOL byRoot, HWND rootHwnd)
{
	vector<WNDINFO> vinfos; vinfos.clear();
	if (!byRoot)
		EnumWindows(EnumWindowsProc, (LPARAM)&vinfos);
	else if (rootHwnd)
		EnumChildWindows(rootHwnd, EnumWindowsProc, (LPARAM)&vinfos);
	if (vinfos.empty())return NULL;
	for (auto iinfo = vinfos.begin(); iinfo != vinfos.end(); ++iinfo)
	{
		if (!(WS_VISIBLE & iinfo->style) || (WS_DISABLED & iinfo->style))continue;
		if (clsName && string(iinfo->clsName).find(clsName) == string::npos)continue;
		if (winText && string(iinfo->winText).find(winText) == string::npos)continue;
		if (ctrlId > 0 && ctrlId != iinfo->ctrlId)continue;
		if (byTidPid && (LPWNDINFOChangjiang->threadId != iinfo->threadId || LPWNDINFOChangjiang->processId != iinfo->processId))continue;
		if (subWinText)
		{
			vector<WNDINFO> vchildinfos; vchildinfos.clear();
			EnumChildWindows(iinfo->hwnd, EnumWindowsProc, (LPARAM)&vchildinfos);
			bool found = false;
			for (auto ichildinfo = vchildinfos.begin(); ichildinfo != vchildinfos.end(); ++ichildinfo)
			{
				if (string(ichildinfo->winText).find(subWinText) != string::npos)
				{
					found = true;
					break;
				}
			}
			if (!found)continue;
		}
		return iinfo->hwnd;
	}
	return NULL;
}
HWND CChangjiang::FindPopup(PCHAR clsName, PCHAR winText, PCHAR sonCtrlText, LONG sonCtrlId, BOOL byTidPid, HWND parentHwnd)
{
	vector<WNDINFO> vinfos; vinfos.clear();
	EnumWindows(EnumWindowsProc, (LPARAM)&vinfos);
	if (vinfos.empty())return NULL;
	for (auto iinfo = vinfos.begin(); iinfo != vinfos.end(); ++iinfo)
	{
		if (!(WS_POPUP & iinfo->style))continue;
		if (!(WS_VISIBLE & iinfo->style) || (WS_DISABLED & iinfo->style))continue;
		if (clsName && string(iinfo->clsName).find(clsName) == string::npos)continue;
		if (winText && string(iinfo->winText).find(winText) == string::npos)continue;
		if (byTidPid && (LPWNDINFOChangjiang->threadId != iinfo->threadId || LPWNDINFOChangjiang->processId != iinfo->processId))continue;
		if (parentHwnd && parentHwnd != iinfo->parentHwnd)continue;
		if (sonCtrlText && sonCtrlId>0)
		{
			vector<WNDINFO> vchildinfos; vchildinfos.clear();
			EnumChildWindows(iinfo->hwnd, EnumWindowsProc, (LPARAM)&vchildinfos);
			bool found = false;
			for (auto ichildinfo = vchildinfos.begin(); ichildinfo != vchildinfos.end(); ++ichildinfo)
			{
				if (ichildinfo->parentHwnd != iinfo->hwnd)continue;
				if (string(ichildinfo->winText).find(sonCtrlText) != string::npos && ichildinfo->ctrlId == sonCtrlId)
				{
					found = true;
					break;
				}
			}
			if (!found)continue;
		}
		return iinfo->hwnd;
	}
	return NULL;
}
HWND CChangjiang::FindCtrl(PCHAR clsName, PCHAR winText, LONG ctrlId, HWND parentHwnd)
{
	vector<WNDINFO> vinfos; vinfos.clear();
	if (parentHwnd)
		EnumChildWindows(parentHwnd, EnumWindowsProc, (LPARAM)&vinfos);
	else
		EnumWindows(EnumWindowsProc, (LPARAM)&vinfos);
	if (vinfos.empty())return NULL;
	for (auto iinfo = vinfos.begin(); iinfo != vinfos.end(); ++iinfo)
	{
		if (!(WS_VISIBLE & iinfo->style) || (WS_DISABLED & iinfo->style))continue;
		if (clsName && string(iinfo->clsName).find(clsName) == string::npos)continue;
		if (winText && string(iinfo->winText).find(winText) == string::npos)continue;
		if (ctrlId > 0 && ctrlId != iinfo->ctrlId)continue;
		if (parentHwnd && parentHwnd != iinfo->parentHwnd)continue;
		if (LPWNDINFOChangjiang->threadId != iinfo->threadId || LPWNDINFOChangjiang->processId != iinfo->processId)continue;
		return iinfo->hwnd;
	}
	return NULL;
}
VOID CChangjiang::SetText(HWND hwnd, const CHAR* winText)
{
	ShowWindow(hwnd, SW_NORMAL); SetForegroundWindow(hwnd); Sleep(100);
	SendMessage(hwnd, WM_SETTEXT, 0, NULL); Sleep(100);
	SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)winText); Sleep(100);
}
BOOL CChangjiang::CallRecImage(LPRECT lpBox, CHAR* ptext)
{
	string exeCmd("ImageRecChangjiang.exe"); 
	exeCmd.append(" ").append(to_string(lpBox->left))
		.append(" ").append(to_string(lpBox->top))
		.append(" ").append(to_string(lpBox->right))
		.append(" ").append(to_string(lpBox->bottom));
	FILE* pp = _popen(exeCmd.c_str(), "r");
	if (NULL != pp)
	{
		fgets(ptext, 16, pp);
		short n = strlen(ptext);
		while (--n>=0 && (ptext[n] == '\n' || ptext[n] == '\r'))
			ptext[n] = 0;
		_pclose(pp);
		return true;
	}
	return false;
}
VOID CChangjiang::GetMoney(HWND hwnd, MONEY &money)
{
	HWND panelHwnd = NULL;
	if(panelHwnd=FindWin("#32770", NULL, 0xe901, "��ѯ�ʽ��Ʊ", true, true, hwnd))
	{
		CHAR text[16];
		HWND ctrlHwnd = NULL;
		if (ctrlHwnd = FindCtrl("Static", NULL, 0x03f7, panelHwnd))
		{
			GetWindowText(ctrlHwnd, text, 16);
			money.totalAssets = atof(text);
		}
		if (ctrlHwnd = FindCtrl("Static", NULL, 0x03f4, panelHwnd))
		{
			GetWindowText(ctrlHwnd, text, 16);
			money.balance = atof(text);
		}
		if (ctrlHwnd = FindCtrl("Static", NULL, 0x03f8, panelHwnd))
		{
			GetWindowText(ctrlHwnd, text, 16);
			money.amountAvailable = atof(text);
		}
		if (ctrlHwnd = FindCtrl("Static", NULL, 0x03f9, panelHwnd))
		{
			GetWindowText(ctrlHwnd, text, 16);
			money.amountDesirable = atof(text);
		}
		if (ctrlHwnd = FindCtrl("Static", NULL, 0x03f6, panelHwnd))
		{
			GetWindowText(ctrlHwnd, text, 16);
			money.marketValue = atof(text);
		}
		if (ctrlHwnd = FindCtrl("Static", NULL, 0x0403, panelHwnd))
		{
			GetWindowText(ctrlHwnd, text, 16);
			money.holdProfit = atof(text);
		}
	}
}
VOID CChangjiang::GetStock(HWND hwnd, map<string, STOCK>&mstocks)
{
	//const map<string, string>mStockDicts = {
	//	{ "֤ȯ����", "codes" }, { "֤ȯ����", "stkName" }, { "��Ʊ���", "stkBalance" }, { "�������", "stkAvailable" },
	//	{ "�ɱ���", "costPrice" }, { "�м�", "marketPrice" }, { "ӯ������(%)", "profitPercent" }, { "ӯ��", "profit" },
	//	{ "��ֵ", "marketValue" }, { "�г�����", "marketCode" }, { "�����г�", "marketName" }, { "�ɶ��ʻ�", "accountId" } };
	HWND treeHwnd = NULL;
	if (treeHwnd = FindWin("Afx:400000:0", "HexinScrollWnd", 0x0081, "HexinScrollWnd2",true, true, hwnd))
	{
		ClickImgIcon("inqryIcon/zijingupiao.png");
		Sleep(1 * 1000);
		HWND gridHwnd = NULL;
		if (gridHwnd = FindWin("Afx:400000:0", "HexinScrollWnd", 0x0417, "HexinScrollWnd2", true, true, hwnd))
		{
			vector<map<string, string>>vmstrss;
			if (SaveGet(gridHwnd, vmstrss))
			{
				for (auto mstrs = vmstrss.begin(); mstrs != vmstrss.end(); ++mstrs)
				{
					STOCK stock;
					stock.codes = mstrs->at("֤ȯ����");
					stock.stkName = mstrs->at("֤ȯ����");
					stock.stkBalance = atoi(mstrs->at("��Ʊ���").c_str());
					stock.stkAvailable = atoi(mstrs->at("�������").c_str());
					stock.costPrice = atof(mstrs->at("�ɱ���").c_str());
					stock.marketPrice = atof(mstrs->at("�м�").c_str());
					stock.profitPercent = atof(mstrs->at("ӯ������(%)").c_str());
					stock.profit = atof(mstrs->at("ӯ��").c_str());
					stock.marketValue = atof(mstrs->at("��ֵ").c_str());
					stock.marketCode = mstrs->at("�г�����");
					stock.marketName = mstrs->at("�����г�");
					stock.accountId = mstrs->at("�ɶ��ʻ�");
					mstocks.insert(make_pair(stock.codes, stock));
				}
			}
		}
	}
}
VOID CChangjiang::GetTodTraded(HWND hwnd, multimap<string, TODTRADED>&mtodtradeds)
{
	HWND treeHwnd = NULL;
	if (treeHwnd = FindWin("Afx:400000:0", "HexinScrollWnd", 0x0081, "HexinScrollWnd2", true, true, hwnd))
	{
		ClickImgIcon("inqryIcon/dangrichengjiao.png");
		Sleep(1 * 1000);
		HWND gridHwnd = NULL;
		if (gridHwnd = FindWin("Afx:400000:0", "HexinScrollWnd", 0x0417, "HexinScrollWnd2", true, true, hwnd))
		{
			vector<map<string, string>>vmstrss;
			if (SaveGet(gridHwnd, vmstrss))
			{
				for (auto mstrs = vmstrss.begin(); mstrs != vmstrss.end(); ++mstrs)
				{
					TODTRADED todtraded;
					todtraded.trdTime = mstrs->at("�ɽ�ʱ��");
					todtraded.codes = mstrs->at("֤ȯ����");
					todtraded.stkName = mstrs->at("֤ȯ����");
					todtraded.operate = mstrs->at("����");
					todtraded.trdVolume = atoi(mstrs->at("�ɽ�����").c_str());
					todtraded.trdPrice = atof(mstrs->at("�ɽ�����").c_str());
					todtraded.trdAmount = atof(mstrs->at("�ɽ����").c_str());
					todtraded.entrustId = mstrs->at("ί�б��");
					todtraded.trdId = mstrs->at("�ɽ����");
					todtraded.businessName = mstrs->at("ҵ������");
					todtraded.retreatVolume = atoi(mstrs->at("��������").c_str());
					todtraded.declareId = mstrs->at("�걨���");
					mtodtradeds.insert(make_pair(todtraded.codes, todtraded));
				}
			}
		}
	}
}
VOID CChangjiang::GetHisTraded(HWND hwnd, map<string, STOCK>&mtempinqrys, multimap<string, HISTRADED>&mhistradeds)
{	
	HWND treeHwnd = NULL;
	if (treeHwnd = FindWin("Afx:400000:0", "HexinScrollWnd", 0x0081, "HexinScrollWnd2", true, true, hwnd))
	{
		ClickImgIcon("inqryIcon/lishichengjiao.png");
		Sleep(1 * 1000);
		HWND pickerWrapperHwnd = NULL;
		if (pickerWrapperHwnd = FindWin("", "", 0xe901, "", true, true, hwnd))
		{
			HWND timeCtrlBegHwnd = NULL, timeCtrlEndHwnd = NULL, gridCtrlHwnd = NULL, buttonCtrlHwnd = NULL;
			if ((timeCtrlBegHwnd = FindCtrl("SysDateTimePick32", NULL, 0x3f1, pickerWrapperHwnd)) &&
				(timeCtrlEndHwnd = FindCtrl("SysDateTimePick32", NULL, 0x3f2, pickerWrapperHwnd)) &&
				(buttonCtrlHwnd = FindCtrl("Button", "ȷ��", 0x3ee, pickerWrapperHwnd)) &&
				(gridCtrlHwnd = FindCtrl("Afx:400000:0", "HexinScrollWnd", 0x0417, pickerWrapperHwnd)))
			{
				SYSTEMTIME sysTimeEnd, sysTimeBeg;
				GetLocalTime(&sysTimeEnd);
				SysTimeDelta(sysTimeEnd, -30 * 24 * 60 * 60, sysTimeBeg);
				int trys = 0;
				while (!mtempinqrys.empty())
				{
					SetInfoByProcess(timeCtrlBegHwnd, LPWNDINFOChangjiang->processId, (WPARAM)sizeof(sysTimeBeg), (LPARAM)&sysTimeBeg);
					SetInfoByProcess(timeCtrlEndHwnd, LPWNDINFOChangjiang->processId, (WPARAM)sizeof(sysTimeEnd), (LPARAM)&sysTimeEnd);
					//HANDLE hProcess = NULL;
					//if (hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, LPWNDINFOChangjiang->processId))
					//{
					//	void *pAddr = NULL;
					//	if (pAddr = VirtualAllocEx(hProcess, NULL, sizeof(sysTimeEnd), MEM_COMMIT, PAGE_READWRITE))
					//	{
					//		if (WriteProcessMemory(hProcess, pAddr, &sysTimeBeg, sizeof(sysTimeEnd), NULL))
					//		{
					//			SendMessage(timeCtrlBegHwnd, DTM_SETSYSTEMTIME, 0, (LPARAM)pAddr);
					//		}
					//		if (WriteProcessMemory(hProcess, pAddr, &sysTimeEnd, sizeof(sysTimeEnd), NULL))
					//		{
					//			SendMessage(timeCtrlEndHwnd, DTM_SETSYSTEMTIME, 0, (LPARAM)pAddr);
					//		}
					//		VirtualFreeEx(hProcess, pAddr, 0, MEM_RELEASE);
					//	}
					//	CloseHandle(hProcess);
					//}
					ClickButton(buttonCtrlHwnd);
					Sleep(1 * 1000);
					vector<map<string, string>>vmstrss;
					if (SaveGet(gridCtrlHwnd, vmstrss))
					{
						for (auto mstrs = vmstrss.begin(); mstrs != vmstrss.end(); ++mstrs)
						{
							auto itempinqry = mtempinqrys.find(mstrs->at("֤ȯ����"));
							if (mtempinqrys.end() == itempinqry)continue;
							HISTRADED histraded;
							histraded.trdDate = mstrs->at("�ɽ�����");
							histraded.trdTime = mstrs->at("�ɽ�ʱ��");
							histraded.codes = mstrs->at("֤ȯ����");
							histraded.stkName = mstrs->at("֤ȯ����");
							histraded.operate = mstrs->at("����");
							histraded.trdVolume = atoi(mstrs->at("�ɽ�����").c_str());
							histraded.trdPrice = atof(mstrs->at("�ɽ�����").c_str());
							histraded.trdAmount = atof(mstrs->at("�ɽ����").c_str());
							histraded.contactId = mstrs->at("��ͬ���");
							histraded.trdId = mstrs->at("�ɽ����");
							histraded.commission = atof(mstrs->at("������").c_str());
							histraded.tax = atof(mstrs->at("ӡ��˰").c_str());
							histraded.feeOther = atof(mstrs->at("�����ӷ�").c_str());
							histraded.amountIncurred = atof(mstrs->at("�������").c_str());
							histraded.businessName = mstrs->at("ҵ������");
							histraded.remarks = mstrs->at("��ע");
							histraded.marketName = mstrs->at("�����г�");
							histraded.accountId = mstrs->at("�ɶ��ʻ�");
							mhistradeds.insert(make_pair(histraded.codes, histraded));
							if ((histraded.operate == string("��")) || (histraded.operate == string("֤ȯ����")))
								itempinqry->second.stkBalance += histraded.trdVolume;
							if ((histraded.operate == string("��")) || (histraded.operate == string("֤ȯ����")))
								itempinqry->second.stkBalance -= histraded.trdVolume;
							if (itempinqry->second.stkBalance <= 0)
								mtempinqrys.erase(itempinqry);
						}
					}
					if (++trys > 10) break;
					SysTimeDelta(sysTimeBeg, -1 * 24 * 60 * 60, sysTimeEnd);
					SysTimeDelta(sysTimeEnd, -30 * 24 * 60 * 60, sysTimeBeg);
				}
			}
		}
	}
}
VOID CChangjiang::GetTodEntrust(HWND hwnd, multimap<string, TODENTRUST>&mtodentrusts)
{
	HWND treeHwnd = NULL;
	if (treeHwnd = FindWin("Afx:400000:0", "HexinScrollWnd", 0x0081, "HexinScrollWnd2", true, true, hwnd))
	{
		ClickImgIcon("inqryIcon/dangriweituo.png");
		Sleep(1 * 1000);
		HWND gridHwnd = NULL;
		if (gridHwnd = FindWin("Afx:400000:0", "HexinScrollWnd", 0x0417, "HexinScrollWnd2", true, true, hwnd))
		{
			vector<map<string, string>>vmstrss;
			if (SaveGet(gridHwnd, vmstrss))
			{
				for (auto mstrs = vmstrss.begin(); mstrs != vmstrss.end(); ++mstrs)
				{
					{
						TODENTRUST todentrust;
						todentrust.entrustTime = mstrs->at("ί��ʱ��");
						todentrust.codes = mstrs->at("֤ȯ����");
						todentrust.stkName = mstrs->at("֤ȯ����");
						todentrust.operate = mstrs->at("����");
						todentrust.remarks = mstrs->at("��ע");
						todentrust.entrustVolume = atoi(mstrs->at("ί������").c_str());
						todentrust.trdVolume = atoi(mstrs->at("�ɽ�����").c_str());
						todentrust.retreatVolume = atoi(mstrs->at("��������").c_str());
						todentrust.entrustPrice_protectPrice = atof(mstrs->at("ί�м۸�/�����޼�").c_str());
						todentrust.isProtectPrice = mstrs->at("�Ƿ��Ǳ����޼�") == string("��") ? true : false;
						todentrust.trdPrice = atof(mstrs->at("�ɽ�����").c_str());
						todentrust.businessName = mstrs->at("ҵ������");
						todentrust.entrustId = mstrs->at("ί�б��");
						todentrust.declareId = mstrs->at("�걨���");
						mtodentrusts.insert(make_pair(todentrust.codes, todentrust));
					}
				}
			}
		}
	}
}
BOOL CChangjiang::SaveGet(HWND hwnd, vector<map<string, string>>&vmstrss)
{
	ClickForActivate(hwnd);
	keybd_event(VK_CONTROL, 0, 0, 0); Sleep(100);
	keybd_event('S', 0, 0, 0); Sleep(200);
	keybd_event('S', 0, KEYEVENTF_KEYUP, 0); Sleep(100);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0); Sleep(1 * 1000);
	HWND saveHwnd = NULL;
	if (saveHwnd = FindPopup(NULL, "���Ϊ", NULL, 0, true, NULL))
	{
		SetForegroundWindow(saveHwnd); 
		HWND ctrlHwnd = NULL;
		if (ctrlHwnd = FindCtrl("Edit", NULL, 0x0480, saveHwnd))
		{
			char dirCur[MAX_PATH]; getcwd(dirCur, MAX_PATH);
			time_t seconds; time(&seconds); stringstream ss; ss.clear();
			ss << dirCur << "\\" << seconds << ".xls"; string fileName = ss.str(); ss.str("");
			SetText(ctrlHwnd, fileName.c_str());
			HWND buttonHwnd = NULL;
			if (buttonHwnd = FindCtrl("Button", "����", 0x0001, saveHwnd))
			{
				ClickButton(buttonHwnd); 
				Sleep(1 * 1000);
				vmstrss.clear();
				ifstream infile(fileName.c_str(), ios::in);
				if (infile.is_open())
				{
					vector<string>vstrs, header;
					string line;
					while (getline(infile, line))
					{
						vstrs.clear();
						split(line, vstrs, '\t');
						if (vstrs.empty())continue;
						if (header.empty())
							header = vstrs;
						else if (vstrs.size()==header.size())
						{
							map<string, string>mstrs; mstrs.clear();
							for (auto ivalue = vstrs.begin(), ikey = header.begin(); ivalue != vstrs.end(); ++ivalue, ++ikey)
							{
								string key = *ikey; key.erase(0, key.find_first_not_of("=\"\b\n\t ")); key.erase(key.find_last_not_of("=\"\b\n\t") + 1);
								string value = *ivalue; value.erase(0, value.find_first_not_of("=\"\b\n\t ")); value.erase(value.find_last_not_of("=\"\b\n\t") + 1);
								mstrs.insert(make_pair(key, value));
							}
							if (!mstrs.empty())
								vmstrss.push_back(mstrs);
						}
					}
					infile.close();
					remove(fileName.c_str());
				}
				if (!vmstrss.empty())return true;
			}
		}
	}
	return false;
}
VOID CChangjiang::ClickButton(HWND hwnd)
{
	ShowWindow(hwnd, SW_NORMAL); SetForegroundWindow(hwnd); Sleep(100);
	SendMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, NULL);
	SendMessage(hwnd, WM_LBUTTONUP, MK_LBUTTON, NULL);
}
VOID CChangjiang::ClickImgIcon(PCHAR imgIcon)
{
	HDC hScreen = CreateDC("DISPLAY", NULL, NULL, NULL);
	HDC	hCompDC = CreateCompatibleDC(hScreen);
	int nWidth = GetSystemMetrics(SM_CXSCREEN);
	int nHeight = GetSystemMetrics(SM_CYSCREEN);
	HBITMAP hBmp = CreateCompatibleBitmap(hScreen, nWidth, nHeight);
	HBITMAP hOld = (HBITMAP)SelectObject(hCompDC, hBmp);
	BitBlt(hCompDC, 0, 0, nWidth, nHeight, hScreen, 0, 0, SRCCOPY);
	SelectObject(hCompDC, hOld);
	DeleteDC(hScreen);
	DeleteDC(hCompDC);
	
	BITMAP bmp;
	GetObject(hBmp, sizeof(BITMAP), &bmp);
	int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
	Mat src;
	src.create(cvSize(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8U, nChannels));
	GetBitmapBits(hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, src.data);
	DeleteObject(hBmp);
	DeleteObject(hOld);
	imwrite("tempScreen.png", src);	
	//char   buffer[MAX_PATH];
	//getcwd(buffer, MAX_PATH);
	Mat obj = imread(imgIcon, 1);
	//imshow("obj", obj); waitKey(2 * 1000);
	src = imread("tempScreen.png", 1);
	Mat rst;
	double minVal, maxVal;
	Point minLoc, maxLoc, matchLoc;
	matchTemplate(src, obj, rst, 1);//��6��ƥ�䷽ʽ
	normalize(rst, rst, 1, 0, NORM_MINMAX);
	minMaxLoc(rst, &minVal, &maxVal, &minLoc, &maxLoc);// �ҵ����ƥ���
	matchLoc.x = minLoc.x + (int)(obj.cols *0.5);
	matchLoc.y = minLoc.y + (int)(obj.rows *0.5);
	//cout << "matchLoc " << matchLoc << " minLoc " << minLoc << " maxLoc " << maxLoc << endl;
	//Mat ftmp[6];
	//for (int i = 0; i < 6; i++)
	//{
	//	Mat src = imread("tempScreen.png", 1);
	//	imshow("title", src); waitKey(2 * 1000);
	//	matchTemplate(src, obj, ftmp[i], i);//��6��ƥ�䷽ʽ
	//	normalize(ftmp[i], ftmp[i], 1, 0, NORM_MINMAX);
	//	minMaxLoc(ftmp[i], &minVal, &maxVal, &minLoc, &maxLoc);// �ҵ����ƥ���
	//	if (i<2)
	//		rectangle(src, Rect(minLoc.x, minLoc.y, obj.cols, obj.rows), 1, 8, 0);//��һ�д��벢�����ƣ���Ϊ�е�ƥ�䷽��������Сֵ��Ϊ���ƥ���
	//	else
	//		rectangle(src, Rect(maxLoc.x, maxLoc.y, obj.cols, obj.rows), 1, 8, 0);
	//	cout << i << " minLoc " << minLoc << " maxLoc " << maxLoc << endl;
	//	char name[32];
	//	sprintf(name, "NNN%d", i);
	//	imshow(name, src);
	//	waitKey(2 * 1000);
	//}

	SetCursorPos(matchLoc.x, matchLoc.y);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	Sleep(10);//Ҫ����ĳЩӦ�õķ�Ӧʱ�� 
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	remove("tempScreen.png");
}
VOID CChangjiang::ClickForActivate(HWND hwnd)
{
	ShowWindow(hwnd, SW_NORMAL); SetForegroundWindow(hwnd); Sleep(100);
	RECT box = { 0, 0, 0, 0 };
	if (GetWindowRect(hwnd, &box))
	{
		SetCursorPos(int(0.5*box.left + 0.5*box.right), int(0.5*box.top + 0.5*box.bottom));
		mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		//Sleep(10);//Ҫ����ĳЩӦ�õķ�Ӧʱ�� 
		//mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(1 * 1000);
	}
}
VOID CChangjiang::GetInfoByProcess(HWND hwnd, DWORD processId, WPARAM wparam, LPARAM lparam)
{
	HANDLE hProcess = NULL;
	if (hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, processId))
	{
		wparam = SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0) + 1;
		void *pAddr = NULL;
		if (pAddr = VirtualAllocEx(hProcess, NULL, wparam, MEM_COMMIT, PAGE_READWRITE))
		{
			if (WriteProcessMemory(hProcess, pAddr, (LPCVOID)lparam, wparam, NULL))
			{
				SendMessage(hwnd, WM_GETTEXT, wparam, (LPARAM)pAddr);
				ReadProcessMemory(hProcess, pAddr, (LPVOID)lparam, wparam, NULL);
			}
			VirtualFreeEx(hProcess, pAddr, 0, MEM_RELEASE);
		}
		CloseHandle(hProcess);
	}
	////���ھ��
	//HWND hwnd=NULL;    //Ŀ�����ID����
	//DWORD PID;
	//HANDLE hProcess;
	//int nColumnCount=0;     //����
	//int nRowCount=0;        //����
	//int nColumnIndex=0;     //������
	//int nRowIndex=0;        //������
	//int nBufferLength=1024; //��������С
	//LVITEM lvitem;    
	//wchar_t *buffer  =new wchar_t[nBufferLength];   //UNICODE 
	//wmemset(buffer,0,nBufferLength);    //Զ������ռ��ַ 
	//LVITEM  *pVirtualItem;    
	//wchar_t *pVirtualBuffer;    
	//hwnd=::FindWindow(NULL,_T("ListDemo"));    
	//if(hwnd)    
	//{        
	//	hwnd=::FindWindowEx(hwnd,NULL,_T("SysListView32"),NULL);
	//}    
	//if(!hwnd)    
	//{       
	//	cout<<(L"Ŀ�����û�д�!"); 
	//	return;   
	//}  
	//GetWindowThreadProcessId(hwnd,&PID); 
	//hProcess=OpenProcess(PROCESS_ALL_ACCESS,false,PID);	//��ȡĿ����̾��ʧ��    
	//if(!hProcess)   
	//	return; 
	//HWND hLVHeader=(HWND)::SendMessage(hwnd,LVM_GETHEADER,0,0);     //��ȡListView��ͷ���
	//nColumnCount=(int)::SendMessage(hLVHeader,HDM_GETITEMCOUNT,0,0); //��ȡListView���� 
	//nRowCount   =(int)::SendMessage(hwnd,LVM_GETITEMCOUNT,0,0);      //��ȡListView����        
	////��Ŀ����̵�ַ�ռ�����ڴ� 
	//pVirtualItem  =(LVITEM  *)VirtualAllocEx(hProcess, NULL, sizeof(LVITEM) , MEM_COMMIT, PAGE_READWRITE);    
	//pVirtualBuffer=(wchar_t *)VirtualAllocEx(hProcess, NULL, nBufferLength*2, MEM_COMMIT, PAGE_READWRITE);    
	//if ((!pVirtualItem)||(!pVirtualBuffer))    
	//{        
	//	MessageBox(L"�ڴ����ʧ��!");   
	//	return;   
	//}    
	//CFile file;    
	//file.Open(L"listdata.txt",CFile::modeCreate|CFile::modeReadWrite); 
	//for(int i=0;i<nRowCount;i++)   
	//{        
	//	for(int j=0;j<nColumnCount;j++)    
	//	{            
	//		nColumnIndex=i;           
	//		nRowIndex=j;         
	//		lvitem.cchTextMax=nBufferLength*2;   
	//		lvitem.iSubItem=nRowIndex;           
	//		lvitem.pszText=pVirtualBuffer;            
	//		//���ļ���:��LVITEM�ṹ�����Ŀ�������,Ȼ��ͨ��LVM_GETITEMTEXT��Ϣ��ȡָ������ı�    
	//		WriteProcessMemory(hProcess, pVirtualItem, &lvitem, sizeof(LVITEM), NULL);   
	//		::SendMessage(hwnd, LVM_GETITEMTEXT, (WPARAM)nColumnIndex, (LPARAM)pVirtualItem); 
	//		ReadProcessMemory(hProcess, pVirtualBuffer, buffer, nBufferLength*2,NULL);           
	//		//ת����ANSI���� 
	//		char *ansiBuffer=ShunW2A(buffer);          
	//		file.Write(ansiBuffer,(UINT)strlen(ansiBuffer));   
	//		file.Write("/t",1);      
	//		delete []ansiBuffer;        
	//	}        
	//	file.Write("/r/n",2);   
	//}    
	//file.Close();  
	////�ͷ�Ŀ������������ڴ�
	//VirtualFreeEx(hProcess,pVirtualItem  ,sizeof(LVITEM) ,MEM_RELEASE);  
	//VirtualFreeEx(hProcess,pVirtualBuffer,nBufferLength*2,MEM_RELEASE);   
	//CloseHandle(hProcess);   
	//delete []buffer;
}
VOID CChangjiang::SetInfoByProcess(HWND hwnd, DWORD processId, WPARAM wparam, LPARAM lparam)
{
	HANDLE hProcess = NULL;
	if (hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, processId))
	{
		void *pAddr = NULL;
		if (pAddr = VirtualAllocEx(hProcess, NULL, (SIZE_T)wparam, MEM_COMMIT, PAGE_READWRITE))
		{
			if (WriteProcessMemory(hProcess, pAddr, (LPCVOID)lparam, (SIZE_T)wparam, NULL))
			{
				SendMessage(hwnd, DTM_SETSYSTEMTIME, 0, (LPARAM)pAddr);
			}
			VirtualFreeEx(hProcess, pAddr, 0, MEM_RELEASE);
		}
		CloseHandle(hProcess);
	}
}
