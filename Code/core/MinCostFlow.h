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
  typedef adjacency_list < listS, vecS, bidirectionalS, 
    VertexProps,
    EdgeProps > MinCostFlowGraph;

  void minCostFlow(MinCostFlowGraph graph) {

    MinCostFlowGraph::vertex_descriptor v  = * vertices(graph).first;
    
    
    vector<double> distances(num_vertices(graph));
    
    bellman_ford_shortest_paths(graph, v, 
				weight_map(get(&EdgeProps::cost, graph)).
				distance_map(make_iterator_property_map
					     (distances.begin(), 
					      get(vertex_index, graph))));
    
    for (unsigned int i = 0; i < num_vertices(graph); ++i) {
      cout << "Vertex: " << graph[i].name << endl;
    }

    cout << "Comput min cost flow." << endl;
  }
};

#endif
