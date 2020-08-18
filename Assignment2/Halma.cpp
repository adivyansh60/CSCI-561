#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <queue>
#include <stack>
#include <algorithm>
#include <ctime>
#include <iomanip>

using namespace std;

struct point
{
	int i,j;
	float val;
};

struct cell
{
	point pos;
	point parent;
	point rootplayer;
	string movetype;
};

struct minheap{
	bool operator()(cell const& n1, cell const& n2)
	{
		return n1.pos.val > n2.pos.val;
	}
};

struct maxheap{
	bool operator()(cell const& n1, cell const& n2)
	{
		return n1.pos.val < n2.pos.val;
	}
};

struct board
{
	vector<cell> moves;
	priority_queue<cell, vector<cell>, maxheap> q;
	queue<cell> s;
	cell best;
	float bestval;
};

bool checkpawnwin(int x, int y, int oppi[19], int oppj[19]) {
	for(int k=0; k<19; k++) {
		if(x==oppi[k] && y==oppj[k])
			return true;
	}
	return false;
}

int util_func(char gamemap[16][16], char col, int curri, int currj, int newi, int newj, int level) {
	//char oppcol;
	int aimi[19]={0}, aimj[19]={0};
	int aimiw[19]={0,0,0,0,0,1,1,1,1,1,2,2,2,2,3,3,3,4,4};
	int aimjw[19]={0,1,2,3,4,0,1,2,3,4,0,1,2,3,0,1,2,0,1};
	int aimib[19]={15,15,15,15,15,14,14,14,14,14,13,13,13,13,12,12,12,11,11};
	int aimjb[19]={15,14,13,12,11,15,14,13,12,11,15,14,13,12,15,14,13,15,14};
	int sum=0;
	gamemap[newi][newj]=col;
	gamemap[curri][currj]='.';
	if(col=='W') {
		copy(begin(aimiw), end(aimiw), begin(aimi));
		copy(begin(aimjw), end(aimjw), begin(aimj));
	}
	else {
		copy(begin(aimib), end(aimib), begin(aimi));
		copy(begin(aimjb), end(aimjb), begin(aimj));
	}

	for(int a=0;a<16;a++){
		for(int b=0;b<16;b++){
			
			if(gamemap[a][b]==col) {
				
				for(int c=0;c<19;c++) {
					if(gamemap[aimi[c]][aimj[c]]=='.' || gamemap[aimi[c]][aimj[c]]==',') {
						sum= sum+sqrt((abs(a-aimi[c]))*(abs(a-aimi[c])) + (abs(b-aimj[c]))*(abs(b-aimj[c])));
					}
				}
			}
		}
	}
	gamemap[newi][newj]='.';
	gamemap[curri][currj]=col;
	return sum;
}

float board_util(char gamemap[16][16], char col, int curri, int currj, int newi, int newj, int level) {
	char oppcol;
	int aimi[19]={0}, aimj[19]={0}, oppi[19]={0}, oppj[19]={0};
	int aimiw[19]={0,0,0,0,0,1,1,1,1,1,2,2,2,2,3,3,3,4,4};
	int aimjw[19]={0,1,2,3,4,0,1,2,3,4,0,1,2,3,0,1,2,0,1};
	int aimib[19]={15,15,15,15,15,14,14,14,14,14,13,13,13,13,12,12,12,11,11};
	int aimjb[19]={15,14,13,12,11,15,14,13,12,11,15,14,13,12,15,14,13,15,14};
	float sum=0.0;
	gamemap[newi][newj]=col;
	gamemap[curri][currj]='.';
	if(col=='W') {
		copy(begin(aimiw), end(aimiw), begin(aimi));
		copy(begin(aimjw), end(aimjw), begin(aimj));
		copy(begin(aimib), end(aimib), begin(oppi));
		copy(begin(aimjb), end(aimjb), begin(oppj));
		oppcol='B';
	}
	else {
		copy(begin(aimib), end(aimib), begin(aimi));
		copy(begin(aimjb), end(aimjb), begin(aimj));
		copy(begin(aimiw), end(aimiw), begin(oppi));
		copy(begin(aimjw), end(aimjw), begin(oppj));
		oppcol='W';
	}

	for(int a=0;a<16;a++){
		for(int b=0;b<16;b++){
			
			if(gamemap[a][b]==col) {
				
				for(int c=0;c<19;c++) {
					if(gamemap[aimi[c]][aimj[c]]=='.' || gamemap[aimi[c]][aimj[c]]==',') {
						sum= sum-sqrt((abs(a-aimi[c]))*(abs(a-aimi[c])) + (abs(b-aimj[c]))*(abs(b-aimj[c])));
					}
				}
			}
			if(gamemap[a][b]==oppcol) {
				
				for(int c=0;c<19;c++) {
					if(gamemap[oppi[c]][oppj[c]]=='.') {
						sum= sum+sqrt((abs(a-oppi[c]))*(abs(a-oppi[c])) + (abs(b-oppj[c]))*(abs(b-oppj[c])));

					}
				}
			}
		}
	}
	gamemap[newi][newj]='.';
	gamemap[curri][currj]=col;
	return sum;
}

