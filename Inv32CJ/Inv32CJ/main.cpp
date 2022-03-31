#include"CurAna.h"
#include"Trading.h"
using namespace std;
string MktDatas = string("..\\MktDatas");
string RulLearn = string("..\\RulLearn");
string RstTrade = string("..\\RstTrade");

HANDLE hmut, hevt[3];
extern CAna *pCAna;
extern CTra *pCTrading;
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
	if (access(MktDatas.c_str(), 0) != 0)mkdir(MktDatas.c_str());
	if (access(RulLearn.c_str(), 0) != 0)mkdir(RulLearn.c_str());
	if (access(RstTrade.c_str(), 0) != 0)mkdir(RstTrade.c_str());
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
