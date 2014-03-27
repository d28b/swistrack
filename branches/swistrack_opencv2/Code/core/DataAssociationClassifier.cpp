#include "DataAssociationClassifier.h"

#include <math.h>
#include <fstream>
#include <iostream>

#define THISCLASS DataAssociationClassifier

#include "Utility.h"

class ConfusionMatrix {
public:
  int truePositives;
  int falsePositives;
  int trueNegatives;
  int falseNegatives;
  ConfusionMatrix():
    truePositives(0), falsePositives(0), trueNegatives(0), falseNegatives(0) {
  }
  
  void print() {
    printf("        Predicted\n");
    printf("            T     F\n");
    printf("Actual  T  %d    %d\n", truePositives, falseNegatives);
    printf("        F  %d    %d\n", falsePositives, trueNegatives);
  }
  
};

bool THISCLASS::IsTrainingFeature(string name) 
{
  if (name == "class" || name.find("particle") == 0) {
    return false;
  } else {
    return true;
  }
}

int THISCLASS::NumTrainingFeatures(Example ex) 
{
  int count = 0;
  for (Example::const_iterator j = ex.begin(); j != ex.end(); ++j) {
    if (IsTrainingFeature(j->first)) {
      count++;
    }
  }
  return count;
}

void THISCLASS::ExampleToMat(const Example e, CvMat ** sample)

{  
  int numTrainingFeatures = NumTrainingFeatures(e);
  if (*sample == NULL) {
    *sample = cvCreateMat(1, numTrainingFeatures, CV_32F);
  }
  int featureIdx = 0;
  for (Example::const_iterator j = e.begin(); j != e.end(); ++j) {
    if (IsTrainingFeature(j->first)) {
      (**sample).data.fl[featureIdx] = j->second;
      featureIdx++;
    }
  }

  

}

void THISCLASS::ExampleTableToMat(const THISCLASS::ExampleTable samples, 
				  CvMat ** data, CvMat ** responses) 
{

  assert(samples.size() != 0);
  const Example example = *samples.begin();
  int numTrainingFeatures = NumTrainingFeatures(example);
  *data = cvCreateMat(samples.size(), numTrainingFeatures, CV_32F );
  *responses = cvCreateMat(samples.size(), 1, CV_32F );

  int samplesIdx = 0;
  for (ExampleTable::const_iterator i = samples.begin(); 
       i != samples.end(); ++i) {
    int featureIdx = 0;
    CvMat sample;
    cvGetRow( *data, &sample, samplesIdx );
    CvMat response;
    cvGetRow( *responses, &response, samplesIdx);
    for (Example::const_iterator j = i->begin(); j != i->end(); ++j) {
      if (j->first == "class") {
	*response.data.fl = j->second;
      } else {
	if (IsTrainingFeature(j->first)) {
	  sample.data.fl[featureIdx] = j->second;
	  featureIdx++;
	}
      }

    }
    samplesIdx++;
  }

}

bool THISCLASS::IsSameTrack(const Particle & p1, const Particle & p2) 
{
  Example e = ComputeExample(p1, p2);
  //CvMat * sample = cvCreateMat(1, e.size(),CV_32F );

  CvMat * sample = NULL;
  ExampleToMat(e, &sample);
  printf("sample: %dx%d\n", sample->rows, sample->cols);
  double r = forest.predict(sample);
  cvReleaseMat(&sample);
  if (r == 1.0) {
    return true;
  } else if (r == 0.0) {
    return false;
  } else {
    assert(0); // only handle binary decisions
    return false;
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
    CvHistogram * mTmp1 = NULL, * mTmp2 = NULL;

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
  vector<string> keys;
  for (Example::const_iterator j = example.begin(); j != example.end(); ++j) {
    keys.push_back(j->first);
  }
  int numTrainingFeatures = NumTrainingFeatures(example);
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

  ConfusionMatrix trainingCm;
  ConfusionMatrix testingCm;


  for (int i = 0; i < numSamples; i++) {

    CvMat sample;
    cvGetRow( data, &sample, i );
    double r = forest.predict( &sample );
    double correctValue = responses->data.fl[i];
    ConfusionMatrix * cm;
    if ( i < numTrainingSamples) {
      cm = &trainingCm;
      train_hr += r;
    } else {
      cm = &testingCm;
      test_hr += r;
    }

    if (r == 1 && correctValue == 1) {
      cm->truePositives += 1;
    } else if (r == 1 && correctValue == 0) {
      cm->falsePositives += 1;
    } else if (r == 0 && correctValue == 1) {
      cm->falseNegatives += 1;
    } else if (r == 0 && correctValue == 0) {
      cm->trueNegatives += 1;
    } else {
      assert(0);
    }
    
    r = fabs((double)r - correctValue) <= FLT_EPSILON ? 1 : 0;
    

  }
  test_hr /= (double)(numSamples-numTrainingSamples);
  train_hr /= (double)numTrainingSamples;
  printf("Training confusion matrix\n");
  trainingCm.print();
  printf("Testing confusion matrix\n");
  testingCm.print();
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
	printf( "%-2d\t%-4.1f\t%s\n", i,
		100.f*var_importance->data.fl[i]/rt_imp_sum, keys[i+1].c_str());
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
      if (key == "") {
	break;
      }
      keys.push_back(key);

    }

    while (! fin.eof()) {
      fin.getline(buffer, SIZE);
      assert(fin.eof() || fin.good());
      if (strlen(buffer) == 0) {
	continue;
      }
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
