#ifndef HEADER_MinCostFlow
#define HEADER_MinCostFlow

#ifdef USE_BOOST

#include <limits.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <boost/config.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <limits>
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
  class InnocPred
  {
  public:
    Graph * pGraph;
    Graph::vertex_descriptor mSource, mSink;
  InnocPred(Graph * graph, Graph::vertex_descriptor source, Graph::vertex_descriptor sink): 
    pGraph(graph), mSource(source), mSink(sink) {
      
    }

    bool operator()(const Graph::edge_descriptor e) {

      if (target(e, *pGraph) == mSink && isInnocuous(source(e, *pGraph))) {
	return true;
      } else {
	return false;
      }
    }
    bool isInnocuous(Graph::vertex_descriptor v) {
      if (v == 1 || v == 2 || v == 3 || v == 4 ||
	  v == 43 || v == 42 ||v == 37 ||v == 40 ||v == 41 ) {
	return false;
      } else {
	return true;
      }
    }
  };
  struct EdgeProps {
    int cost;
    int capacity;
    int flow;
  };
  


  /**
   * reduces costs given a vector of potentials.  Modifies the graph. 
   */
  static void residualNetwork(const Graph & graph, Graph * pResult) {

    pResult->clear();
    Graph::vertex_iterator vi, vi_end;
    for (tie(vi, vi_end) = vertices(graph); vi != vi_end; vi++) {
      Graph::vertex_descriptor d = add_vertex(graph[*vi], *pResult);
      assert(d == *vi);
    }
    struct MinCostFlow::EdgeProps eProps;
    eProps.flow = 0;
    graph_traits < Graph >::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei) {

      eProps.cost = graph[*ei].cost;
      eProps.capacity = graph[*ei].capacity - graph[*ei].flow;
      if (eProps.capacity > 0) {
	add_edge(source(*ei, graph), target(*ei, graph), eProps, *pResult);
      }

      eProps.cost = -graph[*ei].cost;
      eProps.capacity = graph[*ei].flow;
      if (eProps.capacity > 0) {
	add_edge(target(*ei, graph), source(*ei, graph), eProps, *pResult);
      }
    }
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


  static void initializeGraph(Graph * pGraph) {
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


    initializeGraph(&graph);    

    

    while (1) {
      Graph residuals;
      residualNetwork(graph, &residuals);
      WriteGraphviz(residuals, string("residuals.dot"));

      vector<double> distances(num_vertices(residuals));
      vector<Graph::vertex_descriptor> predecessors(num_vertices(residuals));
      Graph::vertex_iterator vi, vi_end;
      for (tie(vi, vi_end) = vertices(residuals); vi != vi_end; vi++) {
	predecessors[*vi] = *vi;
	distances[*vi] = numeric_limits < float >::max();
      }
      distances[sourceVertex] = 0;


      cout << "Shortest path." << endl;

      bool r = bellman_ford_shortest_paths
	(residuals, 
	 weight_map(get(&EdgeProps::cost, residuals)).
	 //distance_map(make_iterator_property_map
	 //(distances.begin(), 
	 //get(vertex_index, residuals))).
	 distance_map(&distances[0]).
	 predecessor_map(&predecessors[0])
	 );
      assert(r); // never have negative weight cycles.


      if (predecessors[sinkVertex] == sinkVertex) {
	break; // we're done, no more paths.
      } else {
	Graph::vertex_descriptor v  = sinkVertex;
	pair<Graph::edge_descriptor, bool> newEdge = edge(predecessors[v], v, residuals);
	assert(newEdge.second);
	Graph::edge_descriptor e = newEdge.first;
	int minCapacity = residuals[e].capacity;
	unsigned int i = 0;
	while (v != sourceVertex) {
	  newEdge =edge(predecessors[v], v, residuals);
	  assert(newEdge.second);
	  e = newEdge.first;
	  if (residuals[e].capacity < minCapacity) {
	    minCapacity = residuals[e].capacity;
	  }
	  v = predecessors[v];
	  i++;
	  if (i > num_vertices(residuals)) {
	    cout << "Cycle detected in bf results!" << endl;
	    WriteGraphviz(residuals, string("residuals.weird.dot"));
	    cout << "Cycle: " << testBfCycle(residuals, sourceVertex, sinkVertex) << endl;
	    assert(0);
	  }
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

  static dynamic_properties propertiesToSerialize(Graph & graph) {
    dynamic_properties dp;
    dp.property("name", get(&VertexProps::name, graph));
    dp.property("net_supply", get(&VertexProps::net_supply, graph));
    
    dp.property("cost", get(&EdgeProps::cost, graph));
    dp.property("capacity", get(&EdgeProps::capacity, graph));
    dp.property("flow", get(&EdgeProps::flow, graph));
    //dp.property("id", get(vertex_index, graph));
    return dp;

  }
  static void WriteGraphviz(Graph & graph, string outfileName) {
    cout <<"Writing." << endl;
    ofstream outfile(outfileName.c_str());
    dynamic_properties dp = propertiesToSerialize(graph);
    write_graphviz(outfile, graph, dp, string("name"));  //, VertexLabeler(&graph), EdgeLabeler(&graph));
    outfile.close();
  }

  static void ReadGraphviz(string infileName, Graph * pGraph) {
    cout << "Clear graph." << endl;
    pGraph->clear();
    ifstream in(infileName.c_str());
    dynamic_properties dp = propertiesToSerialize(*pGraph);
    try
      {
	read_graphviz(in, *pGraph, dp, string("name"));
      }
    catch ( const std::exception & ex )
      {
        cout << "Exception: " << ex.what() << endl;
	assert(0);
      }
    //read_graphviz(infileName, *pGraph);

  }


  static void testFlow1();
  static void testFlow2();
  static void testFlow();
  static void testFlowZeroCostCycle();
  static void testWeirdResiduals();


  static bool testBfCycle(Graph & graph, Graph::vertex_descriptor sourceV, Graph::vertex_descriptor sinkV) {
    vector<double> distances(num_vertices(graph));
    vector<Graph::vertex_descriptor> predecessors(num_vertices(graph));
    bool r = bellman_ford_shortest_paths
      (graph, 
       root_vertex(sourceV).
       weight_map(get(&EdgeProps::cost, graph)).
       distance_map(&distances[0]).
       predecessor_map(&predecessors[0])
       );
    assert(r);
    Graph::vertex_descriptor v  = sinkV;
    assert(predecessors[sinkV] != sinkV);
    unsigned int i = 0;
    while (v != sourceV) {
      cout << "v: " << v << endl;
      v = predecessors[v];
      i++;
      if (i > num_vertices(graph)) {
	cout << "CYCLE in bellman_ford results!" << endl;
	return false;
      }
    }
    return true;
  }
    
};

#endif // USE_BOOST

#endif
