
#ifndef libgeom_h        
#define libgeom_h
//****************************************************************************/
// Filename :   $RCSfile: libgeom.h,v $
// Version  :   $Revision: 1.2 $
//
// Author   :   $Author: produit $
//              Yuri Lopez de Meneses
//              Ecole Polytechnique Federale de Lausanne
//              DMT-IPM-VISION
//              CH-1015 Lausanne
//              Switzerland
//
//
// Date     :   $Date: 2006/10/13 12:32:55 $


/** \file libgeom.h
    \brief Description of the libgeom library

	This file is extracted from the .hpp files of each class.
*/

// See end of this header file for usage notes
/*****************************************************************************/
/******************************** inclusions *********************************/
/*****************************************************************************/

#include <cv.h>
#include <vector>
#include "vispolygon.h"


/*****************************************************************************/
/******************************** defines ************************************/
/*****************************************************************************/

#ifndef PI
	#define PI 3.14159265358979323846
#endif PI

#ifdef linux
  #define VISAPI 
  #define EXPIMP_TEMPLATE 
#else
		#ifdef _USRDLL 
			#define VISAPI __declspec(dllexport) 
			#define EXPIMP_TEMPLATE 
		#else
			#define VISAPI
			#define EXPIMP_TEMPLATE
		#endif _USRDLL
#endif linux

/*****************************************************************************/
/******************************** types **************************************/
/*****************************************************************************/

// Instantiate classes vector<int> and vector<char>
// This does not create an object. It only forces the generation of all
// of the members of classes vector<double> and vector<CvPoint2D32f>. It exports
// them from the DLL and imports them into the .exe file.
#ifdef _USRDLL
	EXPIMP_TEMPLATE template class VISAPI std::allocator<double>;
	EXPIMP_TEMPLATE template class VISAPI std::vector<double,std::allocator<double>>;
	EXPIMP_TEMPLATE template class VISAPI std::allocator<CvPoint2D32f>;
	EXPIMP_TEMPLATE template class VISAPI std::vector<CvPoint2D32f,allocator<CvPoint2D32f>>;
	EXPIMP_TEMPLATE template class VISAPI std::allocator<int>;
	EXPIMP_TEMPLATE template class VISAPI std::vector<int,std::allocator<int>>;
#endif _USRDLL

/*****************************************************************************/
/******************************** prototypes *********************************/
/*****************************************************************************/

inline VISAPI CvPoint2D32f ProjectXY(CvPoint3D32f pt){
	return cvPoint2D32f(pt.x,pt.y);
};

inline VISAPI CvPoint2D32f ProjectYZ(CvPoint3D32f pt){
	return cvPoint2D32f(pt.y,pt.z);
};

inline VISAPI CvPoint2D32f ProjectXZ(CvPoint3D32f pt){
	return cvPoint2D32f(pt.x,pt.z);
};


/**
    
    This class enables to create a template made of reference points and a target point. 
	From a new set of points, based on the template, we can calculate the expected position of the target point by minimizing the error.
	The position of the target correspond to the intersection of the lines going through the middle of the segments determined by two following reference points.
	The model (template) stock the angles between the lines and the segments.
	\image html geometricModel.png "Exemple of a model with 8 reference points and a target point

	 \author $Author: produit $
     \version $Revision: 1.2 $
     \date $Date: 2006/10/13 12:32:55 $


*/
class VISAPI GeometricModel
{
	private:
			/** sine list */
			vector<double> sinus_seq; 
			/** cosinus list  */
			vector<double> cosinus_seq;
			/** learnt template points list  */
			vector<CvPoint2D32f> reference_points;
			/** target point */
			CvPoint2D32f target_point;
			/** Indicates wether a target is defined */
			bool target_set;
			bool model_learnt;
			

			/** draws a cross */
			void DrawCross(CvArr* array, CvPoint pt, int branch , CvScalar color);

	public:


		/** Constructor */
		GeometricModel(); 

