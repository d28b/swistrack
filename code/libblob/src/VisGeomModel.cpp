//******************************* DEFINITION MODULE **********************************************/
//
// Author   :   Cyrille Lebossé, cyrille.lebosse@voila.fr
//              $Author: yuri $
//              Ecole Polytechnique Federale de Lausanne
//              DMT-IPM-VISION
//              CH-1015 Lausanne
//              Switzerland
// Filename :   $RCSfile: VisGeomModel.cpp,v $
// Version:     $Revision 1.0 $
/*************************************************************************************************/

// CVS/SCCS Tag
static char *id="@(#) $Id: VisGeomModel.cpp,v 1.1.1.1 2003/12/17 02:03:57 yuri Exp $";


/*! \file VisGeomModel.cpp
 *  \brief 
 *
 *
 */

/******************************** inclusions *****************************************************/
/*************************************************************************************************/
#include "VisGeomModel.hpp" 



#include <iostream>
#include <stdio.h>

/*************************************************************************************************/
/************ private types, variables, classes and function prototypes **************************/
/*************************************************************************************************/

/*************************************************************************************************/
/************* public class methods and functions implementations ********************************/
/*************************************************************************************************/

//---------------------------------------------------------------------------------------
/** Constructor */	
VisGeomModel :: VisGeomModel()
{ 
	learnt_target.x=0;
	learnt_target.y=0;

	calc_target.x=0;
	calc_target.y=0;
}	

//--------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/** \brief Destructor 
 *
 *         Used to clear the lists and to free memory.
 */
VisGeomModel :: ~VisGeomModel()
{ 
	sinus_seq.clear();
	cosinus_seq.clear();
	learnt_points_seq.clear();
	found_points_seq.clear();
}	


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/** \brief Adds the given point to the learnt template points list. 
 *
 *	This function has to be called before "CreateModel()" in order to 
 *	store the list of points.
 *         
 */
void VisGeomModel :: SetLearntPointsList(CvPoint2D32f point)
{ 
	learnt_points_seq.push_back(point);
}	

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/** \brief Adds the given point to the found template points list. 
 *
 *	This function has to be called before "CreateModel()" in order to 
 *	store the list of points.
 *         
 */
void VisGeomModel :: SetFoundPointsList(CvPoint2D32f point)
{ 
	found_points_seq.push_back(point);
}	


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/** \brief Creates the template and computes the model from the learnt points list. 
 *
 *	This function has to be called after "SetLearntPointsList()". 
 *         
 */
