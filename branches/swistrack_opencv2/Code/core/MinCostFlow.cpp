#include "MinCostFlow.h"

#ifdef USE_BOOST

void MinCostFlow::testFlow() {
  testFlow1();
  testFlow2();
}

void MinCostFlow::testWeirdResiduals() {
  Graph residuals;
  MinCostFlow::ReadGraphviz(string("residuals.weird.dot"), &residuals);
  Graph::vertex_descriptor sourceV, sinkV;
  Graph::vertex_iterator vi, vi_end;
  for (tie(vi, vi_end) = vertices(residuals); vi != vi_end; vi++) {
    if (residuals[*vi].name == "tSource") {
      sourceV = *vi;
    }
    if (residuals[*vi].name == "tSink") {
      sinkV = *vi;
    }
  }
  //  testBfCycle(residuals, sourceV, sinkV);
  //assert(0);
}

void MinCostFlow::testFlow2() {
  cout << "Running second test." << endl;
  MinCostFlow::Graph graph;

  struct MinCostFlow::EdgeProps eProps;
  struct MinCostFlow::VertexProps vProps;


  typedef MinCostFlow::Graph::vertex_descriptor vertex_descriptor;

  vProps.name = "1";
  vProps.net_supply = 5;
  vertex_descriptor v1 = add_vertex(vProps, graph);

  vProps.name = "2";
  vProps.net_supply = 0;
  vertex_descriptor v2 = add_vertex(vProps, graph);

  vProps.name = "3";
  vProps.net_supply = 0;
  vertex_descriptor v3 = add_vertex(vProps, graph);

  vProps.name = "4";
  vProps.net_supply = -3;
  vertex_descriptor v4 = add_vertex(vProps, graph);

  vProps.name = "5";
  vProps.net_supply = -2;
  vertex_descriptor v5 = add_vertex(vProps, graph);

  // outgoing 1
  eProps.capacity = 7;
  eProps.cost = 1;
  add_edge(v1, v2, eProps, graph);

  eProps.capacity = 7;
  eProps.cost = 5;
  add_edge(v1, v3, eProps, graph);

  eProps.capacity = 2;
  eProps.cost = -2;
  add_edge(v2, v3, eProps, graph);

  eProps.capacity = 3;
  eProps.cost = 8;
  add_edge(v2, v4, eProps, graph);

  eProps.capacity = 3;
  eProps.cost = -3;
  add_edge(v3, v4, eProps, graph);

  eProps.capacity = 2;
  eProps.cost = 4;
  add_edge(v3, v5, eProps, graph);


  VertexPair sourceAndSink = addSourceAndSink(&graph);
  Graph::vertex_descriptor sourceVertex = sourceAndSink.first;
  Graph::vertex_descriptor sinkVertex = sourceAndSink.second;
  MinCostFlow::minCostFlow(&graph, sourceVertex, sinkVertex);
  assert(graph[edge(v1, v2, graph).first].flow == 2);
  assert(graph[edge(v1, v3, graph).first].flow == 3);
  assert(graph[edge(v2, v3, graph).first].flow == 2);
  assert(graph[edge(v2, v4, graph).first].flow == 0);
  assert(graph[edge(v3, v4, graph).first].flow == 3);
  assert(graph[edge(v3, v5, graph).first].flow == 2);
  cout << "Cost: " << MinCostFlow::CostOfFlow(graph)  << endl;
  assert(MinCostFlow::CostOfFlow(graph) == 12);

  
}

void MinCostFlow::testFlow1() {
  cout <<"Running first test." << endl;
  MinCostFlow::Graph graph;

  struct MinCostFlow::EdgeProps eProps;
  struct MinCostFlow::VertexProps vProps;


  typedef MinCostFlow::Graph::vertex_descriptor vertex_descriptor;

  
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
  eProps.capacity = 3;
  eProps.cost = 4;
  add_edge(v1, v3, eProps, graph);

  eProps.capacity = 3;
  eProps.cost = 1;
  add_edge(v1, v2, eProps, graph);
  
  // outgoing 2
  eProps.capacity = 7;
  eProps.cost = 2;
  add_edge(v2, v3, eProps, graph);

  // outgoing 3
  eProps.capacity = 1;
  eProps.cost = 8;
  add_edge(v3, v6, eProps, graph);

  eProps.capacity = 7;
  eProps.cost = 5;
  add_edge(v3, v5, eProps, graph);

  eProps.capacity = 5;
  eProps.cost = 2;
  add_edge(v3, v4, eProps, graph);

  // outgoing 4
  eProps.capacity = 3;
  eProps.cost = 1;
  add_edge(v4, v5, eProps, graph);

  // no outgoing 5

  // no outgoing 6

  VertexPair sourceAndSink = addSourceAndSink(&graph);
  Graph::vertex_descriptor sourceVertex = sourceAndSink.first;
  Graph::vertex_descriptor sinkVertex = sourceAndSink.second;



  // make sure it works twice. 
  for (int i = 0; i < 2; i++) {
    MinCostFlow::updateSourceAndSink(&graph, sourceVertex, sinkVertex);
    MinCostFlow::minCostFlow(&graph, sourceVertex, sinkVertex);
    assert(graph[edge(v1, v3, graph).first].flow == 2);
    assert(graph[edge(v1, v2, graph).first].flow == 3);
    
    assert(graph[edge(v3, v6, graph).first].flow == 1);
    assert(graph[edge(v3, v5, graph).first].flow == 1);
    assert(graph[edge(v3, v4, graph).first].flow == 5);
    
    assert(graph[edge(v4, v5, graph).first].flow == 3);
    
    assert(MinCostFlow::CostOfFlow(graph) == 47);
  }
  cout <<"Graphviz." << endl;
  cout <<"Writing." << endl;
  MinCostFlow::WriteGraphviz(graph, string("test.dot"));
  Graph readInGraph;
  cout <<"Reading." << endl;
  MinCostFlow::ReadGraphviz(string("test.dot"), &readInGraph);
  
  cout <<"Assert." << endl;
  assert(MinCostFlow::CostOfFlow(readInGraph) == 47);
  MinCostFlow::updateSourceAndSink(&readInGraph, sourceVertex, sinkVertex);
  MinCostFlow::minCostFlow(&readInGraph, sourceVertex, sinkVertex);
  assert(MinCostFlow::CostOfFlow(readInGraph) == 47);

}

#endif // USE_BOOST
