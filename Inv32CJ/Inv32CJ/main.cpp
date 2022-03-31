#include "HQ.h"
#include"CurAna.h"
#include"Trading.h"
using namespace std;

HANDLE hmut, hevt[3];
extern CHQ *pHQ;
extern CAna *pCAna;
extern CTra *pCTrading;

DWORD WINAPI threadHQ(LPVOID lpParam)
{
	cout << __FUNCTION__ << " " << TimeHMSYMD() << endl;
	while (true)
	{
		if (Marketingtime() == Time_Stop)
		{
			Sleep(5 * 60 * 1000);
			continue;
		}
		if (!pHQ)
			pHQ = new CHQ();
		int trys = 0;
		while (!pHQ->Connect() && ++trys < 10)Sleep(30 * 1000);
		if (trys >= 10)
		{
			cout << "Connect fail. " << TimeHMSYMD() << endl;
			delete pHQ; pHQ = NULL;
			continue;
		}
		unsigned int tod = pHQ->TradingDay, bef = 0;
		pHQ->CheckDatasDate(bef);
		if (bef >= tod)
		{
			cout << "CheckDatasDate error. " << TimeHMSYMD() << endl;
			delete pHQ; pHQ = NULL;
			continue;
		}
		int num = pHQ->GetSecurityCount();
		if (num <= 0)
		{
			cout << "GetSecurityCount zero. " << TimeHMSYMD() << endl;
			delete pHQ; pHQ = NULL;
			continue;
		}
		vector<Scu> vscus; vscus.clear(); vscus.reserve(num);
		pHQ->GetSecurityList(vscus); vscus.shrink_to_fit();
		for (unsigned int date = bef + 1; date <= tod; date++)
		{
			int year = date / 10000, month = (int)fmod(date, 10000) / 100, day = (int)fmod(date, 100);
			if (year < 2004 || 2030 < year || month<1 || 12<month || day < 1 || 31 < day)continue;
			struct tm* mdt; time_t rawtm; time(&rawtm); mdt = localtime(&rawtm);
			mdt->tm_year = year - 1900; mdt->tm_mon = month - 1; mdt->tm_mday = day; mktime(mdt);
			if (mdt->tm_wday == 6 || mdt->tm_wday == 0)continue;
			pHQ->Disconnect();
			trys = 0;
			while (!pHQ->Connect() && ++trys < 10)Sleep(30 * 1000); 
			if (trys >= 10)		
			{
				cout << "Connect fail. " << TimeHMSYMD() << endl;
				delete pHQ; pHQ = NULL;
				continue;
			}
			if (date < tod)
			{
				pHQ->GetTodHisTransactionData(vscus, date);
			}
			else
			{
				pHQ->GetCompanyInfoCategory(vscus, date);
				while (Marketingtime() == Time_MorningMarket)
					Sleep(3 * 60 * 1000);
				pHQ->GetTodHisTransactionData(vscus, date);
			}
		}
		pHQ->Disconnect();
		delete pHQ; pHQ = NULL;
	}
}


DWORD WINAPI threadCurAna(LPVOID lpParam)
{
	cout << __FUNCTION__ <<" "<< TimeHMSYMD() << endl;	
	unsigned int learnTill = 0, learnNew = 0;
	deque<unsigned int>clndr; 
	while (true)
	{
		if (learnNew> learnTill)
		{
			if (!pCAna)pCAna = new CAna(true, clndr);
			learnTill = pCAna->Start();
			if (pCAna){ delete pCAna; pCAna = NULL; }
			Sleep(1 * 60 * 1000);

			if (!pCAna)pCAna = new CAna(false, clndr);
			pCAna->Start();
			if (pCAna){ delete pCAna; pCAna = NULL; }
		}
		//Sleep(10 * 60 * 1000);
		GetCalendar(clndr);
		if (clndr.size() >= FUTURE)
			learnNew = *(clndr.rbegin() + FUTURE);
	}
}

void main(void)
{
#ifdef _DEBUG
	cout << "Debug Inv32CJ Starting." << endl;
#else
	cout << "Release Inv32CJ Starting." << endl;
#endif
	if (access(dirMktDatas, 0) != 0)mkdir(dirMktDatas);
	if (access(dirRulLearn, 0) != 0)mkdir(dirRulLearn);
	if (access(dirRstTrade, 0) != 0)mkdir(dirRstTrade);
	hmut = CreateMutex(NULL, FALSE, NULL);
	hevt[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	hevt[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
	hevt[2] = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	threadCurAna(NULL);
	system("pause");



	//string DatasFZ = string("..");
	//cout << DatasFZ << endl;
	//char m_szInitDir[_MAX_PATH];
	//_fullpath(m_szInitDir, DatasFZ.c_str(), _MAX_PATH);
	//cout << m_szInitDir << endl;
	//DatasFZ = string("..\\RstTrade");
	//_fullpath(m_szInitDir, DatasFZ.c_str(), _MAX_PATH);
	//cout << m_szInitDir << endl;
	while (true)
	{
		//if (Time_Stop != TradingTime())
		{
			if (!pCTrading)pCTrading = new CTra();
			if (pCTrading)
			{
				pCTrading->Run();
				delete pCTrading;
				pCTrading = NULL;
			}
		}
		cout << ".";
		Sleep(10 * 60 * 1000);
	}
	CloseHandle(hevt[0]);
	CloseHandle(hevt[1]);
	CloseHandle(hevt[2]);
	CloseHandle(hmut);
}
