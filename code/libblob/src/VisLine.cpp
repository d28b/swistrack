//******************************* DEFINITION MODULE **********************************************/
//
// Author   :   Cyrille Lebossé, cyrille.lebosse@voila.fr
//              $Author: yuri $
//              Ecole Polytechnique Federale de Lausanne
//              DMT-IPM-VISION
//              CH-1015 Lausanne
//              Switzerland
// Filename :   $RCSfile : Line.cpp ,v $
// Version:     $Revision 1.0 $
/*************************************************************************************************/

// CVS/SCCS Tag
static char *id="@(#) $Id: VisLine.cpp,v 1.4 2004/07/12 12:01:48 yuri Exp $";


/** \file Line.cpp
 *  \brief Definitions of the Line class methods.
 *  
 *	These methods enable to calculate the intersection of 2 lines as well as
 *	the angle between them or the distance if there are parallel.
 *
 *  This file contains the definition of Line class methods.
 */


/******************************** inclusions *****************************************************/
/*************************************************************************************************/
 
#include "VisLine.hpp"


#include <iostream>
//#include <cstdio>
using namespace std;

/*************************************************************************************************/
/************ private types, variables, classes and function prototypes **************************/
/*************************************************************************************************/


/*************************************************************************************************/
/************* public class methods and functions implementations ********************************/
/*************************************************************************************************/

/** \brief Default Constructor 
 * Initialization of blob features.  
 */
VisLine :: VisLine(void)
{
	a=0;
	b=0;
	c=0;
}


/** \brief Constructor
 * Constructs the line passing through two points
 *
 * \param p1: Point P1
 * \param p2: Point P2
 */
VisLine::VisLine(CvPoint p1, CvPoint p2)
{
  a = -(p2.y-p1.y);
  b = p2.x - p1.x;
  c = -a*p1.x - b*p1.y;
}


/** \brief Constructor
 * Constructs the line passing through two points
 *
 * \param p1: Point P1
 * \param p2: Point P2
 */
VisLine::VisLine(CvPoint2D32f p1, CvPoint2D32f p2)
{
  a = -(p2.y-p1.y);
  b = p2.x - p1.x;
  c = -a*p1.x - b*p1.y;
}


/** \brief Constructor
 * Constructs the line passing through a point with a certain orientation
 *
 * \param p1: Point P1
 * \param angle: Orientation with respect to x-axis
 */
VisLine::VisLine(CvPoint p1, double angle)
{
  a = -sin(angle);
  b = cos(angle);
  c = -a*p1.x - b*p1.y;  
}


/** Destructor */
VisLine :: ~VisLine()
{

}


//-------------------------------------------------------------------------------------
/** \brief Calculates angle (in deg) between 2 lines.
 *
 * This method returns the angle between 2 lines or -1000 if lines are not defined.
 * The angle lies between 0 and 90 degrees. If the lines are parallel
 * the angle equals 0.
 *
 *	\param line2 : second line.
 *	\return angle (in deg) or -1000 if lines are not defined.
 */
double VisLine::GetAngle(VisLine line2)
{
	if( (a==0 && b==0) || (line2.a==0 && line2.b==0) )
	{
		return -1000;
	}
	else
	{
		return( acos((a*line2.a + b*line2.b)/sqrt((a*a+b*b)*(line2.a*line2.a+line2.b*line2.b)))*180/PI  );
	}
}
//--------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
/** \brief Returns intersection point of 2 lines.
 *
 * This method returns intersection point of 2 lines or a point whose coordinates are -11111 if lines are not defined.
 * If lines are parallel, it returns a point whose coordinates are -55555.
 *
 *	\param line2 : Second line.
 *	\return Intersection point.
 */
CvPoint2D32f VisLine::GetIntersection(VisLine line2)
{
	if( (a==0 && b==0) || (line2.a==0 && line2.b==0) )
	{
		return( cvPoint2D32f( -11111 , -11111 ) );
	}
	else
	{
		float* p;
		int step;
		CvSize size;
		CvMat* M   = cvCreateMat(2,2, CV_32FC1);
		CvMat* C   = cvCreateMat(2,1, CV_32FC1);
		CvMat* X   = cvCreateMat(2,1, CV_32FC1);
		
		cvGetRawData(M, (uchar**) &p,&step,&size);//p pointe sur M
		p[0]=(float)a;       p[1]=(float)b;
		p[2]=(float)line2.a; p[3]=(float)line2.b;
		

		cvGetRawData(C, (uchar**) &p,&step,&size);//p pointe sur C
		p[0]= (float)-c;       p[1]=(float) -line2.c;
		
       
		cvSolve( M, C,  X, CV_LU );

		cvGetRawData(X, (uchar**) &p,&step,&size);//p pointe sur X

		if( p[0]==0  && p[1]==0)
		{
			double angl = GetAngle(line2);
			if( angl==0 ) return cvPoint2D32f( -55555 , -55555 );
			else return(cvPoint2D32f( p[0] , p[1] ));
		}
		else
		{
	   		return(cvPoint2D32f( p[0] , p[1] ));
		}
	}
}
//--------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
/** \brief Returns distance between 2 lines if they are parallel.
 *
 *  This method returns distance between to 2 lines if lines are parallel or
 *	-1 if lines are not defined.
 *
 *	\param line2 : Second line.
 *	\return Distance.
 */
