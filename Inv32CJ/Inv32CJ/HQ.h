#include"Data_Struct.h"
typedef unsigned char byte;
typedef bool(__stdcall*  TdxHq_ConnectDelegate)(char* IP, int Port, char* Result, char* ErrInfo);
typedef void(__stdcall* TdxHq_DisconnectDelegate)();
typedef bool(__stdcall* TdxHq_GetSecurityBarsDelegate)(byte Category, byte Market,
	char* Zqdm, short Start, short& Count, char* Result, char* ErrInfo);
typedef bool(__stdcall* TdxHq_GetSecurityCountDelegate)(byte Market, short& Result, char* ErrInfo);
typedef bool(__stdcall* TdxHq_GetSecurityListDelegate)(byte Market, short Start, short& Count, char* Result, char* ErrInfo);
typedef bool(__stdcall* TdxHq_GetIndexBarsDelegate)(byte Category, byte Market, char* Zqdm,
	short Start, short& Count, char* Result, char* ErrInfo);
typedef bool(__stdcall* TdxHq_GetMinuteTimeDataDelegate)(byte Market, char* Zqdm, char* Result, char* ErrInfo);
typedef bool(__stdcall* TdxHq_GetHistoryMinuteTimeDataDelegate)(byte Market, char* Zqdm, int Date, char* Result, char* ErrInfo);
typedef bool(__stdcall* TdxHq_GetTransactionDataDelegate) (byte Market, char* Zqdm, short Start, short& Count, char* Result, char* ErrInfo);
typedef bool(__stdcall* TdxHq_GetHistoryTransactionDataDelegate) (byte Market, char* Zqdm, short Start, short& Count, int Date, char* Result, char* ErrInfo);
typedef bool(__stdcall* TdxHq_GetSecurityQuotesDelegate) (byte Market[], char* Zqdm[], short& Count, char* Result, char* ErrInfo);
typedef bool(__stdcall* TdxHq_GetCompanyInfoCategoryDelegate) (byte Market, char* Zqdm, char* Result, char* ErrInfo);
typedef bool(__stdcall* TdxHq_GetCompanyInfoContentDelegate) (byte Market, char* Zqdm, char* FileName, int Start, int Length, char* Result, char* ErrInfo);
typedef bool(__stdcall* TdxHq_GetXDXRInfoDelegate) (byte Market, char* Zqdm, char* Result, char* ErrInfo);
typedef bool(__stdcall* TdxHq_GetFinanceInfoDelegate) (byte Market, char* Zqdm, char* Result, char* ErrInfo);

class CHQ 
{
public:
	CHQ();
	~CHQ();
	bool Connect();
	void Disconnect();
	int GetSecurityCount();
	void GetSecurityList(vector<Scu>&vscus);
	short GetHisMinDatas(short imkt);
	bool GetTodHisTransactionData(vector<Scu>cvscus, const unsigned int date);
	void GetCompanyInfoCategory(const vector<Scu>cvscus, const unsigned int date);
	void GetCompanyInfoContentA(Scu *scu, Category *cate, CompanyInfo &comp);
	void GetCompanyInfoContentB(Scu *scu, Category *cate, CompanyInfo &comp);
	void DataWrite(const vector<Receiv> *cvreceivs, unsigned int date);
	void CompanyInfoWrite(const vector<CompanyInfo>*vcompinfs, unsigned int date);
	void CheckDatasDate(unsigned int &date);
	bool CheckDatasToday();
	unsigned int TradingDay;
private:
	HMODULE TdxApiHMODULE;
	TdxHq_ConnectDelegate TdxHq_Connect;
	TdxHq_DisconnectDelegate TdxHq_Disconnect;
	TdxHq_GetSecurityCountDelegate TdxHq_GetSecurityCount;
	TdxHq_GetSecurityListDelegate TdxHq_GetSecurityList;
	TdxHq_GetSecurityBarsDelegate TdxHq_GetSecurityBars;
	TdxHq_GetIndexBarsDelegate TdxHq_GetIndexBars;
	TdxHq_GetMinuteTimeDataDelegate TdxHq_GetMinuteTimeData;
	TdxHq_GetHistoryMinuteTimeDataDelegate TdxHq_GetHistoryMinuteTimeData;
	TdxHq_GetTransactionDataDelegate TdxHq_GetTransactionData;
	TdxHq_GetHistoryTransactionDataDelegate TdxHq_GetHistoryTransactionData;
	TdxHq_GetSecurityQuotesDelegate TdxHq_GetSecurityQuotes;
	TdxHq_GetCompanyInfoCategoryDelegate TdxHq_GetCompanyInfoCategory;
	TdxHq_GetCompanyInfoContentDelegate TdxHq_GetCompanyInfoContent;
	TdxHq_GetXDXRInfoDelegate TdxHq_GetXDXRInfo;
	TdxHq_GetFinanceInfoDelegate TdxHq_GetFinanceInfo;
	char* ErrInfo;
};
