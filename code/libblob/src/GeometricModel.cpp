//******************************* DEFINITION MODULE **********************************************/
//
// Author   :   Yuri Lopez de Meneses, yuri@ieee.org
//              $Author: yuri $
//              Ecole Polytechnique Federale de Lausanne
//              DMT-IPM-VISION
//              CH-1015 Lausanne
//              Switzerland
// Filename :   $RCSfile: GeometricModel.cpp,v $
// Version:     $Revision: 1.4 $
/*************************************************************************************************/

// CVS/SCCS Tag
static char *id="@(#) $Id: GeometricModel.cpp,v 1.4 2003/12/18 20:08:02 yuri Exp $";

/******************************** inclusions *****************************************************/
/*************************************************************************************************/
#include "GeometricModel.hpp" 


/*************************************************************************************************/
/************ private types, variables, classes and function prototypes **************************/
/*************************************************************************************************/

/*************************************************************************************************/
/************* public class methods and functions implementations ********************************/
/*************************************************************************************************/

GeometricModel::GeometricModel()
{
  target_set = false;
  model_learnt = false;
}


GeometricModel::~GeometricModel()
{
 reference_points.empty();
 sinus_seq.empty();
 cosinus_seq.empty();
}


/** \brief Adds a reference point to the list
 *
 * Detailed description of the function
 *
 * \param point: A reference point
 * \result Returns the number or points
 */
int GeometricModel::AddReferencePoint(CvPoint2D32f point)
{
  reference_points.push_back(point);
  return (int) reference_points.size();
}


/** Adds a series of reference points */
int GeometricModel::AddListReferencePoints(CvPoint2D32f* point_list,int n)
{
  for (int i=0;i<n;i++)
	reference_points.push_back(point_list[i]);
  return (int) reference_points.size();
}



/** Create a model from the reference points to the target 

 * \result Returns the condition number (>0) or else -1 in case of error.
 */
double GeometricModel::CreateModel()
{
  if ( reference_points.empty() || !target_set)
	  return -1.0; // Error

	int i=0;
	int num_centers =  reference_points.size()-1;
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


	// calculates each segment center
	for( i=0; i<num_centers ; i++ )
	{
		temp_point.x = (reference_points[i].x + reference_points[i+1].x)/2;
		temp_point.y = (reference_points[i].y + reference_points[i+1].y)/2;
		
		centers_tab.push_back(temp_point);
	}


	// calculates sinus and cosinus of the half-lines going from these centres 
	//	to the target point 

	for( i=0; i<num_centers ; i++ )
	{
		vect1.x = reference_points[i+1].x - reference_points[i].x;
		vect1.y = reference_points[i+1].y - reference_points[i].y;
		
		vect2.x = target_point.x - centers_tab[i].x;
		vect2.y = target_point.y - centers_tab[i].y;

		cosinus_seq.push_back( (vect1.x*vect2.x + vect1.y*vect2.y)/(sqrt(vect1.x*vect1.x)*sqrt(vect2.x*vect2.x)) );		
		sinus_seq.push_back( (vect1.x*vect2.y-vect1.y*vect2.x)/(sqrt(vect1.x*vect1.x)*sqrt(vect2.x*vect2.x)) );
	
	}


	// Uses sinus and cosinus of the half-lines going from these centres 
	//	to the target point in order to calculate A.
	for( i=0; i<num_centers ; i++ )
	{
		vect1.x = reference_points[i+1].x - reference_points[i].x;
		vect1.y = reference_points[i+1].y - reference_points[i].y;
	
		seg_cos = vect1.x/sqrt(vect1.x*vect1.x+vect1.y*vect1.y);
		seg_sin = vect1.y/sqrt(vect1.x*vect1.x+vect1.y*vect1.y);

		cos_ab = seg_cos*cosinus_seq[i]-seg_sin*sinus_seq[i]; 
		sin_ab = sinus_seq[i]*seg_cos+seg_sin*cosinus_seq[i]; 

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

	model_learnt = true;
//-----


	centers_tab.clear();

	cvReleaseMat( &EV_A);
	cvReleaseMat( &A);
	cvReleaseMat( &B);

	return condition_number;

}

/** Sets the target point */
void GeometricModel::SetTargetPoint(CvPoint2D32f point)
{
  target_point = point;
  target_set = true;
}



int GeometricModel::ClearReferencePoints()
{
  reference_points.clear();
  return -reference_points.size();
}


/** \brief Calculate the target point from the found points list and the sinus and cosinus lists. 
 *
 *	This function has to be called after ...
 *
 *	\param p_cond_number : Pointer on the double where the number of condition of the matrix used in the calculation is stored.         
 *	\param p_error : Pointer on the double where the least square error is stored.
 *
 *	\result : Returns the calculated target point.
 */
CvPoint2D32f GeometricModel::CalculateTarget(double* p_cond_number, double* p_error)
{
	int i=0;
	int num_centers =  reference_points.size()-1;
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
		temp_point.x = (reference_points[i].x + reference_points[i+1].x)/2;
		temp_point.y = (reference_points[i].y + reference_points[i+1].y)/2;
		
		centers_tab.push_back(temp_point);
	}


	// Uses sinus and cosinus of the half-lines going from these centres 
	//	to the target point.
	for( i=0; i<num_centers ; i++ )
	{
		vect1.x = reference_points[i+1].x - reference_points[i].x;
		vect1.y = reference_points[i+1].y - reference_points[i].y;

		seg_cos = vect1.x/sqrt(vect1.x*vect1.x+vect1.y*vect1.y);
		seg_sin = vect1.y/sqrt(vect1.x*vect1.x+vect1.y*vect1.y);

		cos_ab = seg_cos*cosinus_seq[i]-seg_sin*sinus_seq[i]; 
		sin_ab = sinus_seq[i]*seg_cos+seg_sin*cosinus_seq[i]; 

		cvGetRawData(A, (uchar**) &p_mat,&step,&size);
		p_mat[2*i] = -sin_ab; 
		p_mat[2*i + 1] = cos_ab;

		cvGetRawData(B, (uchar**) &p_mat,&step,&size);
		p_mat[i] = centers_tab[i].y*cos_ab - centers_tab[i].x*sin_ab;

	}

	cvPseudoInv ( A, Pseudo_Inv_A );

	cvMatMulAdd ( Pseudo_Inv_A , B, 0 , Target);

	cvGetRawData(Target, (uchar**) &p_mat,&step,&size);
	target_point.x = p_mat[0];
	target_point.y = p_mat[1];
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
	target_set = true;
	return target_point;

}


