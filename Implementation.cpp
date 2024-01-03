#include <bits/stdc++.h>
using namespace std;
#define len(v) (int)v.size()
#define pb push_back
#define ll long long
#define all(v) v.begin(),v.end()
#define rall(v) 

double INF;
double adjust = 1;
vector<string> prin;
vector<string> prout;
map<string,vector<pair<double,double>>> delays;
vector<vector<string>> mp;

void solveA(int argc, char* argv[]){  
	//---------------------------------- MAIN ALGORITHM ----------------------------------
	struct node{
		vector<pair<string,double>> e;
		double val;
		bool came;
		node(){
			val = 0;
			came = false;
		}
	};
	map<string,node> g;
	for(int i=0;i<len(mp);i++){
		if(mp[i][0]=="PRIMARY_INPUTS" or mp[i][0]=="PRIMARY_OUTPUTS" or mp[i][0]=="INTERNAL_SIGNALS" or mp[i][0]=="DFF"){
			for(int j=1;j<len(mp[i]);j++){
				g[mp[i][j]] = node();
			}
			if(mp[i][0]=="DFF"){
				prout.pb(mp[i][1]);
				prin.pb(mp[i][2]);
				g[mp[i][2]].came = true;
			}
			else if(mp[i][0]=="PRIMARY_OUTPUTS"){
				for(int j=1;j<len(mp[i]);j++){
					prout.pb(mp[i][j]);
				}
			}
			else if(mp[i][0]=="PRIMARY_INPUTS"){
				for(int j=1;j<len(mp[i]);j++){
					prin.pb(mp[i][j]);
					g[mp[i][j]].came = true;
				}
			}
		}
	}
	for(int i=0;i<len(mp);i++){
		if(mp[i][0] != "PRIMARY_INPUTS" and mp[i][0] != "PRIMARY_OUTPUTS" and mp[i][0] != "INTERNAL_SIGNALS" and mp[i][0]!="DFF"){
			string end = mp[i][len(mp[i])-1];
			for(int j=1;j<len(mp[i])-1;j++){
				string now = mp[i][j];
				pair<string,double> p = {now,delays[mp[i][0]][0].first};
				g[end].e.pb(p); 				
			}
		}	
	}
	function<void(string)> recur = [&](string curr){
		for(int i=0;i<len(g[curr].e);i++){
			if(!g[g[curr].e[i].first].came){
				recur(g[curr].e[i].first);
			}
		}
		for(int i=0;i<len(g[curr].e);i++){
			g[curr].val = max(g[g[curr].e[i].first].val + g[curr].e[i].second,g[curr].val); 
		}
		g[curr].came = true;
		
	};
	for(int i=0;i<len(prout);i++){
		recur(prout[i]);
	}
	//------------------------------ WRITTING OUTPUTS ------------------------------------
	ofstream fp_out;
	fp_out.open(argv[4]);
	double ans = 0;
	for(int i=0;i<len(prout);i++){
		ans = max(ans,g[prout[i]].val);
	}
	fp_out<<ans<<endl;
	fp_out.close(); 
}
//-----------------------------------------------------------------------SECOND PART------------------------------------------------------------------------------------//