		/** Destructor */
		~GeometricModel();

		/** Sets the target point */
		void SetTargetPoint(CvPoint2D32f point);

		/** Adds a reference point */
		int AddReferencePoint(CvPoint2D32f point);

		/** Adds a series of reference points */
		int AddListReferencePoints(CvPoint2D32f* point_list,int n);

		/** Empty reference points */
		int ClearReferencePoints();

		/** Create a model from the reference points to the target */
		double CreateModel();

		/** Calculate the target point from the found points list and the sines and cosines lists */
		CvPoint2D32f CalculateTarget(double* p_cond_number, double* p_error);

		/** Save Model to file */
		int SaveModel(char* fname);

		/** Load Model from file */
		int LoadModel(char* fname);

		/** Returns the number of reference points currently available*/
		int GetNumReferencePoints();

		/** Returns the number of necessary reference points */
		int GetNumModelPoints();

		/** Draws the reference points and model */
		int DrawModel(IplImage* img);

};

/*! \class VisLine Line.hpp
 *  \brief Definition of the "VisLine class" which contains a line and the methods
 *			which enable to calculate lines intersection, angle between 2 lines
 *			and distance between 2 parallel lines.
 *
 *   Purpose  : Definition of the "VisLine class" which contains a line and the methods
 *			which enable to calculate lines intersection, angle between to lines
 *			and distance between 2 parallel lines.
 *
 *	Line equation : \f[  A*x+B*y+C=0 \f]
 *
 */

class VISAPI VisLine
{

private:
	

	/** Line equation : A*x+B*y+C=0  */
	double a;
	double b;
	double c;
	
public:

	/** Default constructor */
	VisLine(); 

	/**  Constructor */ 
	VisLine::VisLine(double CoefX, double CoefY, double Offset); 
	   
	/* Constructor */
	VisLine(CvPoint p1, CvPoint p2);

	/* Constructor */
	VisLine(CvPoint2D32f p1, CvPoint2D32f p2);

        /* Constructor */
	VisLine(CvPoint p1, double angle);

	/** Destructor */
	~VisLine();        

	VisLine &operator=(const VisLine &line2); // overload of the operator '='

	/** Returns angle between 2 lines (in deg) */
	double GetAngle(VisLine line2);
	/** Returns intersection point of 2 lines */
	CvPoint2D32f GetIntersection(VisLine line2);
	/** Returns distance between 2 lines if they are parallel */
	double GetDistance(VisLine line2);
	/** Returns distance from a line to point */
	double GetDistance(CvPoint2D32f p);
	/** Shifts a line by a certain distance */
	void ShiftNormal(double dist);
	/** Rotate by a certain angle */
	void Rotate(double angle);
	/** Draw the line on an image */
	int Draw(IplImage* img, CvScalar color=cvRealScalar(255), int thickness=1);
	/** Returns the line equation*/
	CvPoint3D32f GetParamEquation(void);
		

};


/*! \class VisBlob VisBlob.hpp
 *  \brief Definition of the "VisBlob class" which contains the blob contour
 *			and its main features. This class must NOT be called by the user.
 *			It is an "internal" class called by the "VisBlobSet" class. You have to call
 *			this one to detect blobs in a binarised image.
 *
 *    Definition fo the "VisBlob class" where some blob features
 *	  are defined with the methods which enable to handle them.
 *	  This class must not be handled directly by the user. It is
 *	  internally used by the "VisBlobSet" class which is the one 
 *	  the user has to call in order to detect blobs in a binarised image
 *	  and calculate their features.
 *
 *	
 *	 \author $Author: produit $
 *   \version $Revision: 1.2 $
 *   \date $Date: 2006/10/13 12:32:55 $
 *
 */

class VISAPI VisBlob
{

	/** private members*/
	private:

		/** Pointer on the sequence of points which make the contour */
		CvSeq* contour;   

		/** Region of interest in the given image and where blobs are searched */
		IplROI roi;
		
