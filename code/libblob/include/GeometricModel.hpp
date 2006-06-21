
#ifndef GeometricModel_h        
#define GeometricModel_h
//****************************************************************************/
// Filename :   $RCSfile: GeometricModel.hpp,v $
// Version  :   $Revision: 1.5 $
//
// Author   :   $Author: yuri $
//              Yuri Lopez de Meneses
//              Ecole Polytechnique Federale de Lausanne
//              DMT-IPM-VISION
//              CH-1015 Lausanne
//              Switzerland
//
//
// Date     :   $Date: 2004/03/24 13:23:39 $
/** \file GeometricModel.hpp
    \brief A geometric shape model

    Purpose  : Detailed description
*/

/**
    
    This class enables to create a template made of reference points and to calculate
	the coordinates of a target point from the template. 

	 \author $Author: yuri $
     \version $Revision: 1.5 $
     \date $Date: 2004/03/24 13:23:39 $


*/

// See end of this header file for usage notes
/*****************************************************************************/
/******************************** inclusions *********************************/
/*****************************************************************************/

#include <iostream>
#include <fstream>
#include <cv.h>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace std; //So that we can use standard library names

/*****************************************************************************/
/******************************** defines ************************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** types **************************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** prototypes *********************************/
/*****************************************************************************/


class GeometricModel
{
	private:
			/** sine list */
			vector<double> sinus_seq; 
			/** cosinus list  */
			vector<double> cosinus_seq;
			/** learnt template points list  */
			vector<CvPoint2D32f> reference_points;
			/** target point */
			CvPoint2D32f target_point;
			/** Indicates wether a target is defined */
			bool target_set;
			bool model_learnt;
			

			/** draws a cross */
			void DrawCross(CvArr* array, CvPoint pt, int branch , int color);

	public:


		/** Constructor */
		GeometricModel(); 

		/** Destructor */
		~GeometricModel();

		/** Sets the target point */
		void SetTargetPoint(CvPoint2D32f point);

		/** Adds a reference point */
		int AddReferencePoint(CvPoint2D32f point);

		/** Adds a series of reference points */
		int AddListReferencePoints(CvPoint2D32f* point_list,int n);

		/** Empty reference points */
		int ClearReferencePoints();

		/** Create a model from the reference points to the target */
		double CreateModel();

		/** Calculate the target point from the found points list and the sines and cosines lists */
		CvPoint2D32f CalculateTarget(double* p_cond_number, double* p_error);

		/** Save Model to file */
		int SaveModel(char* fname);

		/** Load Model from file */
		int LoadModel(char* fname);

		/** Returns the number of reference points currently available*/
		int GetNumReferencePoints();

		/** Returns the number of necessary reference points */
		int GetNumModelPoints();

		/** Draws the reference points and model */
		int DrawModel(IplImage* img);

};

/*****************************************************************************/
/******************************** usage notes ********************************/
/*****************************************************************************/
/*

*/
#endif
