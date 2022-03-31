#include "HQ.h"
using namespace std;
string DatasHistory = string("F:\\ScDatas\\HisFZ5");
string TicksHistory = string("F:\\ScDatas\\HisTik");//TicksHistory2018720 ::J:\\ScDatas\\HisTik"
string CoInfoDir = string("F:\\ScDatas\\CoInfo");
CHQ *pHQ;

CHQ::CHQ()
{
	//载入dll, dll要复制到debug和release目录下,必须采用多字节字符集编程设置,用户编程时需自己控制浮点数显示的小数位数与精度
	TdxApiHMODULE = LoadLibrary("TdxHqApi.dll");
	//获取api函数
	TdxHq_Connect = (TdxHq_ConnectDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_Connect");
	TdxHq_Disconnect = (TdxHq_DisconnectDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_Disconnect");
	TdxHq_GetSecurityCount = (TdxHq_GetSecurityCountDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetSecurityCount");
	TdxHq_GetSecurityList = (TdxHq_GetSecurityListDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetSecurityList");
	TdxHq_GetSecurityBars = (TdxHq_GetSecurityBarsDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetSecurityBars");
	TdxHq_GetIndexBars = (TdxHq_GetIndexBarsDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetIndexBars");
	TdxHq_GetMinuteTimeData = (TdxHq_GetMinuteTimeDataDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetMinuteTimeData");
	TdxHq_GetHistoryMinuteTimeData = (TdxHq_GetHistoryMinuteTimeDataDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetHistoryMinuteTimeData");
	TdxHq_GetTransactionData = (TdxHq_GetTransactionDataDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetTransactionData");
	TdxHq_GetHistoryTransactionData = (TdxHq_GetHistoryTransactionDataDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetHistoryTransactionData");
	TdxHq_GetSecurityQuotes = (TdxHq_GetSecurityQuotesDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetSecurityQuotes");
	TdxHq_GetCompanyInfoCategory = (TdxHq_GetCompanyInfoCategoryDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetCompanyInfoCategory");
	TdxHq_GetCompanyInfoContent = (TdxHq_GetCompanyInfoContentDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetCompanyInfoContent");
	TdxHq_GetXDXRInfo = (TdxHq_GetXDXRInfoDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetXDXRInfo");
	TdxHq_GetFinanceInfo = (TdxHq_GetFinanceInfoDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetFinanceInfo");
	ErrInfo = new char[256];
}
CHQ::~CHQ()
{
	FreeLibrary(TdxApiHMODULE);
	delete[] ErrInfo; ErrInfo = NULL;
}
bool CHQ::Connect()
{
	/*
	char* Result2 = new char[256];
	short hrd = (short)(short)sizeof(HostArray) / sizeof(HostArray[0]);
	int *res = new int[100]; memset(res, 0, sizeof(int)* 100);
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < hrd; j++)
		{
			bool bool1 = TdxHq_Connect((char*)HostArray[j].pszHost, HostArray[j].nPort, Result2, ErrInfo);
			if (bool1)
			{
				res[j]++;
				TdxHq_Disconnect();
			}
			cout << j << " res " << res[j] << " " << HostArray[j].pszName << " " << HostArray[j].pszHost << "				" << Result2 << endl;
		}
		Sleep(1000 * 60*3);
	}
	cin.get();
	return true;
*/
	char* Result = new char[256];
	bool bool1 = false; 
	srand((unsigned int)time(NULL)*RAND_MAX); unsigned int rd = rand(); short trys = 0;
	while (!bool1 && ++trys <= (short)sizeof(hostArray) / sizeof(hostArray[0]))
	{
		short hrd = (short)fmod(rd++, (short)sizeof(hostArray) / sizeof(hostArray[0]));
		HOST host = hostArray[hrd];
		bool1 = TdxHq_Connect((char*)host.pszHost, host.nPort, Result, ErrInfo);//
	};
	if (bool1)
	{
		stringstream ssres(Result);
		char line[1024], res0[33], res1[33];
		unsigned int trd;
		while (ssres.getline(line, sizeof(line)))
		{
			stringstream(line) >> res0 >> trd >> res1;
			if (2014 <= (unsigned int)trd / 10000 && (unsigned int)trd / 10000 <= 2032)TradingDay = trd;
		}
		delete Result; Result = NULL;
	}
	else
	{
		cout << ErrInfo << endl;
		delete Result; Result = NULL;
		return false;
	}
	return true;
}
void CHQ::Disconnect()
{
	TdxHq_Disconnect();
	//std::cout << __FUNCTION__ << endl;
	return;
}
int CHQ::GetSecurityCount()
{
	short Count0 = 0, Count1 = 0;
	bool bool0 = TdxHq_GetSecurityCount(0, Count0, ErrInfo);
	if (!bool0)
	{
		std::cout << ErrInfo << endl;
		return 0;
	}
	bool bool1 = TdxHq_GetSecurityCount(1, Count1, ErrInfo);
	if (!bool1)
	{
		std::cout << ErrInfo << endl;
		return 0;
	}
	//std::cout << __FUNCTION__ << " Market 深 " << Count0 << " Market 沪 " << Count1 << endl;
	return (int)Count0 + Count1;
}
void CHQ::GetSecurityList(vector<Scu>&vscus)
{
	/*
	map<string, Scu>mscus; mscus.clear();
	stringstream ssss, scode;
	for (short i = 0; i < 4; i++)
	for (short j = 9000; j < 10000; j++)
	{
		Scu scu; scu.isindx = true;
		scu.imkt = (strcmp(idx[i], "399") == 0 || strcmp(idx[i], "395") == 0) ? 0 : 1;
		ssss.clear(); scode.clear();
		ssss << j; scode << scu.imkt << idx[i] << ssss.str().substr(1, 3);
		scode >> scu.code; ssss.str(""); scode.str("");
		mscus.insert(map<string, Scu>::value_type(scu.code, scu));
	}
	for (short i = 0; i < 4; i++)
	for (short j = 3000; j < 4000; j++)
	{
		Scu scu; scu.isindx = false;
		scu.imkt = 0;
		ssss.clear(); scode.clear();
		ssss << j; scode << scu.imkt << ssz[i] << ssss.str().substr(1, 3);
		scode >> scu.code; ssss.str(""); scode.str("");
		mscus.insert(map<string, Scu>::value_type(scu.code, scu));
	}
	for (short i = 0; i < 4; i++)
	for (short j = 6000; j < 7000; j++)
	{
		Scu scu; scu.isindx = false;
		scu.imkt = 0;
		ssss.clear(); scode.clear();
		ssss << j; scode << scu.imkt << ssh[i] << ssss.str().substr(1, 3);
		scode >> scu.code; ssss.str(""); scode.str("");
		mscus.insert(map<string, Scu>::value_type(scu.code, scu));
	}*/
	short cnt[] = { 0, 0 }; 
	for (short imkt = 0; imkt <= 1; imkt++)
	{
		short Start = 0, Sccount = 0;
		do
		{
			Start += Sccount; Sccount = 1000;
			char* Result = new char[1024 * 1024];
			byte Market = (imkt == 0) ? 0 : 1;
			bool bool1 = TdxHq_GetSecurityList(Market, Start, Sccount, Result, ErrInfo);
			if (bool1)
			{
				stringstream ssres(Result);
				char line[1024], res[33];
				while (ssres.getline(line, sizeof(line)))
				{
					Scu scu; scu.imkt = imkt; scu.type = -1;
					stringstream(line) >> scu.code >> res >> scu.name >> res >> res >> res >> res >> res;
					short pos = string(scu.code).find_first_of("0123456789");
					if (pos<0 || 2<pos)continue;
					string ccc = string(scu.code).substr(pos, 3);
					string cc = string(scu.code).substr(pos, 2);
					if (imkt == 0)
					{
						if (ccc == string("000") || ccc == string("001") || ccc == string("002") || ccc == string("003"))
						{
							scu.type = 0;//zhuban
						}
						if (ccc == string("300"))
						{
							scu.type = 1;//chuangyeban
						}
						if (cc == string("15") || cc == string("16") || cc == string("18"))
						{
							scu.type = 2;// jijin
						}
						if (ccc == string("395") || ccc == string("399"))
						{
							scu.type = 3;//zhishu
						}
					}
					if (imkt == 1)
					{
						if (ccc == string("600") || ccc == string("601") || ccc == string("603") || ccc == string("605"))
						{
							scu.type = 0;//zhuban
						}
						if (ccc == string("688"))
						{
							scu.type = 1;//chuangyeban
						}
						if (cc == string("50") || cc == string("51"))
						{
							scu.type = 2;// jijin
						}
						if (cc == string("00") || cc == string("88") || cc == string("99"))
						{
							scu.type = 3;//zhishu
						}
					}
					if (scu.type >= 0 && scu.type != 2)
					{
						vscus.push_back(scu);
						++cnt[imkt];
					}
				}
				delete Result; Result = NULL;
				ssres.clear(); ssres.str("");
			}
			else
			{
				std::cerr << ErrInfo << endl;
				delete Result; Result = NULL;
				break;
			}
		} while (Sccount == 1000);
	}
	vscus.shrink_to_fit();
	//std::cout << __FUNCTION__ << " Market 深 " << cnt[0] << " Market 沪 " << cnt[1] << endl;
	return;
}
bool CHQ::GetTodHisTransactionData(vector<Scu>cvscus, const unsigned int date)
{
	vector<Scu> vscus = cvscus; cvscus.clear();
	vector<Receiv> vreceivs; vreceivs.clear(); vreceivs.reserve(vscus.size());
	auto scu = vscus.begin(); short trys = 0; bool id300ed = false;
	while (!vscus.empty())
	{
		if (vscus.end() == scu)
		{
			//std::cout << date << " " << "Try " << ++trys << " downloaded cvscus " << cvscus.size() << " " << TimeHMSYMD() << endl;
			//if (trys >= 2){ cerr << date << " Remain vscus " << vscus.size() << endl; break; }
			if (++trys >= 2)break;
			Disconnect(); short trys2 = 0; while (!Connect() && ++trys2 < 10)Sleep(1000 * 60);
			if (trys2 >= 10){ cerr << "Can't connect.Press anykey skip." << " " << TimeHMSYMD() << endl; system("pause"); break; }
			scu = vscus.begin();
		}
		if (!id300ed && (strcmp(scu->code, "000300") != 0 || scu->imkt != 1))
		{
			scu++; continue;
		}
		byte Market = (scu->imkt == 0) ? 0 : 1;
		short Start = 0, Sccount = 0; vector<stringstream>vdatas; vdatas.clear();
		do
		{
			Start += Sccount; Sccount = 2000;
			char* Result = new char[1024 * 1024];
			bool bool1 = (date == TradingDay) ?
				TdxHq_GetTransactionData(Market, scu->code, Start, Sccount, Result, ErrInfo) :
				TdxHq_GetHistoryTransactionData(Market, scu->code, Start, Sccount, date, Result, ErrInfo);
			if (bool1)
			{
				if (Sccount>0)
					vdatas.insert(vdatas.begin(), stringstream(Result));
				delete Result; Result = NULL;
			}
			else
			{
				std::cerr << ErrInfo << endl;
				delete Result; Result = NULL;
				break;
			}
		} while (Sccount == 2000);
		unsigned short dt = (unsigned short)(date / 10000 - 2004) * 2048 + (unsigned short)fmod(date, 10000);
		Receiv receiv; receiv.scu = *scu;  receiv.fzs = new Fz[48]; std::memset(receiv.fzs, 0, 48 * sizeof(Fz));
		receiv.vticks.clear(); receiv.vticks.reserve(10000);
		bool chkerr = false; short cnt = 0;
		for (auto data = vdatas.begin(); data != vdatas.end(); data++)
		{
			char line[256]; double pric0 = 0;
			while (data->getline(line, sizeof(line)))
			{
				if (!isdigit((unsigned char)line[0]))continue;
				Tick tick; memset(&tick, 0, sizeof(tick));
				stringstream(line) >> tick.time >> tick.pric >> tick.amou >> tick.tiks >> tick.drec;
				if (tick.pric < 0)chkerr = true;
				if (pric0 != 0 && abs(tick.pric / pric0 - 1) * 100>1000)chkerr = true;
				pric0 = tick.pric;
				if (isdigit((unsigned char)tick.time[0]))
				{
					unsigned short tm = (unsigned short)atoi(string(tick.time).substr(0, string(tick.time).find(":")).c_str()) * 60
						+ (unsigned short)(atoi(string(tick.time).substr(string(tick.time).length() - 2, 2).c_str()) / 5 + 1) * 5;
					if (tm < 540 || tm>900)continue; if (tm < 575)tm = 575;
					unsigned short n = (tm - 575) / 5; if (n >= 24)n -= 18;
					receiv.fzs[n].dt = dt;
					receiv.fzs[n].tm = tm;
					receiv.fzs[n].op = (receiv.fzs[n].op == 0) ? (unsigned int)floor(tick.pric * 1000.000f + 0.5) : receiv.fzs[n].op;
					receiv.fzs[n].hg = max(receiv.fzs[n].hg, (unsigned int)floor(tick.pric * 1000.000f + 0.5));
					if (receiv.fzs[n].lw <= 0)receiv.fzs[n].lw = receiv.fzs[n].op;
					receiv.fzs[n].lw = min(receiv.fzs[n].lw, (unsigned int)floor(tick.pric * 1000.000f + 0.5));
					receiv.fzs[n].cl = (unsigned int)floor(tick.pric * 1000.000f + 0.5);
					receiv.fzs[n].vm += (scu->type == 3) ? 0 : tick.amou;
					receiv.fzs[n].mn += (scu->type == 3) ? tick.amou : (unsigned int)floor(tick.pric*tick.amou * 1.000f + 0.5);
					receiv.vticks.push_back(tick);
					++cnt;
				}
				else chkerr = true;
			}
		}
		if (cnt < 100)chkerr = true;
		if (!chkerr)
		{
			receiv.vticks.shrink_to_fit();
			vreceivs.push_back(receiv);
			cvscus.push_back(*scu);
			scu = vscus.erase(scu);
			if (!id300ed)
			{
				id300ed = true;
				scu = vscus.begin();
			}
		}
		else scu++;
	}
	vreceivs.shrink_to_fit();
	if (!vreceivs.empty())
	{
		DataWrite(&vreceivs, date);
		cout << date << " downloaded scus " << vreceivs.size() << " remained " << vscus.size() << " " << TimeHMSYMD() << endl;
	}
	for (auto receiv = vreceivs.begin(); receiv != vreceivs.end(); receiv++)
	{
		delete[] receiv->fzs;
		receiv->vticks.clear();
	}
	vreceivs.clear();
	vscus.clear();
	cvscus.shrink_to_fit();
	return true;
}
void CHQ::GetCompanyInfoCategory(const vector<Scu>cvscus, const unsigned int date)
{
	vector<Scu>vscus = cvscus; for (auto sc = vscus.begin(); sc != vscus.end();){if (sc->type >= 2)sc = vscus.erase(sc); else ++sc;}
	vector<CompanyInfo>vcompinfs; vcompinfs.clear(); vcompinfs.reserve(vscus.size());
	auto scu = vscus.begin(); while (!vscus.empty())
	{
		byte Market = (scu->imkt == 0) ? 0 : 1;
		char* Result = new char[1024 * 1024];
		bool bool1 = TdxHq_GetCompanyInfoCategory(Market, scu->code, Result, ErrInfo);
		if (bool1)
		{
			CompanyInfo comp; comp.scu = *scu; comp.listdate = 0; comp.vcapitals.clear();
			stringstream ssres(Result); char line[1024];
			//cout << Result << endl;
			while (ssres.getline(line, sizeof(line)))
			{
				Category cate;
				stringstream(line) >> cate.type >> cate.file >> cate.beging >> cate.size;
				//cout << line << endl;
				if (cate.beging>=0 && cate.size>=0)
				{
					if (strcmp(cate.type, "公司概况") == 0)
					{
						GetCompanyInfoContentA(&comp.scu, &cate, comp);
					}
					if (strcmp(cate.type, "股本结构") == 0)
					{
						GetCompanyInfoContentB(&comp.scu, &cate, comp);
					}
				}
			}
			if (comp.listdate != 0 && !comp.vcapitals.empty())
				vcompinfs.push_back(comp);
			ssres.clear(); ssres.str("");
			delete Result; Result = NULL;
		}
		else
		{
			std::cerr << ErrInfo << endl;
			delete Result; Result = NULL;
			break;
		}
		scu = vscus.erase(scu);
	}
	vcompinfs.shrink_to_fit();
	if (!vcompinfs.empty())
	{
		CompanyInfoWrite(&vcompinfs, date);
		cout << date << " downloaded vcompinfs " << vcompinfs.size() << " " << TimeHMSYMD() << endl;
	}
	for (auto compinf = vcompinfs.begin(); compinf != vcompinfs.end(); compinf++)
	{
		compinf->vcapitals.clear();
		vector<Capital>(compinf->vcapitals).swap(compinf->vcapitals);
	}
	vcompinfs.clear(); vector<CompanyInfo>(vcompinfs).swap(vcompinfs);
	vscus.clear(); vector<Scu>(vscus).swap(vscus);
	return;
}
void CHQ::GetCompanyInfoContentA(Scu *scu, Category *cate, CompanyInfo &comp)//(byte Market, char* Zqdm, char* FileName, int Start, int Length, char* Result, char* ErrInfo)
{
	byte Market = (scu->imkt == 0) ? 0 : 1;
	char* Result = new char[1024 * 1024];
	bool bool1 = TdxHq_GetCompanyInfoContent(Market, scu->code, cate->file, cate->beging, cate->size, Result, ErrInfo);
	if (bool1)
	{
		stringstream ssres(Result); char line[1024];
		while (ssres.getline(line, sizeof(line)))
		{
			if (string(line).length() >= 32 && string(line).find_first_of("｜") == 0)
			{
				string sline = string(line); string::size_type pos = 0;
				while ((pos = sline.find("｜")) != string::npos)
					sline.replace(pos, string("｜").length(), " ");
				stringstream ssline(sline); string word("");
				while (ssline >> word)
				{
					if (word == string("上市日期"))
					{
						word.clear(); ssline >> word;
						if (word.length() == 10 && '0' <= word[0] && word[0] <= '9' && word.find('-') == 4)
						{
							unsigned date = atoi(word.substr(0, 4).c_str()) * 10000 + atoi(word.substr(5, 2).c_str()) * 100 + atoi(word.substr(8, 2).c_str());
							if (19850101 < date && date < 20391231)
							{
								comp.listdate = date;
							}
						}
					}
					word.clear();
				}
				ssline.clear(); ssline.str("");
			}
		}
		ssres.clear(); ssres.str("");
		delete Result; Result = NULL;
	}
	else
	{
		std::cerr << ErrInfo << endl;
		delete Result; Result = NULL;
	}
	return;
}
void CHQ::GetCompanyInfoContentB(Scu *scu, Category *cate, CompanyInfo &comp)//(byte Market, char* Zqdm, char* FileName, int Start, int Length, char* Result, char* ErrInfo)
{
	byte Market = (scu->imkt == 0) ? 0 : 1;
	char* Result = new char[1024 * 1024];
	bool bool1 = TdxHq_GetCompanyInfoContent(Market, scu->code, cate->file, cate->beging, cate->size, Result, ErrInfo);
	if (bool1)
	{
		stringstream ssres(Result); char line[1024];
		//cout << scu->code << " " << scu->name << endl;
		//cout << Result << endl;
		bool withNextLine = false;
		while (ssres.getline(line, sizeof(line)))
		{
			//cout << line << endl; 
			if (string(line).length() < 4)continue;
			string sline = string(line); string::size_type pos = 0;
			while ((pos = sline.find("｜")) != string::npos)
				sline.replace(pos, string("｜").length(), " ");
			if (string::npos != sline.find("变更日期") && string::npos != sline.find("总股本(股)"))
			{
				withNextLine = true;
				continue;
			}
			if (0== sline.find("【3.限售流通】"))
			{
				withNextLine = false;
				break;
			}
			if (withNextLine)
			{
				stringstream ssline(sline); string word("");
				while (ssline >> word)
				{
					if (word.length() == 10 && '0' <= word[0] && word[0] <= '9' && word.find('-') == 4)
					{
						unsigned date = atoi(word.substr(0, 4).c_str()) * 10000 + atoi(word.substr(5, 2).c_str()) * 100 + atoi(word.substr(8, 2).c_str());
						if (19850101 >= date || date >= 20391231)continue;
						if (date<comp.listdate)continue;
						if (!comp.vcapitals.empty() && date > comp.vcapitals.back().date)continue;
						Capital cap; cap.date = date;
						string tot(""), cur(""), lim("");
						ssline >> tot >> cur >> lim;
						int end = 0;
						if (string::npos != (end = tot.find_first_of("亿")))
						{
							cap.totals = ('0' <= tot[0] && tot[0] <= '9') ? atof(tot.substr(0, end).c_str()) * 10000 * 10000 : 0.0;
						}
						else if (string::npos != (end = tot.find_first_of("万")))
						{
							cap.totals = ('0' <= tot[0] && tot[0] <= '9') ? atof(tot.substr(0, end).c_str()) * 10000 : 0.0;
						}
						end = 0;
						if (string::npos != (end = cur.find_first_of("亿")))
						{
							cap.current = ('0' <= cur[0] && cur[0] <= '9') ? atof(cur.substr(0, end).c_str()) * 10000 * 10000 : 0.0;
						}
						else if (string::npos != (end = cur.find_first_of("万")))
						{
							cap.current = ('0' <= cur[0] && cur[0] <= '9') ? atof(cur.substr(0, end).c_str()) * 10000 : 0.0;
						}
						end = 0;
						if (string::npos != (end = lim.find_first_of("亿")))
						{
							cap.limited = ('0' <= lim[0] && lim[0] <= '9') ? atof(lim.substr(0, end).c_str()) * 10000 * 10000 : 0.0;
						}
						else if (string::npos != (end = lim.find_first_of("万")))
						{
							cap.limited = ('0' <= lim[0] && lim[0] <= '9') ? atof(lim.substr(0, end).c_str()) * 10000 : 0.0;
						}
						comp.vcapitals.push_back(cap);
					}
					word.clear();
				}
				ssline.clear(); ssline.str("");
			}
		}
		ssres.clear(); ssres.str("");
		delete Result; Result = NULL;
	}
	else
	{
		std::cerr << ErrInfo << endl;
		delete Result; Result = NULL;
	}
	return;
}
void CHQ::DataWrite(const vector<Receiv> *cvreceivs, unsigned int date)
{
	if (access(DatasHistory.c_str(), 0) != 0)mkdir(DatasHistory.c_str());
	stringstream ss; ss.clear(); 
	if (date == TradingDay && Marketingtime() == Time_HalfMarket)ss << "\\" << date << "H.5";
	else ss << "\\" << date << ".5";
	string fn = DatasHistory + ss.str(); ss.str("");
	ofstream outfile(fn, ios::binary | ios::trunc);
	if (outfile.is_open())
	{
		outfile.seekp(0, ios::end);
		for (auto receiv = cvreceivs->begin(); receiv != cvreceivs->end(); receiv++)
		{
			outfile.write((char*)&receiv->scu, sizeof(receiv->scu));
			outfile.write((char*)receiv->fzs, 48 * sizeof(Fz));
		}
		outfile.close(); 
	}

	if (access(TicksHistory.c_str(), 0) != 0)mkdir(TicksHistory.c_str());
	ss.clear(); ss << "\\" << date << ".t";
	fn = TicksHistory + ss.str(); ss.str("");
	outfile.open(fn, ios::binary | ios::trunc);
	if (outfile.is_open())
	{
		outfile.seekp(0, ios::end);
		for (auto receiv = cvreceivs->begin(); receiv != cvreceivs->end(); receiv++)
		{
			outfile.write((char*)&receiv->scu, sizeof(receiv->scu));
			unsigned int len = receiv->vticks.size();
			outfile.write((char*)&len, sizeof(len));
			outfile.write((char*)&receiv->vticks.front(), len * sizeof(Tick));
		}
		outfile.close();
	}
	return;
}
void CHQ::CompanyInfoWrite(const vector<CompanyInfo>*vcompinfs, unsigned int date)
{
	if (access(CoInfoDir.c_str(), 0) != 0)mkdir(CoInfoDir.c_str());
	stringstream ss; ss.clear(); ss << "\\captals.bin";
	string fn = CoInfoDir + ss.str(); ss.str("");
	ofstream outfile(fn, ios::binary | ios::trunc);
	if (outfile.is_open())
	{
		outfile.seekp(0, ios::beg);
		outfile.write((char*)&date, sizeof(date));
		for (auto compinf = vcompinfs->begin(); compinf != vcompinfs->end(); compinf++)
		{
			outfile.write((char*)&compinf->scu, sizeof(compinf->scu));
			outfile.write((char*)&compinf->listdate, sizeof(compinf->listdate));
			unsigned int size = compinf->vcapitals.size();
			outfile.write((char*)&size, sizeof(size));
			outfile.write((char*)&compinf->vcapitals.front(), size * sizeof(Capital)); 
		}
		outfile.close();
	}
	return;
}
void CHQ::CheckDatasDate(unsigned int &date)
{
	date = 0;
	_finddata_t info;
	int handle = _findfirst((DatasHistory + "\\*.5").c_str(), &info);
	if (handle > 0)
	{
		do
		{
			string dn(info.name);
			if (dn.substr(dn.length() - 2, 2) != string(".5"))continue;
			if (dn.substr(dn.length() - 3, 3) == string("H.5"))
			{
				if (Marketingtime() == Time_AlldayMarket)
				{
					string fn = DatasHistory + string("\\") + dn;
					if (access(fn.c_str(), 0) == 0)
						DeleteFile(fn.c_str());
					continue;
				}
			}
			unsigned int checkdate = (unsigned int)(atoi(dn.substr(0, 8).c_str()));
			if (checkdate>date)
				date = checkdate;
		} while (_findnext(handle, &info) >= 0);
		_findclose(handle);
	}
	else
	{
		cerr << __FUNCTION__ << "	Dir Null!" << endl;
	}
	return;
}


