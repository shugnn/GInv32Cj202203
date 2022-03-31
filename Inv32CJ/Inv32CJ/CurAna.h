#if !defined(ANA_H)
#define ANA_H
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include"Data_Struct.h"

class   CAna
{
public:
	CAna(bool l_u, deque<unsigned int>&clndr);
	~CAna();
	unsigned int Start();
private:
	void ReadCominf();
	void ReadFzs(const deque<unsigned int>::iterator dfut);
	void GetCap(const deque<unsigned int>::iterator dtod, map<string, Fram>&mframs);
	void GetFzs(const deque<unsigned int>::iterator dtod, map<string, Fram>&mframs, map<string, map<unsigned int, Fz*>> ::iterator mlfzss);
	void Config(const deque<unsigned int>::iterator dtod, map<string, vector<His>>&msvhiss);
	void Record(const deque<unsigned int>::iterator dtod, multiset<SumyTop>&mssumytops, double stopfits, double stopgets);
	void TdxWrite(unsigned int date, string sscodes);
	void RestartHXtdx();

	bool L_U;
	unsigned int DateReadIn, LearnTill;
	deque<unsigned int>calendar;

	vector<vector<TD>>vcombs;
	map<string, CompanyInfo> mcompanyinfos;
	map<string, map<unsigned int, Fz*>>mmlfzsss;
	map<unsigned int, map<string, Fram>>mmframss;
	map<string, map<unsigned int, Sumy>>msmisumyss;

};


#endif
