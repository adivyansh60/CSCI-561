#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <list>
#include <queue>
#include <cctype>
#include <algorithm>

using namespace std;

int nq, ns;

struct predicate {

	string pname;
	vector<string> varconst;
	int numvar;
	int numconst;
	bool sign;
	int sentencenum;

	bool operator == (const predicate& tempxy)
	{
		return tempxy.pname == pname;
	}
};

struct sentence {

	list<predicate> sent;
};

unordered_multimap<string, predicate> umm;
unordered_map<int, sentence> summ;

struct minheap{
	bool operator()(predicate const& n1, predicate const& n2)
	{
		if(summ.find(n1.sentencenum)->second.sent.size() == summ.find(n2.sentencenum)->second.sent.size())
			return count(summ.find(n1.sentencenum)->second.sent.begin(), summ.find(n1.sentencenum)->second.sent.end(), n1) > count(summ.find(n2.sentencenum)->second.sent.begin(), summ.find(n2.sentencenum)->second.sent.end(), n2);
		else
			return summ.find(n1.sentencenum)->second.sent.size() < summ.find(n2.sentencenum)->second.sent.size();
	}
};

void createKB(string s[100], int ns) {

	for(int i=0; i<ns; i++) {

		list<predicate> lp;

		int flag=0;

		size_t implication = s[i].find("=>");
		if(implication != string::npos) {
			//Implication found
			flag=1;
		}
		int j=0;
		while(s[i][j]!='\0') {

			if(s[i][j]==' ' || s[i][j]=='&') {
				j++;
				continue;
			}

			if(s[i][j]=='=' || s[i][j]=='>') {
				flag=0;
				j++;
				continue;
			}

			predicate pred;
			bool predsign=true;
			if(s[i][j]=='~') {
				//Negation
				predsign=false;
				j++;
			}
			size_t openbrack = s[i].find("(",j);
			if(openbrack != string::npos) {
				//Predicate
				int numv=0, numc=0;
				vector<string> vecvc;
				string predname=s[i].substr(j,openbrack-j);
				j=openbrack+1;

				size_t closebrack = s[i].find(")",j);
				if(closebrack != string::npos) {
					while(j<closebrack) {
						size_t seperator = s[i].find(",",j);
						if(seperator != string::npos && seperator<closebrack) {

							string vc=s[i].substr(j,seperator-j);
							if(islower(vc[0])) {
								vc.append(to_string(i));
								numv++;
							}
							else
								numc++;
							vecvc.push_back(vc);
							j=seperator+1;
						}
						else {
							string vc=s[i].substr(j,closebrack-j);
							if(islower(vc[0])) {
								vc.append(to_string(i));
								numv++;
							}
							else
								numc++;
							vecvc.push_back(vc);
							j=closebrack;
						}
					}
				}
				
				pred.pname=predname;
				pred.varconst=vecvc;
				pred.sentencenum=i;
				pred.numvar=numv;
				pred.numconst=numc;
				if(flag==1)
					pred.sign=!predsign;
				else
					pred.sign=predsign;
				umm.insert(make_pair(predname,pred));
				lp.push_back(pred);
			}
			j++;
		}
		sentence st;
		st.sent=lp;
		summ.insert(make_pair(i,st));
	}
}

