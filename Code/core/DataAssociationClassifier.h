#ifndef HEADER_DataAssociationClassifier
#define HEADER_DataAssociationClassifier

#include <string>
#include <map>

using namespace std;

#include "Particle.h"
class DataAssociationClassifier {

 public: 
  typedef map<string, float> FeatureVector;

  static FeatureVector ComputeFeatureVector(const Particle & p1, const Particle & p2);


};


#endif
