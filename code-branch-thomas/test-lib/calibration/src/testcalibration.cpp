#include <cstdio>
#include "calibration.h"


int main()
{
try{
	Calibration* transform = new Calibration(
			"../../../../release/example/bg_acalibration.bmp",//background.bmp",
			"../../../../release/example/calibration.bmp");
			int nrofpoints[4]={8,16,16,28};
					transform->CalibrateCenter();
					transform->CalibrateRoundPattern(4,nrofpoints,0.115);
					
		//	CvPoint2D32f p1=transform->GetCenter();
		//	CvPoint2D32f p2=transform->GetTarget();
		//	CvPoint2D32f p3=transform->GetDxDy();

		//	printf("Center: %f %f\n",p1.x,p1.y);
		//	printf("Target: %f %f\n",p2.x,p2.y);
		//	printf("DxDy: %f %f\n",p3.x,p3.y);

	
	CvPoint2D32f p4= transform->ImageToWorld(cvPoint2D32f(349,278));
	printf("(%f/%f)\n",p4.x,p4.y);
    }
catch(char * str){
    printf("%s\n",str);
}
    return(0);
}

