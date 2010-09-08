#ifndef HEADER_ComponentAdaptiveBackgroundSubtractionMode
#define HEADER_ComponentAdaptiveBackgroundSubtractionMode

#include <cv.h>
#include "Component.h"

#include <vector>
#include <queue>

const int MAX_BUFFER_SIZE = 5;

using namespace std;

class UnitCounter
{
 public:
  uchar color[3];
  int count;  
  UnitCounter()
    {
      color[0] = color[1] = color[2] = 0;	
      count = 0;	
    }
  UnitCounter(char s[3], int c)
    {
      color[0] = s[0];
      color[1] = s[1];
      color[2] = s[2];
      count = c;
    } 
  bool operator<(const UnitCounter& b) const
  {
    return count<b.count;
  }
};

class ArrayCounter
{
  public:
    UnitCounter a[MAX_BUFFER_SIZE];
};

class ComponentAdaptiveBackgroundSubtractionMode: public Component
{
 public:
  ComponentAdaptiveBackgroundSubtractionMode(SwisTrackCore *stc);
  ~ComponentAdaptiveBackgroundSubtractionMode();

  void OnStart();
  void OnReloadConfiguration();
  void OnStep();
  void OnStepCleanup();
  void OnStop();
  Component *Create() 
  {
    return new ComponentAdaptiveBackgroundSubtractionMode(mCore);
  }
  void UpdateBackgroundModel(IplImage * inputImage);

 private:
  queue<IplImage*> mImageBuffer;
  vector<vector<ArrayCounter> > mCounter;
  IplImage * mBackgroundModel;
  IplImage * mOutputImage;
  Display mDisplayOutput;
};

#endif
