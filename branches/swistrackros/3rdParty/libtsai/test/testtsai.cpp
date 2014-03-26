//******************************* DEFINITION MODULE **********************************************/
//
// Author   :   Your name, your e-mail
//              $Author: yuri $
//              Ecole Polytechnique Federale de Lausanne
//              IPR-LPM-VISION
//              CH-1015 Lausanne
//              Switzerland
// Filename :   $RCSfile: testtsai.cpp,v $
// Version:     $Revision: 1.1.1.1 $
/*************************************************************************************************/

/** \file entete_vision.cpp
    
    Implementation of VisClass
*/


// CVS/SCCS Tag
static char *id="@(#) $Id: testtsai.cpp,v 1.1.1.1 2003/12/01 16:11:19 yuri Exp $";

/******************************** inclusions *****************************************************/
/*************************************************************************************************/
#include "testtsai.h"
#include<iostream>

/*************************************************************************************************/
/************ private types, variables, classes and function prototypes **************************/
/*************************************************************************************************/

calibration_data tt_cd;
calibration_constants tt_cc;
camera_parameters tt_cp;

/*************************************************************************************************/
/************* public class methods and functions implementations ********************************/
/*************************************************************************************************/


int main(void)
{
	int CurrentPoint;
	double tx,ty;

	tt_cp.Ncx=752.;
	tt_cp.Nfx=768.;
	tt_cp.dx=0.0065;
	tt_cp.dy=0.00625;
	tt_cp.dpx=tt_cp.dx*tt_cp.Ncx/tt_cp.Nfx;
	tt_cp.dpy=tt_cp.dy*582./576.; //dy*NCy/NFy
	tt_cp.Cx=768./2.;
	tt_cp.Cy=576./2.;
	tt_cp.sx=1.0;


	tt_cc.f = 0.00625;
	tt_cc.kappa1 = 0.0;
	tt_cc.p1 = 0.0;
	tt_cc.p2 = 0.0;
	tt_cc.r1=1.0;
	tt_cc.r2=0.0;
	tt_cc.r3=0.0;
	tt_cc.r4=0.0;
	tt_cc.r5=1.0;
	tt_cc.r6=0.0;
	tt_cc.r7=0.0;
	tt_cc.r8=0.0;
	tt_cc.r9=1.0;
	tt_cc.Rx=0.0;
	tt_cc.Ry=0.0;
	tt_cc.Rz=0.0;
	tt_cc.Tx=0.0;
	tt_cc.Ty=0.0;
	tt_cc.Tz=1.0;


	CurrentPoint=0;//1
	tt_cd.xw[CurrentPoint]=0.3;
	tt_cd.yw[CurrentPoint]=0.9;
	tt_cd.zw[CurrentPoint]=0.0;
	tt_cd.Xf[CurrentPoint]=9.8;
	tt_cd.Yf[CurrentPoint]=10.3;
	CurrentPoint++;//2
	tt_cd.xw[CurrentPoint]=13.2;
	tt_cd.yw[CurrentPoint]=0.1;
	tt_cd.zw[CurrentPoint]=0.0;
	tt_cd.Xf[CurrentPoint]=22.6;
	tt_cd.Yf[CurrentPoint]=10.4;
	CurrentPoint++;//3
	tt_cd.xw[CurrentPoint]=0.2;
	tt_cd.yw[CurrentPoint]=8.7;
	tt_cd.zw[CurrentPoint]=0.0;
	tt_cd.Xf[CurrentPoint]=10.2;
	tt_cd.Yf[CurrentPoint]=27.6;	
	CurrentPoint++;//4
	tt_cd.xw[CurrentPoint]=14.2;
	tt_cd.yw[CurrentPoint]=15.9;
	tt_cd.zw[CurrentPoint]=0.0;
	tt_cd.Xf[CurrentPoint]=24.2;
	tt_cd.Yf[CurrentPoint]=41.8;	
	CurrentPoint++;//5
	tt_cd.xw[CurrentPoint]=42.8;
	tt_cd.yw[CurrentPoint]=11.5;
	tt_cd.zw[CurrentPoint]=0.0;
	tt_cd.Xf[CurrentPoint]=53.3;
	tt_cd.Yf[CurrentPoint]=32.6;
	CurrentPoint++;//6
	tt_cd.xw[CurrentPoint]=15.2;
	tt_cd.yw[CurrentPoint]=22.4;
	tt_cd.zw[CurrentPoint]=0.0;
	tt_cd.Xf[CurrentPoint]=24.9;
	tt_cd.Yf[CurrentPoint]=54.6;
	CurrentPoint++;//7
	tt_cd.xw[CurrentPoint]=34.1;
	tt_cd.yw[CurrentPoint]=7.9;
	tt_cd.zw[CurrentPoint]=0.0;
	tt_cd.Xf[CurrentPoint]=44.3;
	tt_cd.Yf[CurrentPoint]=25.9;

	tt_cd.point_count=CurrentPoint+1;

	
	coplanar_calibration_with_full_optimization(&tt_cd,&tt_cc,&tt_cp);

	printf("f  = %e \n",tt_cc.f);
	printf("k1 = %e \n",tt_cc.kappa1);
	printf("p1 = %e \n",tt_cc.p1);
	printf("p2 = %e \n",tt_cc.p2);
	printf("r1 = %e \n",tt_cc.r1);
	printf("r2 = %e \n",tt_cc.r2);
	printf("r3 = %e \n",tt_cc.r3);
	printf("r4 = %e \n",tt_cc.r4);
	printf("r5 = %e \n",tt_cc.r5);
	printf("r6 = %e \n",tt_cc.r6);
	printf("r7 = %e \n",tt_cc.r7);
	printf("r8 = %e \n",tt_cc.r8);
	printf("r9 = %e \n",tt_cc.r9);
	printf("Rx = %e \n",tt_cc.Rx);
	printf("Ry = %e \n",tt_cc.Ry);
	printf("Rz = %e \n",tt_cc.Rz);
	printf("Tx = %e \n",tt_cc.Tx);
	printf("Ty = %e \n",tt_cc.Ty);
	printf("Tz = %e \n",tt_cc.Tz);
	printf("\n");
	printf("Ncx = %e \n",tt_cp.Ncx);
	printf("Nfx = %e \n",tt_cp.Nfx);
	printf("dx  = %e \n",tt_cp.dx);
	printf("dy  = %e \n",tt_cp.dy);
	printf("dpx = %e \n",tt_cp.dpx);
	printf("dpy = %e \n",tt_cp.dpy);
	printf("Cx = %e \n",tt_cp.Cx);
	printf("Cy = %e \n",tt_cp.Cy);
	printf("sx = %e \n",tt_cp.sx);
	
	world_coord_to_image_coord(tt_cc,tt_cp,14,15,0,&tx,&ty);
	
	printf("\n");
	printf("transformation w to i\n");
	printf("xw = 14\n");
	printf("yw  = 15\n");
	printf("zw  = 0\n");
	printf("xi = %e \n",tx);
	printf("yi = %e \n",ty);


	image_coord_to_world_coord(tt_cc,tt_cp,24,40,0,&tx,&ty);
	
	printf("\n");
	printf("transformation i to w\n");
	printf("xi = 24\n");
	printf("yi = 40\n");
	printf("xw = %e \n",tx);
	printf("yw  = %e \n",ty);
	printf("zw  = 0\n");
	int i;
	std::cin>>i;


	return true;
}
  

 

/*************************************************************************************************/
/************* private class methods and functions implementations *******************************/
/*************************************************************************************************/

/*************************************************************************************************/
/********************** test routines/program ****************************************************/
/*************************************************************************************************/
