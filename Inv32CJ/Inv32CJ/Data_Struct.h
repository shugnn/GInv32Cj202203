#if !defined(SERVER_DATATYPE_STRUCT_H)
#define SERVER_DATATYPE_STRUCT_H
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
# pragma warning (disable:4819)
#pragma warning(disable : 4996)

//#ifndef VC_EXTRALEAN
//#define VC_EXTRALEAN // 从 Windows 头中排除极少使用的资料
//#endif
//#include <afxwin.h> // MFC core and standard components #include <afxext.h> // MFC extensions 
//#include <afxdisp.h> // MFC Automation classes 
//#include <afxdtctl.h> // MFC support for Internet Explorer 4 Common Controls #ifndef _AFX_NO_AFXCMN_SUPPORT 
//#include <afxcmn.h> // MFC support for Windows Common Controls #endif // _AFX_NO_AFXCMN_SUPPORT 
//#include<commctrl.h>
#include<time.h>
#include <ctime>
#include <stdlib.h>
#include<process.h>
#include<direct.h>
#include<io.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include<iostream>
#include <fstream>
#include <vector>
#include<list>
#include<set>
#include<map>
#include<deque>
#include <algorithm>
#include "json\json.h"
#include <Python.h>
#include<opencv2\opencv.hpp>
#include <windows.h>
#include <ShObjIdl.h>
#include <strsafe.h>
using namespace std;
#define DTM_FIRST        0x1000
#define DTM_SETSYSTEMTIME   (DTM_FIRST + 2)

#define TYPES 4
#define DECAS 5
#define PREDAY 45
#define HISSDAY 10
#define FUTURE 5
#define MULT (int)floor(pow(48 * PREDAY / 3, 1.0 / (DECAS - 1)))
#define CLIMB max(min(3.0*FUTURE, 30.0), 10.0)
#define DROP -min(0.5*CLIMB, 9.0)
#define dirMktDatas "dirMktDatas"
#define dirRulLearn "dirRulLearn"
#define dirRstTrade "dirRstTrade"