double mxsolver(){
	//------------------------------------ MAIN ALGORITHM -------------------------------
	struct node{
		vector<string> e;
		vector<string> f;
		vector<double> dp;
		vector<double> comegate; 
		vector<vector<pair<string,double>>> choice;
		bool tpsort;
		bool came;
		bool fcame;
		double val;
		string gate;
		node(){
			val = INF;
			dp.resize(INF+1,INT_MAX); 
			comegate.resize(INF+1,-1);
			choice.resize(INF+1);
			came = false;
			fcame = false;
			tpsort = false;
			
		}
	};
	map<string,node> g;
	for(int i=0;i<len(mp);i++){
		if(mp[i][0]=="PRIMARY_INPUTS" or mp[i][0]=="PRIMARY_OUTPUTS" or mp[i][0]=="INTERNAL_SIGNALS" or mp[i][0]=="DFF"){
			for(int j=1;j<len(mp[i]);j++){
				g[mp[i][j]] = node();
			}
			if(mp[i][0]=="DFF"){
				prout.pb(mp[i][1]);
				prin.pb(mp[i][2]);
			}
			else if(mp[i][0]=="PRIMARY_OUTPUTS"){
				for(int j=1;j<len(mp[i]);j++){
					prout.pb(mp[i][j]);
				}
			}
			else if(mp[i][0]=="PRIMARY_INPUTS"){
				for(int j=1;j<len(mp[i]);j++){
					prin.pb(mp[i][j]);
				}
			}
		}
	}
	for(int i=0;i<len(prin);i++){
		g[prin[i]].came = true;
		g[prin[i]].fcame = true;
		for(int k=0;k<=INF;k++) g[prin[i]].dp[k] = 0;
	}
	for(int i=0;i<len(mp);i++){
		if(mp[i][0] != "PRIMARY_INPUTS" and mp[i][0] != "PRIMARY_OUTPUTS" and mp[i][0] != "INTERNAL_SIGNALS" and mp[i][0] != "DFF"){
			string end = mp[i][len(mp[i])-1];
			g[end].gate = mp[i][0];
			for(int j=1;j<len(mp[i])-1;j++){
				string now = mp[i][j];
				g[end].e.pb(now); 	
				g[now].f.pb(end);			
			}
		}	
	}
	//-------INITIAL RECURSION-------
	function<void(string)> ini_recur = [&](string curr){   // O((V+E)*INF)
		if(g[curr].came == true) return;
		for(int i=0;i<len(g[curr].e);i++){
			if(!g[g[curr].e[i]].came){
				ini_recur(g[curr].e[i]);
			}
		}
		g[curr].came = true;
		string gate = g[curr].gate;
		for(int i=0;i<=INF;i++){
			for(int j=2;j>=0;j--){
				vector<pair<string,double>> fin;
				bool poss = true;
				double ans = delays[gate][j].second;
				for(int k=0;k<len(g[curr].e);k++){
					if(i < delays[gate][j].first or g[g[curr].e[k]].dp[i-delays[gate][j].first]==INT_MAX){
						poss = false;
						break;
					}
					ans += (double)g[g[curr].e[k]].dp[i-delays[gate][j].first];
					fin.pb({g[curr].e[k],i-delays[gate][j].first});
				}
				if(poss and g[curr].dp[i] > ans){
					g[curr].dp[i] = ans;
					g[curr].choice[i] = fin;
					g[curr].comegate[i] = j;
				}
 			}
		}
	};
	for(int i=0;i<len(prout);i++){
		ini_recur(prout[i]);
	}
	//---------INFEASIBILITY CHECK--------------
	for(int i=0;i<len(prout);i++){
		if(g[prout[i]].dp[INF] == INT_MAX){
			return -1;
		}
	}
	//----------RECONCILIATION STEP-----------
	vector<string> order;
	function<void(string)> topsort = [&](string curr){
		for(int i=0;i<len(g[curr].f);i++){
			if(!g[g[curr].f[i]].tpsort){
				topsort(g[curr].f[i]);
			}
		}
		order.pb(curr);
		g[curr].tpsort = true;
	};
	for(int i=0;i<len(prin);i++){
		topsort(prin[i]);
	}
	for(int i=0;i<len(order);i++){
		double VAL = g[order[i]].val;
		for(int j=0;j<len(g[order[i]].choice[VAL]);j++){
			string par = g[order[i]].choice[VAL][j].first;
			double mn = min(g[par].val,g[order[i]].choice[VAL][j].second);
			g[par].val = mn;
		}
	}
	//------------GATE CHANGES-----------
	reverse(all(order));
	for(int i=0;i<len(order);i++){
		string curr = order[i]; 
		for(int j=2;j>=0;j--){
			bool poss = true;
			for(int k=0;k<len(g[curr].e);k++){
				if(g[g[curr].e[k]].val + delays[g[curr].gate][j].first > g[curr].val) poss = false;
			}
			if(poss){
				g[curr].comegate[g[curr].val] = j;
				break;
			}
		}
	}
	double cst = 0;
	for(auto it = g.begin();it!=g.end();it++){
		string curr = it->first;
		if(!g[curr].fcame) cst += delays[g[curr].gate][g[curr].comegate[g[curr].val]].second; 
	}
	return cst; 
};

