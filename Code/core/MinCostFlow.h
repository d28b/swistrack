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
#include <boost/graph/graphviz.hpp>
using namespace boost;
using namespace std;



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
      eProps.capacity = graph[*ei].flow;
      if (eProps.capacity > 0) {
	add_edge(target(*ei, graph), source(*ei, graph), eProps, result);
      }
    }

    return result;
  }

  class TruePred
  {
  public:
    bool operator()(const Graph::edge_descriptor d) {
      return true;
    }
  };

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
    graph_traits < Graph >::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(*pGraph); ei != ei_end; ++ei) {
      (*pGraph)[*ei].flow = 0;
    }
  }

  /**
   * Adds a single source and sink node to the graph, 
   */
  static VertexPair addSourceAndSink(Graph * pGraph) {
    Graph & graph = *pGraph;
    Graph::vertex_descriptor sourceVertex, sinkVertex;
    struct MinCostFlow::VertexProps vProps;    
    vProps.name = "tSource";
    sourceVertex = add_vertex(vProps, graph);

    vProps.name = "tSink";

    sinkVertex = add_vertex(vProps, graph);


    updateSourceAndSink(pGraph, sourceVertex, sinkVertex);
    VertexPair pair(sourceVertex, sinkVertex);

    return pair;
  }

  static void updateSourceAndSink(Graph * pGraph, 
				  Graph::vertex_descriptor sourceVertex, Graph::vertex_descriptor sinkVertex) {

    Graph & graph = *pGraph;
    
    clear_vertex(sourceVertex, graph);
    clear_vertex(sinkVertex, graph);


    graph[sourceVertex].net_supply = 0;
    graph[sinkVertex].net_supply = 0;

    EdgeProps eProps;
    eProps.cost = 0;
    
    Graph::vertex_iterator vi, vi_end;
    for (tie(vi, vi_end) = vertices(graph); vi != vi_end; vi++) {
      int supply = graph[*vi].net_supply;
      if (*vi != sourceVertex && *vi != sinkVertex) {
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
      } else {
	// skip the source and sink we're adding. 
      }
    }

  }

  /**
   * Compute minCost flow for the graph, with the given source and sink vertices. 
   * Use addSourceAndSink to add these vertices to your graph if your graph has more than one
   * source and sink node.  
   */
  static void minCostFlow(Graph * pGraph, Graph::vertex_descriptor sourceVertex, Graph::vertex_descriptor sinkVertex) {
    Graph & graph = *pGraph;
    cout << "Computing min cost flow." << endl;


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
	  pair<Graph::edge_descriptor, bool> ePair = edge(predecessors[v], v, 
						     graph);

	  if (ePair.second) {
	    Graph::edge_descriptor e = ePair.first;
	    graph[e].flow += minCapacity;
	  } else {
	    ePair = edge(v, predecessors[v], graph);
	    assert(ePair.second);
	    graph[ePair.first].flow -= minCapacity;
	  }
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
      cost += graph[*ei].flow * graph[*ei].cost;
      
    }

    return cost;
  }


  /**
   * Prints the flow in the graph.
   */
  static void PrintFlow(const Graph & graph) {
    graph_traits < Graph >::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei) {
      Graph::vertex_descriptor s, t;
      s = source(*ei, graph);
      t = target(*ei, graph);
      cout << "From " << graph[s].name << " to " << graph[t].name;
      cout << " " << graph[*ei].flow << endl;
    }

  }
  
  class VertexLabeler {
  public:
    const Graph * pGraph;
    VertexLabeler(const Graph * g) {
      pGraph = g;
    }
    void operator()(std::ostream& out, const Graph::vertex_descriptor v) {
      out << "[label=\"" << v << " " << (*pGraph)[v].name << "\"]";

    }
  };

  class EdgeLabeler {
  public:
    const Graph * pGraph;
    EdgeLabeler(const Graph * g) {
      pGraph = g;
    }
    void operator()(std::ostream& out, const Graph::edge_descriptor e) {
      EdgeProps props = (*pGraph)[e];
      out << "[label=\"" << e << " " << props.flow << "/" << props.capacity << ", " << props.cost << "\"]";

    }
  };
  static void PrintGraphviz(const Graph & graph, string outfileName) {
    ofstream outfile(outfileName.c_str());
    write_graphviz(outfile, graph, VertexLabeler(&graph), EdgeLabeler(&graph));
    outfile.close();
  }


  static void testFlow1();
  static void testFlow2();
  static void testFlow();


    
};


#endif