typedef struct tagMONEY
{
	double totalAssets;//总资产
	double balance;// 资金余额
	double amountAvailable;//可用金额
	double amountDesirable;//可取金额
	double marketValue;//股票市值
	double holdProfit;//持仓盈亏
}MONEY, *LPMONEY;
typedef struct tagSTOCK
{
	string codes;
	string stkName;
	int stkBalance;
	int stkAvailable;
	double costPrice;
	double marketPrice;
	double profitPercent;
	double profit;
	double marketValue;
	string marketCode;
	string marketName;
	string accountId;
}STOCK, *LPSTOCK;
typedef struct tagTODTRADED
{
	string trdTime;
	string codes;
	string stkName;
	string operate;
	int trdVolume;
	double trdPrice;
	double trdAmount;
	string entrustId;
	string trdId;
	string businessName;
	int retreatVolume;
	string declareId;
}TODTRADED, *LPTODTRADED;
typedef struct tagTODENTRUST
{
	string entrustTime;
	string codes;
	string stkName;
	string operate;
	string remarks;
	int entrustVolume;
	int trdVolume;
	int retreatVolume;
	double entrustPrice_protectPrice;
	bool isProtectPrice;
	double trdPrice;
	string businessName;
	string entrustId;
	string declareId;
}TODENTRUST, *LPTODENTRUST;
typedef struct tagHISTRADED
{
	string trdDate;
	string trdTime;
	string codes;
	string stkName;
	string operate;
	int trdVolume;
	double trdPrice;
	double trdAmount;
	string contactId;
	string trdId;
	double commission;
	double tax;
	double feeOther;
	double amountIncurred;
	string businessName;
	string remarks;
	string marketName;
	string accountId;
}HISTRADED, *LPHISTRADED;
typedef struct tagHISENTRUST
{
	string entrustDate;
	string declareTime;
	string codes;
	string stkName;
	string operate;
	string entrustStatus;
	int entrustVolume;
	int trdVolume;
	double entrustPrice;
	int retreatVolume;
	double trdPrice;
	string contactId;
	string marketName;
	string accountId;
	string entrustSubBusiness;
	string orderId;
	string reciprocalAccount;
	double referenceRate;
	string settlementCurrency;
	string transactionCurrency;
	string stkCnName;
	string errorInfo;
}HISENTRUST, *LPHISENTRUST;
typedef struct tagTICKMARKET
{
	double priceMarket;
	double priceYest;
	double priceOpen;
	double priceHigh;
	double priceLow;
	string tickTime;
	int volumeTotal;
	int volumeMarket;
	double amountTotal;
	double priceBid1;
	double priceAsk1;
	int volumeBid1;
	int volumeAsk1;
	double priceBid2;
	double priceAsk2;
	int volumeBid2;
	int volumeAsk2;
	double priceBid3;
	double priceAsk3;
	int volumeBid3;
	int volumeAsk3;
	double priceBid4;
	double priceAsk4;
	int volumeBid4;
	int volumeAsk4;
	double priceBid5;
	double priceAsk5;
	int volumeBid5;
	int volumeAsk5;
}TICKMARKET, *LPTICKMARKET;
typedef struct tagCANDIDATE
{
	string codes;
	string stkName;
	int prefer;
	double climb;
	double drop;

	bool isDeclaring;
}CANDIDATE, *LPCANDIDATE;
typedef struct tagHOLD
{
	STOCK stock;
	string trdDateLast;
	int holdDays;
	double holdHighestPrice;
	double holdLowestPrice;
	string closeDate;
	double closePrice;
	int closeVolume;
	bool isDeclaring;
	bool updated;
	CHAR remark[1024];
}HOLD, *LPHOLD;
typedef struct tagDECLARE
{
	string declareTime;
	string codes;
	string stkName;
	string operate;
	double declarePrice;
	int declareVolume;
	int dclId;
	bool declared;
	CHAR rslt[1024];
}DECLARE, *LPDECLARE;
typedef struct tagACCOUNT
{
	string date;
	MONEY money;
	map<string, STOCK>mstocks;
	multimap<string, TODTRADED>mtodtradeds;
	multimap<string, TODENTRUST>mtodentrusts;
	multimap<string, HISTRADED>mhistradeds;
	multimap<string, HISENTRUST>mhisentrusts;
	//tagACCOUNT()
	//{
	//	date = "";
	//	ZeroMemory(&money, sizeof(money));
	//	mstocks.clear();
	//	mtodtradeds.clear();
	//	mtodentrusts.clear();
	//	mhistradeds.clear();
	//	mhisentrusts.clear();
	//}
}ACCOUNT, *LPACCOUNT;
typedef enum 
{
	Call_Inqry,
	Call_Declare,
	Call_Null
}CJCALLTYPE;
typedef enum 
{
	Inqry_Morning,
	Inqry_Midtime,
	Inqry_Afternoon,
	Inqry_Null
}INQURYMODE;
struct THREADREF
{
	CJCALLTYPE callType;
	INQURYMODE inqMode;
	vector<DECLARE>vDclares;
	THREADREF()
	{
		callType = Call_Null;
		inqMode = Inqry_Null;
		vDclares.clear();
	}
};
typedef struct tagWNDINFO
{
	HWND hwnd;
	DWORD processId;
	DWORD threadId;
	LONG ctrlId;
	CHAR clsName[64];
	CHAR winText[256];
	HWND parentHwnd;
	LONG style;
}WNDINFO, *LPWNDINFO;

