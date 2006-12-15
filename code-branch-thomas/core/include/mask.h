#ifndef mask_h        
#define mask_h
//****************************************************************************/
// Filename :   $RCSfile: mask.h,v $
// Version  :   $Revision: 1.0.0.0 $
//
// Author   :   $Author: correll $
//              Nikolaus Correll
//              Ecole Polytechnique Federale de Lausanne
//              Swarm-Intelligent Systems Group
//              CH-1015 Lausanne
//              Switzerland
//
//
// Date     :   $Date: 2004/12/16 12:54:17 $


/** \file mask.h
* \brief Mask operations for counting number of objects within masks and determing coverage of its contours
*/


// See end of this header file for usage notes
/*****************************************************************************/
/******************************** inclusions *********************************/
/*****************************************************************************/


#include "libgeom.h"
#include "Track.h"
#include "TrackingImage.h"
#include "Component.h"




using namespace std;


/*****************************************************************************/
/******************************** defines ************************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** types **************************************/

/*****************************************************************************/

/*****************************************************************************/
/******************************** prototypes *********************************/
/*****************************************************************************/

class Mask : public Component
{
  public:
    void ToggleDisplay();
    void DrawMask(TrackingImage* colorimg);
    int GetNrOfMasks();

    Mask();
    ~Mask();
  
    void UpdateCount(vector<Track>* targets, int frame);
  
  private:
    VisBlobSet masks;
    int nrmasks;
    IplImage* maskimg;
    FILE* output;
    vector<CvPoint> areas;
    vector<int> nrobjects;
    int display;

    char* maskfname;
    char* outputfname;
  };

#endif
