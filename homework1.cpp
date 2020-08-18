#include <iostream>
#include <algorithm>
#include <list>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <string>
#include <queue>
#include <map>

using namespace std;

struct xyz {
	int i,j;
};

struct asnode {
	xyz parent;
	int path_cost;
	int heuristic;
	int elevation;
	bool copen;
};

class node {
public:
	xyz child, parent;

	bool operator == (const xyz& tempxy)
	{
		return tempxy.i == child.i && tempxy.j == child.j;
	}
};

map<pair<int, int>, struct asnode> nodemap;
int row, col, elev;

class astar{
public:

	struct compareas{
		bool operator()(struct xyz const& n1, struct xyz const& n2)
		{
			return nodemap[{n1.i,n1.j}].path_cost+nodemap[{n1.i,n1.j}].heuristic > nodemap[{n2.i,n2.j}].path_cost+nodemap[{n2.i,n2.j}].heuristic;
		}
	};

	priority_queue<xyz, vector<xyz>, compareas> openq;

	struct xyz start, target;
	string pth;

	astar(xyz strt, xyz trgt) {
		
		pth.assign("");
		start.i=strt.i;
		start.j=strt.j;
		target.i=trgt.i;
		target.j=trgt.j;
		nodemap[{start.i,start.j}].path_cost=0;
		nodemap[{start.i,start.j}].heuristic=10*(abs(target.i-start.i)+abs(target.j-start.j)) + (-6)*(min(abs(target.i-start.i),abs(target.j-start.j)));
		nodemap[{start.i,start.j}].parent=start;
		
	}

	int astar_search() {
		
		openq.push(start);
		nodemap[{start.i,start.j}].copen=true;
		while(!openq.empty()) {
			struct xyz temp = openq.top();
			if(temp.i==target.i && temp.j==target.j)
				return 1;
			openq.pop();
			nodemap[{temp.i,temp.j}].copen=false;
			if(get_neighbours(temp))
				return 1;
			
		}
		return 0;
	}

	int get_neighbours(xyz site) {
		
		struct xyz neighbour;
	    int i=site.i, j=site.j, val=nodemap[{i,j}].elevation, elev_dif, heur, cost;
	    int x[3]={i-1,i,i+1};
	    int y[3]={j-1,j,j+1};
	    for(int a=0;a<3;a++) {
	        for(int b=0;b<3;b++) {
	            if(a==1 && b==1)
	                continue;
	            if(x[a]>=0 && x[a]<row && y[b]>=0 && y[b]<col) {
	            	elev_dif = abs(val-nodemap[{x[a],y[b]}].elevation);
	                if(elev_dif<=elev) {
	                    neighbour.i=x[a];
	                    neighbour.j=y[b];
	                    if(a==1 || b==1)
	                    	cost = nodemap[{i,j}].path_cost + 10 + elev_dif;
	                    else
	                    	cost = nodemap[{i,j}].path_cost + 14 + elev_dif;
	                    heur = 10*(abs(target.i-x[a])+abs(target.j-y[b])) + (-6)*(min(abs(target.i-x[a]),abs(target.j-y[b])));
	                    if(!check_neighbour(neighbour, cost, heur)) {
	                    	nodemap[{neighbour.i,neighbour.j}].parent=site;
	                    	nodemap[{neighbour.i,neighbour.j}].path_cost = cost;
	                    	nodemap[{neighbour.i,neighbour.j}].heuristic = heur;
	                    	if(!nodemap[{neighbour.i,neighbour.j}].copen) {
	                    		openq.push(neighbour);
	                    		nodemap[{neighbour.i,neighbour.j}].copen=true;
	                    	}
	                    	
	                    }
	                }
	            }
	        }
	    }
	    return 0;
	}

	int check_neighbour(struct xyz& sitexy, int cost, int heur) {
		
		if(nodemap[{sitexy.i,sitexy.j}].copen==true) {
			if(nodemap[{sitexy.i,sitexy.j}].path_cost+nodemap[{sitexy.i,sitexy.j}].heuristic < cost+heur)
				return 1;
			else
				return 0;
		}
		else if(nodemap[{sitexy.i,sitexy.j}].parent.i != -1 && nodemap[{sitexy.i,sitexy.j}].parent.j != -1) {
			if(nodemap[{sitexy.i,sitexy.j}].path_cost+nodemap[{sitexy.i,sitexy.j}].heuristic < cost+heur)
				return 1;
			else
				return 0;
		}
		return 0;
	}

	int astar_path(xyz& trgt)
	{
		if(start.i == trgt.i && start.j == trgt.j) {
			pth.append(to_string(trgt.j));
			pth.append(",");
			pth.append(to_string(trgt.i));
			pth.append(" ");
			return 1;
		}

		astar_path(nodemap[{trgt.i,trgt.j}].parent);
		pth.append(to_string(trgt.j));
		pth.append(",");
		pth.append(to_string(trgt.i));
		pth.append(" ");
		return 1;
	}

};