typedef enum {
	Time_Stop,
	Time_Pause,
	Time_Monitor,
	Time_Clean,
	Time_Candidate,
	Time_Open,
	Time_MorningMarket,
	Time_HalfMarket,
	Time_AlldayMarket
}TRADINGTIME;
inline TRADINGTIME TradingTime()
{
	SYSTEMTIME sys_time; GetLocalTime(&sys_time);
	int dayofweek = (int)sys_time.wDayOfWeek;
	if (dayofweek == 6 || dayofweek == 0)return Time_Stop;
	double tm = (double)((sys_time.wHour) / 10e1) + (double)((sys_time.wMinute) / 10e3)
		+ (double)((sys_time.wSecond) / 10e5);
	if (0.0905 <= tm  && tm < 0.0925)return Time_Pause;
	if (0.0925 <= tm  && tm < 0.1125)return Time_Monitor;
	if (0.1125 <= tm  && tm < 0.1130)return Time_Clean;
	if (0.1130 <= tm  && tm < 0.1230)return Time_Pause;
	if (0.1230 <= tm  && tm < 0.1300)return Time_Candidate;
	if (0.1300 <= tm  && tm < 0.1305)return Time_Open;
	if (0.1305 <= tm  && tm < 0.1500)return Time_Monitor;
	if (0.1500 <= tm  && tm < 0.1510)return Time_Pause;
	return Time_Stop;
}
inline TRADINGTIME Marketingtime()
{
	SYSTEMTIME sys_time; GetLocalTime(&sys_time);
	int dayofweek = (int)sys_time.wDayOfWeek;
	if (dayofweek == 6 || dayofweek == 0)return Time_Stop;
	double tm = (double)((sys_time.wHour) / 10e1) + (double)((sys_time.wMinute) / 10e3)
		+ (double)((sys_time.wSecond) / 10e5);
	if (0.0830 <= tm  && tm <= 0.1133)return  Time_MorningMarket;
	if (0.1133 <= tm  && tm <= 0.1400)return  Time_HalfMarket;
	if (0.1733 <= tm  && tm <= 0.1900)return  Time_AlldayMarket;
	return Time_Stop;
}
inline unsigned int SysDate()
{
	SYSTEMTIME sys_time; GetLocalTime(&sys_time);
	return (unsigned int)(sys_time.wYear * 10000 + sys_time.wMonth * 100 + sys_time.wDay);
}
inline string TimeHMSYMD()
{
	time_t timep;
	time(&timep);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%H:%M:%S %Y-%m-%d", localtime(&timep));
	return tmp;
}
inline string TimeYMD()
{
	time_t timep;
	time(&timep);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y%m%d", localtime(&timep));
	return tmp;
}
inline string TimeHMS()
{
	time_t timep;
	time(&timep);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%H:%M:%S", localtime(&timep));
	return tmp;
}

inline void SysTimeDelta(const SYSTEMTIME &sysTmOrigin, int secDelta, SYSTEMTIME &sysTmObject)
{
	struct tm tmStruct = { sysTmOrigin.wSecond, sysTmOrigin.wMinute, sysTmOrigin.wHour, sysTmOrigin.wDay, sysTmOrigin.wMonth - 1, sysTmOrigin.wYear - 1900, sysTmOrigin.wDayOfWeek, 0, 0 };
	time_t timeDelta = mktime(&tmStruct) + secDelta;
	tmStruct = *localtime(&timeDelta);
	sysTmObject = { 1900 + tmStruct.tm_year, 1 + tmStruct.tm_mon, tmStruct.tm_wday, tmStruct.tm_mday, tmStruct.tm_hour, tmStruct.tm_min, tmStruct.tm_sec, 0 };
}
template<typename T>
inline void split(const std::string& word, T& vec, char delim)
{
	std::stringstream ss(word);
	std::string tmp;
	while (std::getline(ss, tmp, delim))
	{
		vec.push_back(tmp);
	}
}
inline void Strip(CHAR* text, const char* delim)
{
	string strs = text;
	for (size_t i = 0; i < strlen(delim); ++i)
	{
		string::size_type p;
		while (string::npos != (p = strs.find(delim[i])))
			strs.erase(p, 1);
	}
	strcpy(text, strs.c_str());
}
inline void GetCalendar(deque<unsigned int>&calendar)
{
	calendar.clear();
	string fn = string(dirMktDatas) + "\\calendarTillNow.txt";
	ifstream infile(fn, ios::in);
	if (infile.is_open())
	{
		char line[64];
		while (infile.getline(line,sizeof(line)))
		{
			unsigned int date = (unsigned int)atol(string(line).c_str());
			if (20150101 <= date && date <= 20301231)
				calendar.push_back(date);
		}
		infile.close(); 
		sort(calendar.begin(), calendar.end());
	}
	//_finddata_t info; intptr_t handle = _findfirst((string("F:\\ScDatas\\HisFZ5\\*.5")).c_str(), &info);
	_finddata_t info; intptr_t handle = _findfirst((string(dirMktDatas) + string("\\daily\\*.5")).c_str(), &info);
	if (handle > 0) 
	{
		do
		{
			if (info.attrib == _A_SUBDIR)continue;
			string dn(info.name);
			if (dn.substr(dn.length() - 2, 2) == string(".5"))
			{
				unsigned int date = (unsigned int)atol(dn.substr(0, 8).c_str());
				if (date<20150101 || 20301231<date)continue;
				if (!calendar.empty() && (calendar.front() <= date && date <= calendar.back()))continue;
				calendar.push_back((date));
			}
		} while (_findnext(handle, &info) >= 0);
		_findclose(handle);
		sort(calendar.begin(), calendar.end());
	}
	ofstream outfile(fn, ios::out | ios::trunc);
	if (outfile.is_open())
	{
		for (auto it = calendar.begin(); it != calendar.end(); ++it)
			outfile << *it << endl;
		//outfile.write((char*)&calendar.front(), calendar.size() * sizeof(unsigned int));
		outfile.close();
	}
}
typedef struct tagHOST
{
	const char *pszName;
	const char *pszHost;
	int nPort;
}HOST,*LPHOST;

