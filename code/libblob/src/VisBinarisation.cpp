//******************************* DEFINITION MODULE **********************************************/
//
// Author   :   Cyrille Lebossé, cyrille.lebosse@voila.fr
//              $Author: yuri $
//              Ecole Polytechnique Federale de Lausanne
//              IPR-LPM-VISION
//              CH-1015 Lausanne
//              Switzerland
// Filename :   $RCSfile: VisBinarisation.cpp,v $
// Version:     $Revision: 1.1.1.1 $
/*************************************************************************************************/

/** \file VisBinarisation.cpp
    
    
*/


// CVS/SCCS Tag
static char *id="@(#) $Id: VisBinarisation.cpp,v 1.1.1.1 2003/12/17 02:03:48 yuri Exp $";

/******************************** inclusions *****************************************************/
/*************************************************************************************************/
#include "VisBinarisation.hpp" 

typedef double (VisBinarisation::* Function) (double);

/*************************************************************************************************/
/************ private types, variables, classes and function prototypes **************************/
/*************************************************************************************************/

/*************************************************************************************************/
/************* public class methods and functions implementations ********************************/
/*************************************************************************************************/


//---------------------------------------------------------------------------------------

/** Default constructor */
 VisBinarisation :: VisBinarisation()
{ 
	source_image = NULL;
	dest_image = NULL;
	tmp_image = NULL;
	tmp_image_2 = NULL;
	current_threshold = 0;
	opt_threshold = 0;
	init_threshold = 0;
	minimum_threshold = 0;
	maximum_threshold = 0;
	opt_feature_value = 0;
	tol=0;
	blob_area = 0;
	morphology = 0;

}

/** Destructor */
VisBinarisation :: ~VisBinarisation()
{
	source_image = NULL;
	dest_image = NULL;
	current_threshold = 0;
	opt_threshold = 0;
	init_threshold = 0;
	opt_feature_value = 0;
}
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

/** Sets Optimimum value of blob feature, the type of feature, and blob area.
 *  
 *  Sets Optimimum value of blob feature, the type of feature, and blob area if the feature is different 
 *	from area in order to not confuse the reference blob with others.
 * 
 *
 * \param feature_value : value of the chosen feature.
 * \param index_feat : Character indicating which feature of the blob must be optimized.
 * \param given_blob_area : blob area if the feature is different 
 *		from area in order to not confuse the reference blob with others.
 * \result Returns -1 for a bad "index_feat", 0 otherwise.
 * 
 */
