#include "MinCostFlow.h"

void testFlow() 
{
  MinCostFlow flow;
  MinCostFlow::MinCostFlowGraph graph;
  
  //enum {v1, v2, v3, v4, v5};
  
  struct MinCostFlow::EdgeProps eProps;
  struct MinCostFlow::VertexProps vProps;


  typedef MinCostFlow::MinCostFlowGraph::vertex_descriptor vertex_descriptor;
  
  vProps.name = "1";
  vProps.net_supply = 5;
  vertex_descriptor v1 = add_vertex(vProps, graph);

  vProps.name = "2";
  vProps.net_supply = 2;
  vertex_descriptor v2 = add_vertex(vProps, graph);

  vProps.name = "3";
  vProps.net_supply = 0;
  vertex_descriptor v3 = add_vertex(vProps, graph);

  vProps.name = "4";
  vProps.net_supply = -2;
  vertex_descriptor v4 = add_vertex(vProps, graph);

  vProps.name = "5";
  vProps.net_supply = -4;
  vertex_descriptor v5 = add_vertex(vProps, graph);

  vProps.name = "6";
  vProps.net_supply = -1;
  vertex_descriptor v6 = add_vertex(vProps, graph);

  // outgoing 1
  eProps.cost = 3;
  eProps.capacity = 4;
  add_edge(v1, v3, eProps, graph);

  eProps.cost = 3;
  eProps.capacity = 1;
  add_edge(v1, v2, eProps, graph);
  
  // outgoing 2
  eProps.cost = 7;
  eProps.capacity = 2;
  add_edge(v2, v3, eProps, graph);

  // outgoing 3
  eProps.cost = 1;
  eProps.capacity = 8;
  add_edge(v3, v6, eProps, graph);

  eProps.cost = 7;
  eProps.capacity = 5;
  add_edge(v3, v5, eProps, graph);

  eProps.cost = 5;
  eProps.capacity = 2;
  add_edge(v3, v4, eProps, graph);

  // outgoing 4
  eProps.cost = 3;
  eProps.capacity = 1;
  add_edge(v4, v5, eProps, graph);

  // no outgoing 5

  // no outgoing 6

  
  flow.minCostFlow(graph, v1, v4);

}



