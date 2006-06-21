#ifndef VisBinarisation_h        
#define VisBinarisation_h
//****************************************************************************/
// Filename :   $RCSfile : VisBinarisation.hpp,v $
// Version  :   $Revision : 1.0 $
//
// Author   :   $Author: yuri $
//              Cyrille  Lebossé
//              Ecole Polytechnique Federale de Lausanne
//              DMT-IPM-VISION
//              CH-1015 Lausanne
//              Switzerland
//
//
// Date     :   $Date: 2003/12/17 02:03:48 $

/*! \class VisBinarisation VisBinarisation.hpp
 *  \brief Definition of the VisBinarisation class which enables to binarise
 *			gray scale images simply or with optimization.
 *
 *	The optimized binarisation is used to find the threshold giving
 *	the best value for the chosen blob feature. 
 *
 *	
 *	 \author $Author: yuri $
 *   \version $Revision: 1.1.1.1 $
 *   \date $Date: 2003/12/17 02:03:48 $
 *
 */



// See end of this header file for usage notes
/*****************************************************************************/
/******************************** inclusions *********************************/
/*****************************************************************************/

#include <iostream>
#include <math.h>
#include "VisBlobSet.hpp" 
//#include <conio.h>


/*****************************************************************************/
/******************************** defines ************************************/
/*****************************************************************************/
//#define PI 3.14159265



typedef int (VisBlobSet ::* p_blobset_meth) ();
typedef double (VisBlobSet ::* p_blobset_meth_1) ( int);
typedef int (VisBlobSet ::* p_blobset_meth_2) (double);
typedef int (VisBlobSet ::* p_blobset_meth_3) ( double, double);

/*****************************************************************************/
/******************************** prototypes *********************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** types **************************************/
/*****************************************************************************/

/*! \class VisBinarisation VisBinarisation.hpp
 *  \brief Definition of the VisBinarisation class which enables to binarise
 *			gray scale images simply or with optimization.
 *
 *	The optimized binarisation is used to find the threshold giving
 *	the best value for the chosen blob feature. 
 *
 *	
 *	 \author $Author: yuri $
 *   \version $Revision: 1.1.1.1 $
 *   \date $Date: 2003/12/17 02:03:48 $
 *
 */

class VisBinarisation
{

	/** private members*/
	private:

		IplImage* source_image;
		IplImage* dest_image;
		IplImage* tmp_image;
		IplImage* tmp_image_2;
		int current_threshold;
		int opt_threshold;
		int init_threshold;
		int minimum_threshold;
		int maximum_threshold;
		VisBlobSet intern_blobs;
		double opt_feature_value;
		double blob_area;
		double tol;
		int morphology;
		


		/** \brief Gets the feature of the blob chosen for optimization and calculated with the given threshold */
		double GetOptBlobFeature(double temp_threshold);


		//double (VisBlobSet ::* GetFeature) (const int) = &(VisBlobSet::GetArea);
		p_blobset_meth_1  GetFeature ;

		//double (VisBlobSet ::* CalcFeature) (const void) = &(VisBlobSet::CalculateAreas);
		p_blobset_meth  CalcFeature ;


		//double (VisBlobSet ::* SortByFeature) (const double ) = &(VisBlobSet::SortByArea);
		p_blobset_meth_2  SortByFeature;


		//double (BlobSet ::* SelectByFeature) (const double, const double) = &(BlobSet::SelectByArea);
		p_blobset_meth_3  SelectByFeature;


			
		typedef double (VisBinarisation::* Function) (double);		
		
		/** \brief Optimization method used to find the minimum of a one dimension function */
		int GoldenSearch( Function func , double ax, double bx, double cx, double tolerance ,double* p_xmin, double* p_funcmin, VisBinarisation* p_bin);



	/**public members*/
	public:

		/** Default constructor */
		VisBinarisation(); 
		
		/** Destructor */
		~VisBinarisation();  
		
		/** \brief Sets start threshold and search limits for optimized binarisation*/
		void InitializeOptBinarisation( int min_threshold=0, int start_threshold=60, int max_threshold=255, double tolerance=0.01){ init_threshold = start_threshold;  minimum_threshold = min_threshold; maximum_threshold = max_threshold; tol=tolerance;}

		/** \brief Sets Optimimum value of blob feature and blob area if the feature is different from area in order to not confuse the reference blob with others*/
		int SetOptFeature(double feature_value=0, double given_blob_area=0, char index_feat='A');


		/** \brief Sets Source Image (It is highly recommended to define a ROI surrounding the reference blob in order to accelerate the execution and not confuse the reference blob with others) */
		void SetSourceImage(IplImage* src_image){ source_image=src_image; }

		/** \brief Sets Destination Image (It is highly recommended to define a ROI surrounding the reference blob in order to accelerate the execution and not confuse the reference blob with others) */
		void SetDestinationImage(IplImage* d_image){ dest_image=d_image; }

		/** \brief Gets Destination Image */
		IplImage* GetDestImage(void){ return(dest_image); }


		/** \brief Simple Binarisation*/
		int SimpleBinarisation( int threshold );

		/** \brief Morphology to clean binarised image */
		void  clean_morpho(void);

		/** \brief Optimize Binarisation */
		void OptBinarisation(int* opt_threshold, int morpho = 1);
	
};


/*****************************************************************************/
/******************************** usage notes ********************************/
/*****************************************************************************/
/*

*/
#endif