int VisGeomModel :: CreateModel(double* p_cond_number)
{ 
	int i=0;
	int num_centers =  learnt_points_seq.size()-1;
	vector<CvPoint2D32f> centers_tab;
	CvPoint2D32f temp_point;
	CvPoint2D32f vect1;
	CvPoint2D32f vect2;
	CvMat* B = cvCreateMat(num_centers,1, CV_32FC1);
	CvMat* A = cvCreateMat(num_centers,2, CV_32FC1);
	CvMat* EV_A = cvCreateMat(num_centers,num_centers, CV_32FC1);
	float cos_ab , seg_cos;
	float sin_ab , seg_sin;
	int step;
	CvSize size;
	float* p_mat;
	vector<double> eigen_values;
	double condition_number;


	//initialisation
	cvGetRawData(EV_A, (uchar**) &p_mat,&step,&size);

	for(i=0; i<(num_centers*num_centers) ;i++)
	{
		 p_mat[i] =0;
	}
	*p_cond_number=0;


	// calculates each segment center
	for( i=0; i<num_centers ; i++ )
	{
		temp_point.x = (learnt_points_seq[i].x + learnt_points_seq[i+1].x)/2;
		temp_point.y = (learnt_points_seq[i].y + learnt_points_seq[i+1].y)/2;
		
		centers_tab.push_back(temp_point);
	}


	// calculates sinus and cosinus of the half-lines going from these centres 
	//	to the target point 

	for( i=0; i<num_centers ; i++ )
	{
		vect1.x = learnt_points_seq[i+1].x - learnt_points_seq[i].x;
		vect1.y = learnt_points_seq[i+1].y - learnt_points_seq[i].y;
		
		vect2.x = learnt_target.x - centers_tab[i].x;
		vect2.y = learnt_target.y - centers_tab[i].y;

		cosinus_seq.push_back( (vect1.x*vect2.x + vect1.y*vect2.y)/(sqrt(vect1.x*vect1.x)*sqrt(vect2.x*vect2.x)) );		
		sinus_seq.push_back( (vect1.x*vect2.y-vect1.y*vect2.x)/(sqrt(vect1.x*vect1.x)*sqrt(vect2.x*vect2.x)) );
	
	}


	// Uses sinus and cosinus of the half-lines going from these centres 
	//	to the target point in order to calculate A.
	for( i=0; i<num_centers ; i++ )
	{
		vect1.x = learnt_points_seq[i+1].x - learnt_points_seq[i].x;
		vect1.y = learnt_points_seq[i+1].y - learnt_points_seq[i].y;
	
		seg_cos = vect1.x/sqrt(vect1.x*vect1.x+vect1.y*vect1.y);
		seg_sin = vect1.y/sqrt(vect1.x*vect1.x+vect1.y*vect1.y);

		cos_ab = (float)(seg_cos*cosinus_seq[i]-seg_sin*sinus_seq[i]); 
		sin_ab = (float)(sinus_seq[i]*seg_cos+seg_sin*cosinus_seq[i]); 

		cvGetRawData(A, (uchar**) &p_mat,&step,&size);
		p_mat[2*i] = -sin_ab; 
		p_mat[2*i + 1] = cos_ab;

		cvGetRawData(B, (uchar**) &p_mat,&step,&size);
		p_mat[i] = centers_tab[i].y*cos_ab - centers_tab[i].x*sin_ab;

	}


//------
// condition number
	cvSVD( A, EV_A );
	cvGetRawData(EV_A, (uchar**) &p_mat,&step,&size);

	for(i=0; i<(num_centers*num_centers) ;i++)
	{		
		if( p_mat[i] != 0 )
		{
			eigen_values.push_back(p_mat[i]);
		}
	}

	std::sort(eigen_values.begin(),eigen_values.end());
/*
	for(i=0 ; i<eigen_values.size() ; i++)
	{
		cout << " Eigen values de A : " << eigen_values[i] << " \n";
	}
*/
	condition_number = (double) eigen_values.back()/eigen_values.front();
	//cout << "\n Condition number of A : " << condition_number << " \n\n" ;
	*p_cond_number = condition_number;
//-----


	centers_tab.clear();

	cvReleaseMat( &EV_A);
	cvReleaseMat( &A);
	cvReleaseMat( &B);

	return 0;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/** \brief Calculate the target point from the found points list and the sinus and cosinus lists. 
 *
 *	This function has to be called after "SetFoundPointsList()". 
 *
 *	\param p_cond_number : Pointer on the double where the number of condition of the matrix used in the calculation is stored.         
 *	\param p_error : Pointer on the double where the least square error is stored.
 *
 *	\result : Returns the calculated target point.
 */
CvPoint2D32f VisGeomModel :: CalculateTarget(double* p_cond_number, double* p_error )
{ 
	int i=0;
	int num_centers =  found_points_seq.size()-1;
	vector<CvPoint2D32f> centers_tab;
	CvPoint2D32f vect1;
	CvPoint2D32f temp_point;
	CvMat* Target = cvCreateMat(2,1, CV_32FC1);
	CvMat* Temp = cvCreateMat(num_centers,1, CV_32FC1);
	CvMat* Err = cvCreateMat(num_centers,1, CV_32FC1);
	CvMat* Error = cvCreateMat(1,1, CV_32FC1);
	CvMat* B = cvCreateMat(num_centers,1, CV_32FC1);
	CvMat* A = cvCreateMat(num_centers,2, CV_32FC1);
	CvMat* EV_A = cvCreateMat(num_centers,num_centers, CV_32FC1);
	CvMat* Pseudo_Inv_A = cvCreateMat(2, num_centers, CV_32FC1);
	float cos_ab , seg_cos;
	float sin_ab , seg_sin;
	int step;
	CvSize size;
	float* p_mat;
	//int step_2;
	//CvSize size_2;
	//float* p_mat_2;
	vector<double> eigen_values;
	double condition_number;

	//initialisation
	cvGetRawData(EV_A, (uchar**) &p_mat,&step,&size);

	for(i=0; i<(num_centers*num_centers) ;i++)
	{
		 p_mat[i] =0;
	}
	*p_cond_number=0;
	*p_error=0;

	// calculates each segment center
	for( i=0; i<num_centers ; i++ )
	{
		temp_point.x = (found_points_seq[i].x + found_points_seq[i+1].x)/2;
		temp_point.y = (found_points_seq[i].y + found_points_seq[i+1].y)/2;
		
		centers_tab.push_back(temp_point);
	}


	// Uses sinus and cosinus of the half-lines going from these centres 
	//	to the target point.
	for( i=0; i<num_centers ; i++ )
	{
		vect1.x = found_points_seq[i+1].x - found_points_seq[i].x;
		vect1.y = found_points_seq[i+1].y - found_points_seq[i].y;

		seg_cos = vect1.x/sqrt(vect1.x*vect1.x+vect1.y*vect1.y);
		seg_sin = vect1.y/sqrt(vect1.x*vect1.x+vect1.y*vect1.y);

		cos_ab = (float)(seg_cos*cosinus_seq[i]-seg_sin*sinus_seq[i]); 
		sin_ab = (float)(sinus_seq[i]*seg_cos+seg_sin*cosinus_seq[i]); 

		cvGetRawData(A, (uchar**) &p_mat,&step,&size);
		p_mat[2*i] = -sin_ab; 
		p_mat[2*i + 1] = cos_ab;

		cvGetRawData(B, (uchar**) &p_mat,&step,&size);
		p_mat[i] = centers_tab[i].y*cos_ab - centers_tab[i].x*sin_ab;

	}

	cvPseudoInv ( A, Pseudo_Inv_A );

	cvMatMulAdd ( Pseudo_Inv_A , B, 0 , Target);

	cvGetRawData(Target, (uchar**) &p_mat,&step,&size);
	calc_target.x = p_mat[0];
	calc_target.y = p_mat[1];
//------

	cvMatMulAdd ( A , Target, 0 , Temp);
	cvSub(B,Temp,Err);
	cvMulTransposed( Err, Error , 1 );

	cvGetRawData(Error, (uchar**) &p_mat,&step,&size);

	*p_error = sqrt(p_mat[0]/num_centers);

/*
	cvGetRawData(B, (uchar**) &p_mat,&step,&size);
	cvGetRawData(Temp, (uchar**) &p_mat_2,&step_2,&size_2);
	for(i=0 ; i<num_centers ; i++)
	{
		*p_error += (p_mat[i] - p_mat_2[i])*(p_mat[i] - p_mat_2[i]);
	}
	*p_error = sqrt(*p_error/num_centers);
*/

//------
// condition number
	cvSVD( A, EV_A );
	cvGetRawData(EV_A, (uchar**) &p_mat,&step,&size);

	for(i=0; i<(num_centers*num_centers) ;i++)
	{		
		if( p_mat[i] != 0 )
		{
			eigen_values.push_back(p_mat[i]);
		}
	}

	std::sort(eigen_values.begin(),eigen_values.end());
/*
	for(i=0 ; i<eigen_values.size() ; i++)
	{
		cout << " Eigen values de A : " << eigen_values[i] << " \n";
	}
*/
	condition_number = (double) eigen_values.back()/eigen_values.front();
	//cout << "\n Condition number of A : " << condition_number << " \n\n" ;
	*p_cond_number = condition_number;
//-----
	cvReleaseMat( &Target);
	cvReleaseMat( &Temp);
	cvReleaseMat( &Err);
	cvReleaseMat( &Error);
	cvReleaseMat( &EV_A);
	cvReleaseMat( &A);
	cvReleaseMat( &B);
	cvReleaseMat( &Pseudo_Inv_A);
	return calc_target;
}	
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/*************************************************************************************************/
/************* private class methods and functions implementations *******************************/
/*************************************************************************************************/

/*************************************************************************************************/
/********************** test routines/program ****************************************************/
/*************************************************************************************************/

#ifdef VISGEOMMODEL_TEST

#include <iostream>
#include <fstream>
#include "ipllpm.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/timeb.h>
//#include "Line.hpp"
#include "simple_im_proc.h"
#include <conio.h>
#include <cv.h>
#include <time.h>

using namespace std; //So that we can use standard library names



void tst0(void) 
{

};

void tst1(void) 
{


};

void tst2(void) 
{

};


void tst3(void) 
{



};


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
	cout << "\n\n 0: VisGeomModel functions test \n" << endl ;

    cout << "0-9 tst0->tst9" << endl;
    cout << "q quit" << endl << endl;
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