double mnsolver(){
	//------------------------------------ MAIN ALGORITHM -------------------------------
	struct node{
		vector<string> e;
		vector<string> f;
		vector<double> dp;
		vector<double> comegate; 
		vector<vector<pair<string,double>>> choice;
		bool tpsort;
		bool came;
		bool fcame;
		double val;
		string gate;
		node(){
			val = INF;
			dp.resize(INF+1,INT_MAX); 
			comegate.resize(INF+1,-1);
			choice.resize(INF+1);
			came = false;
			fcame = false;
			tpsort = false;
			
		}
	};
	map<string,node> g;
	for(int i=0;i<len(mp);i++){
		if(mp[i][0]=="PRIMARY_INPUTS" or mp[i][0]=="PRIMARY_OUTPUTS" or mp[i][0]=="INTERNAL_SIGNALS" or mp[i][0]=="DFF"){
			for(int j=1;j<len(mp[i]);j++){
				g[mp[i][j]] = node();
			}
			if(mp[i][0]=="DFF"){
				prout.pb(mp[i][1]);
				prin.pb(mp[i][2]);
			}
			else if(mp[i][0]=="PRIMARY_OUTPUTS"){
				for(int j=1;j<len(mp[i]);j++){
					prout.pb(mp[i][j]);
				}
			}
			else if(mp[i][0]=="PRIMARY_INPUTS"){
				for(int j=1;j<len(mp[i]);j++){
					prin.pb(mp[i][j]);
				}
			}
		}
	}
	for(int i=0;i<len(prin);i++){
		g[prin[i]].came = true;
		g[prin[i]].fcame = true;
		for(int k=0;k<=INF;k++) g[prin[i]].dp[k] = 0;
	}
	for(int i=0;i<len(mp);i++){
		if(mp[i][0] != "PRIMARY_INPUTS" and mp[i][0] != "PRIMARY_OUTPUTS" and mp[i][0] != "INTERNAL_SIGNALS" and mp[i][0] != "DFF"){
			string end = mp[i][len(mp[i])-1];
			g[end].gate = mp[i][0];
			for(int j=1;j<len(mp[i])-1;j++){
				string now = mp[i][j];
				g[end].e.pb(now); 	
				g[now].f.pb(end);			
			}
		}	
	}
	//-------INITIAL RECURSION-------
	function<void(string)> ini_recur = [&](string curr){   // O((V+E)*INF)
		if(g[curr].came == true) return;
		for(int i=0;i<len(g[curr].e);i++){
			if(!g[g[curr].e[i]].came){
				ini_recur(g[curr].e[i]);
			}
		}
		g[curr].came = true;
		string gate = g[curr].gate;
		for(int i=0;i<=INF;i++){
			for(int j=2;j>=0;j--){
				vector<pair<string,double>> fin;
				bool poss = true;
				double ans = delays[gate][j].second;
				for(int k=0;k<len(g[curr].e);k++){
					if(i < delays[gate][j].first or g[g[curr].e[k]].dp[i-delays[gate][j].first]==INT_MAX){
						poss = false;
						break;
					}
					ans += (double)g[g[curr].e[k]].dp[i-delays[gate][j].first]/len(g[g[curr].e[k]].f);
					fin.pb({g[curr].e[k],i-delays[gate][j].first});
				}
				if(poss and g[curr].dp[i] >= ans){
					g[curr].dp[i] = ans;
					g[curr].choice[i] = fin;
					g[curr].comegate[i] = j;
				}
 			}
		}
	};
	for(int i=0;i<len(prout);i++){
		ini_recur(prout[i]);
	}
	//---------INFEASIBILITY CHECK--------------
	for(int i=0;i<len(prout);i++){
		if(g[prout[i]].dp[INF] == INT_MAX){
			return -1;
		}
	}
	//----------RECONCILIATION STEP-----------
	vector<string> order;
	function<void(string)> topsort = [&](string curr){
		for(int i=0;i<len(g[curr].f);i++){
			if(!g[g[curr].f[i]].tpsort){
				topsort(g[curr].f[i]);
			}
		}
		order.pb(curr);
		g[curr].tpsort = true;
	};
	for(int i=0;i<len(prin);i++){
		topsort(prin[i]);
	}
	for(int i=0;i<len(order);i++){
		double VAL = g[order[i]].val;
		for(int j=0;j<len(g[order[i]].choice[VAL]);j++){
			string par = g[order[i]].choice[VAL][j].first;
			double mn = min(g[par].val,g[order[i]].choice[VAL][j].second);
			g[par].val = mn;
		}
	}
	//------------GATE CHANGES-----------
	reverse(all(order));
	for(int i=0;i<len(order);i++){
		string curr = order[i]; 
		for(int j=2;j>=0;j--){
			bool poss = true;
			for(int k=0;k<len(g[curr].e);k++){
				if(g[g[curr].e[k]].val + delays[g[curr].gate][j].first > g[curr].val) poss = false;
			}
			if(poss){
				g[curr].comegate[g[curr].val] = j;
				break;
			}
		}
	}
	double cst = 0;
	for(auto it = g.begin();it!=g.end();it++){
		string curr = it->first;
		if(!g[curr].fcame) cst += delays[g[curr].gate][g[curr].comegate[g[curr].val]].second; 
	}
	return cst; 
	
};
void brute(ll gateno, int argc, char* argv[]){
	ll steps = powl(3,gateno);
	vector<string> gno;
	double cst = INT_MAX;
	//---------------------------------- MAIN ALGORITHM ----------------------------------
	struct node{
		vector<string> e;
		string gate;
		int gateno;
		double val;
		bool came;
		node(){
			val = 0;
			came = false;
			gate = "";
			gateno = -1;
		}
	};
	map<string,node> g;
	for(int i=0;i<len(mp);i++){
		if(mp[i][0]=="PRIMARY_INPUTS" or mp[i][0]=="PRIMARY_OUTPUTS" or mp[i][0]=="INTERNAL_SIGNALS" or mp[i][0]=="DFF"){
			for(int j=1;j<len(mp[i]);j++){
				g[mp[i][j]] = node();
			}
			if(mp[i][0]=="DFF"){
				prout.pb(mp[i][1]);
				prin.pb(mp[i][2]);
				g[mp[i][2]].came = true;
			}
			else if(mp[i][0]=="PRIMARY_OUTPUTS"){
				for(int j=1;j<len(mp[i]);j++){
					prout.pb(mp[i][j]);
				}
			}
			else if(mp[i][0]=="PRIMARY_INPUTS"){
				for(int j=1;j<len(mp[i]);j++){
					prin.pb(mp[i][j]);
					g[mp[i][j]].came = true;
				}
			}
		}
	}
	for(int i=0;i<len(mp);i++){
		if(mp[i][0] != "PRIMARY_INPUTS" and mp[i][0] != "PRIMARY_OUTPUTS" and mp[i][0] != "INTERNAL_SIGNALS" and mp[i][0]!="DFF"){
			string end = mp[i][len(mp[i])-1];
			g[end].gate = mp[i][0];
			for(int j=1;j<len(mp[i])-1;j++){
				string now = mp[i][j];
				g[end].e.pb(now); 				
			}
		}	
	}
	
	for(auto it = g.begin();it!=g.end();it++){
		if(it->second.gate != ""){
			gno.pb(it->first);
		}
	}
	function<void(string)> recur = [&](string curr){
		if(g[curr].gate == "") return;
		for(int i=0;i<len(g[curr].e);i++){
			if(!g[g[curr].e[i]].came){
				recur(g[curr].e[i]);
			}
		}
		for(int i=0;i<len(g[curr].e);i++){
			g[curr].val = max(g[g[curr].e[i]].val + delays[g[curr].gate][g[curr].gateno].first , g[curr].val); 
		}
		g[curr].came = true;
	};
	for(int i = 0;i<steps;i++){
		int now = i;
		for(auto it = g.begin();it!=g.end();it++){
			it->second.val = 0;
			it->second.came = false;
		}
		for(int j=0;j<len(prin);j++){
			g[prin[j]].came = true;
		}
		double cost = 0;
		for(int j=0;j<len(gno);j++){
			int rem = now%3;
			g[gno[j]].gateno = rem;
			cost += delays[g[gno[j]].gate][rem].second;
			now/=3;
		}
		for(int j=0;j<len(prout);j++){
			recur(prout[j]);
		}
		double ans = 0;
		for(int j=0;j<len(prout);j++){
			ans = max(ans,g[prout[j]].val);
		}
		if(ans<=INF){
			cst = min(cst,cost);
		}
	}
	//------------------------------ WRITTING OUTPUTS ------------------------------------
	if(cst==INT_MAX) cst = -1;
	ofstream fp_out;
	fp_out.open(argv[5]);
	fp_out<<cst<<endl;
	fp_out.close(); 
	
}
void solveB(int argc, char* argv[]){
	double cst = INT_MAX;
	cst = min(mnsolver(),mxsolver());
	//------------------------------ WRITTING OUTPUTS ------------------------------------
	ofstream fp_out;
	fp_out.open(argv[5]);
	fp_out<<cst<<endl;
	fp_out.close(); 
}