int find_jumps(char gamemap[16][16], int campi[19], int campj[19], bool checkmap[16][16], char myplay, int ri, int rj, int pi, int pj, int tempx, int tempy, vector<cell> &moves, queue<cell> &s, int level=-1) {
	
	gamemap[tempx][tempy]=',';
	int mtx[3]={tempx-1, tempx, tempx+1};
	int mty[3]={tempy-1, tempy, tempy+1};
	int jtx[3]={tempx-2, tempx, tempx+2};
	int jty[3]={tempy-2, tempy, tempy+2};
	for(int c=0; c<3; c++) {
		for(int d=0; d<3; d++) {
			if(!(c==1 && d==1) && (jtx[c]>=0 && jtx[c]<16 && jty[d]>=0 && jty[d]<16)) {
				if(gamemap[jtx[c]][jty[d]]=='.' && (gamemap[mtx[c]][mty[d]]=='B' || gamemap[mtx[c]][mty[d]]=='W' || gamemap[mtx[c]][mty[d]]==',')) {
					find_jumps(gamemap, campi, campj, checkmap, myplay,ri,rj,tempx,tempy,jtx[c],jty[d],moves,s,level);

				}
			}
		}
	}

	cell po;
	po.pos.i=tempx;
	po.pos.j=tempy;
	if(level==2 || level==-1)
		po.pos.val=board_util(gamemap,myplay,ri,rj,tempx,tempy,level);
	po.parent.i=pi;
	po.parent.j=pj;
	po.movetype="J";
	po.rootplayer.i=ri;
	po.rootplayer.j=rj;
	if(level==0 || level==-1) {
		s.push(po);
	}
	if(!checkmap[tempx][tempy]) {
		if(checkpawnwin(ri,rj,campi,campj)==true && checkpawnwin(tempx,tempy,campi,campj)==false)
			moves.push_back(po);
		else if((myplay=='W' && tempx<=ri && tempy<=rj) || (myplay=='B' && tempx>=ri && tempy>=rj)) {
			moves.push_back(po);
		}
	}

	gamemap[tempx][tempy]='.';
	checkmap[tempx][tempy]=true;
}

void find_valid_move(char gamemap[16][16], int campi[19], int campj[19], char myplay, int i, int j, vector<cell> &moves, queue<cell> &s, int level=-1) {

	int move_x[3]={i-1, i, i+1};
	int move_y[3]={j-1, j, j+1};
	int jump_x[3]={i-2, i, i+2};
	int jump_y[3]={j-2, j, j+2};

	bool checkmap[16][16]={false};

	for(int a=0; a<3; a++) {
		for(int b=0; b<3; b++) {
			if(!(a==1 && b==1) && (move_x[a]>=0 && move_x[a]<16 && move_y[b]>=0 && move_y[b]<16)) {
				if(gamemap[move_x[a]][move_y[b]]=='.') {

						if((myplay=='W' && move_x[a]<=i && move_y[b]<=j) || (myplay=='B' && move_x[a]>=i && move_y[b]>=j)) {
							cell po;
							po.pos.i=move_x[a];
							po.pos.j=move_y[b];
							if(level==2 || level==-1)
								po.pos.val=board_util(gamemap,myplay,i,j,move_x[a],move_y[b],level);
							po.parent.i=i;
							po.parent.j=j;
							po.movetype="E";
							po.rootplayer.i=i;
							po.rootplayer.j=j;
							if(level==0 || level==-1) {
								s.push(po);
							}
							moves.push_back(po);
						}
				
				}
				if(gamemap[jump_x[a]][jump_y[b]]=='.' && (gamemap[move_x[a]][move_y[b]]=='W' || gamemap[move_x[a]][move_y[b]]=='B') && (jump_x[a]>=0 && jump_x[a]<16 && jump_y[b]>=0 && jump_y[b]<16)) {
					find_jumps(gamemap, campi, campj, checkmap, myplay, i, j, i,j,jump_x[a], jump_y[b], moves, s, level);
				}
			}
		}
	}
}