int unify(unordered_multimap<string, predicate> &solveumm, predicate svit, predicate pred) {

	int flag=0;
	unordered_map<string, string> matching;
	//first string is svit
	//second string is pred
	for(int i=0; i<pred.numvar+pred.numconst; i++) {
		if(isupper(svit.varconst[i][0]) && isupper(pred.varconst[i][0]) && svit.varconst[i].compare(pred.varconst[i])!=0) {
			//unification not possible
			//any two constants dont match
			flag=1;
			break;
		}
		if(isupper(svit.varconst[i][0]) && islower(pred.varconst[i][0])) {
			if(matching.empty()) {
				matching.insert(make_pair(pred.varconst[i],svit.varconst[i]));
			}
			else if(matching.find(pred.varconst[i]) == matching.end()) {
				matching.insert(make_pair(pred.varconst[i],svit.varconst[i]));
			}
			else if(matching.find(pred.varconst[i]) != matching.end()) {
				if(matching.find(pred.varconst[i])->second.compare(svit.varconst[i]) != 0) {
					//unification not possible
					//a matching was there which later got impossible to achieve unificatiion
					flag=1;
					break;
				}
			}
		}
		if(isupper(pred.varconst[i][0]) && islower(svit.varconst[i][0])) {
			if(matching.empty()) {
				matching.insert(make_pair(svit.varconst[i],pred.varconst[i]));
			}
			else if(matching.find(svit.varconst[i]) == matching.end()) {
				matching.insert(make_pair(svit.varconst[i],pred.varconst[i]));
			}
			else if(matching.find(svit.varconst[i]) != matching.end()) {
				if(matching.find(svit.varconst[i])->second.compare(pred.varconst[i]) != 0) {
					//unification not possible
					//a matching was there which later got impossible to achieve unificatiion
					flag=1;
					break;
				}
			}
		}
		if(islower(svit.varconst[i][0]) && islower(pred.varconst[i][0]) && svit.varconst[i].compare(pred.varconst[i])!=0) {
			if(matching.empty()) {
				//matching.insert(make_pair(u,pred.varconst[i]));
				matching.insert(make_pair(pred.varconst[i],svit.varconst[i]));
			}
			else if(matching.find(pred.varconst[i]) == matching.end()) {
				if(matching.find(svit.varconst[i]) == matching.end()) {
					//matching.insert(make_pair(u,pred.varconst[i]));
					matching.insert(make_pair(pred.varconst[i],svit.varconst[i]));
				}
				else {
					matching.insert(make_pair(pred.varconst[i],matching.find(svit.varconst[i])->second));
				}
			}
			else if(matching.find(pred.varconst[i]) != matching.end()) {
				if(matching.find(svit.varconst[i]) == matching.end()) {
					//matching.insert(make_pair(svit.varconst[i],matching.find(pred.varconst[i])->second));
					matching.insert(make_pair(svit.varconst[i],matching.find(pred.varconst[i])->second));
				}
				else {
					if(matching.find(pred.varconst[i])->second.compare(matching.find(svit.varconst[i])->second) != 0) {
						//unification not possible
						//matchings were there which later got impossible to achieve unificatiion
						flag=1;
						break;
					}
				}
			}
		}
	}
	if(flag==1) { //unification impossible
		return -1;
	}
	if(flag==0) { //no condition violated. Perfect matching or a unification found.
		if(matching.empty()) {
			sentence snt = summ[pred.sentencenum];
			list<predicate> lps = snt.sent;

			for(auto ps=lps.begin(); ps!=lps.end(); ++ps) {
				predicate p = *ps;
				if(p.pname==pred.pname) {
					for(int x=0; x<p.numvar+p.numconst; x++) {
						if(p.varconst[x]!=pred.varconst[x]) {
							solveumm.insert(make_pair(p.pname,p));
							break;
						}
					}
				}
				else {
					solveumm.insert(make_pair(p.pname,p));
				}
			}
			return pred.sentencenum;
		}
			//perfect matching
			//delete this predicate from solveumm
			//add the other predicates from the sentence
		else {
			sentence snt = summ[pred.sentencenum];
			list<predicate> lps = snt.sent;

			for(auto ps=lps.begin(); ps!=lps.end(); ++ps) {
				predicate p = *ps;
				if(p.pname==pred.pname) {
					for(int x=0; x<p.numvar+p.numconst; x++) {
						if(p.varconst[x]!=pred.varconst[x]) {
							for(int x=0; x<p.numvar+p.numconst; x++) {
								if(islower(p.varconst[x][0])) {
									if(matching.find(p.varconst[x]) != matching.end())
										p.varconst[x]=matching.find(p.varconst[x])->second;
								}
							}
							solveumm.insert(make_pair(p.pname,p));
							break;
						}
					}
				}
				else {
					for(int x=0; x<p.numvar+p.numconst; x++) {
						if(islower(p.varconst[x][0])) {
							if(matching.find(p.varconst[x]) != matching.end())
								p.varconst[x]=matching.find(p.varconst[x])->second;
						}
					}
					solveumm.insert(make_pair(p.pname,p));
				}
			}
			//Predicates with svit sentencenum should also be unified based on the matching

			unordered_multimap<string, predicate>::iterator svitt = solveumm.begin();
	 		for(; svitt!=solveumm.end(); ++svitt) {
 				if(svitt->second.pname==svit.pname) {
 					for(int x=0; x<svitt->second.numvar+svitt->second.numconst; x++) {
 						if(svitt->second.varconst[x]!=svit.varconst[x])
 						{
 							for(int x=0; x<svitt->second.numvar+svitt->second.numconst; x++) {
 								if(islower(svitt->second.varconst[x][0])) {
									if(matching.find(svitt->second.varconst[x]) != matching.end()) {
										svitt->second.varconst[x]=matching.find(svitt->second.varconst[x])->second;
									}
								}
 							}
 							break;
 						}
					}
 				}
 				else {
 					for(int x=0; x<svitt->second.numvar+svitt->second.numconst; x++) {
						if(islower(svitt->second.varconst[x][0])) {
							if(matching.find(svitt->second.varconst[x]) != matching.end()) {
								svitt->second.varconst[x]=matching.find(svitt->second.varconst[x])->second;
							}
						}
					}
 				}
	 		}
			return pred.sentencenum;
		}
			//unification
			//unify the other predicates too from the sentence base
			//add the unified predicates
			//delete this predicate from solveumm 
	}

}

