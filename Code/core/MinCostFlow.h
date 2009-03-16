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
  struct VertexProps;
  struct EdgeProps;
  typedef adjacency_list < listS, vecS, bidirectionalS, 
    VertexProps,
    EdgeProps > MinCostFlowGraph;

  struct VertexProps {
    std::string name;
    int net_supply;
  };
  struct EdgeProps {
    double cost;
    int capacity;
    int residual_capacity;
    MinCostFlowGraph::edge_descriptor reverse;
    // flow is capacity - residual_capacity
  };

  typedef pair<MinCostFlowGraph::vertex_descriptor, 
    MinCostFlowGraph::vertex_descriptor> VertexPair;

  void addReversedEdges(MinCostFlowGraph * pGraph) {
    MinCostFlowGraph & graph = *pGraph;
    
    graph_traits < MinCostFlowGraph >::edge_iterator ei, ei_end;

    vector<MinCostFlowGraph::edge_descriptor> cachedEdges;
      
    for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei) {
      cachedEdges.push_back(*ei);
    }

    struct MinCostFlow::EdgeProps eProps;
    eProps.capacity = 0;
    eProps.cost = 0;
    eProps.residual_capacity = 0;
    
    for (vector<MinCostFlowGraph::edge_descriptor>::iterator i = 
	   cachedEdges.begin();
	 i != cachedEdges.end(); i++) {
      
      MinCostFlowGraph::vertex_descriptor s = source(*i, graph);
      MinCostFlowGraph::vertex_descriptor t = target(*i, graph);
      eProps.reverse = *i;
      pair<MinCostFlowGraph::edge_descriptor, bool> newEdge 
	= add_edge(t, s, eProps, graph);
      graph[*i].reverse = newEdge.first;
    }
    
    
  }

  VertexPair addSourceAndSink(MinCostFlowGraph * pGraph) {
    MinCostFlowGraph & graph = *pGraph;
    MinCostFlowGraph::vertex_descriptor sourceVertex, sinkVertex;
    
    
    
    struct MinCostFlow::VertexProps vProps;

    vProps.name = "source";
    vProps.net_supply = 0;
    sourceVertex = add_vertex(vProps, graph);

    vProps.name = "sink";
    vProps.net_supply = 0;
    sinkVertex = add_vertex(vProps, graph);

    EdgeProps eProps;
    eProps.cost = 0;

    MinCostFlowGraph::vertex_iterator vi, vi_end;
    for (tie(vi, vi_end) = vertices(graph); vi != vi_end; vi++) {
      int supply = graph[*vi].net_supply;
      
      if (supply > 0) {
	eProps.capacity = supply;
	add_edge(sourceVertex, *vi, eProps, graph);
	graph[sourceVertex].net_supply += supply;
      } else if (supply < 0) {
	eProps.capacity = -supply;
	add_edge(*vi, sinkVertex, eProps, graph);
	graph[sinkVertex].net_supply += supply;
      } else {
	// do nothing if it equals zero.
      }
    }
    VertexPair pair(sourceVertex, sinkVertex);
    return pair;
  }

  void minCostFlow(MinCostFlowGraph graph) {
    cout << "Computing min cost flow." << endl;


    VertexPair sourceAndSink = addSourceAndSink(&graph);
    MinCostFlowGraph::vertex_descriptor sourceVertex = sourceAndSink.first;
    MinCostFlowGraph::vertex_descriptor sinkVertex = sourceAndSink.second;
    addReversedEdges(&graph);




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

    property_map<MinCostFlowGraph, edge_reverse_t>::type 
      rev = get(edge_reverse, graph);

    
    long flow = push_relabel_max_flow
      (graph, sourceVertex, sinkVertex,
       capacity_map(get(&EdgeProps::capacity, graph)).
       residual_capacity_map(get(&EdgeProps::residual_capacity, graph)).
       vertex_index_map(identity_property_map()).
       reverse_edge_map(get(&EdgeProps::reverse, graph)));
    cout << "Flow: " << flow << endl;
    graph_traits < MinCostFlowGraph >::edge_iterator ei, ei_end;    
    for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei) {
      cout << "Flow in node: " << graph[source(*ei, graph)].name << 
	" to " << graph[target(*ei, graph)].name << ": ";
      cout << (graph[*ei].capacity - graph[*ei].residual_capacity) << endl;
    }


  }
};

#endif