		/** Area of the blob */
		double area;        

		/** Perimeter of the blob */
		double perimeter;   

		/** Center coordinates of the blob (whith a precision of 1 pixel) */
		CvPoint centre ;     
		/** Real center coordinates of the blob */
		CvPoint2D32f centre_real; 

		/** Real center coordinates of the minimum enclosing circle */
		CvPoint2D32f min_enclo_circle_centre; 
		/** Radius of the minimum enclosing circle */
		float min_enclo_circle_radius;
		
		/**  Minimum area circumscribed rectangle for the blob */
		CvBox2D equivalent_rect;

		/** compactness = 4*Pi*area/(perimeter*perimeter)  */
		/** --> compacness=1 for a circle */
		/** --> compactness=0 for highly elongated shape */ 
		double compactness;  

		/** Orientation of the blob (in deg) */
		double orientation;

		/** Eccentricity of the blob */
		double eccentricity;

		/** Major Axis Length of the equivalent ellipse of the blob */
		double ellipsemajoraxislength;

		/** Minor Axis Length of the equivalent ellipse of the blob */
		double ellipseminoraxislength;

		/** Major Axis Length of the blob */
		double majoraxislength;

		/** Minor Axis Length of the blob */
		double minoraxislength;

		/** Minor Axis of the blob */
		VisLine  minoraxis;

		/** Minor Axis of the blob */
		VisLine  majoraxis;

		/** Bounding rectangle of the blob */
		CvRect bounding_rect;

		/** Ellipse similarity factor */
		double ellipse_sim; 

		/** Sortkey use to sort blobs */
		double key; 

		/** Angle of the segment constituted by the blob center and a given point */
		double segment_angle;


	/**public members*/
	public:

		/** Default constructor */
		VisBlob(); 
		/** Constructor from a given contour */
		VisBlob(CvSeq* c,_IplROI* r=NULL); 

		/** Destructor */
		~VisBlob();        


		/** \brief Returns the angle of the segment constituted by the blob center and the given point  */
		double GetSegmentAngle( CvPoint2D32f point );

		/**\brief Sets blob eccentricity */
		void SetEccentricity(double eccent){ eccentricity=eccent;}
		void SetCompactness(double comp){ compactness=comp;}
		
		/** \brief Returns blob contour */
		CvSeq* GetContour(){return contour;}
		/** \brief Returns blob area */
		double GetArea(){return area;}
		/** \brief Returns blob perimeter */
		double GetPerimeter(){return perimeter;}
		/** \brief Returns blob center */
		CvPoint GetCenter(){return centre;}
		/** \brief Returns real blob center */
		CvPoint2D32f GetCentreReal(){return centre_real;}
		/** \brief Returns blob compactness */
		double GetCompactness(){return compactness;}
		/** \brief Returns blob orientation */
		double GetOrientation(){ return orientation; };
		/** \brief Returns real center coordinates of the minimum enclosing circle */
		CvPoint2D32f GetMinEncloCircleCenter(){ return min_enclo_circle_centre; };
		/** \brief Returns radius of the minimum enclosing circle */
		float GetMinEncloCircleRadius(){ return min_enclo_circle_radius; };
		/** \brief Returns minimum area circumscribed rectangle for the blob */
		CvBox2D GetEquivalentRect(){ return equivalent_rect; };
		/** \brief Returns blob eccentricity */
		double GetEccentricity(){return eccentricity;}
		/** \brief Returns major axis length of the equivalent ellipse of the blob */
		double GetEllipseMajorAxisLength(){return ellipsemajoraxislength;}
		/** \brief Returns minor axis length of the equivalent ellipse of the blob */
		double GetEllipseMinorAxisLength(){return ellipseminoraxislength;}
		/** \brief Returns blob minor axis */
		VisLine GetMinorAxis(){return minoraxis;}
		/** \brief Returns blob major axis */
		VisLine GetMajorAxis(){return majoraxis;}
		/** \brief Returns major axis length of the blob */
		double GetMajorAxisLength(){return majoraxislength;}
		/** \brief Returns minor axis length of the blob */
		double GetMinorAxisLength(){return minoraxislength;}