typedef bool(*fn1)(unsigned int);
typedef bool(*fn2)(unsigned int, unsigned int);
typedef bool(__stdcall*  TdxHq_ConnectDelegate)(char* IP, int Port, char* Result, char* ErrInfo);
typedef bool(__stdcall* TdxHq_GetSecurityQuotesDelegate) (byte Market[], char* Zqdm[], short& Count, char* Result, char* ErrInfo);
typedef void(__stdcall* TdxHq_DisconnectDelegate)();

inline string UTF8ToGB(const char* str)
{
	string result;
	WCHAR *strSrc;
	LPSTR szRes;

	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

	//获得临时变量的大小
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new CHAR[i + 1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

	result = szRes;
	delete[]strSrc;
	delete[]szRes;

	return result;
}
inline string GBKToUTF8(const char* strGBK)
{
	int len = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, strGBK, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	string strTemp = str;
	if (wstr) delete[] wstr;
	if (str) delete[] str;
	return strTemp;
}
inline int preNUm(unsigned char byte) {
	unsigned char mask = 0x80;
	int num = 0;
	for (int i = 0; i < 8; i++) {
		if ((byte & mask) == mask) {
			mask = mask >> 1;
			num++;
		}
		else {
			break;
		}
	}
	return num;
}
inline bool isUtf8(unsigned char* data, int len) {
	int num = 0;
	int i = 0;
	while (i < len) {
		if ((data[i] & 0x80) == 0x00) {
			// 0XXX_XXXX
			i++;
			continue;
		}
		else if ((num = preNUm(data[i])) > 2) {
			// 110X_XXXX 10XX_XXXX
			// 1110_XXXX 10XX_XXXX 10XX_XXXX
			// 1111_0XXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
			// 1111_10XX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
			// 1111_110X 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
			// preNUm() 返回首个字节8个bits中首 ?0bit前面1bit的个数，该数量也是该字符所使用的字节数        
			i++;
			for (int j = 0; j < num - 1; j++) {
				//判断后面num - 1 个字节是不是都是10开
				if ((data[i] & 0xc0) != 0x80) {
					return false;
				}
				i++;
			}
		}
		else {
			//其他情况说明不是utf-8
			return false;
		}
	}
	return true;
}
inline bool isGBK(unsigned char* data, int len)  {
	int i = 0;
	while (i < len)  {
		if (data[i] <= 0x7f) {
			//编码小于等于127,只有一个字节的编码，兼容ASCII
			i++;
			continue;
		}
		else {
			//大于127的使用双字节编码
			if (data[i] >= 0x81 &&
				data[i] <= 0xfe &&
				data[i + 1] >= 0x40 &&
				data[i + 1] <= 0xfe &&
				data[i + 1] != 0xf7) {
				i += 2;
				continue;
			}
			else {
				return false;
			}
		}
	}
	return true;
}
typedef enum {
	GBK,
	UTF8,
	UNKOWN
} CODING;
//需要说明的是，isGBK()是通过双字节是否落在gbk的编码范围内实现的，
//而utf-8编码格式的每个字节都是落在gbk的编码范围内 ?
//所以只有先调用isUtf8()先判断不是utf-8编码，再调用isGBK()才有意义
inline CODING GetCoding(unsigned char* data, int len) {
	CODING coding;
	if (isUtf8(data, len) == true) {
		coding = UTF8;
	}
	else if (isGBK(data, len) == true) {
		coding = GBK;
	}
	else {
		coding = UNKOWN;
	}
	return coding;
}
inline void ResolveShortcut(LPCOLESTR wChar, PTCHAR szPath)
{
	CoInitialize(NULL);IShellLink *psl;
	if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&psl))))
	{
		IPersistFile *ppf;
		if (SUCCEEDED(psl->QueryInterface(&ppf)))
		{
			if (SUCCEEDED(ppf->Load(wChar, STGM_READ)))
				psl->GetPath(szPath, MAX_PATH, NULL, SLGP_RAWPATH);
			ppf->Release();
		}
		psl->Release();
	}
}

