#include "utils.h"

double squareDistance(CvPoint2D32f p1, CvPoint2D32f p2)
 {
  double dx = p1.x - p2.x;
  double dy = p1.y - p2.y;
  return dx*dx + dy*dy;
}
