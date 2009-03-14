#include <string.h>
#include <boost/config.hpp>
#include <iostream>
#include <string>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/read_dimacs.hpp>
#include <boost/graph/graph_utility.hpp>
using namespace boost;
class MinCostFlow {
  struct VertexProps {
    std::string name;
  };
  struct EdgeProps {
    double cost;
    int flow;
  };
  typedef adjacency_list < listS, vecS, bidirectionalS, 
    VertexProps, EdgeProps > Graph;
};