		/** \brief Returns ellipse similarity factor */
		double GetEllipseSim(){return ellipse_sim;}

		/** \brief Returns blob major axis */
		CvRect GetBoundingRectangle(){return bounding_rect;}


		/** \brief Sets the sortkey with the given feature value */
		void SetKey(double nkey){ key = nkey; }
		/** \brief Returns sortkey value */
		double GetKey(void){ return key; }

		/** Calculates blob area */
		void Calc_Area(void);
		/** Calculates blob perimeter */
		void Calc_Perimeter(void);
		/** Calculates blob center */
		void Calc_Centre(void);
		/** Calculates blob compactness */
		void Calc_Compactness(void);
		/** Calculates blob orientation */
		void Calc_Orientation(void);
		/** Calculates the minimum enclosing circle of the blob */
		void Calc_MinEncloCircle(void);
		/** Calculates the minimum area circumscribed rectangle for the blob */
		void Calc_EquivalentRect(void);
		/** Calculates blob eccentricity */
		void Calc_Eccentricity(void);
		/** Calculates major axis length of the equivalent ellipse of the blob */
		void Calc_EllipseMajorAxisLength(void);
		/** Calculates minor axis length of the equivalent ellipse of the blob */
		void Calc_EllipseMinorAxisLength(void);
		/** Calculates blob minor axis */
		void Calc_MinorAxis(void);
		/** Calculates blob major axis */
		void Calc_MajorAxis(void);
		/** Calculates major axis length of the blob */
		void Calc_MajorAxisLength(void);
		/** Calculates minor axis length of the blob */
		void Calc_MinorAxisLength(void);

		/** Calculates ellipse similarity factor */
		void Calc_EllipseSim(void);

		/** Calculates blob bounding rectangle */
		void Calc_BoundingRectangle(void);


		/** \brief Operator "<" to sort blobs in a descending order 
			The function "std::sort" is used to sort blobs. It sorts thanks to
 			the operator "<". That's why here its action has been reversed. So
 			blobs are sorted in descending order. This sort is based on the sortkey
 			value.*/
		bool operator<(VisBlob& a){return ( this->GetKey() > a.GetKey() );}


};

class VISAPI VisCoordinateFrame
{
private:
	float alpha;// around Z axis of VisCoordinateFrame parent [rad] 
	float beta;// around Y axis of VisCoordinateFrame parent [rad] (only for 3D)
	float gamma;// around X axis of VisCoordinateFrame parent [rad] (only for 3D)
	CvPoint3D32f origin;// in VisCoordinateFrame parent 
	CvMat* matToParent;	
	CvMat* matFromParent;
	CvMat* matSquare;// internal temporary usage
	CvMat* matCol;// internal temporary usage
	VisCoordinateFrame* parent;
#ifdef _USRDLL
		EXPIMP_TEMPLATE template class VISAPI std::allocator<VisCoordinateFrame*>;
		EXPIMP_TEMPLATE template class VISAPI std::vector<VisCoordinateFrame*, std::allocator<VisCoordinateFrame*>>;
#endif _USRDLL
	vector<VisCoordinateFrame*> children;

	void UpdateMats(void);
	void Init(void);
	
public:
	// constructors / destructor
	VisCoordinateFrame(CvPoint3D32f orig, float az, float ay, float ax, VisCoordinateFrame* vcf_parent=NULL);
	VisCoordinateFrame(CvPoint2D32f orig, float angle, VisCoordinateFrame* vcf_parent=NULL);
	VisCoordinateFrame(CvPoint2D32f orig, CvPoint2D32f dest, VisCoordinateFrame* vcf_parent=NULL);
	~VisCoordinateFrame();