int GeometricModel::SaveModel(char* fname)
{
  unsigned int i;
  if (model_learnt)
  {
    ofstream of(fname);
    of << sinus_seq.size()<< endl;
	for (i=0;i<sinus_seq.size();i++)
	{
     of << sinus_seq[i] << endl;
	}
    of << cosinus_seq.size() << endl;
	for (i=0;i<cosinus_seq.size();i++)
	{
     of << cosinus_seq[i] << endl;
	}
	of.close();
	return 0;
  }
  else return -1;
}


int GeometricModel::LoadModel(char* fname)
{
  int i;
  ifstream ifs(fname);
  if (ifs.fail()) return -1;

  if (model_learnt)
  {
    sinus_seq.clear();
	cosinus_seq.clear();
  }
  

	int n;
	double val;
    ifs >> n;
	for (i=0;i<n;i++)
	{
     ifs >> val;
	 sinus_seq.push_back(val);
	}
    ifs >> n;
	for (i=0;i<n;i++)
	{
		ifs >> val;
		cosinus_seq.push_back(val);
	}
	ifs.close();
	model_learnt = true;
	return sinus_seq.size()+1;
 
}


/** Returns the number of reference points */
int GeometricModel::GetNumReferencePoints()
{
  return reference_points.size();
}


/** Returns the number of model points */
int GeometricModel::GetNumModelPoints()
{
  if (model_learnt)
  return 1+sinus_seq.size();
  else return 0;
}



/** Draws the reference points and model */
int GeometricModel::DrawModel(IplImage* img)
{
  double color = 200.0;
  double color1 = 255.0;
  double cond,error;
  CvPoint mid;
  CvPoint p1,p2;
  unsigned int i;

  // Draw a circle aroun each point
  if (!reference_points.empty())
  { 
	 p1 = cvPoint((int)(reference_points[0].x),(int)(reference_points[0].y));
     cvCircle(img, p1, 5, CV_RGB(color,color,color), 1 );
  }
  for (i=1;i<reference_points.size();i++)
  {
	p2 = p1;
	p1 = cvPoint((int)(reference_points[i].x),(int)(reference_points[i].y));
    cvCircle(img, p1, 5, CV_RGB(color,color,color), 1 );
	cvLine( img, p2, p1, CV_RGB(color,color,color), 1, 8 );
  }
  if (model_learnt)
  {
    if (!target_set) target_point = CalculateTarget(&cond, &error);
    for (i=1;i<reference_points.size();i++)
	{
      p1 = cvPoint((int)(reference_points[i].x),(int)(reference_points[i].y));
      cvCircle(img, p1, 5, CV_RGB(color,color,color), 1 );
	  mid.x = (int)((reference_points[i-1].x+reference_points[i].x)/2);
	  mid.y = (int)((reference_points[i-1].y+reference_points[i].y)/2);
      p1 = cvPoint((int)(target_point.x),(int)(target_point.y));
	  cvLine( img, mid, p1, CV_RGB(color1,color1,color1), 1, 8 );
	}
	// Draws a cross hair at the target
	DrawCross(img, p1, 5 , (int)color1);
	cvCircle(img, p1, 5, CV_RGB(color1,color1,color1), 1 );
  }
  return 0;
}

