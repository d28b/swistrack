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
  int numTrainingFeatures = example.size() - 1;
  *data = cvCreateMat(samples.size(), numTrainingFeatures, CV_32F );
  *responses = cvCreateMat(samples.size(), 1, CV_32F );


  int samplesIdx = 0;
  for (ExampleTable::const_iterator i = samples.begin(); 
       i != samples.end(); ++i) {
    int featureIdx = 0;
    for (Example::const_iterator j = i->begin(); j != i->end(); ++j) {
      if (j->first == "class") {
	float* response = responses[0]->data.fl + samplesIdx;
	*response = j->second;
      } else {
	float * ddata = data[0]->data.fl + numTrainingFeatures * samplesIdx;
	ddata[featureIdx] = j->second;
	featureIdx++;
      }

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
    out["differenceInColor"] = -1;
  }
  
  return out;
}


void THISCLASS::Train(const ExampleTable samples)
{
  assert(samples.size() != 0);

  const Example example = *samples.begin();
  int numTrainingFeatures = example.size() - 1; // don't include the class feature.
  int numSamples = samples.size();
  CvMat * var_type = cvCreateMat(numTrainingFeatures + 1, 1, CV_8U );
  cvSet(var_type, cvScalarAll(CV_VAR_ORDERED));
  cvSetReal1D(var_type, numTrainingFeatures, CV_VAR_CATEGORICAL );
  int numTrainingSamples = (int) (numSamples * 0.5);
  CvMat * trainingOrTesting = cvCreateMat(1, numSamples, CV_8UC1 );
  {

    CvMat tmp;
    cvGetCols(trainingOrTesting, &tmp, 0, numTrainingSamples);
    cvSet( &tmp, cvRealScalar(1) );
    cvGetCols(trainingOrTesting, &tmp, numTrainingSamples, numSamples);
    cvSetZero(&tmp);
  }
  CvMat* responses = 0;
  CvMat* data = 0;
  ExampleTableToMat(samples, &data, &responses);

  printf("response: %dx%d\n", responses->rows, responses->cols);
  printf("Data: %dx%d\n", data->rows, data->cols);
  printf("Training or testing: %dx%d\n", trainingOrTesting->rows, trainingOrTesting->cols);
  printf("var type: %dx%d\n", var_type->rows, var_type->cols);

  for (int i = 0; i < responses->rows; i++) {
    float value = cvGetReal1D(responses, i);
    if (value != 0 && value != 1) {
      printf("Weird value[%d]: %f\n", i, value);
    }
  }

  CvRTParams params(10,10,
		    0,false,
		    15,0,
		    true,
		    0,100,
		    0.01f,CV_TERMCRIT_ITER);
  //forest.train(data, CV_ROW_SAMPLE, responses, 0, trainingOrTesting, var_type, 0, params);
  forest.train(data, CV_ROW_SAMPLE, responses, 0, NULL, var_type, 0, params);
  
  double train_hr = 0, test_hr = 0;

  for (int i = 0; i < numSamples; i++) {

    CvMat sample;
    cvGetRow( data, &sample, i );
    double r = forest.predict( &sample );
    r = fabs((double)r - responses->data.fl[i]) <= FLT_EPSILON ? 1 : 0;
    
    if( i < numTrainingSamples ) {
      train_hr += r;
    } else {
      test_hr += r;
    }
  }
  test_hr /= (double)(numSamples-numTrainingSamples);
  train_hr /= (double)numTrainingSamples;
  printf( "Recognition rate: train = %.1f%%, test = %.1f%%\n",
	  train_hr*100., test_hr*100. );
  printf( "Number of trees: %d\n", forest.get_tree_count() );
  // Print variable importance
  CvMat* var_importance = (CvMat*)forest.get_var_importance();
  if( var_importance )
    {
      double rt_imp_sum = cvSum( var_importance ).val[0];
      printf("var#\timportance (in %%):\n");
      for(int i = 0; i < var_importance->cols; i++ )
	printf( "%-2d\t%-4.1f\n", i,
		100.f*var_importance->data.fl[i]/rt_imp_sum);
    }
}


THISCLASS::ExampleTable THISCLASS::fromFile(const string fileName)
 {
#define SIZE 1024
    char buffer[SIZE];

    vector<string> keys;
    ExampleTable data;

    ifstream fin(fileName.c_str());

    assert(fin.good());
    fin.getline(buffer, SIZE);
    assert(fin.good());
    istringstream line(buffer);
    
    while (true) {
      string key;
      line >> key;
      if (key == "" || key.find("particle") == 0) {
	break;
      }
      keys.push_back(key);
    }

    while (! fin.eof()) {
      fin.getline(buffer, SIZE);
      assert(fin.eof() || fin.good());
      istringstream line(buffer);
      Example ex;
      for (unsigned int i = 0; i < keys.size(); i++) {
	float value;
	line >> value;
	ex[keys[i]] = value;
      }
      data.push_back(ex);
    }
    fin.close();
    
    
    return data;
    
  }
