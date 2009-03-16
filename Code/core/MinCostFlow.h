#ifndef HEADER_MinCostFlow
#define HEADER_MinCostFlow
#include <limits.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <boost/config.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/read_dimacs.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
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
    //int residual_capacity;
    int flow;
    MinCostFlowGraph::edge_descriptor reverse;
    // flow is capacity - residual_capacity
  };

  MinCostFlowGraph residualNetwork(const MinCostFlowGraph & graph) {
    MinCostFlowGraph result;
    MinCostFlowGraph::vertex_iterator vi, vi_end;
    for (tie(vi, vi_end) = vertices(graph); vi != vi_end; vi++) {
      add_vertex(graph[*vi], result);
    }
    struct MinCostFlow::EdgeProps eProps;
    graph_traits < MinCostFlowGraph >::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei) {

      eProps.cost = graph[*ei].cost;
      eProps.capacity = graph[*ei].capacity - graph[*ei].flow;
      add_edge(source(*ei, graph), target(*ei, graph), eProps, result);

      eProps.cost = -graph[*ei].cost;
      eProps.capacity =  graph[*ei].flow;
      add_edge(target(*ei, graph), source(*ei, graph), eProps, result);
    }

    return result;
  }

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

  void zeroFlows(MinCostFlowGraph * pGraph) {
    MinCostFlowGraph & graph = *pGraph;
    graph_traits < MinCostFlowGraph >::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei) {
      graph[*ei].flow = 0;
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
    //addReversedEdges(&graph);
    zeroFlows(&graph);
    



    while (1) {
      cout << "Making residuals." << endl;
      MinCostFlowGraph residuals = residualNetwork(graph);


      vector<double> distances(num_vertices(residuals));
      vector<MinCostFlowGraph::vertex_descriptor> 
	predecessors(num_vertices(residuals));

      cout << "Finding a path." << endl;
      bool r = bellman_ford_shortest_paths
	(residuals, 
	 root_vertex(sourceVertex).
	 weight_map(get(&EdgeProps::cost, residuals)).
	 distance_map(make_iterator_property_map
		      (distances.begin(), 
		       get(vertex_index, residuals))).
	 predecessor_map(&predecessors[0])
	 );

      cout << "Augmenting flow." << endl;
      if (!r) {
	break; // we're done, no more paths.
      } else {
	MinCostFlowGraph::vertex_descriptor v  = sinkVertex;
	MinCostFlowGraph::edge_descriptor e = edge(predecessors[v], v, residuals).first;
	int minCapacity = residuals[e].capacity;
	while (v != sourceVertex) {
	  e = edge(predecessors[v], v, residuals).first;
	  if (residuals[e].capacity <= minCapacity) {
	    minCapacity = residuals[e].capacity;
	  }
	  v = predecessors[v];
	}
	v  = sinkVertex;
	while (v != sourceVertex) {
	  MinCostFlowGraph::edge_descriptor e = edge(predecessors[v], v, 
						     residuals).first;
	  graph[e].flow += minCapacity;
	  v = predecessors[v];
	}	
      }
    }
    cout << "Printing results." << endl;
    graph_traits < MinCostFlowGraph >::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei) {
      MinCostFlowGraph::vertex_descriptor s, t;
      s = source(*ei, graph);
      t = target(*ei, graph);
      cout << "From " << graph[s].name << " to " << graph[t].name;
      cout << " " << graph[*ei].flow << endl;
    }



    

    //property_map<MinCostFlowGraph, edge_reverse_t>::type 
    //rev = get(edge_reverse, graph);

    /*(    
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
      }*/


  }
};

#endif
