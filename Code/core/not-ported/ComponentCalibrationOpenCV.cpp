#include "ComponentCalibrationOpenCV.h"
#define THISCLASS ComponentCalibrationOpenCV

#include "DisplayEditor.h"
#include "DataStructureParticles.h"
#include <wx/log.h>
#include <fstream>

THISCLASS::ComponentCalibrationOpenCV(SwisTrackCore * stc):
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

void printResults(cv::Mat intrinsic_matrix, cv::Mat distortion_coeffs, cv::Mat rotation_vectors, cv::Mat translation_vectors) {
	//intrinsic_matrix, distortion_coeffs, rotation_vectors, translation_vectors);
	printf("INTRINSIC MATRIX\n");
	for (int i = 0; i < 3 ; i++){
		for (int j = 0; j < 3; j++) {
			printf("%f ", intrinsic_matrix->data.fl[i*3 + j]);
		}
		printf("\n");
	}

	printf("distortion_coeffs\n");
	for (int i = 0; i < 4 ; i++){
		printf("%f ", distortion_coeffs->data.fl[i]);
	}
	printf("\n");

	printf("rotation_vectors\n");
	for (int i = 0; i < 3 ; i++){
		printf("%f ", rotation_vectors->data.fl[i]);
	}
	printf("\n");

	printf("translation_vectors\n");
	for (int i = 0; i < 3 ; i++){
		printf("%f ", translation_vectors->data.fl[i]);
	}
	printf("\n");

}

int interpolate(cv::Mat mapx, cv::Mat mapy, cv::Size image_size) {

	int oo_X = 0;
	int oo_Y = 0;
	int x_count , y_count;
	int xx, yy;
	cv::Mat tmpX(image_size.height, image_size.width, CV_32SC1);
	cv::Mat tmpY(image_size.height, image_size.width, CV_32SC1);

	cvSetZero(tmpX);
	cvSetZero(tmpY);

	for (unsigned int i = 0; i < 778; i++) {
		for (unsigned int j = 0; j < 1032; j++){
			x_count = 0;
			y_count = 0;
			xx = 0;
			yy = 0;

			if (mapx->data.i[i*1032 + j] < 0 ){
				if ( i > 0  && mapx->data.i[(i-1)*1032 + j] >= 0) {
					x_count++;
					xx += mapx->data.i[(i-1)*1032 + j];
				}
				if ( j > 0  && mapx->data.i[(i)*1032 + j - 1] >= 0) {
					x_count++;
					xx += mapx->data.i[(i)*1032 + j - 1];
				}
				if ( i < 777  && mapx->data.i[(i+1)*1032 + j] >= 0) {
					x_count++;
					xx += mapx->data.i[(i+1)*1032 + j];
				}
				if ( j < 1031  && mapx->data.i[(i)*1032 + j + 1] >= 0) {
					x_count++;
					xx += mapx->data.i[(i)*1032 + j + 1];
				}

				if ( i > 0  && j > 0 && mapx->data.i[(i-1)*1032 + j-1] >= 0) {
					x_count++;
					xx += mapx->data.i[(i-1)*1032 + j-1];
				}
				if ( j > 0  && i < 777 && mapx->data.i[(i+1)*1032 + j - 1] >= 0) {
					x_count++;
					xx += mapx->data.i[(i+1)*1032 + j - 1];
				}
				if ( i < 777  && j < 1031 && mapx->data.i[(i+1)*1032 + j+1] >= 0) {
					x_count++;
					xx += mapx->data.i[(i+1)*1032 + j +1];
				}
				if ( j < 1031  && i > 0 && mapx->data.i[(i-1)*1032 + j + 1] >= 0) {
					x_count++;
					xx += mapx->data.i[(i-1)*1032 + j + 1];
				}
				if (x_count == 0){
					oo_X++;
					//mapx->data.i[i*1032 + j] = 0;
				}
				else
					tmpX->data.i[i*1032 + j] = 1 + (int)(((float)xx) / x_count);
			}

			if (mapy->data.i[i*1032 + j] < 0 ){
				if ( i > 0  && mapy->data.i[(i-1)*1032 + j] >= 0) {
					y_count++;
					yy += mapy->data.i[(i-1)*1032 + j];
				}
				if ( j > 0  && mapy->data.i[(i)*1032 + j - 1] >= 0) {
					y_count++;
					yy += mapy->data.i[(i)*1032 + j - 1];
				}
				if ( i < 777  && mapy->data.i[(i+1)*1032 + j] >= 0) {
					y_count++;
					yy += mapy->data.i[(i+1)*1032 + j];
				}
				if ( j < 1031  && mapy->data.i[(i)*1032 + j + 1] >= 0) {
					y_count++;
					yy += mapy->data.i[(i)*1032 + j + 1];
				}

				if ( i > 0  && j > 0 && mapy->data.i[(i-1)*1032 + j-1] >= 0) {
					y_count++;
					yy += mapy->data.i[(i-1)*1032 + j-1];
				}
				if ( j > 0  && i < 777 && mapy->data.i[(i+1)*1032 + j - 1] >= 0) {
					y_count++;
					yy += mapy->data.i[(i+1)*1032 + j - 1];
				}
				if ( i < 777  && j < 1031 && mapy->data.i[(i+1)*1032 + j+1] >= 0) {
					y_count++;
					yy += mapy->data.i[(i+1)*1032 + j +1];
				}
				if ( j < 1031  && i > 0 && mapy->data.i[(i-1)*1032 + j + 1] >= 0) {
					y_count++;
					yy += mapy->data.i[(i-1)*1032 + j + 1];
				}
				if (y_count == 0){
					oo_Y++;
					//mapy->data.i[i*1032 + j] = 0;
				}
				else
					tmpY->data.i[i*1032 + j] = 1 + (int)(((float)yy) / y_count) ;
			}


		}
	}

	//printf("olmadi X : %d, olmadi Y : %d\n", oo_X, oo_Y);
	cvAdd(mapx, tmpX, mapx);
	cvAdd(mapy, tmpY, mapy);

	cvReleaseMat(&tmpX);
	cvReleaseMat(&tmpY);
	return oo_X + oo_Y;
}

