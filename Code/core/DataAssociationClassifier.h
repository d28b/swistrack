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
  typedef map<string, double> Example;
  typedef vector<Example> ExampleTable;

  
  void Train(const vector<Example> samples);
  bool IsSameTrack(const Particle & p1, const Particle & p2);


  static Example ComputeExample(const Particle & p1, const Particle & p2);


  static void ExampleTableToMat(const ExampleTable samples, 
				CvMat ** data, CvMat ** responses) ;


  static ExampleTable fromFile(const string fileName);
  static bool IsTrainingFeature(string name);
  static int NumTrainingFeatures(Example ex);

  void ExampleToMat(const Example e, CvMat ** sample);


 private:
  CvRTrees forest;

  
};


#endif
