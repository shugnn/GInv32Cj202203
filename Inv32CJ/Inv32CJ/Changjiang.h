#include"Data_Struct.h"

class   CChangjiang
{
public:
	CChangjiang();
	~CChangjiang();
	BOOL Login();
	BOOL Inqury(INQURYMODE inqryMode);
	BOOL Declare(vector<DECLARE> &vDeclares);
	VOID Logout();
private:
	VOID ClearTradingApp();
	HWND FindWin(PCHAR clsName, PCHAR winText, LONG ctrlId, PCHAR subWinText, BOOL byTidPid, BOOL byRoot, HWND rootHwnd);
	HWND FindPopup(PCHAR clsName, PCHAR winText, PCHAR sonCtrlText, LONG sonCtrlId, BOOL byTidPid, HWND parentHwnd);
	HWND FindCtrl(PCHAR clsName, PCHAR winText, LONG ctrlId, HWND parentHwnd);
	VOID ClickButton(HWND hwnd);
	VOID ClickImgIcon(PCHAR imgIcon);
	BOOL CallRecImage(LPRECT lpBox,CHAR* ptext);
	VOID GetMoney(HWND hwnd, MONEY &money);
	VOID GetStock(HWND hwnd, map<string, STOCK>&mstocks);
	VOID GetTodTraded(HWND hwnd, multimap<string, TODTRADED>&mtodtradeds);
	VOID GetHisTraded(HWND hwnd, map<string, STOCK>&mtempinqrys, multimap<string, HISTRADED>&mhistradeds);
	VOID GetTodEntrust(HWND hwnd, multimap<string, TODENTRUST>&mtodentrusts);
	VOID SetText(HWND hwnd, const CHAR* winText);
	VOID GetInfoByProcess(HWND hwnd, DWORD processId, WPARAM wparam, LPARAM lparam);
	VOID SetInfoByProcess(HWND hwnd, DWORD processId, WPARAM wparam, LPARAM lparam);
	BOOL SaveGet(HWND hwnd, vector<map<string, string>>&vmstrss);
	VOID ClickForActivate(HWND hwnd);
private:
	LPWNDINFO LPWNDINFOChangjiang;


};