class ucs{
public:
	struct compareucs{
	    bool operator()(struct xyz const& n1, struct xyz const& n2)
	    {
	        return nodemap[{n1.i,n1.j}].path_cost > nodemap[{n2.i,n2.j}].path_cost;
	    }
	};

	priority_queue<xyz, vector<xyz>, compareucs> openq;
	struct xyz start, target;
	string pth;

	ucs(xyz strt, xyz trgt) {
		
		pth.assign("");
		start.i=strt.i;
		start.j=strt.j;
		target.i=trgt.i;
		target.j=trgt.j;
		nodemap[{start.i,start.j}].path_cost=0;
		nodemap[{start.i,start.j}].parent=start;
	}

	int ucs_search() {
		
		openq.push(start);
		nodemap[{start.i,start.j}].copen=true;
		while(!openq.empty()) {
			struct xyz temp = openq.top();
			if(temp.i==target.i && temp.j==target.j) {
				return 1;
			}
			openq.pop();
			nodemap[{temp.i,temp.j}].copen=false;
			if(get_neighbours(temp))
				return 1;
		}
		return 0;
	}

	int get_neighbours(xyz site) {

		struct xyz neighbour;
	    int i=site.i, j=site.j, val=nodemap[{i,j}].elevation, elev_dif, cost;
	    int x[3]={i-1,i,i+1};
	    int y[3]={j-1,j,j+1};
	    for(int a=0;a<3;a++) {
	        for(int b=0;b<3;b++) {
	            if(a==1 && b==1)
	                continue;
	            if(x[a]>=0 && x[a]<row && y[b]>=0 && y[b]<col) {
	            	elev_dif = abs(val-nodemap[{x[a],y[b]}].elevation);
	                if(elev_dif<=elev) {
	                    neighbour.i=x[a];
	                    neighbour.j=y[b];
	                    if(a==1 || b==1)
	                    	cost = nodemap[{i,j}].path_cost + 10;
	                    else
	                    	cost = nodemap[{i,j}].path_cost + 14;
	                    if(!check_neighbour(neighbour, cost)) {
	                    	nodemap[{neighbour.i,neighbour.j}].parent=site;
	                    	nodemap[{neighbour.i,neighbour.j}].path_cost = cost;
	                    	if(!nodemap[{neighbour.i,neighbour.j}].copen) {
	                    		openq.push(neighbour);
	                    		nodemap[{neighbour.i,neighbour.j}].copen=true;
	                    	}
	                    	
	                    }
	                }
	            }
	        }
	    }
	    return 0;
	}

	int check_neighbour(xyz& sitexy, int cost) {
		
		if(nodemap[{sitexy.i,sitexy.j}].copen==true) {
			if(nodemap[{sitexy.i,sitexy.j}].path_cost < cost)
				return 1;
			else
				return 0;
		}
		else if(nodemap[{sitexy.i,sitexy.j}].parent.i != -1 && nodemap[{sitexy.i,sitexy.j}].parent.j != -1) {
			if(nodemap[{sitexy.i,sitexy.j}].path_cost < cost)
				return 1;
			else
				return 0;
		}
		return 0;
	}

	int ucs_path(xyz& trgt)
	{
		if(start.i == trgt.i && start.j == trgt.j) {
			pth.append(to_string(trgt.j));
			pth.append(",");
			pth.append(to_string(trgt.i));
			pth.append(" ");
			return 1;
		}	

		ucs_path(nodemap[{trgt.i,trgt.j}].parent);
		pth.append(to_string(trgt.j));
		pth.append(",");
		pth.append(to_string(trgt.i));
		pth.append(" ");
		return 1;	
	}
};


class bfs{
public:
	
	queue<xyz> bqueue;	
	xyz start, target;
	string pth;
	list<node> bfstreenode;

	bfs(xyz strt, xyz trgt) {

		start.i=strt.i;
		start.j=strt.j;
		target.i=trgt.i;
		target.j=trgt.j;
		pth.assign("");
	}

	int bfs_search() {

		bqueue.push(start);
		nodemap[{start.i,start.j}].copen=true;
		while(!bqueue.empty()) {
			xyz temp = bqueue.front();
			if(temp.i==target.i && temp.j==target.j) {
				return 1;
			}
			bqueue.pop();
			if(get_neighbours(temp))
				return 1;
		}
		return 0;
	}

