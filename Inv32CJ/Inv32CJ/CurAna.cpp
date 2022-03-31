//#include <algorithm>
//#include<math.h>
//#include<iomanip>
//#include<process.h>
#include"CurAna.h"
extern string MktDatas;
extern string RulLearn;
extern HANDLE hmut;
//string DatasFZ = string("F:\\ScDatas\\HisFZ5");
string DirStd = string("F:\\ScDatas\\INVESTING\\Inv64-2\\Result");
CAna *pCAna;
CAna::CAna(bool l_u, deque<unsigned int>&clndr)
{
	if (access(DirStd.c_str(), 0) != 0)mkdir(DirStd.c_str());
	hmut = CreateMutex(NULL, FALSE, NULL);
	this->calendar = clndr;
	L_U = l_u;
	DateReadIn = 0; LearnTill = 0;
	vcombs.clear(); vcombs.swap(vector<vector<TD>>());
	if (L_U)
	{
		vector<TD>vtds; vtds.clear(); vtds.swap(vector<TD>(vtds));
		for (int t = 0; t < TYPES; t++)
		for (int d = 0; d < DECAS; d++)
		{
			TD td;
			td.t = t;
			td.d = d;
			vtds.push_back(td);
		}
		vtds.shrink_to_fit();
		vcombs.reserve(30000);
		int num = 0; while (++num <= (int)min(vtds.size(), 5))
		{
			vector<TD>vtemp(num, { 0, 0 });
			Combination2(vcombs, vtds, vtemp, 0, 0, (int)vtds.size() - 1, num); vtemp.clear();
		}

		msmisumyss.clear();
		stringstream ss; ss.clear();
		ss << RulLearn << "\\P" << PREDAY << "F" << FUTURE << "H" << HISSDAY << "msmisumyss.txt";
		string fn = ss.str(); ss.str("");
		ifstream infile1(fn, ios::in);
		if (infile1.is_open())
		{
			infile1.seekg(0, ios::beg);
			char line[256] = { 0 };
			infile1.getline(line, sizeof(line));
			string notes;
			stringstream sss(line); sss >> notes >> DateReadIn; sss.str("");
			if (notes == string("LearnTill") && DateReadIn>20150101)
			{
				LearnTill = DateReadIn;
				while (infile1.getline(line, sizeof(line)))
				{
					string item; map<unsigned int, Sumy>misumys; int dsize = 0;
					sss.clear(); sss << line; sss >> item >> dsize; sss.str("");
					for (int i = 0; i < dsize; ++i)
					{
						unsigned int date; Sumy sumy; int csize = 0;
						infile1.getline(line, sizeof(line));
						sss.clear(); sss << line; sss >> date >> sumy.days >> sumy.todfit >> sumy.sumfit >> sumy.topfit >> sumy.numget >> sumy.todget >> sumy.sumget >> sumy.topget;
						for (int j = 0; j < FUTURE; ++j)sss >> sumy.nums[j]; sss >> csize; sss.str("");
						for (int k = 0; k < csize; ++k)
						{
							infile1.getline(line, sizeof(line));
							Cnf cnf;
							sss.clear(); sss << line; sss >> cnf.codes >> cnf.fit >> cnf.clsd >> cnf.simfit; sss.str("");
							sumy.mscnfs.insert(cnf);
						}
						misumys.insert(map<unsigned int, Sumy> ::value_type(date, sumy));
					}
					msmisumyss.insert(map<string, map<unsigned int, Sumy>>::value_type(item, misumys));
				}
			}
			infile1.close();
		}
	}
	if (!L_U)
	{
		vcombs.reserve(100);
		stringstream ss; ss.clear();
		ss << RulLearn << "\\P" << PREDAY << "F" << FUTURE << "H" << HISSDAY << "Learn.txt";
		string fn = ss.str(); ss.str("");
		ifstream infile(fn, ios::in);
		if (infile.is_open())
		{
			char line[1024] = { 0 }; stringstream ssbuf;
			infile.seekg(0, ios::beg);
			while (infile.getline(line, sizeof(line)))
			{
				if (string(line).compare(0, ss.str().size(), ss.str()) == 0)
				{
					vector<TD>vtds; vtds.clear(); vtds.swap(vector<TD>(vtds));
					size_t pos = ss.str().size();
					while (pos < string(line).size())
					{
						if (string(line).substr(pos, 1) == string("_"))
						{
							TD td; memset(&td, 0, sizeof(td));
							ssbuf.clear(); ssbuf << line[pos + 1];  ssbuf >> td.t; ssbuf.str("");
							ssbuf.clear(); ssbuf << line[pos + 2];  ssbuf >> td.d; ssbuf.str("");
							vtds.push_back(td);
						}
						else if (string(line).substr(pos, 1) == string(" "))break;
						++pos;
					}
					vtds.shrink_to_fit();
					vcombs.push_back(vtds);
				}
				else if (string(line).compare(0, 3, "202") == 0)
				{
					ssbuf.clear(); ssbuf << line; ssbuf >> DateReadIn; ssbuf.str("");
					LearnTill = DateReadIn;
				}
			}
			infile.close();
		}
	}
	vcombs.shrink_to_fit();
}
CAna::~CAna()
{
	if (L_U && LearnTill>DateReadIn)
	{
		string fn; stringstream ss; ss.clear();
		ss << DirStd << "\\P" << PREDAY << "F" << FUTURE << "H" << HISSDAY;
		fn = ss.str() + string("msmisumyss.txt"); ss.str("");
		ofstream outfile1(fn, ios::out | ios::trunc);
		if (outfile1.is_open())
		{
			outfile1 << "LearnTill " << LearnTill << endl;
			for (auto imisumys = msmisumyss.begin(); imisumys != msmisumyss.end(); ++imisumys)
			{
				outfile1 << imisumys->first << " " << (int)imisumys->second.size() << endl;
				for (auto iisumy = imisumys->second.begin(); iisumy != imisumys->second.end(); ++iisumy)
				{
					outfile1 << iisumy->first << " " << iisumy->second.days << " " << iisumy->second.todfit << " " << iisumy->second.sumfit << " " << iisumy->second.topfit << " " << iisumy->second.numget
						<< " " << iisumy->second.todget << " " << iisumy->second.sumget << " " << iisumy->second.topget; for (int i = 0; i < FUTURE; ++i)outfile1 << " " << iisumy->second.nums[i];
					outfile1 << " " << iisumy->second.mscnfs.size() << endl;
					for (auto isum = iisumy->second.mscnfs.begin(); isum != iisumy->second.mscnfs.end(); ++isum)
					{
						outfile1 << isum->codes << " " << isum->fit << " " << isum->clsd << " " << isum->simfit << endl;
					}
				}
			}
			outfile1.close();
		}
	}
	CloseHandle(hmut);
	//std::cout << __FUNCTION__ << " L_U=" << L_U << " DateReadIn " << DateReadIn << " LearnTill " << LearnTill <<" " << TimeHMSYMD() << endl;
}
unsigned int CAna::Start()
{
	int reservedays = PREDAY + (int)max(FUTURE, HISSDAY);
	if ((int)calendar.size()<reservedays)return LearnTill;
	deque<unsigned int>::iterator dstart = calendar.begin();
	while (LearnTill != 0 && dstart + reservedays<calendar.end() && *(dstart + reservedays) <= LearnTill)++dstart;
	calendar.erase(calendar.begin(), dstart);
	WaitForSingleObject(hmut, INFINITE);
	if (L_U)
		std::cout << endl << "Calendar P" << PREDAY << "H" << HISSDAY << "F" << FUTURE << " " << calendar.front() << "-" << calendar.back()
		<< " LearnTill " << LearnTill << " Learning " << (int)calendar.size() - reservedays - FUTURE << " " << TimeHMSYMD() << endl;
	if (!L_U)
		std::cout << endl << "Calendar P" << PREDAY << "H" << HISSDAY << "F" << FUTURE << " " << calendar.front() << "-" << calendar.back()
		<< " LearnTill " << LearnTill << " Using at back " << TimeHMSYMD() << endl;
	ReleaseMutex(hmut);
	ReadCominf();
	for (deque<unsigned int>::iterator dtod = calendar.begin(), dfut = dtod; dtod < calendar.end(); ++dtod, ++dfut)
	{
		if (dfut == calendar.begin() + FUTURE)
			dtod -= FUTURE;
		if (dfut < calendar.end())
			ReadFzs(dfut);
		if (dfut == calendar.end()) --dfut;
		if (dtod < calendar.begin() + (int)max(PREDAY, HISSDAY))continue;
		if (L_U && dtod + FUTURE >= calendar.end())break;//////////////////////////////////////////////////////////////
		map<string, Fram>mframs;
		GetCap(dtod, mframs);
		for (map<string, map<unsigned int, Fz*>>::iterator mlfzss = mmlfzsss.begin(); mlfzss != mmlfzsss.end(); ++mlfzss)
		{
			if (mlfzss->second.size() <= PREDAY)continue;
			GetFzs(dtod, mframs, mlfzss);
		}
		vector<Fram> vframs;
		for (auto frm = mframs.begin(); frm != mframs.end(); frm++)
			vframs.push_back(frm->second);
		vframs.shrink_to_fit();
		std::sort(vframs.begin(), vframs.end(), CmpByRefChgs);
		std::partial_sort(vframs.begin(), vframs.begin() + (int)min(300, vframs.size()), vframs.end(), CmpByRefMnys);
		map<unsigned int, map<string, Fram>>::iterator imsframs = mmframss.find(*dtod);
		if (mmframss.end() == imsframs)
		{
			mframs.clear();
			auto res = mmframss.insert(map<unsigned int, map<string, Fram>>::value_type(*dtod, mframs));
			if (res.second)imsframs = res.first;
		}
		for (vector<Fram>::iterator ifram = vframs.begin(); ifram <vframs.begin() + (int)min(100, vframs.size()); ++ifram)
			imsframs->second.insert(map<string, Fram>::value_type(ifram->scu.cd, *ifram));
		vframs.clear(); vframs.swap(vector<Fram>());

		if (mmframss.empty() || (dtod - HISSDAY) == calendar.end() || *(dtod - HISSDAY) < mmframss.begin()->first)continue;
		if (L_U && *dtod <= LearnTill)continue;
		if (!L_U && dtod < dfut)continue;
		map<string, vector<His>>msvhiss;
		for (map<string, Fram>::iterator ifram = imsframs->second.begin(); ifram != imsframs->second.end(); ++ifram)
		{
			vector<His> vhis; vhis.clear();
			for (map<unsigned int, map<string, Fram>>::iterator imsframsy = mmframss.begin(); imsframsy != mmframss.end(); ++imsframsy)
			{
				if (imsframsy->first < *(dtod - HISSDAY))continue;
				if (imsframsy == imsframs)continue;
				for (map<string, Fram>::iterator iframy = imsframsy->second.begin(); iframy != imsframsy->second.end(); ++iframy)
				{
					His his;  his.free = 0.0;
					his.afit = (iframy->second.clsd <= 0) ? 0.0 : iframy->second.fit / iframy->second.clsd;
					for (int nn = 0; nn < TYPES; nn++)
					for (int dd = 0; dd < DECAS; dd++)
					{
						his.std[nn][dd] = (iframy->second.cnt[nn][dd] - ifram->second.cnt[nn][dd])*(iframy->second.cnt[nn][dd] - ifram->second.cnt[nn][dd]);
					}
					vhis.push_back(his);
				}
				vhis.shrink_to_fit();
				for (int nn = 0; nn < TYPES; nn++)
				for (int dd = 0; dd < DECAS; dd++)
				{
					for (vector<His>::iterator hs = vhis.begin(); hs != vhis.end(); ++hs)
						hs->free = hs->std[nn][dd];
					std::sort(vhis.begin(), vhis.end(), CmpByHisfree);
					for (vector<His>::iterator hs = vhis.begin(); hs != vhis.end(); ++hs)
						hs->std[nn][dd] = int(hs - vhis.begin() + 1);
				}
			}
			msvhiss.insert(map<string, vector<His>>::value_type(ifram->first, vhis));
		}
		Config(dtod, msvhiss);
		msvhiss.clear();
		while (mmframss.begin()->first <= *(dtod - HISSDAY))
			mmframss.erase(mmframss.begin());
		LearnTill = *dtod;
		if (AvaiMem() < 1000)break;
	}
	return LearnTill;
}
void CAna::ReadCominf()
{
	mcompanyinfos.clear();
	string fn = MktDatas + "\\" + "captals.bin";
	ifstream infile(fn, ios::binary | ios::in);
	if (infile.is_open())
	{
		Scu scu; infile.seekg(0, ios::beg);
		int date; infile.read((char*)&date, sizeof(date));
		while (infile.read((char*)&scu, sizeof(scu)))
		{
			if ((scu.imkt != 0 && scu.imkt != 1) || (scu.type<0 && scu.type>3))continue;
			//ScCodeType codes; 
			//std::strcpy(codes.code, (((scu.imkt == 0) ? string("sz") : string("sh")) + string(scu.cd)).c_str());
			//std::strcpy(codes.name, scu.nm);
			unsigned int listdate = 0;
			infile.read((char*)&listdate, sizeof(listdate));
			unsigned int size = 0;
			infile.read((char*)&size, sizeof(size));
			if (size > 0)
			{
				auto md = mcompanyinfos.find(scu.cd);
				if (mcompanyinfos.end() == md)
				{
					CompanyInfo companyinfo;
					companyinfo.vcapitals.reserve(size); companyinfo.listdate = listdate; companyinfo.scu = scu;
					auto res = mcompanyinfos.insert(map<string, CompanyInfo>::value_type(scu.cd, companyinfo));
					if (res.second)md = res.first;
				}
				for (unsigned int i = 0; i < size; i++)
				{
					Capital captal;
					infile.read((char*)&captal, sizeof(Capital));
					//if (captal.current>0)
					md->second.vcapitals.push_back(captal);
				}
			}
			//if (mcompanyinfos.size()>6)break;
		}
		infile.close();
	}
	return;
}
void CAna::GetCap(const deque<unsigned int>::iterator dtod, map<string, Fram>&mframs)
{
	for (auto companyinfo = mcompanyinfos.begin(); companyinfo != mcompanyinfos.end(); companyinfo++)
	{
		if ((*dtod - companyinfo->second.listdate) / 100 < 6)continue;
		auto frm = mframs.find(companyinfo->first);
		if (mframs.end() == frm)
		{
			Fram fram; memset(&fram, 0, sizeof(fram));
			fram.scu = companyinfo->second.scu;
			//fram.date = *dtod;
			auto res = mframs.insert(map<string, Fram>::value_type(companyinfo->first, fram));
			if (res.second)frm = res.first;
		}
		for (int d = 0; d <= PREDAY; d++)
		{
			for (auto cap = companyinfo->second.vcapitals.begin(); cap != companyinfo->second.vcapitals.end(); cap++)
			{
				if (cap->date <= *(dtod - PREDAY + d))
				{
					frm->second.current[d] = cap->current;
					break;
				}
			}
		}
		if (frm->second.current[PREDAY] * frm->second.current[PREDAY - 1] <= 0.0)
		{
			mframs.erase(frm);
		}
	}
	return;
}
void CAna::ReadFzs(const deque<unsigned int>::iterator dfut)
{
	stringstream sss; ifstream infile;
	sss.clear(); sss.str(""); sss << *dfut;
	infile.open(MktDatas + string("\\") + sss.str() + string(".5"), ios::binary | ios::in);
	if (!infile.is_open())
		infile.open(MktDatas + string("\\") + sss.str() + string("H.5"), ios::binary | ios::in);
	if (infile.is_open())
	{
		Scu scu; infile.seekg(0, ios::beg);
		Fz fenz[48];
		while (infile.read((char*)&scu, sizeof(scu)))
		{
			if (infile.read((char*)fenz, 48 * sizeof(Fz)))
			{
				if ((scu.imkt == 0 || scu.imkt == 1) && 0 <= scu.type && scu.type <= 3)
				{
					if (scu.type < 2)
					{
						auto mlfzss = mmlfzsss.find(scu.cd);
						if (mmlfzsss.end() == mlfzss)
						{
							map<unsigned int, Fz*>mlfenzss;
							auto res = mmlfzsss.insert(map<string, map<unsigned int, Fz*>>::value_type(scu.cd, mlfenzss));
							if (res.second)mlfzss = res.first;
						}
						Fz *fzs = new Fz[48]();
						memcpy(fzs, fenz, 48 * sizeof(Fz));
						mlfzss->second.insert(map<unsigned int, Fz*>::value_type(*dfut, fzs));
						if (dfut >= calendar.begin() + PREDAY + FUTURE)
						{
							while (mlfzss->second.begin()->first < *(dfut - PREDAY - FUTURE))
							{
								delete[] mlfzss->second.begin()->second;
								mlfzss->second.begin()->second = NULL;
								mlfzss->second.erase(mlfzss->second.begin());
							}
						}
					}
				}
			}
		}
		infile.close();
		//delete[] fzs; fzs = NULL;
	}
	return;
}
void CAna::GetFzs(const deque<unsigned int>::iterator dtod, map<string, Fram>&mframs, map<string, map<unsigned int, Fz*>> ::iterator mlfzss)
{
	map<string, Fram>::iterator frm = mframs.find(mlfzss->first);
	if (mframs.end() == frm)return;
	float opn = 0.0, hig = 0.0, low = 0.0, cls = 0.0, ave = 0.0, clsy = 0.0;
	double mny = 0.0, vlm = 0.0;
	float higs[DECAS] = { 0.0 }, lows[DECAS] = { 0.0 };
	double mnys[DECAS] = { 0.0 }, mnyy[DECAS] = { 0.0 }, vlms[DECAS] = { 0.0 }, chgs[DECAS] = { 0.0 };
	bool overed = false, canbuy = false;
	for (deque<unsigned int>::iterator ddat = dtod - PREDAY; ddat <= dtod + FUTURE; ddat++)
	{
		if (ddat >= calendar.end() || *ddat > mlfzss->second.rbegin()->first)break;
		auto lfzs = mlfzss->second.find(*ddat); if (mlfzss->second.end() == lfzs)continue;
		for (auto fz = lfzs->second; fz < lfzs->second + 48; fz++)
		{
			//Fz* fenz = afzs->second + i;
			unsigned int date = (unsigned int)(abs(fz->dt / 2048) + 2004) * 10000 + (unsigned int)fmod(fz->dt, 2048);
			int time = (int)fz->tm / 60 * 100 + (int)fmod(fz->tm, 60);
			int nn = (fz->tm - 575) / 5; if (nn >= 24)nn -= 18;
			opn = fz->op / 1000.000f;
			hig = fz->hg / 1000.000f;
			low = fz->lw / 1000.000f;
			cls = fz->cl / 1000.000f;
			mny = (double)fz->mn*100.0;
			vlm = (double)fz->vm*100.0;
			ave = ((float)vlm <= 0) ? 0.0f : (float)mny / (float)vlm;
			int cur = (int)(ddat - (dtod - PREDAY)) * 48 + nn;
			int ent = PREDAY * 48 + 24;
			int his = (PREDAY - HISSDAY) * 48 + 24;
			if (date == *ddat && nn<48 && opn > 0 && hig > 0 && low > 0 && cls>0 && ave>0)
			{
				for (int dd = 0; dd < DECAS; ++dd)//5min,20min,80min,1day,3day
				{
					int clc = 3 * (int)pow(MULT, dd);
					int beg = ent - clc;
					if (0 <= cur && cur < beg)
					{
						mnyy[dd] += mny;
					}
					if (beg <= cur && cur < ent)
					{
						mnys[dd] += mny;
						vlms[dd] += vlm;
						higs[dd] = max(hig, higs[dd]);
						lows[dd] = (lows[dd] <= 0.0) ? low : min(low, lows[dd]);
						chgs[dd] += (0 <= (PREDAY - (ent - cur + 23) / 48) && (PREDAY - (ent - cur + 23) / 48) <= PREDAY) ? vlm / frm->second.current[PREDAY - (ent - cur + 23) / 48] * 100.0 : 0.0;
					}
					if (cur == ent - 1)
					{
						frm->second.cnt[0][dd] = (mnys[dd] * vlms[dd] <= 0.0) ? 0.0 : (cls / (mnys[dd] / vlms[dd]) - 1)*100.0;//cat
						frm->second.cnt[1][dd] = chgs[dd];//chg
						frm->second.cnt[2][dd] = (mnyy[dd] <= 0.0) ? 0.0 : mnys[dd] / mnyy[dd];//mnr
						frm->second.cnt[3][dd] = (higs[dd] + lows[dd] <= 0.0) ? 0.0 : (higs[dd] - lows[dd]) / (higs[dd] + lows[dd]) / 0.5*100.0;//rng
					}
				}
				if (his <= cur && cur < ent)
				{
					frm->second.sormnys += mny;
					//frm->second.sorchgs += (0 <= (PREDAY - (ent - cur + 24) / 48) && (PREDAY - (ent - cur + 24) / 48) <= PREDAY) ? vlm / frm->second.current[PREDAY - (ent - cur + 24) / 48] * 100.0 : 0.0;
					frm->second.sorchgs += (0 <= (PREDAY - (ent - cur + 23) / 48) && (PREDAY - (ent - cur + 23) / 48) <= PREDAY) ? vlm / frm->second.current[PREDAY - (ent - cur + 23) / 48] * 100.0 : 0.0;
				}
				if (ent - 48 - 24 <= cur && cur < ent - 48 + 24)clsy = cls;
				if (cur == ent - 1)
				if ((cls / clsy - 1)*100.0 < 9.8)
					canbuy = true;
				if (ent <= cur && !overed)
				{
					if (frm->second.posopn <= 0)frm->second.posopn = ave;
					frm->second.poshig = max(hig, frm->second.poshig);
					if (ent + 24 <= cur)
					{
						frm->second.poscls = cls;
						float drop = (ave / frm->second.poshig - 1)*100.0f;
						float clmb = (ave / frm->second.posopn - 1)*100.0f;
						frm->second.clsd = (int)(ddat - dtod);
						frm->second.fit = clmb;
						if (drop < DROP || clmb > CLIMB)
							overed = true;
					}
				}
				if (overed && frm->second.clsd <= 0)
					canbuy = false;
			}
		}
	}
	if (!canbuy)
		mframs.erase(frm);
	return;
}
void CAna::Config(const deque<unsigned int>::iterator dtod, map<string, vector<His>>&msvhiss)
{
	map<unsigned int, map<string, Fram>>::iterator imsframs = mmframss.find(*dtod); if (mmframss.end() == imsframs)return;
	multiset<SumyTop>mssumytops; double stopfits = 0.0, stopgets = 0.0;
	for (vector<vector<TD>>::iterator icomb = vcombs.begin(); icomb != vcombs.end(); ++icomb)
	{
		stringstream ss; ss.clear(); ss << "P" << PREDAY << "F" << FUTURE << "H" << HISSDAY << "TD";
		for (vector<TD>::iterator itd = icomb->begin(); itd != icomb->end(); itd++)ss << "_" << itd->t << itd->d;
		map<string, map<unsigned int, Sumy>>::iterator imisumys = msmisumyss.find(ss.str());
		if (msmisumyss.end() == imisumys)
		{
			map<unsigned int, Sumy> misumys;
			auto res = msmisumyss.insert(map<string, map<unsigned int, Sumy>>::value_type(ss.str(), misumys));
			if (res.second)imisumys = res.first;
		}
		ss.str("");
		Sumy sumy, sumy_yes;
		if (!imisumys->second.empty())
			sumy_yes = imisumys->second.rbegin()->second;
		else
		{
			sumy_yes.days = 0; sumy_yes.mscnfs.clear();
			for (int dd = 0; dd < FUTURE; ++dd)sumy_yes.nums[dd] = 0;
			sumy_yes.todfit = 0.0; sumy_yes.sumfit = 0.0; sumy_yes.topfit = 0.0;
			sumy_yes.numget = 0; sumy_yes.todget = 0.0; sumy_yes.sumget = 0.0; sumy_yes.topget = 0.0;
		}
		sumy.days = sumy_yes.days + 1; sumy.mscnfs.clear();
		for (int dd = 0; dd < FUTURE - 1; ++dd)sumy.nums[dd] = sumy_yes.nums[dd + 1]; sumy.nums[FUTURE - 1] = 0;
		sumy.todfit = 0.0; sumy.sumfit = 0.0; sumy.topfit = 0.0;
		sumy.numget = 0; sumy.todget = 0.0; sumy.sumget = 0.0; sumy.topget = 0.0;

		for (map<string, Fram>::iterator ifram = imsframs->second.begin(); ifram != imsframs->second.end(); ++ifram)
		{
			map<string, vector<His>>::iterator svhis = msvhiss.find(ifram->first);
			if (msvhiss.end() == svhis)continue;
			if (svhis->second.empty())continue;
			for (vector<His>::iterator hs = svhis->second.begin(); hs != svhis->second.end(); ++hs)
			{
				hs->free = 0.0;
				for (vector<TD>::iterator itd = icomb->begin(); itd != icomb->end(); itd++)
					hs->free += hs->std[itd->t][itd->d];
			}
			int size = (int)min(ceil(0.1*svhis->second.size()), 30);
			std::partial_sort(svhis->second.begin(), svhis->second.begin() + size, svhis->second.end(), CmpByHisfree);

			Cnf cnf; cnf.simfit = 0.0;
			cnf.codes = ifram->first; cnf.fit = ifram->second.fit; cnf.clsd = ifram->second.clsd;
			for (vector<His>::iterator hs = svhis->second.begin(); hs < svhis->second.begin() + size; ++hs)
				cnf.simfit += hs->afit;
			cnf.simfit /= size;
			if (cnf.simfit > 0.0)
				sumy.mscnfs.insert(cnf);
			if (sumy.mscnfs.size() > 5)
				sumy.mscnfs.erase((--sumy.mscnfs.end()));
		}
		for (multiset<Cnf>::iterator icf = sumy.mscnfs.begin(); icf != sumy.mscnfs.end(); ++icf)
		{
			sumy.todfit += (icf->clsd <= 0) ? 0.0 : icf->fit / icf->clsd;
			if (sumy.nums[0]< 5)
			{
				for (int dd = 0; dd < icf->clsd; ++dd)sumy.nums[dd]++;
				sumy.todget += 0.2*icf->fit;
				sumy.numget++;
			}
		}
		if (!sumy.mscnfs.empty())sumy.todfit /= sumy.mscnfs.size();
		sumy.sumfit = sumy.todfit + sumy_yes.sumfit;
		sumy.topfit = max(sumy.sumfit, sumy_yes.topfit);
		stopfits = max(sumy.topfit, stopfits);
		sumy.sumget = sumy.todget + sumy_yes.sumget;
		sumy.topget = max(sumy.sumget, sumy_yes.topget);
		stopgets = max(sumy.topget, stopgets);
		imisumys->second.insert(map<unsigned int, Sumy>::value_type(*dtod, sumy));
		while (imisumys->second.begin()->first < *(dtod - PREDAY))
			imisumys->second.erase(imisumys->second.begin());
		SumyTop sumytop; sumytop.item = imisumys->first;
		sumytop.sumfit_sort = sumy.sumfit; sumytop.sumget_sort = 0.0;
		mssumytops.insert(sumytop);
		while (mssumytops.size() > 100)
			mssumytops.erase(--mssumytops.end());
	}
	Record(dtod, mssumytops, stopfits, stopgets);
	mssumytops.clear();
	return;
}
void CAna::Record(const deque<unsigned int>::iterator dtod, multiset<SumyTop>&mssumytops, double stopfits, double stopgets)
{
	string nowtime = TimeHMSYMD();
	string fn; stringstream ss; ss.clear();
	ss << DirStd << "\\P" << PREDAY << "F" << FUTURE << "H" << HISSDAY;
	if (L_U)
	{
		multiset<SumyTop>mssumytops_learn;
		fn = ss.str() + string("LearnDetail.txt");
		ofstream outfile1(fn, ios::out | ios::trunc); if (!outfile1.is_open())return;
		for (multiset<SumyTop>::iterator imisumys = mssumytops.begin(); imisumys != mssumytops.end(); ++imisumys)
		{
			map<string, map<unsigned int, Sumy>>::iterator ismisumys = msmisumyss.find(imisumys->item); if (msmisumyss.end() == ismisumys)continue;
			outfile1 << ismisumys->first << endl;
			for (auto iisumy = ismisumys->second.rbegin(); iisumy != ismisumys->second.rend(); iisumy++)
			{
				outfile1 << "    " << iisumy->second.days << " " << iisumy->first << " dnm " << iisumy->second.mscnfs.size() << " todfit " << iisumy->second.todfit
					<< " sumfit " << iisumy->second.sumfit << " topfit " << iisumy->second.topfit << " stopfits " << stopfits << endl;
				int dnm = 0; for (auto it = iisumy->second.mscnfs.begin(); it != iisumy->second.mscnfs.end(); it++)
					outfile1 << "        " << ++dnm << " " << it->codes << " simfit " << it->simfit << " clsd " << it->clsd << " fit " << it->fit << " get " << (dnm < iisumy->second.numget ? it->fit*0.2 : 0.0) << endl;
				outfile1 << "        Exist"; for (int dd = 0; dd < FUTURE; dd++) outfile1 << " " << iisumy->second.nums[dd];
				outfile1 << " numget " << iisumy->second.numget << " todget " << iisumy->second.todget << " sumget " << iisumy->second.sumget << " topget " << iisumy->second.topget << " stopgets " << stopgets << endl;
				if (imisumys == mssumytops.begin() && iisumy == ismisumys->second.rbegin())
				{
					std::cout << iisumy->second.days << " " << iisumy->first << " " << ismisumys->first << " todfit " << iisumy->second.todfit << " sumfit " << iisumy->second.sumfit
						<< " topfit " << iisumy->second.topfit << " stopfits " << stopfits << " |numget " << iisumy->second.numget << " todget " << iisumy->second.todget
						<< " sumget " << iisumy->second.sumget << " topget " << iisumy->second.topget << " stopgets " << stopgets << " " << nowtime << "    " << GetMem() << endl;
				}
			}
			SumyTop sumytop = *imisumys;
			sumytop.sumget_sort = ismisumys->second.rbegin()->second.sumget;
			mssumytops_learn.insert(sumytop);
			if (mssumytops_learn.size() > 10)
				mssumytops_learn.erase(--mssumytops_learn.end());
		}
		outfile1.close();
		fn = ss.str() + string("Learn.txt"); ss.str("");
		ofstream outfile2(fn, ios::out | ios::trunc); if (!outfile2.is_open())return;
		outfile2 << *dtod << endl;
		for (multiset<SumyTop>::iterator imisumys = mssumytops_learn.begin(); imisumys != mssumytops_learn.end(); ++imisumys)
		{
			map<string, map<unsigned int, Sumy>>::iterator ismisumys = msmisumyss.find(imisumys->item);
			if (msmisumyss.end() == ismisumys)continue;
			auto iisumy = ismisumys->second.rbegin();
			outfile2 << ismisumys->first << " days " << iisumy->second.days << " sumfit " << iisumy->second.sumfit << " av_sumfit " << iisumy->second.sumfit / iisumy->second.days
				<< " sumget " << iisumy->second.sumget << " av_sumget " << iisumy->second.sumget / iisumy->second.days << endl;
		}
		outfile2.close();
		mssumytops_learn.clear();
	}
	if (!L_U)
	{
		fn = ss.str() + string("Use.txt"); ss.str("");
		ofstream outfile(fn, ios::out | ios::trunc); if (!outfile.is_open())return;
		outfile << *dtod << " LearnTill " << LearnTill << " CLIMB " << CLIMB << " DROP " << DROP << endl;
		ss.clear(); ss << "Use at " << *dtod << " By LearnTill " << LearnTill << " CLIMB " << CLIMB << " DROP " << DROP;
		std::cout << ss.str() << endl;
		string emailTitle = ss.str();  ss.str("");
		int inums = 0; ss.clear();
		for (multiset<SumyTop>::iterator imisumys = mssumytops.begin(); imisumys != mssumytops.end(); ++imisumys)
		{
			map<string, map<unsigned int, Sumy>>::iterator ismisumys = msmisumyss.find(imisumys->item); if (msmisumyss.end() == ismisumys)continue;
			auto iisumy = ismisumys->second.find(*dtod); if (ismisumys->second.end() == iisumy)continue;
			outfile << ismisumys->first << " gets " << iisumy->second.mscnfs.size() << endl;
			std::cout << "    " << ismisumys->first << " gets " << iisumy->second.mscnfs.size() << endl;
			if (!iisumy->second.mscnfs.empty())
			{
				outfile << "    ";
				std::cout << "        ";
				for (auto iconf = iisumy->second.mscnfs.begin(); iconf != iisumy->second.mscnfs.end(); iconf++)
				{
					outfile << " " << iconf->codes;
					std::cout << " " << iconf->codes;
					if (inums == 0)
						ss << " " << iconf->codes;
				}
				outfile << endl;
				std::cout << endl;
				++inums;
			}
			if (inums >= 10)break;
		}
		outfile.close();
		Email163(string("8866228@qq.com"), emailTitle, ss.str(), fn);
		TdxWrite(*dtod, ss.str());
		RestartHXtdx();
		ss.str("");
	}
	return;
}
void CAna::TdxWrite(unsigned int date, string sscodes)
{
	struct TTDXUserBlockRecord
	{
		char        szName[50];
		char        szAbbr[70];        // 也是文件名称前缀 T0002\blocknew\xxxx.blk
	};
	TTDXUserBlockRecord rec; memset(&rec, 0, sizeof(rec));
	sprintf(rec.szName, "%c%d", 'B', date);
	sprintf(rec.szAbbr, "%c%d", 'B', date);
	//itoa(date, rec.szName, 10);
	//itoa(date, rec.szAbbr, 10);
	//strcpy(rec.szName , "20210927");
	//strcpy(rec.szAbbr , "20210927");
	//                                                         
	string DirBlk = "G:\\new_hxzq_hc\\T0002\\blocknew";
	string fn = DirBlk + string("\\blocknew.cfg");
	ofstream outfile(fn, ios::binary | ios::app);
	if (outfile.is_open())
	{
		outfile.seekp(0, ios::beg);
		outfile.write((char*)&rec, sizeof(rec));
		outfile.close();
	}
	stringstream ss; ss.clear();
	vector<string> vcodes; vcodes.clear();
	split(sscodes, vcodes, ' ');
	if (vcodes.empty())return;
	fn = DirBlk + string("\\") + string(rec.szAbbr) + string(".blk");
	outfile.open(fn, ios::out | ios::trunc);
	if (outfile.is_open())
	{
		outfile.seekp(0, ios::beg);
		for (auto it = vcodes.begin(); it != vcodes.end(); ++it)
		{
			string cc = (*it).substr(0, 2);
			string ccc = (*it).substr(0, 3);
			if (ccc == string("000") || ccc == string("001") || ccc == string("002") || ccc == string("003")
				|| ccc == string("300")
				|| cc == string("15") || cc == string("16") || cc == string("18"))
			{
				outfile << "0" << *it << endl;
			}
			else if (ccc == string("600") || ccc == string("601") || ccc == string("603") || ccc == string("605")
				|| ccc == string("688")
				|| cc == string("50") || cc == string("51")
				|| cc == string("00") || cc == string("88") || cc == string("99"))
			{
				outfile << "1" << *it << endl;
			}

		}
		outfile.close();
	}
	cout << __FUNCTION__ << " " << fn << endl;
	return;
}
void CAna::RestartHXtdx()
{
	HWND hwnd, hwnd1; int trys;
	hwnd = FindWindow("TdxW_MainFrame_Class", NULL);
	if (!hwnd)
	{
		if (!FindWindow(NULL, "华彩人生牛金岁月V8.04"))
		{
			ShellExecute(0, "open", "G:\\new_hxzq_hc\\TdxW.exe", "", "", 1);
			Sleep(10 * 1000);
		}
	}
	else
	{
		ShowWindow(hwnd, SW_SHOWMAXIMIZED);
		SetForegroundWindow(hwnd);
		PostMessage(hwnd, WM_CLOSE, 0, 0);
		trys = 0; hwnd = 0;
		while (!hwnd)
		{
			if (++trys>100)
			{
				cerr << "Can't FindWindow(退出确认)" << endl;
				return;
			}
			Sleep(950);
			hwnd = FindWindow(NULL, "退出确认");
		}
		SetForegroundWindow(hwnd);
		trys = 0; hwnd1 = 0;
		while (!hwnd1)
		{
			if (++trys > 100)
			{
				cerr << "Can't FindWindowEx(重新登录)" << endl;
				return;
			}
			Sleep(950);
			hwnd1 = FindWindowEx(hwnd, 0, "Button", "重新登录");
		}
		SetForegroundWindow(hwnd1);
		RECT rect;
		GetWindowRect(hwnd1, &rect);
		SetCursorPos(int((rect.left + rect.right) / 2), int((rect.top + rect.bottom) / 2));
		mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(10 * 1000);
	}
	trys = 0; hwnd = 0;
	while (!hwnd)
	{
		if (++trys>100)
		{
			cerr << "Can't FindWindow(华彩人生牛金岁月V8.04)" << endl;
			return;
		}
		Sleep(950);
		hwnd = FindWindow(NULL, "华彩人生牛金岁月V8.04");
	}
	ShowWindow(hwnd, SW_SHOWNORMAL);
	SetForegroundWindow(hwnd);
	trys = 0; hwnd1 = 0;
	while (!hwnd1)
	{
		if (++trys>100)
		{
			cerr << "Can't GetDlgItem(0x16D)" << endl;
			return;
		}
		Sleep(950);
		hwnd1 = GetDlgItem(hwnd, 0x16D);
	}
	SetForegroundWindow(hwnd1);
	RECT rect;
	GetWindowRect(hwnd1, &rect);
	SetCursorPos(int((rect.left + rect.right) / 2), int((rect.top + rect.bottom) / 2));
	mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	Sleep(30 * 1000);
	trys = 0; hwnd = 0;
	while (!hwnd)
	{
		if (++trys>100)
		{
			cerr << "Can't FindWindow(TdxW_MainFrame_Class)" << endl;
			return;
		}
		Sleep(950);
		hwnd = FindWindow("TdxW_MainFrame_Class", NULL);
	}
	cout << __FUNCTION__ << " " << TimeHMSYMD() << endl;
	return;
}
