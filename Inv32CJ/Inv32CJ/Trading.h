#include"Data_Struct.h"
class   CTra
{
public:	
	HWND consolHwnd;
	vector<DECLARE>vDeclares;
	map<string, int>mSints;
private:
	vector<CANDIDATE>vCandidates;
	vector<HOLD>vHolds;
	map<string,TICKMARKET>mSTickMarckets;
	int dclIdLocal;
public:
	CTra();
	~CTra();
	void Run();
	void UpdateAccountHoldCandidate();
private:
	bool OpenTeminalCJ();
	void FetchAccount();
	bool ConnectMarket();
	void Supervise();
	void GetCandidate();
	void MarketQuate();
	void CleanOpsition();
	void OpenOpsition();
	void FreshAccount();
	void DisConnectMarket();
	void TidyAccount();
	void CloseTeminalCJ();
	void ReadAccountHold();
	void WriteAccountHold();
	int CalcHoldDaysOnCalendar(string dateYYYYMMDD);
	TdxHq_ConnectDelegate TdxHq_Connect;
	TdxHq_GetSecurityQuotesDelegate TdxHq_GetSecurityQuotes;
	TdxHq_DisconnectDelegate TdxHq_Disconnect;
};
DWORD WINAPI threadTeminalCJ(LPVOID lpParam);