int VisBinarisation::SetOptFeature(double feature_value, double given_blob_area, char index_feat)
{

	int retvalue = -1;
	
	opt_feature_value = feature_value;
	blob_area = given_blob_area;

	//recover the init chosen blob contour pointer

	if( index_feat == 'A' || index_feat == 'a' || index_feat == 'C' || index_feat == 'c' || index_feat == 'O' || index_feat == 'o')
	{
		switch(index_feat)
		{
			case 'A' : //Area
					GetFeature = &VisBlobSet::GetArea;
					CalcFeature = &VisBlobSet::CalculateAreas;
					SortByFeature = &VisBlobSet::SortByArea;
					SelectByFeature = &VisBlobSet::SelectByArea;
					blob_area = opt_feature_value;
					break;
			case 'a' : //Area
					GetFeature = &VisBlobSet::GetArea;
					CalcFeature = &VisBlobSet::CalculateAreas;
					SortByFeature = &VisBlobSet::SortByArea;
					SelectByFeature = &VisBlobSet::SelectByArea;
					blob_area = opt_feature_value;
					break;
      
		
			case 'C' : //Compactness 
					GetFeature = &VisBlobSet::GetCompactness;
					CalcFeature = &VisBlobSet::CalculateCompactness;
					SortByFeature = &VisBlobSet::SortByCompactness;
					SelectByFeature = &VisBlobSet::SelectByCompactness;
					break;
			case 'c' : //Compactness 
					GetFeature = &VisBlobSet::GetCompactness;
					CalcFeature = &VisBlobSet::CalculateCompactness;
					SortByFeature = &VisBlobSet::SortByCompactness;
					SelectByFeature = &VisBlobSet::SelectByCompactness;
					break;


			case 'O' : //Orientation
					GetFeature = &VisBlobSet::GetOrientation;
					CalcFeature = &VisBlobSet::CalculateOrientations;
					SortByFeature = &VisBlobSet::SortByOrientation;
					SelectByFeature = &VisBlobSet::SelectByOrientation;
					break;
			case 'o' : //Orientation
					GetFeature = &VisBlobSet::GetOrientation;
					CalcFeature = &VisBlobSet::CalculateOrientations;
					SortByFeature = &VisBlobSet::SortByOrientation;
					SelectByFeature = &VisBlobSet::SelectByOrientation;
					break;

			case 'E' : //Eccentricity
					GetFeature = &VisBlobSet::GetEccentricity;
					CalcFeature = &VisBlobSet::CalculateEccentricities;
					SortByFeature = &VisBlobSet::SortByEccentricity;
					SelectByFeature = &VisBlobSet::SelectByEccentricity;
					break;
			case 'e' : //Eccentricity
					GetFeature = &VisBlobSet::GetEccentricity;
					CalcFeature = &VisBlobSet::CalculateEccentricities;
					SortByFeature = &VisBlobSet::SortByEccentricity;
					SelectByFeature = &VisBlobSet::SelectByEccentricity;
					break;

		}


		retvalue = 0;

	}
	else
	{
		retvalue = -1;
	}


    return retvalue;

}


//--------------------------------------------------------------------------------------

/** Optimized binarisation of a grey scale image
 * 
 * Detailed description of the method
 * 
 *
 * \param opt_threshold : Pointer on the optimized value of the threshold.
 * \param morpho : Flag indicating if morphology must be applied on the binarised image to clean it and have better results.
 *					If morpho=1, morphology is applied, if morpho=0, it is not applied.
 *					By default, morphology is applied.
 * \result Returns nothing.
 * 
 */
void VisBinarisation::OptBinarisation(int* opt_threshold, int morpho)
{
	
	double opt_thresh = 0;
	double opt_value = 0;

	morphology = morpho;


	GoldenSearch( &VisBinarisation::GetOptBlobFeature , minimum_threshold, init_threshold , maximum_threshold, tol , &opt_thresh, &opt_value, this);

	*opt_threshold = (int)opt_thresh;

	
}

//----------------------------------------------------------------------------------
/** Returns the optimized difference between current feature and opt_feature for the threshold value given.
 * 
 * 
 *
 * \param temp_threshold : Value of threshold for which the feature of the blob is calculated and compared with the optimized given value.
 * \result Returns absolute value of the difference between current feature value and optimized one..
 * 
 */

double VisBinarisation::GetOptBlobFeature(double temp_threshold)
{
	double retvalue = -1;

	double value=0;
	double min_area=0, max_area=0;

	current_threshold = (int)temp_threshold;

	SimpleBinarisation(current_threshold);

	if( morphology == 1 )
	{
		clean_morpho();
	}


//-----------------------------------------------------------------------
//cout << "\n\n current_threshold = " << current_threshold << " \n";
//-----------------------------------------------------------------------


	intern_blobs.FindBlobs(dest_image);

	intern_blobs.CalculateAreas();

	if( blob_area > 0 )
	{
		min_area = blob_area - 20*blob_area/100;
		max_area = blob_area + 20*blob_area/100;;

		intern_blobs.SelectByArea( max_area , min_area );
	}


	(intern_blobs.*CalcFeature)();

	//find the chosen blob in the new image 
	(intern_blobs.*SortByFeature)(opt_feature_value);

	value = (intern_blobs.*GetFeature)(0);

//------------------------------------------------
//	cout << " blob area = " << intern_blobs.GetArea(0) << " \n";
//	cout << " blob compactness = " << intern_blobs.GetCompactness(0) << " ";
//	cout << " \n numblobs =  " << intern_blobs.GetSelectedNumBlobs() << " \n";
//------------------------------------------------
//------------------------------------------------
//	cout << " \n value =  " << value << " \n";
//------------------------------------------------

//------------------------------------------------
//	cout << " \n opt_feature_value =  " << opt_feature_value << " \n";
//------------------------------------------------

	retvalue = fabs( opt_feature_value - value );

//------------------------------------------------
//	cout << " \n final retvalue =  " << retvalue << " \n\n";
//	getch();
//------------------------------------------------

	intern_blobs.EmptyDetectedBlobsList();

    return retvalue;
}