void THISCLASS::OnStart() {
	printf("on start, calibrate camera\n");
	// Read the file containing the calibration points
	wxString filenameString = GetConfigurationString(wxT("CalibrationPoints"), wxT(""));
	wxFileName filename = mCore->GetProjectFileName(filenameString);
	wxLogNull log;
	wxXmlDocument document;
	bool isopen = document.Load(filename.GetFullPath());
	if (! isopen) {
		AddError(wxT("The file \'") + filename.GetFullPath() + wxT("\' could not be read."));
		printf("exit 1\n");
		return;
	}

	// Select the root element and check its name
	SetRootNode(document.GetRoot());
	if (GetRootNode()->GetName() != wxT("pointlist")) {
		AddError(wxT("The XML root node must be called 'pointlist'!"));
		printf("exit 2\n");
		return;
	}

	// Enumerate all points in the list
	SelectChildNode(wxT("points"));
	if (! mSelectedNode) {
		AddError(wxT("No node 'points' found!"));
		printf("exit 3\n");
		return;
	}

	// Empty the vector
	calibrationPointList.clear();

	// Fill the vector with the readen points
	wxXmlNode * node = mSelectedNode->GetChildren();
	while (node) {
		if (node->GetName() == wxT("point"))
			ReadPoint(node);
		node = node->GetNext();
	}

	cv::Mat object_points = cvCreateMat(calibrationPointList.size(), 3, CV_32F);
	cv::Mat image_points = cvCreateMat(calibrationPointList.size(), 2, CV_32F);
	int number_of_views = 1;
	cv::Mat point_counts = cvCreateMat(number_of_views, 1, CV_32SC1);
	point_counts->data.i[0] = calibrationPointList.size();

	//vSize( int width, int height );
	cv::Size image_size = cvSize(1032, 778);

	//outputs:
	cv::Mat intrinsic_matrix(3, 3, CV_32F);
	cv::zero(intrinsic_matrix);
	// Initial values currently not used, let openCV calcultate them
	intrinsic_matrix.at<float>(0, 0) = 0.2f; //fx
	intrinsic_matrix.at<float>(0, 2) = 1032 / 2; //cx

	intrinsic_matrix.at<float>(1, 1) = 0.2f; //fy
	intrinsic_matrix.at<float>(1, 2) = 778 / 2; //cy
	intrinsic_matrix.at<float>(2, 2) = 1;

	cv::Mat distortion_coeffs(4, 1, CV_32F);
	cv::Mat rotation_vectors(number_of_views, 3, CV_32FC1);
	cv::zero(rotation_vectors);
	cv::Mat translation_vectors(number_of_views, 3, CV_32FC1);
	cv::zero(translation_vectors);

	// Build up matrices
	for (unsigned int i = 0; i < calibrationPointList.size(); i++) {
		// Image coordinates
		image_points.at<float>(i, 0) = calibrationPointList.at(i).xImage;
		image_points.at<float>(i, 1) = calibrationPointList.at(i).yImage;

		// World coordinates
		object_points.at<float>(i, 0) = calibrationPointList.at(i).xWorld;
		object_points.at<float>(i, 1) = calibrationPointList.at(i).yWorld;
		object_points.at<float>(i, 2) = 1;
	}

	printf("CV calculate camera parameters\n");
	cv::calibrateCamera(object_points, image_points, point_counts, image_size, intrinsic_matrix, distortion_coeffs, rotation_vectors, translation_vectors, 0 & CV_CALIB_USE_INTRINSIC_GUESS);

	cv::Mat rotation_matrix(3, 3, CV_32FC1);
	cv::Rodrigues(rotation_vectors, rotation_matrix);
	cv::Mat rotation_matrix_inverse = (intrinsic_matrix * rotation_matrix).inv();

	mapx = cvCreateMat(image_size.height, image_size.width, CV_32SC1);
	mapy = cvCreateMat(image_size.height, image_size.width, CV_32SC1);

	cv::Mat tmp_mapx = cvCreateMat(image_size.height, image_size.width, CV_32FC1);
	cv::Mat tmp_mapy = cvCreateMat(image_size.height, image_size.width, CV_32FC1);

	cvInitUndistortMap(intrinsic_matrix, distortion_coeffs, tmp_mapx, tmp_mapy);
	cv::Mat intrinsic_transfer = intrinsic_matrix * translation_vectors.t();

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

	std::fstream calc_world_coord;
	wxFileName calc_world_coord_name = mCore->GetRunFileName(wxT("calc_world_coord.log"));
	calc_world_coord.open(calc_world_coord_name.GetFullPath().mb_str(wxConvFile), std::fstream::out | std::fstream::trunc);

	std::fstream test_image_coord;
	wxFileName test_image_coord_name = mCore->GetRunFileName(wxT("test_image_coord.log"));
	test_image_coord.open(test_image_coord_name.GetFullPath().mb_str(wxConvFile), std::fstream::out | std::fstream::trunc);

	std::fstream mxfile;
	wxFileName mxfile_name = mCore->GetRunFileName(wxT("mxfile.log"));
	mxfile.open(mxfile_name.GetFullPath().mb_str(wxConvFile), std::fstream::out | std::fstream::trunc);
	std::fstream myfile;
	wxFileName myfile_name = mCore->GetRunFileName(wxT("myfile.log"));
	myfile.open(myfile_name.GetFullPath().mb_str(wxConvFile), std::fstream::out | std::fstream::trunc);

	//cvSetZero(mapx);
	//cvSetZero(mapy);
	cvSet(mapx, cvScalarAll(-1), 0);
	cvSet(mapy, cvScalarAll(-1), 0);
	for (unsigned int i = 0; i < 778; i++) {
		for (unsigned int j = 0; j < 1032; j++){
			int tmp_x = tmp_mapx->data.fl[i*1032 + j];
			int tmp_y = tmp_mapy->data.fl[i*1032 + j];
			mapx->data.i[(int)tmp_y*1032 + (int)tmp_x] = j;
			mapy->data.i[(int)tmp_y*1032 + (int)tmp_x] = i;
		}
	}

	while (interpolate(mapx, mapy, image_size) > 1032 * 778 * 0.0);

	for (unsigned int i = 0; i < 778; i++) {
		for (unsigned int j = 0; j < 1032; j++){
			mxfile << mapx->data.i[i*1032 + j] << "\t";
			myfile << mapy->data.i[i*1032 + j] << "\t";
		}

		mxfile << std::endl;
		myfile << std::endl;
	}

	for (unsigned int i = 0; i < calibrationPointList.size(); i++) {
		cv::Point2f originalImage;
		originalImage.x = (float) calibrationPointList.at(i).xImage;
		originalImage.y = (float) calibrationPointList.at(i).yImage;
		coord << "\t" << originalImage.x << "\t" << originalImage.y << std::endl;

		cv::Point2f originalWorld;
		originalWorld.x = (float) calibrationPointList.at(i).xWorld;
		originalWorld.y = (float) calibrationPointList.at(i).yWorld;
		//if (originalWorld.y > 9 || originalWorld.y <  6.5) continue;
		world_coord << "\t" << originalWorld.x << "\t" << originalWorld.y << std::endl;
		cv::Point2f finalWorld = THISCLASS::Image2World(originalImage);
		calc_world_coord << "\t" << finalWorld.x << "\t" << finalWorld.y << std::endl;

		double tmpErrorX = (double) fabsf(originalWorld.x - finalWorld.x);
		double tmpErrorY = (double) fabsf(originalWorld.y - finalWorld.y);
		double tmpErrorD = sqrt(tmpErrorX * tmpErrorX + tmpErrorY * tmpErrorY);

		errorX += tmpErrorX;
		errorY += tmpErrorY;
		errorD += tmpErrorD;

		if (tmpErrorX > maxErrorX) maxErrorX = tmpErrorX;
		if (tmpErrorY > maxErrorY) maxErrorY = tmpErrorY;
		if (tmpErrorD > maxErrorD) maxErrorD = tmpErrorD;
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
	logfile << "Intrinsic matrix: 3x3" << std::endl;

	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++) {
			logfile << "\t" << intrinsic_matrix->data.fl[i*3 + j];
		}
		logfile << std::endl;
	}

	logfile << std::endl << std::endl;

	logfile << "distortion_coeffs: 4x1" << std::endl;
	for (int i = 0; i < 4; i++){
		logfile << "\t" << distortion_coeffs->data.fl[i];
	}
	logfile << std::endl << std::endl;

	logfile << "rotation_matrix: 3x3" << std::endl;
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++) {
			logfile << "\t" << rotation_matrix->data.fl[i*3 + j];
		}
		logfile << std::endl;
	}

	logfile << std::endl << std::endl;

	logfile << "inverse(intrinsic*rotation): 3x3" << std::endl;
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++) {
			logfile << "\t" << rotation_matrix_inverse->data.fl[i*3 + j];
		}
		logfile << std::endl;
	}

	logfile << std::endl << std::endl;

	logfile << "intrinsic*translation_vectors: 3x1" << std::endl;
	for (int i = 0; i < 3; i++){
		logfile << "\t" << intrinsic_transfer->data.fl[i];
	}
	logfile << std::endl;

	logfile.close();

	printf("finished on start\n");
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	// These are the particles to transform
	DataStructureParticles::tParticleVector * particles = mCore->mDataStructureParticles.mParticles;
	if (! particles) return;

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

