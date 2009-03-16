#ifndef HEADER_MinCostFlow
#define HEADER_MinCostFlow
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <boost/config.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/read_dimacs.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
using namespace boost;
using namespace std;

void testFlow();

class MinCostFlow {
 public:
  struct VertexProps {
    std::string name;
  };
  struct EdgeProps {
    double cost;
    int capacity;
  };
  typedef adjacency_list < listS, vecS, directedS, 
    VertexProps,
    EdgeProps > MinCostFlowGraph;

  void minCostFlow(MinCostFlowGraph graph) {
    cout << "Computing min cost flow." << endl;

    MinCostFlowGraph::vertex_descriptor v  = * vertices(graph).first;
    
    
    vector<double> distances(num_vertices(graph));
    cout << "name: " << graph[v].name << endl;
    bool r = bellman_ford_shortest_paths
      (graph,
       weight_map(get(&EdgeProps::cost, graph)).
       distance_map(make_iterator_property_map
		    (distances.begin(), 
		     get(vertex_index, graph))).
       root_vertex(v)
       );
    if (r) {
      for (unsigned int i = 0; i < num_vertices(graph); ++i) {
	cout << "Vertex: " << graph[i].name << " ";
	cout << distances[i] << endl;
      }
    } else {
      cout << "Negative cycle." << endl;
    }


  }
};

#endif