//--------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/** 1-line description of the method
 * 
 *  Applies fixed-level threshold to grayscale image. This is the basic operation
 *  to be performed before retrieving contours
 * 
 *
 * \param  threshold : Value of of the gray level limit between 0 and 255.
 * \result Returns -1 if threshold is not between 0 and 255, 0 if binarisation is possible.
 * 
 */
int VisBinarisation:: SimpleBinarisation(int threshold=0)
{
	int retvalue = -1;

	if( threshold > 0 && threshold < 256 )
	{

		current_threshold = threshold;

//-----------------------------------------------------------------------
//cout << "\n\n current_threshold = " << current_threshold << " \n";
//-----------------------------------------------------------------------


		cvThreshold (source_image, dest_image, current_threshold, 255, CV_THRESH_BINARY);
		retvalue = 0;

	}
	else
	{
		retvalue = -1;
	}

    return retvalue;
}

//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//********************************************************************************
//  clean_morpho
//********************************************************************************
/** \Performs advanced morphological operations
 *
 * Performs advanced morphological operations in order to clean the source image.
 * Uses Opencv methods (MorphologyEx) but you can use Ipl functions too (iplClose,iplOpen)
 * May one of these functions is faster, a test is required ! 
 *
 * \result Returns nothing
 */

void VisBinarisation:: clean_morpho(void)
{

	//structuring element
	IplConvKernel* str_elt;

	//----- Ipl functions
	//iplClose(imgS, imgS, 1);
	//iplOpen(imgS, imgS, 1);

	tmp_image = cvCreateImage(cvSize(source_image->width,source_image->height),IPL_DEPTH_8U,1);
	tmp_image->roi = source_image->roi;
	cvCopyImage (source_image, tmp_image);
	

	tmp_image_2 = cvCreateImage(cvSize(source_image->width,source_image->height),IPL_DEPTH_8U,1);
	tmp_image_2->roi = source_image->roi;
	cvCopyImage (source_image, tmp_image_2);
	


	//----- OpenCv functions
	str_elt = cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_RECT,NULL);
	cvMorphologyEx(dest_image,tmp_image_2,tmp_image,str_elt,CV_MOP_OPEN,1); // Open --> to remove small objects
	//cvMorphologyEx(imgS,imgD,imgtmp,str_elt,CV_MOP_CLOSE,1); // Close
	

	dest_image->roi = tmp_image_2->roi;
	cvCopyImage (tmp_image_2, dest_image );
	

	tmp_image->roi=NULL;
	tmp_image_2->roi=NULL;

	cvReleaseImage (&tmp_image);
	cvReleaseImage (&tmp_image_2);
}

//********************************************************************************
//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------

