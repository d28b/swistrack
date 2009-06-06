#include "DataAssociationClassifier.h"

#include <math.h>

#define THISCLASS DataAssociationClassifier

#include "Utility.h"

THISCLASS::FeatureVector THISCLASS::ComputeFeatureVector(const Particle & p1, const Particle & p2)
{
  
  FeatureVector out;

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