typedef char ScCode[8];
typedef char ScName[16];
struct Scu
{
	short imkt;
	short type;//0:zhuban,1:chuangyeban;2:jijin;3:zhishu
	ScCode code;
	ScName name;
};
struct Fram
{
	Scu scu;
	double current[PREDAY + 1];
	double sormnys, sorchgs;
	double cnt[TYPES][DECAS];
	float fit;
	int clsd;
	float posopn, poshig, poscls;
};
struct Tick
{
	char time[6];
	double pric;
	unsigned int amou;//sz;lots;idx:money;
	unsigned int tiks;//sz:bishu;sh gupiao;0;
	short drec;
	//int resv;
};
struct Fz
{
	unsigned short dt;//2:year:(int)x/2048+2014,mon:mod(x,2048)/100,day:mod(mod(x,2048),100)
	unsigned short tm;//2:575-900,9:05-15:00
	unsigned int op;//4:*100
	unsigned int hg;//4:*100
	unsigned int lw;//4:*100
	unsigned int cl;//4:*100
	unsigned int vm;//4:lot*100
	unsigned int mn;
	//int rs;
};
struct Category
{
	char type[64];
	char file[64];
	int beging;
	int size;
};
struct Capital
{
	unsigned int date = 0;
	double totals = 0.0;
	double current = 0.0;
	double limited = 0.0;
}; 
struct CompanyInfo
{
	Scu scu;
	vector<Capital>vcapitals;
	unsigned int listdate = 0;
};
struct Receiv
{
	Scu scu;
	vector<Tick>vticks;
	Fz* fzs;
};

