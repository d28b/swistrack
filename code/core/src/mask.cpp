#include "mask.h"


/** \brief Constructor
*
* Loads the mask image from disk and finds all contours in it. It also creates the output file and stores center and size of each contour found
*/
Mask::Mask(){

   try{
	display=0;

     
   
	maskimg=cvLoadImage(maskfname,0);
	//cvFlip(maskimg,maskimg);
	masks.EmptyDetectedBlobsList();			// clear the blob list,
	masks.FindBlobs(maskimg);				// and get the new ones,
	masks.CalculateFeatures();              // calculate all blob features

	nrmasks=masks.GetTotalNumBlobs();
	
    if(strcmp(outputfname,"")!=0){
        fopen_s(&output,outputfname,"w+");
        }
    else{
        output=0;
        }

	for(int i=0; i<nrmasks; i++){
		CvPoint center = masks.GetCenter(i);
		if(output) fprintf(output,"%d: CENTER (%d/%d), AREA (%f)\n",i,center.x,center.y,masks.GetArea(i));
	//	printf("%d: CENTER (%d/%d), AREA (%f)\n",i,center.x,center.y,masks.GetArea(i));
		
		areas.push_back(center);
		nrobjects.push_back(0);
		}
       }
   catch (...) {
       throw "[Core] Mask::Mask";
       }
   }

/** \brief Destructor
* Closes output file
*/
Mask::~Mask(){
    try{
	if(output) fclose(output);
        }
    catch (...) {
        throw "[Core] Mask::~Mask";
        }
	}

/** \brief Counts the number of trajectories which end point is in either of the mask contours
*
*/
void Mask::UpdateCount(vector<Track>* targets, int frame){

    try{
    if(nrmasks){
		int* count = new int[nrmasks];
		int  i,j;

		for(i=0; i<nrmasks; i++) count[i]=0; // Initialize counter

		for(i=0; i<(int)(targets->size()); i++){  // Go through all trajectories
			
			vector<int> p_blob_list;

			if(masks.IsPointInsideBlob(cvPointFrom32f(targets->at(i).trajectory.back()),&p_blob_list)!=-1){  // returns all contours that contain the point
				for(j=0; j<(int)(p_blob_list.size()); j++){
						count[p_blob_list.at(j)]++;
					}
				
				}
			
			}

	//	printf("%d: ",frame);
         if(output) fprintf(output,"%d: ",frame);
		 for(i=0; i<nrmasks; i++){
			if(output) fprintf(output,"%d ",count[i]);
			//printf("%d ",count[i]);
			nrobjects.at(i)=count[i];
			}
		 if(output) fprintf(output,"\n");
      
		//printf("\n");

		delete(count);
	    }
        }
    catch (...){
        throw "[Core] Mask::UpdateCount";
        }
	}

/** \brief Returns the number of contours found in the mask image
* \return Number of contours found
*/
int Mask::GetNrOfMasks()
{
	return(nrmasks);
}

/** \brief Draws all contours from the mask image and the number of objects within each contour on the progress image
*
* \param trackingimg: pointer ot the TrackingImage class that keeps the progress images
*/
void Mask::DrawMask(TrackingImage *trackingimg)
{
    try{
	if(display){
	trackingimg->DrawContours(&masks);
	for(int i=0; i<nrmasks; i++){
		char counter[5];
	
		sprintf_s(counter,"%d",nrobjects.at(i));
		trackingimg->Text(counter,areas.at(i),CV_RGB(255,0,0));
		}
	}
        }
    catch (...) {
        throw "[Core] Mask::DrawMask";
        }
}

/** \brief Toggles displaying of the mask on the progress image
*
*/

void Mask::ToggleDisplay()
{
	display=!display;
}
