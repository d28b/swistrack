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
  vertex_descriptor v1 = add_vertex(vProps, graph);

  vProps.name = "2";
  vertex_descriptor v2 = add_vertex(vProps, graph);

  vProps.name = "3";
  vertex_descriptor v3 = add_vertex(vProps, graph);

  vProps.name = "4";
  vertex_descriptor v4 = add_vertex(vProps, graph);

  vProps.name = "5";
  vertex_descriptor v5 = add_vertex(vProps, graph);

  // outgoing 1
  eProps.cost = 5;
  eProps.capacity = 11;
  add_edge(v1, v2, eProps, graph);

  eProps.cost = 4;
  eProps.capacity = 16;
  add_edge(v1, v3, eProps, graph);
  
  // outgoing 2
  eProps.cost = 4;
  eProps.capacity = 18;
  add_edge(v2, v4, eProps, graph);

  // outgoing 3
  eProps.cost = 3;
  eProps.capacity = 13;
  add_edge(v3, v4, eProps, graph);

  // no outgoing 4

  // outgoing 5
  eProps.cost = 3;
  eProps.capacity = 10;
  add_edge(v5, v2, eProps, graph);

  eProps.cost = 5;
  eProps.capacity = 11;
  add_edge(v5, v1, eProps, graph);

  eProps.cost = 5;
  eProps.capacity = -25;
  add_edge(v5, v3, eProps, graph);


  
  flow.minCostFlow(graph);

}