bool checkendcond(vector<int> sentarr) {

	int n=sentarr.size();
	if(n>=ns*2) {
		if((sentarr[(n/2)-1]==sentarr[n-1] && sentarr[0]==sentarr[n/2]) || (sentarr[(n/2)-1]==sentarr[n-1] && sentarr[(n/2)-2]==sentarr[n-2]))
			return true;
		else
			return false;
	}
	else 
		return false;
}

bool getsolution(unordered_multimap<string, predicate> &solveumm, vector<int> &sentarr, int &endcond, int countarr[100]) {

	if(endcond==1)
		return false;
	if(endcond==2)
		return true;

	if(solveumm.size()==2 && endcond!=-1) {
		int rs=-1;
		unordered_multimap<string, predicate>::iterator sit = solveumm.begin();
		predicate ftp = sit->second;
		sit++;
		predicate stp = sit->second;
		if(ftp.pname==stp.pname && ftp.sign!=stp.sign) {
			rs = unify(solveumm, ftp, stp);
			if(rs!=-1) {
				endcond=2;
				return true;
			}
			else {
				solveumm.clear();
				solveumm.insert(make_pair(ftp.pname,ftp));
				solveumm.insert(make_pair(stp.pname,stp));
			}
		}
	}
	
	unordered_multimap<string, predicate>::iterator svit = solveumm.begin();
	string p = svit->first;
	bool psign = svit->second.sign;
	predicate pdt=svit->second;
	priority_queue<predicate, vector<predicate>, minheap> pq;
	queue<predicate> tq;

	auto pit = umm.equal_range(p);
	for(auto i=pit.first; i!=pit.second; ++i)
		pq.push(i->second);
	while(!pq.empty()) {
		predicate pred=pq.top();
		pq.pop();
		if(psign!=pred.sign) {

			if(countarr[pred.sentencenum]==1) {
				tq.push(pred);
				if(pq.empty()) {
					while(!tq.empty()) {
						pq.push(tq.front());
						tq.pop();
					}
				}
				countarr[pred.sentencenum]=0;
				continue;
			}

			int res = unify(solveumm, pdt, pred);
			if(res!=-1) {
				sentarr.push_back(res);
				countarr[res] = countarr[res]+1;
				if(checkendcond(sentarr)) {

					endcond=1;
					return false;
				}
				unordered_multimap<string, predicate>::iterator svitt = solveumm.begin();
		 		int n;
		 		int ss=solveumm.size();
		 		for(int i=0; i<ss; i++) {
		 			if(svitt->first == pdt.pname) {
		 				n=0;
		 				for(int j=0; j<pdt.numvar+pdt.numconst; j++) {
		 					if(svitt->second.varconst[j]!=pdt.varconst[j]){
		 						n=1;
		 						break;
		 					}
		 				}
		 				if(n==0) {
			 				solveumm.erase(svitt);
			 				break;
		 				}
		 			}
		 			svitt++;
		 		}
				
				if(!solveumm.empty()) {

					bool r = getsolution(solveumm, sentarr, endcond, countarr);
					if(endcond==1)
						return false;
					if(endcond==2)
						return true;
					if(r==false) {
						solveumm.insert(make_pair(pdt.pname,pdt));
					}
					else {
						return true;
					}
				}
				else {
					return true;
				}
			}
		}
		if(pq.empty() && !tq.empty()) {
			while(!tq.empty()) {
				pq.push(tq.front());
				tq.pop();
			}
		}
	}
	if(solveumm.empty()) {
	 	return true;
	}
	else {
	 	//delete all preds with the sentence number of svit
	 	if(pdt.sentencenum<summ.size()) {

	 		int n=0;
	 		while(n==0 && !solveumm.empty()) {
	 			int f=0;
	 			unordered_multimap<string, predicate>::iterator svitt = solveumm.begin();
		 		for(; svitt!=solveumm.end(); ++svitt) {
		 			if(svitt->second.sentencenum == pdt.sentencenum) {
		 				solveumm.erase(svitt);
		 				f=1;
		 				break;
		 			}
		 		}
		 		if(f==0) {
		 			n=1;
		 		}
	 		}
	 	}
	 	return false;
	}
}