/** Minimize function of one variable using golden section search
 * 
 *	 Minimize function of one variable using golden section search
 *
 *   VisGoldenSearch(func, ax, bx, cx, tolerance, *xmin, *funcmin) computes a local minimum
 *   of func. xmin is the computed local minimizer of func and funcmin is
 *   func(xmin). xmin is computed to an relative accuracy of tolerance.
 *
 *   The parameters ax, bx and cx must satisfy the following conditions:
 *   ax < bx < cx, func(bx) < func(ax) and func(bx) < func(cx).
 *
 *   xmin satisfies ax < xmin < cx. golden is guaranteed to succeed if func
 *   is continuous between ax and cx
 *
 *
 * \param (*func) : Pointer to the function which has to be minimized.
 * \param ax : Minimum X-coordinate of segment search.
 * \param bx : Starting  point X-coordinate.
 * \param cx : Maximum X-coordinate of segment search.
 * \param tolerance : Accuracy of computation.
 * \param p_xmin : Pointer on the computed Func minimum.
 * \param p_funcmin : Pointer on the value of func for xmin.
 * \result Returns -1 if the poiner to tfunc is NULL, 0 if computation is possible .
 * 
 */
int VisBinarisation::GoldenSearch( Function func, double ax=0, double bx=80, double cx=255, double tolerance=0.01 ,double* p_xmin=NULL, double* p_funcmin=NULL, VisBinarisation* p_bin=NULL)
{
	int retvalue = -1;
	double C = 0;
	double R = 0;
	double x0 = 0 , x1 = 0 , x2 = 0 , x3 = 0; 
	double f1 =0 , f2 = 0;
	//double xmin=0 , fmin=0;
	int k=0;


	if( &func == NULL )
	{
		retvalue = -1;
	}

	else
	{	

		C = (3-sqrt(5.0))/2;
		R = 1-C;
 
		x0 = ax;
		x3 = cx;

		if ( fabs(cx-bx) > fabs(bx-ax) )
		{
			x1 = bx;
			x2 = bx + C*(cx-bx);
		}
		else
		{
			x2 = bx;
			x1 = bx - C*(bx-ax);
		}
	
		f1 = (p_bin->*func)(x1);

		f2 = (p_bin->*func)(x2);


		k = 1;
		while( fabs(x3-x0) > tolerance*( fabs(x1) + fabs(x2) ) )
		{	
		
			if (f2 < f1)
			{
				x0 = x1;
				x1 = x2;
				x2 = R*x1 + C*x3;   
				f1 = f2;
				f2 = (p_bin->*func)(x2);
			}
   
			else
			{
				x3 = x2;
				x2 = x1;
				x1 = R*x2 + C*x0;   
				f2 = f1;
				f1 = (p_bin->*func)(x1);
			}

			k = k+1;
		}
 
		if ( f1 < f2 )
		{
			*p_xmin = x1;
			*p_funcmin = f1;
		}
		else
		{
			*p_xmin = x2;
			*p_funcmin = f2;
		}

		retvalue = 0;
	}

//---------------------------------------------------------
//	cout << "\n xmin =  " << *p_xmin << " \n\n";
//---------------------------------------------------------
    return retvalue;
}

//--------------------------------------------------------------------------------

/*************************************************************************************************/
/************* private class methods and functions implementations *******************************/
/*************************************************************************************************/

/*************************************************************************************************/
/********************** test routines/program ****************************************************/
/*************************************************************************************************/

#ifdef BINARISATION_TEST
#include <iostream.h>

void tst0(void) {};
void tst1(void) {};
void tst2(void) {};
void tst3(void) {};
void tst4(void) {};
void tst5(void) {};
void tst6(void) {};
void tst7(void) {};
void tst8(void) {};
void tst9(void) {};


int main(void)
  {
  char ans;
  do
    {
    cout << "0-9 tst0->tst9" << endl;
    cout << "q quit" << endl;
    cin >> ans;
    switch(ans)
      {
      case '0' : tst0(); break;
      case '1' : tst1(); break;
      case '2' : tst2(); break;
      case '3' : tst3(); break;
      case '4' : tst4(); break;
      case '5' : tst5(); break;
      case '6' : tst6(); break;
      case '7' : tst7(); break;
      case '8' : tst8(); break;
      case '9' : tst9(); break;
      }
    }
  while (ans!='q');
  return 0;
  }
  
#endif  // test routines

/************************** End of File **********************************************************/







