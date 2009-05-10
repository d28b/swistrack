#include "ComponentCalibrationOpenCV.h"
#define THISCLASS ComponentCalibrationOpenCV

#include "DisplayEditor.h"
#include "DataStructureParticles.h"
#include <wx/log.h>
#include <fstream>

THISCLASS::ComponentCalibrationOpenCV(SwisTrackCore *stc):
		Component(stc, wxT("CalibrationOpenCV")),
		mDisplayOutput(wxT("Output"), wxT("Linear Calibration: Output")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryCalibration);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentCalibrationOpenCV() {
}


void printResults(CvMat* intrinsic_matrix, CvMat* distortion_coeffs, CvMat* rotation_vectors, CvMat* translation_vectors) {
//intrinsic_matrix, distortion_coeffs, rotation_vectors, translation_vectors);	
   printf("INTRINSIC MATRIX\n");
	for(int i = 0; i<3 ; i++){
		for(int j=0; j<3; j++) {
			printf("%f ", intrinsic_matrix->data.fl[i*3 + j]);
		}
		printf("\n");
	}

   printf("distortion_coeffs\n");
	for(int i = 0; i<4 ; i++){
		printf("%f ", distortion_coeffs->data.fl[i]);
	}
	printf("\n");

   printf("rotation_vectors\n");
	for(int i = 0; i<3 ; i++){
			printf("%f ", rotation_vectors->data.fl[i]);
	}
	printf("\n");

	printf("translation_vectors\n");
	for(int i = 0; i<3 ; i++){
			printf("%f ", translation_vectors->data.fl[i]);
	}
	printf("\n");

}
void THISCLASS::OnStart() {
	printf("on start\n");
	// Read the file containing the calibration points
	wxString filename_string = GetConfigurationString(wxT("CalibrationPoints"), wxT(""));
	wxFileName filename = mCore->GetProjectFileName(filename_string);
	wxLogNull log;
	wxXmlDocument document;
	bool isopen = document.Load(filename.GetFullPath());
	if (! isopen) {
		AddError(wxT("The file \'") + filename.GetFullPath() + wxT("\' could not be read."));
		return;
	}

	// Select the root element and check its name
	SetRootNode(document.GetRoot());
	if (GetRootNode()->GetName() != wxT("pointlist")) {
		AddError(wxT("The XML root node must be called 'pointlist'!"));
		return;
	}

	// Enumerate all points in the list
	SelectChildNode(wxT("points"));
	if (! mSelectedNode) {
		AddError(wxT("No node 'points' found!"));
		return;
	}

	// Empty the vector
	calibrationPointList.clear();

	// Fill the vector with the readen points
	wxXmlNode *node = mSelectedNode->GetChildren();
	while (node) {
		if (node->GetName() == wxT("point")) {
			ReadPoint(node);
		}
		node = node->GetNext();
	}


/*
void cvCalibrateCamera2( const CvMat* object_points, const CvMat* image_points,
                         const CvMat* point_counts, CvSize image_size,
                         CvMat* intrinsic_matrix, CvMat* distortion_coeffs,
                         CvMat* rotation_vectors=NULL, CvMat* translation_vectors=NULL,
                         int flags=0 );
*/
// build required matrices for above function 

/*


object_points
    The joint matrix of object points, 3xN or Nx3, where N is the total number of points in all views. 
image_points
    The joint matrix of corresponding image points, 2xN or Nx2, where N is the total number of points in all views. 
point_counts
    Vector containing numbers of points in each particular view, 1xM or Mx1, where M is the number of a scene views. 
image_size
    Size of the image, used only to initialize intrinsic camera matrix. 
intrinsic_matrix
    The output camera matrix (A) [fx 0 cx; 0 fy cy; 0 0 1]. If CV_CALIB_USE_INTRINSIC_GUESS and/or CV_CALIB_FIX_ASPECT_RATION are specified, some or all of fx, fy, cx, cy must be initialized. 
distortion_coeffs
    The output 4x1 or 1x4 vector of distortion coefficients [k1, k2, p1, p2]. 
rotation_vectors
    The output 3xM or Mx3 array of rotation vectors (compact representation of rotation matrices, see cvRodrigues2). 

Rodrigues2

Converts rotation matrix to rotation vector or vice versa

int  cvRodrigues2( const CvMat* src, CvMat* dst, CvMat* jacobian=0 );


translation_vectors
    The output 3xM or Mx3 array of translation vectors. 
*/


	CvMat* object_points = cvCreateMat(calibrationPointList.size() , 3, CV_32F);
	// corresponding image points
	CvMat* image_points = cvCreateMat(calibrationPointList.size() , 2, CV_32F);
	//don't we have only  one view?
	int number_of_views = 1;
	CvMat* point_counts = cvCreateMat(number_of_views, 1, CV_32SC1);
	point_counts->data.i[0] = calibrationPointList.size();



	//vSize( int width, int height );
	CvSize image_size = cvSize(1032, 778); 

	//outputs:
	intrinsic_matrix = cvCreateMat(3, 3, CV_32F);
	distortion_coeffs = cvCreateMat(4, 1, CV_32F);
	rotation_vectors = cvCreateMat(number_of_views ,3 , CV_32FC1);
	cvZero(rotation_vectors);
	translation_vectors = cvCreateMat(number_of_views , 3, CV_32FC1);
	cvZero(translation_vectors);

	// Build up matrices
	for (unsigned int i = 0; i < calibrationPointList.size(); i++) {

		///image coordinates
		image_points->data.fl[i*2] = (calibrationPointList.at(i)).xImage;
		image_points->data.fl[i*2+1] = (calibrationPointList.at(i)).yImage;

		///world coordinates
		object_points->data.fl[i*3] = (calibrationPointList.at(i)).xWorld;
		object_points->data.fl[i*3 + 1] = (calibrationPointList.at(i)).yWorld;
		object_points->data.fl[i*3 + 2] = 1;
	}


cvCalibrateCamera2( object_points, image_points, point_counts, image_size, 
					intrinsic_matrix, distortion_coeffs, rotation_vectors, translation_vectors);	
//double cvInvert( const CvArr* src, CvArr* dst, int method=CV_LU );


	rotation_matrix = cvCreateMat(3, 3, CV_32FC1);
	rotation_matrix_inverse = cvCreateMat(3, 3, CV_32FC1);

	//cvRodrigues2( const CvMat* src, CvMat* dst, CvMat* jacobian=0 );
	cvRodrigues2( rotation_vectors, rotation_matrix);
	CvMat* tmp = cvCreateMat(3, 3, CV_32FC1);
	cvMatMul(intrinsic_matrix , rotation_matrix, tmp );
	cvInvert(tmp, rotation_matrix_inverse);
cvReleaseMat(&tmp);


	translation_tr = cvCreateMat(3, 1,CV_32FC1);
	cvTranspose( translation_vectors , translation_tr  );
//	printResults(intrinsic_matrix, distortion_coeffs, rotation_vectors, translation_vectors);

/*
InitUndistortMap

Computes undistorion map

void cvInitUndistortMap( const CvMat* intrinsic_matrix,
                         const CvMat* distortion_coeffs,
                         CvArr* mapx, CvArr* mapy );

intrinsic_matrix
    The camera matrix (A) [fx 0 cx; 0 fy cy; 0 0 1]. 
distortion_coeffs
    The vector of distortion coefficients, 4x1 or 1x4 [k1, k2, p1, p2]. 
mapx
    The output array of x-coordinates of the map. 
mapy
    The output array of y-coordinates of the map. 

The function cvInitUndistortMap pre-computes the undistortion map - coordinates of the corresponding pixel in the distorted image for every pixel in the corrected image. Then, the map (together with input and output images) can be passed to cvRemap function. 

*/

	mapx = cvCreateImage( image_size,IPL_DEPTH_32F, 1);
	mapy = cvCreateImage( image_size,IPL_DEPTH_32F, 1);

	cvInitUndistortMap( intrinsic_matrix,
                        distortion_coeffs,
                        mapx, mapy );	


	//Compute error of calibration on calibration points
	double errorX, errorY, maxErrorX, maxErrorY, maxErrorD, errorD;
	maxErrorX = 0;
	maxErrorY = 0;
	errorX = 0;
	errorY = 0;
	maxErrorD = 0;
	errorD = 0;
	std::fstream coord;
	wxFileName coord_name = mCore->GetRunFileName(wxT("coord.log"));	
	coord.open(coord_name.GetFullPath().mb_str(wxConvFile), std::fstream::out | std::fstream::trunc);

	std::fstream world_coord;
	wxFileName world_coord_name = mCore->GetRunFileName(wxT("world_coord.log"));	
	world_coord.open(world_coord_name.GetFullPath().mb_str(wxConvFile), std::fstream::out | std::fstream::trunc);

	for (unsigned int i = 0; i < calibrationPointList.size(); i++) {
		double tmpErrorX, tmpErrorY, tmpErrorD;
		CvPoint2D32f originalImage, originalWorld, finalWorld;
		originalImage.x = (float)(calibrationPointList.at(i)).xImage;
		originalImage.y = (float)(calibrationPointList.at(i)).yImage;
		coord << "\t" << originalImage.x << "\t" << originalImage.y <<std::endl;
		originalWorld.x = (float)(calibrationPointList.at(i)).xWorld;
		originalWorld.y = (float)(calibrationPointList.at(i)).yWorld;
		world_coord << "\t" << originalWorld.x << "\t" << originalWorld.y <<std::endl;
		finalWorld = THISCLASS::Image2World(originalImage);
		tmpErrorX = abs(originalWorld.x - finalWorld.x);
		tmpErrorY = abs(originalWorld.y - finalWorld.y);
		tmpErrorD = sqrt(tmpErrorX * tmpErrorX + tmpErrorY * tmpErrorY);
		errorX += tmpErrorX;
		errorY += tmpErrorY;
		errorD += tmpErrorD;
		if (tmpErrorX > maxErrorX) {
			maxErrorX = tmpErrorX;
		}
		if (tmpErrorY > maxErrorY) {
			maxErrorY = tmpErrorY;
		}
		if (tmpErrorD > maxErrorD) {
			maxErrorD = tmpErrorD;
		}
	}
	errorX = errorX / calibrationPointList.size();
	errorY = errorY / calibrationPointList.size();
	errorD = errorD / calibrationPointList.size();

	// Write the calibration parameters
	std::fstream logfile;
	wxFileName logfilename = mCore->GetRunFileName(wxT("CalibrationOpenCV.log"));
	
	logfile.open(logfilename.GetFullPath().mb_str(wxConvFile), std::fstream::out | std::fstream::trunc);
	logfile << "Calibration error" << std::endl;
	logfile << "Max error in X: \t" << maxErrorX << std::endl;
	logfile << "Max error in Y: \t" << maxErrorY << std::endl;
	logfile << "Average error in X: \t" << errorX << std::endl;
	logfile << "Average error in Y: \t" << errorY << std::endl;
	logfile << "Max distance error: \t" << maxErrorD << std::endl;
	logfile << "Average distance error: \t" << errorD << std::endl;
/*
	CvMat* intrinsic_matrix;
	CvMat* distortion_coeffs;
	CvMat* rotation_vectors;
	CvMat* rotation_matrix;
	CvMat* rotation_matrix_inverse;
	CvMat* translation_vectors;
*/
	logfile << "Intrinsic matrix: 3x3" << std::endl;
	for(int i = 0; i < 3; i++){
		for(int j = 0; j<3; j++) {
			logfile << "\t" <<intrinsic_matrix->data.fl[i*3 + j];
		}
		logfile << std::endl;
	}

	logfile << std::endl<< std::endl;


	logfile << "distortion_coeffs: 4x1" << std::endl;
	for(int i = 0; i < 4; i++){
		logfile << "\t" <<distortion_coeffs->data.fl[i];
	}
	logfile << std::endl<< std::endl;


	logfile << "rotation_matrix: 3x3" << std::endl;
	for(int i = 0; i < 3; i++){
		for(int j = 0; j<3; j++) {
			logfile << "\t" <<rotation_matrix->data.fl[i*3 + j];
		}
		logfile << std::endl;
	}

	logfile << std::endl<< std::endl;

	logfile << "rotation_matrix_inverse: 3x3" << std::endl;
	for(int i = 0; i < 3; i++){
		for(int j = 0; j<3; j++) {
			logfile << "\t" <<rotation_matrix_inverse->data.fl[i*3 + j];
		}
		logfile << std::endl;
	}

	logfile << std::endl<< std::endl;

	logfile << "translation_vectors: 3x1" << std::endl;
	for(int i = 0; i < 3; i++){
		logfile << "\t" <<translation_vectors->data.fl[i];
	}
	logfile << std::endl;

	logfile.close();

	std::fstream logfile2;
	logfilename = mCore->GetRunFileName(wxT("mapx.log"));
	
	logfile2.open(logfilename.GetFullPath().mb_str(wxConvFile), std::fstream::out | std::fstream::trunc);

	std::fstream logfile3;
	logfilename = mCore->GetRunFileName(wxT("mapy.log"));
	
	logfile3.open(logfilename.GetFullPath().mb_str(wxConvFile), std::fstream::out | std::fstream::trunc);

	for(int i = 0; i< 778; i++){
		for(int j = 0; j < 1032; j++) {
			//	p_undistort.x = (mapx->imageData)[(int)p.y* mapx->widthStep + (int)p.x*mapx->nChannels ];
	//p_undistort.y = (mapy->imageData)[(int)p.y* mapy->widthStep + (int)p.x*mapy->nChannels ]; 
			logfile2 << "\t" << (float)((mapx->imageData)[int(i*mapx->widthStep + j*mapx->nChannels) ]);
			logfile3 << "\t" << (float)((mapy->imageData)[int(i*mapy->widthStep + j*mapy->nChannels) ]);
		}
		logfile2 << std::endl;
		logfile3 << std::endl;
	}

printf("bitti\n");
cvReleaseMat(&object_points);
cvReleaseMat(&image_points);
cvReleaseMat(&point_counts);
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	// These are the particles to transform
	DataStructureParticles::tParticleVector *particles = mCore->mDataStructureParticles.mParticles;
	if (! particles) {
		return;
	}

	// Transform all particle positions
	DataStructureParticles::tParticleVector::iterator it = particles->begin();
	while (it != particles->end()) {
		it->mWorldCenter = Image2World(it->mCenter);
		it++;
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

void THISCLASS::ReadPoint(wxXmlNode *node) {
	mSelectedNode = node;

	CalibrationPoint calibrationPoint;
	calibrationPoint.xImage = ConfigurationConversion::Double(ReadChildContent(wxT("ximage")), 0);
	calibrationPoint.yImage = ConfigurationConversion::Double(ReadChildContent(wxT("yimage")), 0);
	calibrationPoint.xWorld = ConfigurationConversion::Double(ReadChildContent(wxT("xworld")), 0);
	calibrationPoint.yWorld = ConfigurationConversion::Double(ReadChildContent(wxT("yworld")), 0);
	calibrationPointList.push_back(calibrationPoint);
}

CvPoint2D32f THISCLASS::Image2World(CvPoint2D32f p) {
	CvPoint2D32f w;

	CvPoint2D32f p_undistort;
/*
  // get the image data
  height    = img->height;
  width     = img->width;
  step      = img->widthStep;
  channels  = img->nChannels;
  data      = (uchar *)img->imageData;
  for(i=0;i<height;i++) for(j=0;j<width;j++) for(k=0;k<channels;k++)
    data[i*step+j*channels+k]=255-data[i*step+j*channels+k];


*/
	p_undistort.x = (mapx->imageData)[(int)p.y* mapx->widthStep + (int)p.x*mapx->nChannels ];
	p_undistort.y = (mapy->imageData)[(int)p.y* mapy->widthStep + (int)p.x*mapy->nChannels ];
	CvMat* image_coor = cvCreateMat(3, 1,CV_32FC1);
	CvMat* tmp = cvCreateMat(3, 1,CV_32FC1);
	image_coor->data.fl[0] = p_undistort.x;
	image_coor->data.fl[1] = p_undistort.y;
	image_coor->data.fl[2] = 1;

	//define cvMatMul( src1, src2, dst ) cvMatMulAdd( src1, src2, 0, dst )


	

	//oid cvSub( const CvArr* src1, const CvArr* src2, CvArr* dst, const CvArr* mask=NULL );

	//void cvTranspose( const CvArr* src, CvArr* dst );
	cvSub(image_coor, translation_tr , tmp);   //tmp = image - translation

	cvMatMul(rotation_matrix_inverse ,tmp,image_coor);
	


	//cvMatMul(rotation_matrix_inverse , tmp, image_coor);

	w.x = image_coor->data.fl[0]/image_coor->data.fl[2];
	w.y = image_coor->data.fl[1]/image_coor->data.fl[2];

	return(w);
}