int print_jumps(queue<cell> &s,int rx, int ry, int x, int y, string &str) {

	cell temp = s.front();
	s.pop();
	if(rx==temp.parent.i && ry==temp.parent.j && temp.pos.i==x && temp.pos.j==y && temp.movetype=="J") {
		if(str.compare("")!=0)
			str.append("\n");
		str.append("J ");
		str.append(to_string(temp.parent.j));
		str.append(",");
		str.append(to_string(temp.parent.i));
		str.append(" ");
		str.append(to_string(temp.pos.j));
		str.append(",");
		str.append(to_string(temp.pos.i));
		return 1;
	}
	else if(rx==temp.rootplayer.i && ry==temp.rootplayer.j && temp.pos.i==x && temp.pos.j==y && temp.movetype=="J") {
		print_jumps(s,rx,ry,temp.parent.i, temp.parent.j, str);
		if(str.compare("")!=0)
			str.append("\n");
		str.append("J ");
		str.append(to_string(temp.parent.j));
		str.append(",");
		str.append(to_string(temp.parent.i));
		str.append(" ");
		str.append(to_string(temp.pos.j));
		str.append(",");
		str.append(to_string(temp.pos.i));
		return 1;
	}
	else {
		print_jumps(s,rx,ry,x,y,str);
		return 1;
	}
}


