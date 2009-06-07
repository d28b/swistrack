#ifndef HEADER_DataAssociationClassifier
#define HEADER_DataAssociationClassifier

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <ml.h>

using namespace std;

#include "Particle.h"

class DataAssociationClassifier {

 public: 
  typedef map<string, float> Example;
  typedef vector<Example> ExampleTable;

  

  static Example ComputeExample(const Particle & p1, const Particle & p2);

  void Train(const vector<Example> samples);
    

  static void ExampleTableToMat(const ExampleTable samples, 
				CvMat ** data, CvMat ** responses) ;

  static ExampleTable fromFile(const string fileName);



 private:
  CvRTrees forest;

  
};


#endif