	// set parameters
	void SetAngleAroundX(float angle);
	void SetAngleAroundY(float angle);
	void SetAngleAroundZ(float angle);
	void SetAngle(float angle){ SetAngleAroundZ(angle); };

	void SetOrigin(CvPoint3D32f orig);
	void SetOrigin(CvPoint2D32f orig){ SetOrigin(cvPoint3D32f(orig.x,orig.y,0)); };
	void SetOriginX(float orig_x);
	void SetOriginY(float orig_y);
	void SetOriginZ(float orig_z);

	// get parameters
	float GetAngleAroundX(void);
	float GetAngleAroundY(void);
	float GetAngleAroundZ(void);

	CvPoint3D32f GetOrigin(void);		
	CvPoint2D32f GetOrigin2D(void);
	
	CvMat* GetMatToParent(void);
	CvMat* GetMatFromParent(void);
	CvMat* GetMatToGlobal(void);
	CvMat* GetMatFromGlobal(void);
	CvMat* GetMatTo(VisCoordinateFrame* vcf);

	VisCoordinateFrame* GetParent(void);

	// transformation
	CvPoint3D32f Transform(CvPoint3D32f pt,
		VisCoordinateFrame* vcf = new VisCoordinateFrame(cvPoint3D32f(0,0,0),0,0,0));
	CvPoint2D32f Transform2D(CvPoint2D32f pt, 
		VisCoordinateFrame* vcf = new VisCoordinateFrame(cvPoint2D32f(0,0),0,0))
	{ return ProjectXY( Transform(cvPoint3D32f(pt.x, pt.y, 0), vcf) ); };
	
	// others
	void AddChild(VisCoordinateFrame* vcf);
	bool IsInPlaneXY(void);
	void Draw(IplImage* img);
};

class VISAPI VisGeomModel
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


#ifdef _USRDLL
	EXPIMP_TEMPLATE template class VISAPI std::allocator<VisGeomModel>;
	EXPIMP_TEMPLATE template class VISAPI std::vector<VisGeomModel,std::allocator<VisGeomModel>>;
	EXPIMP_TEMPLATE template class VISAPI std::allocator<VisBlob>;
	EXPIMP_TEMPLATE template class VISAPI std::vector<VisBlob,std::allocator<VisBlob>>;
#endif _USRDLL
/**
    
     The "VisBlobSet class" enables to detect blobs
	 in a binarised image, to gather them in a list,
	 to sort this list and to calculate their features.
	
		The binarised image can be defined with a Region Of Interest(ROI).
 	
 		It is possible indeed to select some rectangular part of the image or a certain color plane in the
 		image, and process only this part. The selected rectangle is called "Region of
 		Interest". The structure "IplImage" contains the field roi for this purpose. If the
 		pointer is not NULL, it points to the structure "IplROI" that contains parameters of selected
 		ROI, otherwise a whole image is considered selected.

 		If a ROI is defined, blobs are only searched in the ROI.

	 \author $Author: produit $
     \version $Revision: 1.2 $
     \date $Date: 2006/10/13 12:32:55 $


*/
class VISAPI VisBlobSet
{
	/** private members*/
	private:

			/** \brief Detected Blobs list */
			vector<VisBlob> blob_seq; 
			/** \brief Selected Blobs list (temporary) */
			vector<VisBlob> blob_temp; 
			/** \brief Memory storage for Blob contour */
			CvMemStorage* mem;
			/** \brief Sorting Blobs private method */
			int Sort(void);

			/** \brief Reference models and target points list */
			vector<VisGeomModel> models_seq;
			/** \brief Reference Blobs list (1 list for all reference models)  !!!!!!   ONLY ONE REFERENCE CAN BE MADE  FOR ONE VISBLOBSET  -> another list of reference blobs should be created for another reference  !!!!!!   */
			vector<VisBlob> ref_blob_seq;
			

			/** \brief Sort a vector of Blobs according to the angle of the segment between blob center and gravity center of all blobs */
			int SortByAngle(vector<VisBlob>* p_blob_list);