double VisLine::GetDistance(VisLine line2)
{
	double Mx=0.0, My=0.0;


	if( (a==0 && b==0) || (line2.a==0 && line2.b==0) )
	{
		return(-1);
	}
	else
	{

		if( a == 0 )
		{
			Mx = 1;
			My = -(double)c/b;
		}
		else
		{
			if( b == 0 )
			{
				Mx = -(double)c/a;
				My = 1;
			}
			else
			{

				Mx = 1;
				My = -(double)(a+c)/b;
			}
		}

	VisLine line3(b,-a, (a*My-b*Mx));
	CvPoint2D32f N;

	N = line3.GetIntersection(line2);

	return( sqrt( (N.x-Mx)*(N.x-Mx) + (N.y-My)*(N.y-My) ) );


	}
}
//--------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
/** \brief Overload of the '=' operator 
 *
 *
 *	\param line2 : Second line.
 *	\return line.
 */

VisLine &VisLine :: operator=(const VisLine &line2)
{
	a = line2.a;
	b = line2.b;
    c = line2.c;
    
   return *this;
}


//------------------------------------------------------------------------------


int VisLine::Draw(IplImage* img,double color, int thickness)
{
  // Determine intersection with image borders (clipping)
  CvPoint2D32f p1 = GetIntersection(VisLine(1,0,0));  // intersection with left-most image border
  cout << p1.x << " " << p1.y << endl;
  CvPoint2D32f p2 = GetIntersection(VisLine(0,1,0));  // intersection with top-most image border
  cout << p2.x << " " << p2.y << endl;
  CvPoint2D32f p3 = GetIntersection(VisLine(1,0,-(img->width-1)));  // intersection with right-most image border
  cout << p3.x << " " << p3.y << endl;
  CvPoint2D32f p4 = GetIntersection(VisLine(0,1,-(img->height-1)));  // intersection with bottom-most image border
  cout << p4.x << " " << p4.y << endl;

  CvPoint pa = cvPoint(img->width,img->height);
  CvPoint pb = cvPoint(0,0);
  int  found = 0;
  // Determine which border is intersected, if ever
  if ( (p1.y >= 0 ) && (p1.y < img->height)   )
  {
	  if (p1.x < pa.x )  pa = cvPoint((int)p1.x,(int)p1.y);
      found = 1;
  }
  if ( (p2.x >= 0 ) && (p2.x < img->width)  )
  {
	  if (p2.x < pa.x ) pa = cvPoint((int)p2.x,(int)p2.y);
      if (p2.x > pb.x ) pb = cvPoint((int)p2.x,(int)p2.y);
      found = 1;
  }
  if ( (p3.y >= 0 ) && (p3.y < img->height) )
  {
	  if (p3.x > pb.x ) pb = cvPoint((int)p3.x,(int)p3.y);
	  found = 1 ;
  }
  if ( (p4.x >= 0 ) && (p4.x < img->width) )
  { 
	  if (p4.x < pa.x ) pa = cvPoint((int)p4.x,(int)p4.y);
	  if (p4.x > pb.x ) pb = cvPoint((int)p4.x,(int)p4.y);
	  found = 1;
  }
  cout << found << endl;
  cout << pa.x << " " << pa.y << endl;
  cout << pb.x << " " << pb.y << endl;
  if (found) cvLine(img,pa,pb,CV_RGB(color,color,color),thickness,8);
  
  return found;
}

/*************************************************************************************************/
/********************** test routines/program ****************************************************/
/*************************************************************************************************/

#ifdef LINE_TEST
#include <iostream.h>
#include <stdio.h>

#include "highgui.h"


void tst0(void) 
{
  VisLine v1(0,1,-10);
  VisLine v2(1,2,10);

  cout << " v1 ^ v2 =" << v1.GetAngle(v2) << endl; 
  cout << " v2 ^ v1 =" << v2.GetAngle(v1) << endl; 

};

void tst1(void) 
{
  IplImage* img = cvCreateImage(cvSize(400,400),IPL_DEPTH_8U,1);

  cvNamedWindow("test",CV_WINDOW_AUTOSIZE);
  CvPoint p1 = cvPoint(150,100);
  CvPoint p2 = cvPoint(100,150);

  VisLine myline(p1,p2);
  myline.Draw(img);
  cvShowImage("test",img);
  cvWaitKey(0);
  cvDestroyWindow("test");
  cvReleaseImage(&img);

};
void tst2(void) 
{
  IplImage* img = cvCreateImage(cvSize(400,400),IPL_DEPTH_8U,1);

  cvNamedWindow("test",CV_WINDOW_AUTOSIZE);
  CvPoint p1 = cvPoint(100,100);
  cout << "Angle in degrees?" << endl;
  double angle;
  cin >> angle;
  VisLine myline(p1,angle*3.1415/180);
  myline.Draw(img);
  cvShowImage("test",img);
  cvWaitKey(0);
  cvDestroyWindow("test");
  cvReleaseImage(&img);
};
void tst3(void) 
{


};
void tst4(void) 
{

};
void tst5(void) 
{
	
	


};
void tst6(void) 
{
	

};
void tst7(void) 
{





};
void tst8(void) {};
void tst9(void) {};


int main(void)
  {
  char ans;
  do
    {
    cout << "0: " << endl ;
	cout << "1: Construct line from 2 points" << endl ;
	cout << "2: Construct line from point and angle" << endl ;
	cout << "3: " << endl ;
	cout << "4: " << endl ;
	cout << "5: " << endl ;
	cout << "6: " << endl ;
	cout << "7: " << endl ;
    cout << "0-9 tst0->tst9" << endl << endl;
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






