#ifndef HEADER_Graph
#define HEADER_Graph
#include <wx/datetime.h>
#include <cv.h>
#include <list>

#define VcountMax 100						//!< Number of vertices in the graph (= number of tracked particles).
#define conCompMax 100

//! A graph.
class Graph {

public:
	int  Vcount;
	int conComp;
	int AdjM[VcountMax][VcountMax];			//!< Adjacency matrix for the graph.
	int conCompSize[conCompMax][VcountMax]; 		//!< Number of vertices for each connected component of the graph.
	
	void getNeighbors(int v, std::list<int> x);		//!< returns the list of neighbors for each vertex (used for BFS in the graph)
	int GetMaxConnectedComponents(); 	//!< returns the cardinality of the largest connected component of the graph

	//! Constructor.
	Graph();
	//! Destructor.
	~Graph() {
	}
};

#endif