/*************************************************************************************************/
/************* private class methods and functions implementations *******************************/
/*************************************************************************************************/

/** displays a cross 
 *
 * This function enables you to display a cross whose center and size have to be
 * specified as parameters.
 *
 * \param CvArr* array : pointer on the source image 
 * \param CvPoint pt : cross center
 * \param int branch : cross length 
 *
 * \result Returns nothing
 */
void GeometricModel::DrawCross(CvArr* array, CvPoint pt, int branch , int color)
{

	CvPoint pt1 = cvPoint(0,0);
	CvPoint pt2 = cvPoint(0,0);
	
	pt1.x = pt.x - branch;
	pt1.y = pt.y;

	pt2.x = pt.x + branch;
	pt2.y = pt.y;

	cvLine (array, pt1, pt2, CV_RGB(color,color,color), 1, 8);

	pt1.x = pt.x;
	pt1.y = pt.y - branch;

	pt2.x = pt.x;
	pt2.y = pt.y + branch;

	cvLine (array, pt1, pt2,CV_RGB(color,color,color), 1, 8);
}



/*************************************************************************************************/
/********************** test routines/program ****************************************************/
/*************************************************************************************************/

#ifdef GEOMETRIC_MODEL_TEST
#include <iostream>
#include <highgui.h>

void tst0(void) {
 GeometricModel mod1;
 CvPoint2D32f p = cvPoint2D32f(0.0,1.0);
 mod1.AddReferencePoint(p);
 p = cvPoint2D32f(1.0,0.0);
 mod1.AddReferencePoint(p);
 p = cvPoint2D32f(0.0,0.0);
 cout << "There are " << mod1.AddReferencePoint(p) << "reference points" << endl;
 CvPoint2D32f t = cvPoint2D32f(1.0,1.0);
 mod1.SetTargetPoint(t);
 mod1.CreateModel();
 mod1.SaveModel("mod1.mod");

 GeometricModel mod2;
 mod2.LoadModel("mod1.mod");
 mod2.SaveModel("mod2.mod");

 p = cvPoint2D32f(0.0,1.0);
 mod2.AddReferencePoint(p);
 p = cvPoint2D32f(1.0,0.0);
 mod2.AddReferencePoint(p);
 p = cvPoint2D32f(0.0,0.0);
 cout << "There are " << mod2.AddReferencePoint(p) << "reference points" << endl;
 double c,e;
 p = mod2.CalculateTarget(&c,&e);
 cout << "Target: (" << p.x << ", " << p.y << ") ";
 cout << "condition: " << c << " error: " << e << endl;

};

void tst1(void) {
 GeometricModel mod1;
 CvPoint2D32f p = cvPoint2D32f(0.0,1.0);
 mod1.AddReferencePoint(p);
 p = cvPoint2D32f(1.0,0.0);
 mod1.AddReferencePoint(p);
 p = cvPoint2D32f(0.0,0.0);
 cout << "There are " << mod1.AddReferencePoint(p) << "reference points" << endl;
 CvPoint2D32f t = cvPoint2D32f(1.0,1.0);
 mod1.SetTargetPoint(t);
 mod1.CreateModel();
 mod1.SaveModel("mod1.mod");

 GeometricModel mod2;

 cvNamedWindow( "image", CV_WINDOW_AUTOSIZE );
 IplImage* img = cvCreateImage(cvSize(256,256),IPL_DEPTH_8U,1);
 mod2.DrawModel(img);
 cvShowImage( "image", img );
 cout << " There should be an empty image " << endl;
 cvWaitKey(0);

 p = cvPoint2D32f(10.0,100.0);
 mod2.AddReferencePoint(p);
 p = cvPoint2D32f(100.0,10.0);
 mod2.AddReferencePoint(p);
 p = cvPoint2D32f(10.0,10.0);
 mod2.AddReferencePoint(p);

 mod2.DrawModel(img);
 cvShowImage( "image", img );
 cout << " There should be " << mod2.GetNumReferencePoints() << " points connected" << endl;
 cvWaitKey(0);

 mod2.LoadModel("mod1.mod");
 mod2.DrawModel(img);
 cvShowImage( "image", img );
 cout << "Target point should also appear" << endl; 
 cvWaitKey(0);

};
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