	/** public members*/
	public:
		/** \brief Constructor */
		VisBlobSet(); 

		/** \brief Destructor */
		~VisBlobSet();	

		/** \brief Number of reference Blobs list (Number of reference Blobs for each reference model) */
		vector<int> num_ref_blob_seq;

		/** \brief Accessor to the Reference Blobs list */
		vector<VisBlob>* GetRefBlobSeq(){ return(&ref_blob_seq); }

		/** \brief Accessor to the Reference models and target points list */
		vector<VisGeomModel>* GetModelsSeq(){ return(&models_seq); }

		/** \brief Clear Reference models list and reference blobs list */
		void ClearModelsList(void){ models_seq.clear(); ref_blob_seq.clear(); num_ref_blob_seq.clear(); }


		
		/** \brief Find all blobs in a binarised image */
		int FindBlobs(IplImage *imgS);

		/** \brief Returns number of selected blobs */
		int GetSelectedNumBlobs();

		/** \brief Returns total number of detected blobs */
		int GetTotalNumBlobs();
		
		/** \brief Empties the detected Blobs list */
		int EmptyDetectedBlobsList(void);

		/** \brief Returns the selected blob */
		//VisBlob GetBlob(int blobnum=0);
		

		/** \brief Sorts blobs in a descending order thanks to their area or
		 considering the distance between BlobArea and the given area*/
		int SortByArea(double area = 0);
		/** \brief Sorts blobs in a descending order thanks to their perimeter or
		 considering the distance between BlobPerimeter and the given perimeter*/
		int SortByPerimeter(double perimeter = 0);
		/** \brief Sorts blobs in a descending order thanks to their compactness or
		 considering the distance between BlobCompactness and the given compactness*/
		int SortByCompactness(double compactness = 0);
		/** \brief Sorts blobs in a descending order thanks to their eccentricity or
		 considering the distance between BlobEccentricity and the given eccentricity*/
		int SortByEccentricity(double eccentricity = 0);
		/** \brief Sorts blobs in a descending order thanks to their EllipseMajorAxisLength or
		 considering the distance between BlobMajorAxisLength and the given EllipseMajorAxisLength*/
		int SortByEllipseMajorAxisLength(double EllipseMajorAxisLength = 0);
		/** \brief Sorts blobs in a descending order thanks to their EllipseMinorAxisLength or
		 considering the distance between BlobMinorAxisLength and the given EllipseMinorAxisLength*/
		int SortByEllipseMinorAxisLength(double EllipseMinorAxisLength = 0);
		/** \brief Sorts blobs in a descending order thanks to their orientation or
		 considering the distance between BlobOrientation and the given orientation*/
		int SortByOrientation(double oriention = 0);
		/** \brief Sorts blobs in a ascending order considering the distance between blob center and the given CvPoint */
		int SortByPosition(CvPoint2D32f point);

		
		/** \brief Selects blobs whose area is within the given limits */
		int SelectByArea(double max , double min);
		/** \brief Selects blobs whose perimeter is within the given limits */
		int SelectByPerimeter(double max , double min);
		/** \brief Selects blobs whose compactness is within the given limits */
		int SelectByCompactness(double max , double min);
		/** \brief Selects blobs whose eccentricity is within the given limits */
		int SelectByEccentricity(double max , double min);
		/** \brief Selects blobs whose Ellipse Major Axis Length is within the given limits */
		int SelectByEllipseMajorAxisLength(double max , double min);
		/** \brief Selects blobs whose Ellipse Minor Axis Length is within the given limits */
		int SelectByEllipseMinorAxisLength(double max , double min);
		/** \brief Selects blobs whose orientation is within the given limits */
		int SelectByOrientation(double max , double min);
		/** \brief Selects all the detected blobs */
		int SelectAll(void);
		/** \brief Selects only the "blobnum"th blob of the selected blobs list */
		int Select(int blobnum_start = 0, int blobnum_end = -1);
		/** \brief Erases only the "blobnum"th blob of the selected blobs list */
		int Erase(int blobnum = 0);

