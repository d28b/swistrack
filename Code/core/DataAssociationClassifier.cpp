#include "DataAssociationClassifier.h"

#include <math.h>
#include <fstream>
#include <iostream>

#define THISCLASS DataAssociationClassifier

#include "Utility.h"



void THISCLASS::ExampleTableToMat(const THISCLASS::ExampleTable samples, 
				  CvMat ** data, CvMat ** responses) 
{

  assert(samples.size() != 0);
  const Example example = *samples.begin();
  *data = cvCreateMat(samples.size(), example.size(), CV_32F );
  *responses = cvCreateMat(samples.size(), 1, CV_32F );


  int samplesIdx = 0;
  for (ExampleTable::const_iterator i = samples.begin(); 
       i != samples.end(); ++i) {
    int featureIdx = 0;
    for (Example::const_iterator j = i->begin(); j != i->end(); ++j) {
      float * ddata = data[0]->data.fl + example.size() * samplesIdx;
      ddata[featureIdx] = j->second;
      featureIdx++;
    }
    samplesIdx++;
  }

}

THISCLASS::Example THISCLASS::ComputeExample(const Particle & p1, const Particle & p2)
{
  
  Example out;

  out["particle.1.mCenter.x"] = p1.mCenter.x;
  out["particle.1.mCenter.y"] = p1.mCenter.y;
  out["particle.1.timestamp"] = Utility::toMillis(p1.mTimestamp);

  out["particle.2.mCenter.x"] = p2.mCenter.x;
  out["particle.2.mCenter.y"] = p2.mCenter.y;
  out["particle.2.timestamp"] = Utility::toMillis(p2.mTimestamp);
  
  out["distanceInSpacePixels"] = Utility::Distance(p1.mCenter, p2.mCenter);
  out["distanceInTimeSeconds"] = fabs(p1.mTimestamp.Subtract(p2.mTimestamp).GetMilliseconds().ToDouble() / 1000.0);
  out["differenceInAreaPixels"] = fabs(p1.mArea - p2.mArea);


  if (p1.mColorModel != NULL && p2.mColorModel != NULL) {
    CvHistogram * mTmp1, * mTmp2;
    cvCopyHist(p1.mColorModel, &mTmp1);
    cvCopyHist(p2.mColorModel, &mTmp2);
    cvNormalizeHist(mTmp1, 1);
    cvNormalizeHist(mTmp2, 1);
    double colorSim = cvCompareHist(mTmp1, mTmp2, CV_COMP_BHATTACHARYYA); 
    out["differenceInColor"] = colorSim;
  } else {
    //out["differenceInColor"] = 0;
  }
  
  return out;
}


void THISCLASS::Train(const ExampleTable samples)
{
  assert(samples.size() != 0);

  const Example example = *samples.begin();
  int numFeatures = example.size();
  int numSamples = samples.size();
  CvMat * var_type = cvCreateMat(numFeatures + 1, 1, CV_8U );
  cvSet(var_type, cvScalarAll(CV_VAR_ORDERED));
  cvSetReal1D(var_type, numFeatures, CV_VAR_CATEGORICAL );
  CvMat * trainingOrTesting = cvCreateMat( 1, numSamples, CV_8UC1 );
  {
    int numTrainingSamples = 100;
    CvMat tmp;
    cvGetCols(trainingOrTesting, &tmp, 0, numTrainingSamples);
    cvSet( &tmp, cvRealScalar(1) );
    cvGetCols(trainingOrTesting, &tmp, numTrainingSamples, numSamples);
    cvSetZero(&tmp);
  }
  CvMat* responses = 0;
  CvMat* data = 0;

  forest.train(data, CV_ROW_SAMPLE, responses, 0, trainingOrTesting, var_type, 0,
	       CvRTParams(10,10,0,false,15,0,true,4,100,0.01f,CV_TERMCRIT_ITER));
  

}


THISCLASS::ExampleTable THISCLASS::fromFile(const string fileName)
 {
#define SIZE 1024
    char buffer[SIZE];

    vector<string> keys;
    ExampleTable data;

    ifstream fin(fileName.c_str());
    cout << "File: " << fileName << endl;

    assert(fin.good());
    fin.getline(buffer, SIZE);
    assert(fin.good());
    istringstream line(buffer);
    
    do {
      string key;
      line >> key;
      keys.push_back(key);
    } while (line);


    while (! fin.eof()) {
      fin.getline(buffer, SIZE);
      assert(fin.eof() || fin.good());
      istringstream line(buffer);
      
      
    }
    fin.close();
    
    
    return data;
    
  }
