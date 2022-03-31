#include"Trading.h"
#include"Changjiang.h"
extern string RulLearn;
extern string RstTrade;
extern HANDLE hmut, hevt[3];
extern CChangjiang *pCCJ;
CTra *pCTrading; 
ACCOUNT account;

CTra::CTra()
{
	consolHwnd = FindWindow("ConsoleWindowClass", NULL);
	ShowWindow(consolHwnd, SW_MAXIMIZE);

	vDeclares.clear();
	vCandidates.clear();
	vHolds.clear();
	mSTickMarckets.clear();
	dclIdLocal = 0;
	mSints.clear();
	//WaitForSingleObject(hmut, INFINITE);
	//std::cout << endl<<"Trader Begining " << TimeHMSYMD() << endl;
	//ReleaseMutex(hmut);
}
CTra::~CTra()
{
	WaitForSingleObject(hmut, INFINITE);
	std::cout << "Trader Exit. " << TimeHMSYMD() << endl;
	ReleaseMutex(hmut);
}
void CTra::Run()
{
	if (!OpenTeminalCJ())return;
	FetchAccount();
	if (!ConnectMarket())return;
	Supervise();
	DisConnectMarket();
	TidyAccount();
	CloseTeminalCJ();
}
bool CTra::OpenTeminalCJ()
{
	if (!pCCJ)pCCJ = new CChangjiang();
	int trys = 0;
	//ShowWindow(consolHwnd, SW_HIDE);
	while (!pCCJ->Login())
	{
		if (++trys > 10)return false;
		Sleep(30 * 1000);
	}
	//ShowWindow(consolHwnd, SW_NORMAL);
	return true;
}
void CTra::FetchAccount()
{
	ReadAccountHold();
	THREADREF threadref;
	threadref.callType = Call_Inqry;
	threadref.inqMode = Inqry_Morning;
	HANDLE han = CreateThread(NULL, 0, threadTeminalCJ, (LPVOID)&threadref, 0, NULL);
	CloseHandle(han);
	WaitForSingleObject(hevt[0], INFINITE);
	if (vHolds.empty())return;
	cout << __FUNCTION__ << endl;
	for (auto ihold = vHolds.begin(); ihold != vHolds.end(); ++ihold)
		cout << (int)(ihold - vHolds.begin()) + 1 << ". " << ihold->stock.codes << " " << ihold->stock.stkName << " bala " << ihold->stock.stkBalance << " cost " << ihold->stock.costPrice
		<< " high " << ihold->holdHighestPrice << " lows " << ihold->holdLowestPrice
		<< " date " << ihold->trdDateLast << " days " << ihold->holdDays << endl;
}
bool CTra::ConnectMarket()
{
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
	HMODULE TdxApiHMODULE = LoadLibrary("TdxHqApi.dll");
	TdxHq_Connect = (TdxHq_ConnectDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_Connect");
	TdxHq_GetSecurityQuotes = (TdxHq_GetSecurityQuotesDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetSecurityQuotes");
	TdxHq_Disconnect = (TdxHq_DisconnectDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_Disconnect");
	char* Result = new char[256];
	bool bool1 = false; char* ErrInfo = new char[256];
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
			//if (2014 <= (unsigned int)trd / 10000 && (unsigned int)trd / 10000 <= 2022)TradingDay = trd;
		}
		//cout << Result << endl;
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
void CTra::Supervise()
{
	while (true)
	{
		TRADINGTIME tdtm = TradingTime();
		if (Time_Stop == tdtm)
			break;
		else if (Time_Candidate == tdtm)
			GetCandidate();
		else if (Time_Pause != tdtm)
		{
			MarketQuate();
			CleanOpsition();
			OpenOpsition();
			FreshAccount();
		}
		Sleep(15 * 1000);
	}
	return;
}
void CTra::GetCandidate()
{
	static bool haveGot = false; if (haveGot)return; haveGot = true;
	unsigned int CDDDate = 0, LearnTill = 0;
	string fn; stringstream ss; ss.clear(); ss.str("");
	ss << "P" << PREDAY << "F" << FUTURE << "H" << HISSDAY;
	fn = RulLearn + string("\\") + ss.str() + string("Use.txt");
	ifstream infile(fn, ios::in);
	if (infile.is_open())
	{
		char line[1024] = { 0 };
		infile.seekg(0, ios::beg);
		infile.getline(line, sizeof(line));
		if (string(line).substr(0, 3) == string("202"))
		{
			stringstream ssbuf(line); string buff; double climb = 0.0, drop = 0.0;
			ssbuf >> CDDDate >> buff >> LearnTill >> buff >> climb >> buff >> drop; ssbuf.str("");
			//if (useDate >= account.useDate && LearnTill + FUTURE >= useDate)
			while (infile.getline(line, sizeof(line)))
			{
				int size = 0;
				ssbuf.clear(); ssbuf << line; ssbuf >> buff >> buff >> size; ssbuf.str("");
				if (size > 0)
				{
					infile.getline(line, sizeof(line)); string sline(line);
					sline.erase(0, sline.find_first_not_of(" "));
					sline.erase(sline.find_last_not_of(" ") + 1);
					vector<string>vcodes; vcodes.reserve(size);
					split(string(sline), vcodes, ' '); int pref = 0;
					for (auto code = vcodes.begin(); code != vcodes.end(); ++code)
					{
						if (code->length() != 6)continue;
						if (code->substr(0, 2) != string("00") && code->substr(0, 2) != string("30") &&
							code->substr(0, 2) != string("60") && code->substr(0, 2) != string("68"))continue;
						bool isHold = false;
						for (auto ihold = vHolds.begin(); ihold != vHolds.end(); ++ihold)
						{
							if (*code == ihold->stock.codes)
							{
								strcat(ihold->remark, ihold->trdDateLast.c_str());
								strcat(ihold->remark, ";OpenRetain");
								ihold->trdDateLast = TimeYMD();
								ihold->holdDays = 0;
								isHold=true;
								break;
							}
						}
						if (isHold)continue;
						CANDIDATE cdd;
						cdd.codes = *code;
						cdd.prefer = pref++;
						cdd.climb = climb;
						cdd.drop = drop;
						cdd.isDeclaring = false;
						vCandidates.push_back(cdd);
						TICKMARKET tick;
						mSTickMarckets.insert(map<string, TICKMARKET>::value_type(*code, tick));
					}
					ssbuf.str("");
				}
				if (!vCandidates.empty())break;
			}
		}
		infile.close();
	}
	cout << __FUNCTION__ << " CDDDate:" << CDDDate << " LearnTill:" << LearnTill << " msuses.size:" << vCandidates.size() << endl;
	if (CDDDate != SysDate())vCandidates.clear();
	return;
}
void CTra::MarketQuate()
{
	short cn = (short)mSTickMarckets.size(); if (cn <= 0 || cn > 50)return;
	char** zqdm = new char*[cn]; short count = 0;
	byte* market = new byte[cn];
	for (auto itick = mSTickMarckets.begin(); itick != mSTickMarckets.end(); itick++)
	{
		string cc = itick->first.substr(0, 2);
		string ccc = itick->first.substr(0, 3);
		if (ccc == string("000") || ccc == string("001") || ccc == string("002") || ccc == string("003")
			|| ccc == string("300")
			|| cc == string("15") || cc == string("16") || cc == string("18"))
		{
			market[count] = 0;
			zqdm[count] = new char[itick->first.size()];
			strcpy(zqdm[count++], itick->first.c_str());
		}
		else if (ccc == string("600") || ccc == string("601") || ccc == string("603") || ccc == string("605")
			|| ccc == string("688")
			|| cc == string("50") || cc == string("51")
			|| cc == string("00") || cc == string("88") || cc == string("99"))
		{
			market[count] = 1;
			zqdm[count] = new char[itick->first.size()];
			strcpy(zqdm[count++], itick->first.c_str());
		}
	}
	char* Result = new char[1024 * 1024]; char* ErrInfo = new char[256];
	bool bool1 = TdxHq_GetSecurityQuotes(market, zqdm, count, Result, ErrInfo);
	if (bool1)
	{
		stringstream datas(Result); char line[1024];
		while (datas.getline(line, sizeof(line)))
		{
			//cout << line << endl;
			vector<string> vbuffs; vbuffs.clear(); vbuffs.reserve(44);
			split(string(line), vbuffs, '	');//cout << vbuffs[1] << endl;
			auto itick = mSTickMarckets.find(vbuffs[1]);
			if (mSTickMarckets.end() != itick)
			{
				stringstream(vbuffs[3]) >> itick->second.priceMarket;
				stringstream(vbuffs[4]) >> itick->second.priceYest;
				stringstream(vbuffs[5]) >> itick->second.priceOpen;
				stringstream(vbuffs[6]) >> itick->second.priceHigh;
				stringstream(vbuffs[7]) >> itick->second.priceLow;
				stringstream(vbuffs[8]) >> itick->second.tickTime;
				stringstream(vbuffs[10]) >> itick->second.volumeTotal;
				stringstream(vbuffs[11]) >> itick->second.volumeMarket;
				stringstream(vbuffs[12]) >> itick->second.amountTotal;
				stringstream(vbuffs[17]) >> itick->second.priceBid1;
				stringstream(vbuffs[18]) >> itick->second.priceAsk1;
				stringstream(vbuffs[19]) >> itick->second.volumeBid1;
				stringstream(vbuffs[20]) >> itick->second.volumeAsk1;
				if (itick->second.priceMarket <= 0.0)itick->second.priceMarket = itick->second.priceYest;
			}
		}
	}
	delete[] market;
	delete[] zqdm;
	delete[] Result;
	delete[] ErrInfo;
	return;
}
void CTra::CleanOpsition()
{
	if (vHolds.empty())return;
	vector<DECLARE> vDcls; vDcls.clear();
	static int cnt = 0;
	if (fmod(++cnt, 50) == 1) cout <<"STickMarcket "<< TimeHMS() << endl;
	for (auto ihold = vHolds.begin(); ihold != vHolds.end(); ++ihold)
	{
		if (ihold->isDeclaring)continue;
		LPTICKMARKET ptickmarket = &mSTickMarckets[ihold->stock.codes];
		//auto itickmarket = mSTickMarckets.find(ihold->stock.codes); 
		//if (mSTickMarckets.end() == itickmarket)continue;
		ihold->stock.marketPrice = ptickmarket->priceMarket;
		ihold->holdHighestPrice = max(ptickmarket->priceMarket, ihold->holdHighestPrice);
		ihold->holdLowestPrice = min(ptickmarket->priceMarket, ihold->holdLowestPrice);
		double drop = (ihold->stock.marketPrice / ihold->holdHighestPrice - 1)*100.0;
		double clmb = (ihold->stock.marketPrice / ihold->stock.costPrice - 1)*100.0;
		if (fmod(cnt, 50) == 1) cout << (int)(ihold - vHolds.begin()) + 1 << ". " << ihold->stock.codes << " " << ihold->stock.stkName << " cost " << ihold->stock.costPrice << " high " << ihold->holdHighestPrice
			<< " market " << ihold->stock.marketPrice << " pfit " << (ihold->stock.marketPrice / ihold->stock.costPrice - 1)*100.0 << " drop " << drop << endl;
		int holddays = ihold->holdDays;
		if (holddays <= 0)continue;
		if (drop < DROP || clmb > CLIMB || (holddays >= FUTURE && Time_Clean == TradingTime()))
		{
			DECLARE dcl; memset(&dcl, 0, sizeof(dcl));
			dcl.declareTime = TimeHMS();
			dcl.codes = ihold->stock.codes;
			dcl.stkName = ihold->stock.stkName;
			dcl.operate="SEL";
			dcl.declarePrice = ptickmarket->priceAsk1;
			dcl.declareVolume=ihold->stock.stkAvailable;
			dcl.dclId = dclIdLocal++;
			vDcls.push_back(dcl);
			ihold->isDeclaring = true;
		}
	}
	if (!vDcls.empty())
	{
		THREADREF threadref;
		threadref.callType = Call_Declare;
		threadref.vDclares = vDcls;
		HANDLE han = CreateThread(NULL, 0, threadTeminalCJ, (LPVOID)&threadref, 0, NULL);
		CloseHandle(han);
	}
}
void CTra::OpenOpsition()
{
	if (Time_Open != TradingTime())return;
	if (vCandidates.empty())return;
	vector<DECLARE> vDcls; vDcls.clear();
	for (auto icdd = vCandidates.begin(); icdd != vCandidates.end(); ++icdd)
	{
		if (icdd->isDeclaring)continue;
		LPTICKMARKET ptickmarket = &mSTickMarckets[icdd->codes];
		if (icdd->prefer < 5 - (int)vHolds.size())
		{
			DECLARE dcl; memset(&dcl, 0, sizeof(dcl));
			dcl.declareTime = TimeHMS();
			dcl.codes = icdd->codes;
			dcl.operate = "BUY";
			dcl.declarePrice = ptickmarket->priceBid1;
			dcl.declareVolume = (int)floor(min(0.2*account.money.totalAssets, account.money.amountAvailable) / ptickmarket->priceBid1 / 100) * 100;
			if (dcl.declareVolume>0)
			{
				dcl.dclId = dclIdLocal++;
				vDcls.push_back(dcl);
				icdd->isDeclaring = true;
			}
			else
			{
				cout << "OpenOpsition fail:" << dcl.codes << " declareVolume " << dcl.declareVolume << " totalAssets " << account.money.totalAssets
					<< " amountAvailable " << account.money.amountAvailable << " " << TimeHMS() << endl;
				Sleep(30 * 1000);
			}
		}
	}
	if (!vDcls.empty())
	{
		THREADREF threadref;
		threadref.callType = Call_Declare;
		threadref.vDclares = vDcls;
		HANDLE han = CreateThread(NULL, 0, threadTeminalCJ, (LPVOID)&threadref, 0, NULL);
		CloseHandle(han);
	}
}
void CTra::FreshAccount()
{
	static time_t tnow, tlst=0; time(&tnow);
	if (difftime(tnow, tlst) < 15 * 60)return; 
	if (tlst != 0 && mSints.empty())return;
	tlst = tnow;
	THREADREF threadref;
	threadref.callType = Call_Inqry;
	threadref.inqMode = Inqry_Midtime;
	HANDLE han = CreateThread(NULL, 0, threadTeminalCJ, (LPVOID)&threadref, 0, NULL);
	CloseHandle(han);
}
void CTra::UpdateAccountHoldCandidate()
{
	for (map<string, STOCK>::iterator istock = account.mstocks.begin(); istock != account.mstocks.end(); ++istock)
	{
		bool existed = false;
		for (vector<HOLD>::iterator ihold = vHolds.begin(); ihold != vHolds.end(); ++ihold)
		{
			if (ihold->stock.codes == istock->first)
			{
				ihold->stock = istock->second;
				existed = true; break;
			}
		}
		if (!existed)
		{
			HOLD hold; 
			hold.stock = istock->second;
			hold.updated = false;
			vHolds.push_back(hold);
		}
		if (mSTickMarckets.end() == mSTickMarckets.find(istock->first))
		{
			TICKMARKET tickmarket;
			mSTickMarckets.insert(map<string, TICKMARKET>::value_type(istock->first, tickmarket));
		}
	}
	bool existing = false;
	for (vector<HOLD>::iterator ihold = vHolds.begin(); ihold != vHolds.end(); ++ihold)
	{
		auto iitod = account.mtodtradeds.equal_range(ihold->stock.codes);
		auto iihis = account.mhistradeds.equal_range(ihold->stock.codes);
		if (iitod.first != std::end(account.mtodtradeds))
		{
			for (auto itod = iitod.first; itod != iitod.second; ++itod)
			{
				if (itod->second.operate == "证券买入")
				{
					ihold->trdDateLast = TimeYMD();
					ihold->holdDays = 0;
					ihold->holdHighestPrice = ihold->stock.marketPrice;
					ihold->holdLowestPrice = ihold->stock.marketPrice;
					ihold->closeDate = "";
					ihold->closePrice = 0.0;
					ihold->closeVolume = 0;
					ihold->isDeclaring = false;
					ihold->updated = true;
					strcpy(ihold->remark, "OpenFirst");
				}
				else if (itod->second.operate == "证券卖出" && ihold->stock.stkBalance <= 0)
				{
					ihold->closeDate = TimeYMD();
					ihold->closePrice = min(itod->second.trdPrice, ihold->closePrice);
					ihold->closeVolume += itod->second.trdVolume;
				}
			}
		}
		else if (iihis.first != std::end(account.mhistradeds) && !ihold->updated)
		{
			for (auto ihis = iihis.first; ihis != iihis.second; ++ihis)
			{
				if (atol(ihis->second.trdDate.c_str()) > atol(ihold->trdDateLast.c_str()))ihold->trdDateLast = ihis->second.trdDate;
				ihold->holdDays = CalcHoldDaysOnCalendar(ihold->trdDateLast);
				ihold->holdHighestPrice = ihold->stock.marketPrice;
				ihold->holdLowestPrice = ihold->stock.marketPrice;
				ihold->closeDate = "";
				ihold->closePrice = 0.0;
				ihold->closeVolume = 0;
				ihold->isDeclaring = false;
				ihold->updated = true;
				strcpy(ihold->remark, "OpenBefore");
			}
		}
		auto iietr = account.mtodentrusts.equal_range(ihold->stock.codes);
		if (iietr.first != std::end(account.mtodentrusts))
		{
			for (auto ietr = iietr.first; ietr != iietr.second; ++ietr)
			{
				if (ietr->second.operate == "证券卖出" && ietr->second.retreatVolume >= ietr->second.entrustVolume)
				{
					ihold->isDeclaring = false;
				}
			}
		}
	}
	for (auto icdd = vCandidates.begin(); icdd != vCandidates.end(); ++icdd)
	{
		auto iietr = account.mtodentrusts.equal_range(icdd->codes);
		if (iietr.first != std::end(account.mtodentrusts))
		{
			for (auto ietr = iietr.first; ietr != iietr.second; ++ietr)
			{
				if (ietr->second.operate == "证券买入" &&
					(ietr->second.retreatVolume >= ietr->second.entrustVolume ||
					ietr->second.trdVolume >= ietr->second.entrustVolume))
				{
					icdd->isDeclaring = false;
				}
				for (auto idcl = vDeclares.begin(); idcl != vDeclares.end();)
				{
					if (idcl->codes == ietr->first && idcl->operate == "证券买入")
						idcl->declareVolume -= (ietr->second.trdVolume + ietr->second.retreatVolume);
					if(idcl->declareVolume<=0)
						idcl = vDeclares.erase(idcl);
					else  ++idcl;
				}
			}
		}
	}
	return;
}
void CTra::DisConnectMarket()
{
	TdxHq_Disconnect();
	//cout << __FUNCTION__ << endl;
	return;
}
void CTra::TidyAccount()
{
	THREADREF threadref;
	threadref.callType = Call_Inqry;
	threadref.inqMode = Inqry_Afternoon;
	HANDLE han = CreateThread(NULL, 0, threadTeminalCJ, (LPVOID)&threadref, 0, NULL);
	CloseHandle(han);
	WaitForSingleObject(hevt[0], INFINITE);
	WriteAccountHold();
}
void CTra::CloseTeminalCJ()
{
	if (pCCJ)
	{
		pCCJ->Logout();
		delete pCCJ;
		pCCJ = NULL;
	}
}
void CTra::ReadAccountHold()
{
	string fn; stringstream ss; ss.clear(); ss.str("");
	ss << "P" << PREDAY << "F" << FUTURE << "H" << HISSDAY;
	fn = RstTrade + string("\\") + ss.str() + string("AccountHold.json");
	ifstream infile(fn, ios::in);
	if (infile.is_open())
	{
		Json::Reader reader;
		Json::Value root;
		if (reader.parse(infile, root))
		{
			account.date = root["date"].asString();
			account.money.totalAssets = root["money"]["totalAssets"].asDouble();
			account.money.balance = root["money"]["balance"].asDouble();
			account.money.amountAvailable = root["money"]["amountAvailable"].asDouble();
			account.money.amountDesirable = root["money"]["amountDesirable"].asDouble();
			account.money.marketValue = root["money"]["marketValue"].asDouble();
			account.money.holdProfit = root["money"]["holdProfit"].asDouble();
			for (auto ihold = root["mholds"].begin(); ihold != root["mholds"].end(); ++ihold)
			{
				if ((*ihold)["stock"]["stkBalance"].asInt() <= 0)continue;
				HOLD hold;
				hold.stock.codes = (*ihold)["stock"]["codes"].asString();
				hold.stock.stkName = (*ihold)["stock"]["stkName"].asString();
				hold.stock.stkBalance = (*ihold)["stock"]["stkBalance"].asInt();
				hold.stock.stkAvailable = (*ihold)["stock"]["stkAvailable"].asInt();
				hold.stock.costPrice = (*ihold)["stock"]["costPrice"].asDouble();
				hold.stock.marketPrice = (*ihold)["stock"]["marketPrice"].asDouble();
				hold.stock.profitPercent = (*ihold)["stock"]["profitPercent"].asDouble();
				hold.stock.profit = (*ihold)["stock"]["profit"].asDouble();
				hold.stock.marketValue = (*ihold)["stock"]["marketValue"].asDouble();
				hold.stock.marketName = (*ihold)["stock"]["marketName"].asString();
				hold.stock.accountId = (*ihold)["stock"]["accountId"].asString();
				hold.stock.profitPercent = (*ihold)["stock"]["profitPercent"].asDouble();
				hold.trdDateLast = (*ihold)["trdDateLast"].asString();
				hold.holdDays = CalcHoldDaysOnCalendar(hold.trdDateLast);
				hold.holdHighestPrice = (*ihold)["holdHighestPrice"].asDouble();
				hold.holdLowestPrice = (*ihold)["holdLowestPrice"].asDouble();
				hold.closeDate = (*ihold)["closeDate"].asString();
				hold.closePrice = (*ihold)["closePrice"].asDouble();
				hold.closeVolume = (*ihold)["closeVolume"].asInt();
				hold.isDeclaring = (*ihold)["isDeclaring"].asBool();
				hold.updated = false;
				strcpy(hold.remark, (*ihold)["remark"].asString().c_str());
				vHolds.push_back(hold);
			}
			Json::StyledWriter swrite;
			cout << __FUNCTION__ << endl << swrite.write(root) << endl;
		}
		else
		{
			cout << "parse error\n" << endl;
		}
		infile.close();
	}
	//cout << __FUNCTION__ << " totalAssets:" << trader.money.totalAssets << " amountAvailable:" << trader.money.amountAvailable << " curHolds:" << trader.curHolds << endl;
	return;
}
void CTra::WriteAccountHold()
{
	Json::Value root, money, mstocks, mtodentrusts, mholds;
	root["date"] = TimeYMD();
	money["totalAssets"] = Json::Value(account.money.totalAssets);
	money["balance"] = Json::Value(account.money.balance);
	money["amountAvailable"] = Json::Value(account.money.amountAvailable);
	money["amountDesirable"] = Json::Value(account.money.amountDesirable);
	money["marketValue"] = Json::Value(account.money.marketValue);
	money["holdProfit"] = Json::Value(account.money.holdProfit);
	root["money"] = Json::Value(money);
	root["mholds"] = Json::Value(mholds);
	for (auto ihold = vHolds.begin(); ihold != vHolds.end(); ihold++)
	{
		Json::Value hold, stock;
		stock["codes"] = Json::Value(ihold->stock.codes);
		stock["stkName"] = Json::Value(ihold->stock.stkName);
		stock["stkBalance"] = Json::Value(ihold->stock.stkBalance);
		stock["stkAvailable"] = Json::Value(ihold->stock.stkAvailable);
		stock["costPrice"] = Json::Value(ihold->stock.costPrice);
		stock["marketPrice"] = Json::Value(ihold->stock.marketPrice);
		stock["profitPercent"] = Json::Value(ihold->stock.profitPercent);
		stock["profit"] = Json::Value(ihold->stock.profit);
		stock["marketValue"] = Json::Value(ihold->stock.marketValue);
		stock["marketCode"] = Json::Value(ihold->stock.marketCode);
		stock["marketName"] = Json::Value(ihold->stock.marketName);
		stock["accountId"] = Json::Value(ihold->stock.accountId);
		hold["stock"] = Json::Value(stock);
		hold["trdDateLast"] = Json::Value(ihold->trdDateLast);
		hold["holdDays"] = Json::Value(ihold->holdDays);
		hold["holdHighestPrice"] = Json::Value(ihold->holdHighestPrice);
		hold["holdLowestPrice"] = Json::Value(ihold->holdLowestPrice);
		hold["closeDate"] = Json::Value(ihold->closeDate);
		hold["closePrice"] = Json::Value(ihold->closePrice);
		hold["closeVolume"] = Json::Value(ihold->closeVolume);
		hold["isDeclaring"] = Json::Value(ihold->isDeclaring);
		hold["updated"] = Json::Value(ihold->updated);
		hold["remark"] = Json::Value(ihold->remark);
		root["mholds"].append(hold);
	}
	root["mtodentrusts"] = Json::Value(mtodentrusts);
	for (auto itodentrust = account.mtodentrusts.begin(); itodentrust != account.mtodentrusts.end(); itodentrust++)
	{
		Json::Value todentrust;
		todentrust["entrustTime"] = Json::Value(itodentrust->second.entrustTime);
		todentrust["codes"] = Json::Value(itodentrust->second.codes);
		todentrust["stkName"] = Json::Value(itodentrust->second.stkName);
		todentrust["operate"] = Json::Value(itodentrust->second.operate);
		todentrust["remarks"] = Json::Value(itodentrust->second.remarks);
		todentrust["entrustVolume"] = Json::Value(itodentrust->second.entrustVolume);
		todentrust["trdVolume"] = Json::Value(itodentrust->second.trdVolume);
		todentrust["retreatVolume"] = Json::Value(itodentrust->second.retreatVolume);
		todentrust["entrustPrice_protectPrice"] = Json::Value(itodentrust->second.entrustPrice_protectPrice);
		todentrust["isProtectPrice"] = Json::Value(itodentrust->second.isProtectPrice);
		todentrust["trdPrice"] = Json::Value(itodentrust->second.trdPrice);
		todentrust["businessName"] = Json::Value(itodentrust->second.businessName);
		todentrust["entrustId"] = Json::Value(itodentrust->second.entrustId);
		todentrust["declareId"] = Json::Value(itodentrust->second.declareId);
		root["mtodentrusts"].append(todentrust);
	}
	string fn; stringstream ss; ss.clear(); ss.str("");
	ss << "P" << PREDAY << "F" << FUTURE << "H" << HISSDAY;
	fn = RstTrade + string("\\") + ss.str() + string("AccountHold.json");
	ofstream outfile(fn, ios::out);
	if (outfile.is_open())
	{
		Json::StyledWriter swriter;
		outfile << swriter.write(root);
		outfile.close();
		cout << __FUNCTION__ << endl << swriter.write(root) << endl;
	}
	return;
}
int CTra::CalcHoldDaysOnCalendar(string dateYYYYMMDD)
{
	unsigned int date = (unsigned int)atol(dateYYYYMMDD.c_str());
	if (date== SysDate())return 0;
	deque<unsigned int>Calendar; Calendar.clear();
	GetCalendar(Calendar); if (Calendar.empty())return 0;
	deque<unsigned int>::iterator beg = Calendar.begin();
	while (*beg < date && beg < Calendar.end())++beg;
	if (Calendar.back() < SysDate())return (int)(Calendar.end() - beg) - 1;
	else return  (int)(Calendar.end() - beg);
}
DWORD WINAPI threadTeminalCJ(LPVOID lpParam)
{
	THREADREF *pThreadref = new THREADREF();
	pThreadref=(THREADREF*)lpParam;
	if (Call_Inqry == pThreadref->callType)
	{
		//ShowWindow(pCTrading->consolHwnd, SW_HIDE);
		pCCJ->Inqury(pThreadref->inqMode);
		//ShowWindow(pCTrading->consolHwnd, SW_NORMAL);
		pCTrading->UpdateAccountHoldCandidate();
		SetEvent(hevt[0]);
		static unsigned int uLastTime = 0;
		for (auto itod = account.mtodentrusts.begin(); itod != account.mtodentrusts.end(); ++itod)
		{
			auto iint = pCTrading->mSints.find(itod->first);
			if (iint != std::end(pCTrading->mSints))
			{
				unsigned int uEntrustTime = atol(itod->second.entrustTime.c_str());
				if (uLastTime<uEntrustTime)
				{
					iint->second -= itod->second.retreatVolume;
					iint->second -= itod->second.trdVolume;
					uLastTime = uEntrustTime;
				}
				if (iint->second < 0)
					pCTrading->mSints.erase(iint);
			}
		}
	}
	else if (Call_Declare == pThreadref->callType)
	{
		//ShowWindow(pCTrading->consolHwnd, SW_HIDE);
		pCCJ->Declare(pThreadref->vDclares);
		//ShowWindow(pCTrading->consolHwnd, SW_NORMAL);
		for (auto idcl = pThreadref->vDclares.begin(); idcl != pThreadref->vDclares.end(); ++idcl)
		{
			auto iint = pCTrading->mSints.find(idcl->codes);
			if (iint == std::end(pCTrading->mSints))
			{
				auto res=pCTrading->mSints.insert(map<string, int>::value_type(idcl->codes, 0));
				if (res.second)iint = res.first;
			}
			iint->second += idcl->declareVolume;
			pCTrading->vDeclares.push_back(*idcl);			
		}
	}
	return 0;
}