int main(int argc, char* argv[]){
	ios:: sync_with_stdio(false);
	fstream:: sync_with_stdio(false);
	//---------------------------------- TAKING INPUTS ------------------------------------
	ifstream fp_in;
	string s;
	string news;
	// OPENING CIRCUIT FILE
	fp_in.open(argv[2]);
	while(fp_in>>s){
		if(len(s)>=2 and s[0]=='/' and s[1]=='/'){
			getline(fp_in,news);
			continue;			
		}
		else{
			getline(fp_in,news);
			if(int(news[len(news)-1]) == 10 or int(news[len(news)-1]) == 13){
				news.erase(news.begin() + len(news)-1);
			}
			vector<string> vec;
			vec.pb(s);
			string t = "";
			int i = 0;
			while(i<len(news)){
				if(news[i]==' '){
					if(t!="") vec.pb(t);
					t = "";
					i++;
				}
				else{
					t.pb(news[i]);
					i++;
				}
			}
			if(t != ""){ 
				vec.pb(t);
			}
			mp.pb(vec);
			continue;
		}
	}
	fp_in.close();
	// OPENING GATE DELAYS FILE
	fp_in.open(argv[3]);
	while(fp_in>>s){
		if(len(s)>=2 and s[0]=='/' and s[1]=='/'){
			getline(fp_in,news);
			continue;			
		}
		else{
			getline(fp_in,news);
			if(int(news[len(news)-1]) == 10 or int(news[len(news)-1]) == 13){
				news.erase(news.begin() + len(news)-1);
			}
			while(len(news)>0 and news[0]==' '){
				news.erase(news.begin());
			}
			string t = "";
			string gate = "";
			pair<double,double> p;
			int i = 0;
			for(;i<len(news);i++){
				if(news[i]!=' '){
					gate = gate + news[i];
				}
				else break;
			}
			i++;
			t = "";
			for(;i<len(news);i++){
				if(news[i]!=' '){
					t = t + news[i]; 
				}
				else break;
			} 
			i++;
			double f = stof(t);
			p.first = f; 
			t = "";
			for(;i<len(news);i++){
				if(news[i]!=' '){
					t = t + news[i]; 
				}
				else break;
			}
			i++;
			f = stof(t);
			p.second = f;
			delays[gate].pb(p);
			sort(all(delays[gate]));
			continue;
		}
	}
	fp_in.close();
	if(strcmp("B",argv[1]) == 0){
		// OPENING MAX DELAYS FILE
		fp_in.open(argv[4]);
		while(fp_in>>s){
			if(len(s)>=2 and s[0]=='/' and s[1]=='/'){
				getline(fp_in,news);
				continue;			
			}
			else{
				getline(fp_in,news);
				double f = stof(s);
				INF = f;   
				continue;
			}
		}
		fp_in.close();
	}
	//--------- PART A SOLVER --------
	if(strcmp("A",argv[1]) == 0){
		solveA(argc,argv);
	}
	//-------- MAKE ADJUSTMENTS --------
	else if(strcmp("B",argv[1]) == 0){
		int gateno = 0;
		int nodes = 0;
		auto make_adjust = [&](){
			double cnt = 0;
			for(int i=0;i<len(mp);i++){
				if(mp[i][0]=="PRIMARY_INPUTS" or mp[i][0]=="PRIMARY_OUTPUTS" or mp[i][0]=="INTERNAL_SIGNALS" or mp[i][0]=="DFF"){
					cnt += len(mp[i]) - 1; 
					nodes += len(mp[i]) - 1;
					gateno -= 1;
				}
				gateno += 1;
			}
			adjust = (double)1e6/(INF*cnt);
			INF = INF*adjust;
			INF = (int)INF;
			for(auto it = delays.begin();it!=delays.end();it++){
				for(int i=0;i<3;i++){
					it->second[i].first = it->second[i].first*adjust;
					it->second[i].first = (int)it->second[i].first;
				}
			}
		};
		make_adjust();
		//----------PART B SOLVER------------
		if(gateno<12 and (ll)nodes*pow(gateno,3)<1e6) brute(gateno,argc,argv);
		else solveB(argc,argv);
	}
	return 0;
}