bool solvequery(string q) {

	unordered_multimap<string, predicate> solveumm;
	predicate pred;

	int j=0;
	while(q[j]!='\0') {

		bool predsign=true;
		if(q[0]=='~') {
			predsign=false;
			j++;
		}
		size_t openbrack = q.find("(",j);
		if(openbrack != string::npos) {
			int numv=0, numc=0;
			vector<string> vecvc;
			string predname=q.substr(j,openbrack-j);
			j=openbrack+1;

			size_t closebrack = q.find(")",j);
			if(closebrack != string::npos) {
				while(j<closebrack) {
					size_t seperator = q.find(",",j);
					if(seperator != string::npos && seperator<closebrack) {

						string vc=q.substr(j,seperator-j);
						vecvc.push_back(vc);
						if(islower(vc[0]))
							numv++;
						else
							numc++;
						j=seperator+1;
					}
					else {
						string vc=q.substr(j,closebrack-j);
						vecvc.push_back(vc);
						if(islower(vc[0]))
							numv++;
						else
							numc++;
						j=closebrack;
					}
				}
			}
			
			pred.pname=predname;
			pred.varconst=vecvc;
			pred.numvar=numv;
			pred.numconst=numc;
			pred.sign=!predsign;
			solveumm.insert(make_pair(predname,pred));

			pred.sentencenum=summ.size();
			umm.insert(make_pair(predname,pred));

			list<predicate> lsp;
			lsp.push_back(pred);
			sentence st;
			st.sent=lsp;
			summ.insert(make_pair(pred.sentencenum,st));


		}
		j++;
	}

	unordered_multimap<string,predicate>::iterator ummit = umm.begin();
	string temp = ummit->first;
	int temparr[100]={0};
	if(ummit->second.sign)
		temparr[0]++;
	else
		temparr[0]--;
	int abc=0;
	ummit++;
	for(; ummit!=umm.end(); ummit++)
	{
		string tmp=ummit->first;
		if(tmp.compare(temp)!=0) {
			abc++;
			temp=tmp;
		}
		if(ummit->second.sign)
			temparr[abc]++;
		else
			temparr[abc]--;

	}
	int sum=0;
	bool result;
	for(int i=0; i<100; i++) 
		sum= sum+temparr[i];

	vector<int> sentarr;
	
	int countarr[100] = {0};

	if(sum!=0) {
		int a=0;
		result = getsolution(solveumm, sentarr, a, countarr);
	}
	if(sum==0) {
		int a=-1;
		result = getsolution(solveumm, sentarr, a, countarr);
	}

	summ.erase(pred.sentencenum);

	int f=1;
	unordered_multimap<string,predicate>::iterator ummitt = umm.begin();
	for(; ummitt!=umm.end(); ummitt++)
	{
		if(ummitt->first==pred.pname) {
			for(int x=0; x<(ummitt->second.numvar+ummitt->second.numconst); x++) {
				if(ummitt->second.varconst[x]!=pred.varconst[x])
					f=0;
			}
			if(f==1) {
				umm.erase(ummitt);
				break;
			}
		}
	}

	// if(sum==0)
	// 	return false;
	// else
		return result;
}

int main() {

	string q[10], s[100];

	ifstream myfile("inp.txt");
	if(myfile.is_open()) {

		myfile>>nq;
		for(int i=0; i<nq; i++) {
			myfile>>q[i];
		}
		myfile>>ns;
		for(int i=0; i<ns; i++) {
			int letsbrk=0;

			while(letsbrk==0) {
				if(s[i][s[i].size()-1]=='>' && s[i][s[i].size()-2]=='=') {
					letsbrk=1;
				}
				string str;
				myfile>>str;
				if(str.compare("")==0) {
					letsbrk=1;
					continue;
				}
				if(s[i].compare("")!=0) {
					if((str[0]!='=' && str[0]!='&') && (s[i][s[i].size()-1]!='>' && s[i][s[i].size()-1]!='&')) {
						i++;
						s[i].append(str);
						i--;
						break;
					}
					else {
						s[i].append(" ");
					}
				}
				s[i].append(str);
				
			}
		}			
		myfile.close();
	}

	ofstream myfiley("output.txt");

	createKB(s,ns);

	for(int k=0; k<nq; k++) {

		string ansstr="";
		bool r = solvequery(q[k]);
		if(r==true) {
			ansstr.append("TRUE");
		}
		else {
			ansstr.append("FALSE");
		}
		myfiley<<ansstr<<"\n";
	}

	myfiley.close();

	return 0;
}