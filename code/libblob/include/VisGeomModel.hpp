
#ifndef VISGEOMMODEL_HPP        
#define VISGEOMMODEL_HPP
//****************************************************************************/
// Filename :   $RCSfile : VisGeomModel.hpp, v $
// Version  :   $Revision : 1.0 $
//
// Author   :   $Author: yuri $
//              Cyrille Lebossé
//              Ecole Polytechnique Federale de Lausanne
//              DMT-IPM-VISION
//              CH-1015 Lausanne
//              Switzerland
//
//
// Date     :   $Date: 2004/03/24 13:23:39 $
// See end of this header file for usage notes

/*****************************************************************************/
/** \file VisGeomModel.hpp

    \brief Definition of the "VisGeomModel class" which enables to create a geometrical model 
			and to calculate a target point thanks to the references.
*/


/**
    
    This class enables to create a template made of reference points and to calculate
	the coordinates of a target point from the template. 

	 \author $Author: yuri $
     \version $Revision: 1.3 $
     \date $Date: 2004/03/24 13:23:39 $


*/

/*****************************************************************************/
/******************************** inclusions *********************************/
/*****************************************************************************/

#include <iostream>
#include <cv.h>
#include <vector>
#include <algorithm>
#include <math.h>


/*****************************************************************************/
/******************************** defines ************************************/
/*****************************************************************************/

using namespace std; //So that we can use standard library names


//*****************************************************************************/
/** \file VisGeomModel.hpp

    \brief Definition of the "VisGeomModel class" which enables to create a geometrical model 
			and to calculate a target point thanks to the references.
*/


/**
    
    This class enables to create a template made of reference points and to calculate
	the coordinates of a target point from the template. 

	 \author $Author: yuri $
     \version $Revision: 1.3 $
     \date $Date: 2004/03/24 13:23:39 $


*/

/*****************************************************************************/
/******************************** types **************************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** prototypes *********************************/
/*****************************************************************************/

class VisGeomModel
{
	/** private members*/
	private:

			/** \brief sinus list */
			vector<double> sinus_seq; 
			/** \brief cosinus list  */
			vector<double> cosinus_seq;
			/** \brief learnt template points list  */
			vector<CvPoint2D32f> learnt_points_seq;
			/** \brief found template points list  */
			vector<CvPoint2D32f> found_points_seq;
			/** \brief learnt target point */
			CvPoint2D32f learnt_target;
			/** \brief calculated target point */
			CvPoint2D32f calc_target;
			

	/** public members*/
	public:
		/** \brief Constructor */
		VisGeomModel(); 

		/** \brief Destructor */
		~VisGeomModel();
		
		/** \brief Sets the learnt target point */
		void SetLearntTargetPoint(CvPoint2D32f point){ learnt_target = point; }

		/** \brief Adds the given point to the learnt template points list  */
		void SetLearntPointsList(CvPoint2D32f point); 

		/** \brief Adds the given point to the found template points list  */
		void SetFoundPointsList(CvPoint2D32f point); 

		/** \brief Creates the template and computes the model from the points list */
		int CreateModel(double* p_cond_number);

		/** \brief Calculate the target point from the found points list and the sinus and cosinus lists */
		CvPoint2D32f CalculateTarget(double* p_cond_number, double* p_error);

		/** \brief Clears "FoundPointsList" */
		void ClearFoundPointsList(void){ found_points_seq.clear(); }

		/** \brief Clears "LearntPointsList" */
		void ClearLearntPointsList(void){ learnt_points_seq.clear(); sinus_seq.clear(); cosinus_seq.clear(); }

		/** \brief Accessor to the found points list */
		vector<CvPoint2D32f>* GetFoundPointsSeq(){ return(&found_points_seq); }

		/** \brief Accessor to the learnt points list */
		vector<CvPoint2D32f>* GetLearntPointsSeq(){ return(&learnt_points_seq); }
		



};

/*****************************************************************************/
/******************************** usage notes ********************************/
/*****************************************************************************/
/*

*/
#endif