float alphabetafunc(char gamemap[16][16], char myplay, bool minmax, int level, string &str, float alpha, float beta, cell &besti, int curri=0, int currj=0, int newi=0, int newj=0) {
	
	int flag=0;
	char oppcol;
	vector<cell> moves;
	queue<cell> s;
	cell best;
	float bestval;

	float al=alpha, bet=beta;

	priority_queue<cell, vector<cell>, maxheap> final_move;

	int campi[19]={0}, campj[19]={0};
	int campib[19]={0,0,0,0,0,1,1,1,1,1,2,2,2,2,3,3,3,4,4};
	int campjb[19]={0,1,2,3,4,0,1,2,3,4,0,1,2,3,0,1,2,0,1};
	int campiw[19]={15,15,15,15,15,14,14,14,14,14,13,13,13,13,12,12,12,11,11};
	int campjw[19]={15,14,13,12,11,15,14,13,12,11,15,14,13,12,15,14,13,15,14};

	if(myplay=='W') {
		copy(begin(campiw), end(campiw), begin(campi));
		copy(begin(campjw), end(campjw), begin(campj));
		oppcol='B';
	}
	else {
		copy(begin(campib), end(campib), begin(campi));
		copy(begin(campjb), end(campjb), begin(campj));
		oppcol='W';
	}

	if(level!=0) {
		gamemap[newi][newj]=oppcol;
		gamemap[curri][currj]='.';
	}

	int win=0;
	cell winmove;

	if(level<3) {

		for(int c=0;c<19;c++) {

			if(gamemap[campi[c]][campj[c]]==myplay)
				find_valid_move(gamemap,campi,campj,myplay,campi[c],campj[c],moves,s,level);

		}

		if(level==2)
			std::sort(moves.begin(), moves.end(), maxheap());

		for(int k=0;k<moves.size();k++) {

			if(level==0 && win==1) {
				break;
			}

			if(level==2) {
				alpha=max(alpha,moves[k].pos.val);
				if(alpha>=beta) {
					gamemap[newi][newj]='.';
					gamemap[curri][currj]=oppcol;
					return alpha;
				}
			}

			if(level==0 && util_func(gamemap,myplay,moves[k].rootplayer.i,moves[k].rootplayer.j,moves[k].pos.i,moves[k].pos.j,level)==0)
			{
				win=1;
				winmove=moves[k];
				flag=1;
				break;
			}

			if(level<2) {
				float minmax = alphabetafunc(gamemap, oppcol, !minmax, level+1, str, alpha, beta, besti, moves[k].rootplayer.i, moves[k].rootplayer.j, moves[k].pos.i, moves[k].pos.j);
				
				if(level==1) {
					beta=min(beta,minmax);
					if(alpha>=beta) {
						gamemap[newi][newj]='.';
						gamemap[curri][currj]=oppcol;
						return beta;
					}

				}

				if(level==0) {
					alpha=max(alpha,minmax);
					moves[k].pos.val=alpha;
					final_move.push(moves[k]);
				}
			}	
			flag=1;
		}

		if(level==0 && flag==1 && win!=1) {

			while(flag == 1 && !final_move.empty()) {
				cell temp = final_move.top();

				if(checkpawnwin(temp.pos.i,temp.pos.j,campi,campj) && ((myplay=='B' && !(temp.rootplayer.i<temp.pos.i && temp.rootplayer.j<temp.pos.j)) || (myplay=='W' && !(temp.rootplayer.i>temp.pos.i && temp.rootplayer.j>temp.pos.j)))) {
					final_move.pop();
				}
				else {
					flag=0;
					break;
				}
			}
			if(final_move.empty()) {
				flag=0;
			}
			else {
				cell tempo = final_move.top();
				final_move.pop();
				
				while(!final_move.empty()) {
					cell temp = final_move.top();
					if(checkpawnwin(temp.pos.i,temp.pos.j,campi,campj))
						final_move.pop();
					else
						break;
				}
                if(final_move.empty())
                    final_move.push(tempo);
				flag=1;
			}
		}

		if(flag==0) {

			alpha=al;
			beta=bet;
			moves.clear();

			while(!final_move.empty())
				final_move.pop();

			if(myplay=='B') {
				for(int a=0;a<16;a++) {

					for(int b=0;b<16;b++) {
						
						if(gamemap[a][b]==myplay && checkpawnwin(a,b,campi,campj)==false)
							find_valid_move(gamemap,campi,campj,myplay,a,b,moves,s,level);

					}
				}
			}

			else if(myplay=='W') {
				for(int a=15;a>=0;a--) {

					for(int b=15;b>=0;b--) {
						
						if(gamemap[a][b]==myplay && checkpawnwin(a,b,campi,campj)==false)
							find_valid_move(gamemap,campi,campj,myplay,a,b,moves,s,level);

					}
				}
			}

			if(level==2)
				std::sort(moves.begin(), moves.end(), maxheap());

			for(int k=0;k<moves.size();k++) {

				if(level==0 && win==1)
					break;

				if(level==2) {
					alpha=max(alpha,moves[k].pos.val);
					if(alpha>=beta) {
						gamemap[newi][newj]='.';
						gamemap[curri][currj]=oppcol;
						return alpha;
					}
				}

				if(level==0 && util_func(gamemap,myplay,moves[k].rootplayer.i,moves[k].rootplayer.j,moves[k].pos.i,moves[k].pos.j,level)==0)
				{
					win=1;
					winmove=moves[k];
					break;
				}

				if(level<2) {
					float minmax = alphabetafunc(gamemap, oppcol, !minmax, level+1, str, alpha, beta, besti, moves[k].rootplayer.i, moves[k].rootplayer.j, moves[k].pos.i, moves[k].pos.j);
					
					if(level==1) {
						beta=min(beta,minmax);
						if(alpha>=beta) {
							gamemap[newi][newj]='.';
							gamemap[curri][currj]=oppcol;
							return beta;
						}

					}

					if(level==0) {
						alpha=max(alpha,minmax);
						moves[k].pos.val=alpha;
						final_move.push(moves[k]);
					}
				}	
			}

		}
		
	}

	if(level!=0) {  
		gamemap[newi][newj]='.';
		gamemap[curri][currj]=oppcol;
	}


	if(level==0) {
		cell gamemove;
		if(win==1) {
			gamemove = winmove;
		}
		else
			gamemove = final_move.top();

		if(gamemove.movetype.compare("E")==0) {
			//cout<<"hi";
			str.append("E ");
			str.append(to_string(gamemove.rootplayer.j));
			str.append(",");
			str.append(to_string(gamemove.rootplayer.i));
			str.append(" ");
			str.append(to_string(gamemove.pos.j));
			str.append(",");
			str.append(to_string(gamemove.pos.i));
		}
		else if(gamemove.movetype=="J") {
			print_jumps(s,gamemove.rootplayer.i,gamemove.rootplayer.j,gamemove.pos.i,gamemove.pos.j,str);
		}
		besti=gamemove;
		return gamemove.pos.val;
	}

	else {
		if(level==1)
			return beta;
		else
			return alpha;
	}

}

 

int main() {

	time_t begin, end;

    time(&begin);

	char gamemap[16][16];

	string line, gametype, color, temp;
	float gametime;
	int i=0;

	ifstream myfile("input.txt");
	if(myfile.is_open()) {
		myfile>>gametype;
		myfile>>color;
		myfile>>gametime;
		for(int a=0;a<16;a++){
			myfile>>line;
			temp.assign(line);
			for(int b=0;b<16;b++){
				gamemap[a][b]=temp[b];
			}
		}
		myfile.close();
	}
	
	char myplay = color[0];

	ofstream myfiley("output.txt");
	
	cell gamemove;

	string stri = "";

	float gameval = alphabetafunc(gamemap,myplay,true,0,stri,-100000,100000,gamemove);

	myfiley<<stri;
	
	myfiley.close();

	return 0;

}