void THISCLASS::ReadPoint(wxXmlNode * node) {
	mSelectedNode = node;

	CalibrationPoint calibrationPoint;
	calibrationPoint.xImage = ConfigurationConversion::Double(ReadChildContent(wxT("ximage")), 0);
	calibrationPoint.yImage = ConfigurationConversion::Double(ReadChildContent(wxT("yimage")), 0);
	calibrationPoint.xWorld = ConfigurationConversion::Double(ReadChildContent(wxT("xworld")), 0);
	calibrationPoint.yWorld = ConfigurationConversion::Double(ReadChildContent(wxT("yworld")), 0);
	calibrationPointList.push_back(calibrationPoint);
}

cv::Point2f THISCLASS::Image2World(cv::Point2f p) {
	cv::Point2f p_undistort;
	p_undistort.x = mapx->data.i[(int)p.y*1032 + (int)p.x];
	p_undistort.y = mapy->data.i[ (int)p.y*1032 + (int)p.x];

	float v_th =  rotation_matrix_inverse.at<float>[6] * p_undistort.x +
	              rotation_matrix_inverse.at<float>[7] * p_undistort.y +
	              rotation_matrix_inverse.at<float>[8];

	float rct_th =  rotation_matrix_inverse.at<float>[6] * intrinsic_transfer.at<float>[0] +
	                rotation_matrix_inverse.at<float>[7] * intrinsic_transfer.at<float>[1] +
	                rotation_matrix_inverse.at<float>[8] * intrinsic_transfer.at<float>[2];

	float s = (rct_th + 1) / v_th;

	cv::Mat p_prime(3, 1, CV_32FC1);
	p_prime.at<float>(0, 0) = p_undistort.x * s;
	p_prime.at<float>(1, 0) = p_undistort.y * s;
	p_prime.at<float>(2, 0) = s;

	// find corresponding point in world
	cv::Mat rotated = rotation_matrix_inverse * (p_prime - intrinsic_transfer);

	if (rotated.at<float>(2, 0) > 1.01 || rotated.at<float>(2, 0) < 0.99)
		printf("%f  ", p_prime.at<float>(2, 0));

	cv::Point2f w;
	w.x = rotated.at<float>(0, 0) / rotated.at<float>(2, 0);
	w.y = rotated.at<float>(1, 0) / rotated.at<float>(2, 0);
	return w;
}