		/** \brief Sorts blobs in a descending order according to their similarity 
 			with the first selected blob of the given BlobSet */
		int BlobFitting(VisBlobSet* pblobset, double area_variance=0);
		
		/** \brief Calculates all the features of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateFeatures(void);
		/** \brief Calculates area of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateAreas(void);
		/** \brief Calculates perimeter of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculatePerimeters(void);
		/** \brief Calculates compactness of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateCompactness(void);
		/** \brief Calculates center of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateCenters(void);
		/** \brief Calculates orientation (in deg) of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateOrientations(void);
		/** \brief Calculates eccentricity of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateEccentricities(void);
		/** \brief Calculates "equivalent" Ellipse Major Axis Length of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateEllipseMajorAxisLengths(void);
		/** \brief Calculates "equivalent" Ellipse Minor Axis Length of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateEllipseMinorAxisLengths(void);
		/** \brief Calculates Major Axis of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateMajorAxis(void);
		/** \brief Calculates Minor Axis of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateMinorAxis(void);
		/** \brief Calculates Major Axis length of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateMajorAxisLength(void);
		/** \brief Calculates Minor Axis length of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateMinorAxisLength(void);
		/** \brief Calculates ellipse similarity factor of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateEllipseSimFacts(void);
		

		/** \brief Calculates the minimum enclosing circle of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateMinimumEnclosingCircle(void);
		/** \brief Calculates the minimum area circumscribed rectangle of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateEquivalentRectangle(void);

		/** \brief Calculates the bounding rectangle of the selected blobs (By defect, all the detected blobs are selected) */
		int CalculateBoundingRectangle(void);
		

		/** \brief Returns area of the "blobnum"th blob among selected blobs */
		double GetArea(int blobnum = 0);
		/** \brief Returns perimeter of the "blobnum"th blob among selected blobs */
		double GetPerimeter(int blobnum = 0);
		/** \brief Returns compactness of the "blobnum"th blob among selected blobs */
		double GetCompactness(int blobnum = 0);
		/** \brief Returns ellipse similarity factor of the "blobnum"th blob among selected blobs */
		double GetEllipseSim(int blobnum = 0);
		/** \brief Returns center (with a precision of 1 pixel)  of the "blobnum"th blob among selected blobs */
		CvPoint GetCenter(int blobnum = 0);
		/** \brief Returns real center of the "blobnum"th blob among selected blobs */
		CvPoint2D32f GetCenterReal(int blobnum = 0);
		/** \brief Returns orientation (in deg) of the "blobnum"th blob among selected blobs */
		double GetOrientation(int blobnum = 0);
		/** \brief Returns eccentricity of the "blobnum"th blob among selected blobs */
		double GetEccentricity(int blobnum = 0);
		/** \brief Returns major axis length of the equivalent ellipse of the "blobnum"th blob among selected blobs */
		double GetEllipseMajorAxisLength(int blobnum = 0);
		/** \brief Returns minor axis length of the equivalent ellipse of the "blobnum"th blob among selected blobs */
		double GetEllipseMinorAxisLength(int blobnum = 0);


		/** \brief Returns the radius of the minimum enclosing circle of the "blobnum"th blob among selected blobs */
		float GetMinimumEnclosingCircleRadius(int blobnum = 0);
		/** \brief Returns the real center of the minimum enclosing circle of the "blobnum"th blob among selected blobs */
		CvPoint2D32f GetMinimumEnclosingCircleCenter(int blobnum = 0);
		/** \brief Draws the minimum enclosing circle of the selected blobs in the given destination image */
		int DrawMinimumEnclosingCircle( IplImage *dstimg=0, CvScalar color=cvRealScalar(255), int thickness=1);
		/** \brief Returns the minimum area circumscribed rectangle for the "blobnum"th blob among selected blobs */
		CvBox2D GetEquivalentRectangle(int blobnum = 0);
		/** \brief Draws the "equivalent" rectangle of the selected blobs in the given destination image */
		int DrawEquivalentRectangle( IplImage *dstimg=0, CvScalar color=cvRealScalar(255), int thickness=1);

