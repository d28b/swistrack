#include "Graph.h"
#define THISCLASS Graph
#include <queue>
using namespace std;

THISCLASS::Graph() {
	for (int i=0; i<VcountMax; i++) {
		for (int j=0; j<VcountMax; j++) {
			AdjM[i][j] = 0;
		}
	}
}

void  THISCLASS::getNeighbors(int v, std::list<int> x) {
	int n=Vcount;
//	list<int> x;
	if(v<0||v>=n)
    //return x;
	return;
	
	for(int j=0;j<n;j++)
		{
		if(AdjM[v][j]!=0)
			x.push_front(j);
		}
	//return x;
}


int THISCLASS::GetMaxConnectedComponents() {
	
	int components = 0;					// number of connected components
	int numVinC = 0;					// number of vertexes in connected component
	list<int> VnuminCC;					// number of vertexes associated to the connected components
    queue<int> S;
    int n = Vcount;						// number of vertices in graph
    bool visited [n];		// flag associated to each vertex
    for(int i=1;i<n;i++)
        visited[i]=false;				// initialization of flags (no visited vertexes)

    visited[0]=true;				
    S.push(0);
    while(!S.empty())
    {
        int v = S.front();
        S.pop();
        for (int i=0; i<n; i++) {
			if (AdjM[v][i]>0){
                if(visited[i]==false)
                {
                    S.push(i);
					numVinC++;
                    visited[i]=true;
                }
            }
        }

        if(S.empty())
        {
            components++;
			VnuminCC.push_back(++numVinC);	// numVinC is finally incremented with to account for the initial push
            for(int i=1;i<n;i++)
            {
                if(visited[i]==false)
                {
                    S.push(i);
                    visited[i]=true;
					numVinC = 0;	
                    break;
                }
            }
        }
    }
	
	// extract the maximum from the list to return it
	int max;
	max = 0;
	list<int>::iterator iter;
	
	for (iter = VnuminCC.begin(); iter!= VnuminCC.end(); iter++) {
		//printf("cs %i\t", *iter);
		if (*iter >= max) max = *iter;
	}
	
	
    return max;
}