struct His
{
	double afit;
	double std[TYPES][DECAS];
	double free;
};
struct TD
{
	int t;
	int d;
};
struct Cnf
{
	string codes;
	float fit;
	int clsd;
	double simfit;
	bool operator <(const Cnf &cmp)const
	{
		return cmp.simfit < simfit;
	}
};
struct Sumy
{
	multiset<Cnf>mscnfs;
	double todfit;
	double sumfit;
	double topfit;
	int numget;
	double todget;
	double sumget;
	double topget;
	int nums[FUTURE];
	int days;
};
struct SumyTop
{
	string item;
	double sumfit_sort;
	double sumget_sort;
	double stopfit;
	double stopget;
	bool operator <(const SumyTop &cmp)const
	{
		return cmp.sumfit_sort + cmp.sumget_sort < sumfit_sort + sumget_sort;
	}
};
inline void Combination2(vector<vector<TD>>&vcombs, vector<TD>vins, vector<TD>vtemp, int pos, int beg, int end, int num)
{
	if (0 == num)
	{
		vcombs.push_back(vtemp);
		return;
	}
	if (beg > end)return;
	for (int idx = beg; idx <= end; idx++)
	{
		vtemp.at(pos) = vins.at(idx);
		Combination2(vcombs, vins, vtemp, pos + 1, idx + 1, end, num - 1);
	}
	return;
}
inline bool CmpByRefChgs(const Fram &ob0, const Fram &ob1)
{
	return ob1.sorchgs < ob0.sorchgs;
};
inline bool CmpByRefMnys(const Fram &ob0, const Fram &ob1)
{
	return ob1.sormnys < ob0.sormnys;
};
inline bool CmpByHisfree(const His &ob0, const His &ob1)
{
	return ob0.free < ob1.free;
};
inline unsigned long long AvaiMem()
{
	MEMORYSTATUSEX l_oStatex;
	l_oStatex.dwLength = sizeof(l_oStatex);
	GlobalMemoryStatusEx(&l_oStatex);
	return (unsigned long long)min(l_oStatex.ullAvailPhys / 1024 / 1024, l_oStatex.ullAvailVirtual / 1024 / 1024);
}
inline string GetMem()
{
	MEMORYSTATUSEX l_oStatex;
	l_oStatex.dwLength = sizeof(l_oStatex);
	GlobalMemoryStatusEx(&l_oStatex);
	stringstream sss;
	sss << "ullAvailPhys " << (unsigned long long)l_oStatex.ullAvailPhys / 1024 / 1024 << "MB";
	string rst = sss.str(); sss.clear(); sss.str("");
	return rst;
}
inline void Email163(string targetEmail, string emailTitle, string content, string filePath)
{
#ifdef _DEBUG
	HMODULE hdll = LoadLibrary("Smpt_debug.dll");
#else
	HMODULE hdll = LoadLibrary("Smpt_release.dll");
#endif
	typedef int(*callfn)(int, string, string, string, string, string, string, string);
	callfn MailSendApi = (callfn)GetProcAddress(hdll, "MailSendApi");
	int port = 25;
	string srvDomain = string("smtp.163.com");
	string userName = string("shugnn@163.com");
	string password = string("FUTURE050130");
	MailSendApi(port, srvDomain, userName, password, targetEmail, emailTitle, content, filePath);
	FreeLibrary(hdll);
	return;
}

HOST hostArray[] =
{
	{ "招商深圳行情", "119.147.212.81", 7709 },
	{ "深圳双线主站1", "120.79.60.82", 7709 },
	{ "上证云行情E328", "58.63.254.191", 7709 },
	{ "华西B51", "139.217.8.133", 7709 },
	{ "招商北京行情", "61.49.50.190", 7709 },
	{ "招商广州行情", "116.57.224.5", 7709 },
	{ "华西电信ES1", "182.131.7.196", 7709 },
	{ "华西电信ES2", "182.131.7.198", 7709 },
	{ "华西电信ES3", "182.131.7.199", 7709 },
	{ "华西电信ES4", "182.131.7.200", 7709 },
	{ "华西电信ES5", "182.131.7.211", 7709 },
	{ "华西移动ES1", "183.221.89.160", 7709 },
	{ "华西移动ES2", "183.221.89.161", 7709 },
	{ "华西移动ES3", "183.221.89.162", 7709 },
	{ "华西联通XX1", "119.4.167.141", 7709 },
	{ "华西联通XX2", "119.4.167.142", 7709 },
	{ "国信AZURESS", "40.73.76.10", 443 },
	{ "国信AZURE云行情", "40.73.76.10", 7709 },
	{ "国信东莞移动", "120.234.57.15", 7709 },
	{ "上证云行情N491", "117.184.225.184", 7709 },
	{ "上证云行情S351", "120.232.150.205", 7709 },
	{ "上证云行情R370", "117.175.57.172", 7709 },
	{ "上证云行情K459", "123.125.108.57", 7709 },
	{ "上证云行情E923", "58.63.254.247", 7709 }
};

#endif


