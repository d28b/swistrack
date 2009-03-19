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

/**
 *
 * Implemented following
 * http://www.topcoder.com/tc?module=Static&d1=tutorials&d2=minimumCostFlow1
 */
class MinCostFlow {
 public:
  struct VertexProps;
  struct EdgeProps;
  typedef adjacency_list < listS, vecS, bidirectionalS, 
    VertexProps,
    EdgeProps > Graph;

  struct VertexProps {
    std::string name;
    int net_supply;
  };
  struct EdgeProps {
    double cost;
    int capacity;
    int flow;
    //Graph::edge_descriptor reverse;
    // flow is capacity - residual_capacity
  };

  static Graph residualNetwork(const Graph & graph) {
    Graph result;
    //Graph::vertex_iterator vi, vi_end;
    //for (tie(vi, vi_end) = vertices(graph); vi != vi_end; vi++) {
    //Graph::vertex_descriptor d = add_vertex(graph[*vi], result);
    //cout << "Descriptor: " << d << " " << *vi << " " << graph[d].name << endl;
    //}
    struct MinCostFlow::EdgeProps eProps;
    graph_traits < Graph >::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei) {

      eProps.cost = graph[*ei].cost;
      eProps.capacity = graph[*ei].capacity - graph[*ei].flow;
      if (eProps.capacity > 0) {
	add_edge(source(*ei, graph), target(*ei, graph), eProps, result);
      }

      eProps.cost = -graph[*ei].cost;
      eProps.capacity =  graph[*ei].flow;
      if (eProps.capacity > 0) {
	add_edge(target(*ei, graph), source(*ei, graph), eProps, result);
      }
    }

    return result;
  }

  typedef pair<Graph::vertex_descriptor, 
    Graph::vertex_descriptor> VertexPair;

  static void addReversedEdges(Graph * pGraph) {
    Graph & graph = *pGraph;
    
    graph_traits < Graph >::edge_iterator ei, ei_end;

    vector<Graph::edge_descriptor> cachedEdges;
      
    for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei) {
      cachedEdges.push_back(*ei);
    }

    struct MinCostFlow::EdgeProps eProps;
    eProps.capacity = 0;
    eProps.cost = 0;

    
    for (vector<Graph::edge_descriptor>::iterator i = 
	   cachedEdges.begin();
	 i != cachedEdges.end(); i++) {
      
      Graph::vertex_descriptor s = source(*i, graph);
      Graph::vertex_descriptor t = target(*i, graph);
      //eProps.reverse = *i;
      pair<Graph::edge_descriptor, bool> newEdge 
	= add_edge(t, s, eProps, graph);
      //graph[*i].reverse = newEdge.first;
    }
    
    
  }

  static void zeroFlows(Graph * pGraph) {
    Graph & graph = *pGraph;
    graph_traits < Graph >::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei) {
      graph[*ei].flow = 0;
    }
  }
  static VertexPair addSourceAndSink(Graph * pGraph) {
    Graph & graph = *pGraph;
    Graph::vertex_descriptor sourceVertex, sinkVertex;
    
    
    
    struct MinCostFlow::VertexProps vProps;

    vProps.name = "source";
    vProps.net_supply = 0;
    sourceVertex = add_vertex(vProps, graph);

    vProps.name = "sink";
    vProps.net_supply = 0;
    sinkVertex = add_vertex(vProps, graph);

    EdgeProps eProps;
    eProps.cost = 0;

    Graph::vertex_iterator vi, vi_end;
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

  static void minCostFlow(Graph * pGraph) {
    Graph & graph = *pGraph;
    cout << "Computing min cost flow." << endl;


    VertexPair sourceAndSink = addSourceAndSink(&graph);
    Graph::vertex_descriptor sourceVertex = sourceAndSink.first;
    Graph::vertex_descriptor sinkVertex = sourceAndSink.second;
    //addReversedEdges(&graph);
    zeroFlows(&graph);
    



    while (1) {
      Graph residuals = residualNetwork(graph);

      vector<double> distances(num_vertices(residuals));
      vector<Graph::vertex_descriptor> 
	predecessors(num_vertices(residuals));


      bellman_ford_shortest_paths
	(residuals, 
	 root_vertex(sourceVertex).
	 weight_map(get(&EdgeProps::cost, residuals)).
	 distance_map(make_iterator_property_map
		      (distances.begin(), 
		       get(vertex_index, residuals))).
	 predecessor_map(&predecessors[0])
	 );


      if (predecessors[sinkVertex] == sinkVertex) {
	break; // we're done, no more paths.
      } else {
	Graph::vertex_descriptor v  = sinkVertex;
	Graph::edge_descriptor e = edge(predecessors[v], v, residuals).first;
	int minCapacity = residuals[e].capacity;
	int i = 0;
	while (v != sourceVertex) {
	  e = edge(predecessors[v], v, residuals).first;
	  if (residuals[e].capacity < minCapacity) {
	    minCapacity = residuals[e].capacity;
	  }
	  v = predecessors[v];
	  i++;
	}
	v  = sinkVertex;
	while (v != sourceVertex) {
	  Graph::edge_descriptor e = edge(predecessors[v], v, 
						     graph).first;
	  graph[e].flow += minCapacity;
	  v = predecessors[v];
	}	
      }
    }
  }

  /**
   * Returns the cost of the flow in the graph. 
   * This is the cost times the flow over all edges. 
   */
  static double CostOfFlow(const Graph & graph) {
    
    double cost = 0;

    graph_traits < Graph >::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei) {
      Graph::vertex_descriptor s, t;
      s = source(*ei, graph);
      t = target(*ei, graph);
      cout << "From " << graph[s].name << " to " << graph[t].name;
      cout << " " << graph[*ei].flow << endl;
      cost += graph[*ei].flow * graph[*ei].cost;
      
    }

    return cost;
  }
    
};

#endif