	int get_neighbours(xyz site) {

		xyz neighbour;
		int elev_dif, i=site.i, j=site.j, val=nodemap[{i,j}].elevation;
	    int x[3]={i-1,i,i+1};
	    int y[3]={j-1,j,j+1};
	    for(int a=0;a<3;a++) {
	        for(int b=0;b<3;b++) {
	            if(a==1 && b==1)
	                continue;
	            if(x[a]>=0 && x[a]<row && y[b]>=0 && y[b]<col) {
	            	elev_dif = abs(val-nodemap[{x[a],y[b]}].elevation);
	                if(elev_dif<=elev) {
	                    neighbour.i=x[a];
	                    neighbour.j=y[b];
	                    
	                    if(!nodemap[{neighbour.i,neighbour.j}].copen) {
	                    	node temp;
	                    	temp.child = neighbour;
	                    	temp.parent = site;
	                    	bqueue.push(neighbour);
	                    	bfstreenode.push_back(temp);
	                    	nodemap[{neighbour.i,neighbour.j}].copen=true;
	                    	if(temp.child.i==target.i && temp.child.j==target.j)
	                    		return 1;
	                    }
	                }
	            }
	        }
	    }
	    return 0;
	}
	int bfs_path(xyz& trgt)
	{
		if(start.i == trgt.i && start.j == trgt.j) {
			pth.append(to_string(trgt.j));
			pth.append(",");
			pth.append(to_string(trgt.i));
			pth.append(" ");
			return 1;
		}	
		list<node>::iterator i = find(bfstreenode.begin(), bfstreenode.end(), trgt);
		if(i!=bfstreenode.end()) {
			node temp = *i;
			bfs_path(temp.parent);
			pth.append(to_string(trgt.j));
			pth.append(",");
			pth.append(to_string(trgt.i));
			pth.append(" ");
			return 1;
		}
		else
			return 0;
	}

};

int main() {

	string line, type;
	int i=0;
	int si, sj, nf;
	int fi[1000], fj[1000];

	struct asnode fake;
	fake.parent.i=-1;
	fake.parent.j=-1;
	fake.path_cost=-1;
	fake.heuristic=-1;
	fake.copen=false;

	struct xyz start, target;

	ifstream myfile("input.txt");
	if(myfile.is_open()) {
		/*while(getline(myfile,line))
		{
				if(i==0) {
					type.assign(line);
					myfile>>col>>row;
					myfile>>si>>sj;
					myfile>>elev;
					myfile>>nf;
					for(int j=0; j<nf; j++) {
						myfile>>fi[j]>>fj[j];
					}
					for(int a=0;a<row;a++){
						for(int b=0;b<col;b++){
							myfile>>fake.elevation;
							nodemap[{a,b}]=fake;
						}
					}
					break;
				}
		}*/
		myfile>>type;
		myfile>>col>>row;
		myfile>>si>>sj;
		myfile>>elev;
		myfile>>nf;
		for(int j=0; j<nf; j++) {
			myfile>>fi[j]>>fj[j];
		}
		for(int a=0;a<row;a++){
			for(int b=0;b<col;b++){
				myfile>>fake.elevation;
				nodemap[{a,b}]=fake;
			}
		}
		myfile.close();
	}

    start.i=sj;
    start.j=si;
    int reps=nf, test, path_test;

    ofstream fp;
    fp.open("output.txt");

    while(nf--) {
    	if(type.compare("BFS")==0) {

    		target.i=fj[reps-nf-1];
		    target.j=fi[reps-nf-1];

			bfs b = bfs(start,target);
		    test = b.bfs_search();
		    if(test==0) {
		    	fp<<"FAIL\n";
		    }
		    else if(test==1) {
		    	path_test = b.bfs_path(target);
		    	fp<<b.pth;
		    	fp<<"\n";
		    }

		    for(int a=0;a<row;a++){
				for(int b=0;b<col;b++){
					nodemap[{a,b}].copen=false;
				}
			}
    	}
    	else if(type.compare("UCS")==0) {

    		target.i=fj[reps-nf-1];
		    target.j=fi[reps-nf-1];

			ucs u = ucs(start,target);
		    test = u.ucs_search();
		    if(test==0) {
		    	fp<<"FAIL\n";
		    }
		    else if(test==1) {
		    	path_test = u.ucs_path(target);
		    	fp<<u.pth;
		    	fp<<"\n";
		    }

		    for(int a=0;a<row;a++){
				for(int b=0;b<col;b++){
					nodemap[{a,b}].parent.i=-1;
					nodemap[{a,b}].parent.j=-1;
					nodemap[{a,b}].path_cost=-1;
					nodemap[{a,b}].copen=false;
				}
			}
    	}
    	else if(type.compare("A*")==0) {

    		target.i=fj[reps-nf-1];
		    target.j=fi[reps-nf-1];

			astar a = astar(start,target);
		    test = a.astar_search();
		    if(test==0) {
		    	fp<<"FAIL\n";
		    }
		    else if(test==1) {
		    	path_test = a.astar_path(target);
		    	fp<<a.pth;
		    	fp<<"\n";
		    }

		    for(int a=0;a<row;a++){
				for(int b=0;b<col;b++){
					nodemap[{a,b}].parent.i=-1;
					nodemap[{a,b}].parent.j=-1;
					nodemap[{a,b}].path_cost=-1;
					nodemap[{a,b}].heuristic=-1;
					nodemap[{a,b}].copen=false;
				}
			}
    	}

    }

  	fp.close();

    return 0;
}