		/** \brief Draws the bounding rectangle of the selected blobs in the given destination image */
		int DrawBoundingRectangle( IplImage *dstimg=0, CvScalar color=cvRealScalar(255), int thickness=1);


		/** \brief Returns major axis of the "blobnum"th blob among selected blobs */
		VisLine GetMajorAxis(int blobnum = 0);
		/** \brief Returns major axis of the "blobnum"th blob among selected blobs */
		VisLine GetMinorAxis(int blobnum = 0);
		/** \brief Returns major axis length of the "blobnum"th blob among selected blobs */
		double GetMajorAxisLength(int blobnum = 0);
		/** \brief Returns minor axis length of the "blobnum"th blob among selected blobs */
		double GetMinorAxisLength(int blobnum = 0);

		/** Returns Bounding Rectangle of the "blobnum"th blob among selected blobs */
		CvRect GetBoundingRectangle(int blobnum = 0);
		
		/** Returns an int indicating if the blob is near from an elllipse. */
		int GetEllipse_Similarity(int blobnum = 0);

		/** \brief Draws contours of the selected blobs in the given destination image  */
		int DrawContours(IplImage *dstimg, CvScalar external_color, CvScalar hole_color, int max_level=0, int thickness=1, int connectivity=8);

		/** \brief Draws a cross of the center of the "blobnum"th blob among selected blobs in the given destination image */
		int DisplayCross(IplImage *dstimg, CvScalar color, int thickness, int size, int blobnum = 0);


		/** \brief Sets the reference blobs list by taking the "blobnum"th blob among selected blobs as a reference*/
		int LoadReferenceBlob(int blobnum = 0);

		/** \brief Learns the Reference from the given point and the reference blobs list   !!!!!!   ONLY ONE REFERENCE CAN BE MADE  FOR ONE VISBLOBSET  -> another list of reference blobs should be created for another reference  !!!!!! */
		int LearningReference( CvPoint2D32f target , double* p_cond_number=NULL);

		/** \brief Calculates the target point from the learnt reference created with the given BlobSet where blobs have been searched in a new image */
		CvPoint2D32f GetReference( VisBlobSet* pblobset , int num_ref=0 , double area_variance=0 , double* p_cond_number=NULL, double* p_error=NULL);

		/** \brief Returns the angle of the segment constituted by the "blobnum"th blob center and the given point  */
		double GetSegmentAngle( CvPoint2D32f point, int blobnum = 0 );

		/** \brief Returns the list of blobs containing the given point  */
		int IsPointInsideBlob(CvPoint point, vector<int>* p_blob_list);

		/** \brief Return a pointer to the contour */
		CvSeq* GetContour(int blobnum = 0);

		/** \brief Returns the Minimal Area Bounding Box (every orientation) for the "blobnum"th blob  */
		CvBox2D GetMinBoundingBox(int blobnum = 0);
};


typedef int (VisBlobSet ::* p_blobset_meth) ();
typedef double (VisBlobSet ::* p_blobset_meth_1) ( int);
typedef int (VisBlobSet ::* p_blobset_meth_2) (double);
typedef int (VisBlobSet ::* p_blobset_meth_3) ( double, double);


/*! \class VisBinarisation VisBinarisation.hpp
 *  \brief Definition of the VisBinarisation class which enables to binarise
 *			gray scale images simply or with optimization.
 *
 *	The optimized binarisation is used to find the threshold giving
 *	the best value for the chosen blob feature. 
 *
 *	
 *	 \author $Author: produit $
 *   \version $Revision: 1.2 $
 *   \date $Date: 2006/10/13 12:32:55 $
 *
 */

class VISAPI VisBinarisation
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
