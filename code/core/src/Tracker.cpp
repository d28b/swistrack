// tracker.cpp: implementation of the tracker class.
//
//////////////////////////////////////////////////////////////////////

#include "Tracker.h"

/** \file tracker.cpp
    \brief Core tracking functionalities
*/

/** \class Tracker
*	\brief Core tracking functionalities
*  
*   The tracker class maintains the trajectories of the objects to track and provides
*	the core data association algoritm for associating particles from the segmenter class
*	to the object trajectories.
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Tracker::Tracker(Calibration* parent, xmlpp::Element* cfgRoot, TrackingImage* trackingimg) 
	{
	this->cfgRoot=cfgRoot;
    this->parent=parent;
    this->trackingimg=trackingimg;
	
	mask=0;

    if(!IsAttrByXPath(cfgRoot,"/CFG/COMPONENTS/TRACKER","mode"))
        throw "[Tracker::Tracker] Tracker mode undefined (/CFG/COMPONENTS/TRACKER)";

    mode=GetIntAttrByXPath(cfgRoot,"/CFG/COMPONENTS/TRACKER","mode");
    
    switch(mode){
        case 0 : /////////////// Nearest neighbor //////////////////////////
            CreateExceptionIfEmpty(cfgRoot,"/CFG/TRACKER[@mode='0']");
            CreateExceptionIfEmpty(cfgRoot,"/CFG/TRACKER[@mode='0']/MAXSPEED");				
			CreateExceptionIfEmpty(cfgRoot,"/CFG/TRACKER[@mode='0']/NOBJECTS");				
           break;
        default : throw "[Tracker::Tracker] tracker mode not implemented";
        };

	
    style = TRAJFULL;  /** \todo this should go into trackingimage */
	
    particlefilter = new ParticleFilter(cfgRoot,trackingimg);	
	status = particlefilter->GetStatus();
	
	SetParameters(); // this function updates all variable parameters and can also be called from the GUI

	for (int i=0;i<nr_objects;i++)          // initiate nr_objects Track classes
		{
		sharedage.push_back(0);
		oldshared.push_back(0);
		restingtraj.push_back(0);
		targets.push_back(Track(i,					// id number
			trackingimg,nr_objects));
		}
	
/*	if(status==READY_TO_START){
		status=particlefilter->init(100); // initializes segmenter
		}*/
	}

Tracker::~Tracker()
	{
   	if(particlefilter){
		delete(particlefilter);
		particlefilter=NULL;
		}
	if(targets.size()) targets.clear();
	if(mask) delete(mask);
	}

int Tracker::GetStatus()
	{
	return(status);
	}


void Tracker::AddCompetitor(int c){
				int exist=0;
				for(vector<int>::iterator it=competitors.begin(); it != competitors.end(); it++)
					if(*it == c)
						exist=1;
					if(!exist)
						competitors.push_back(c);
				}

void Tracker::AddLostItem(int c){
				int exist=0;
				for(vector<int>::iterator it=lostitems.begin(); it != lostitems.end(); it++)
					if(*it == c)
						exist=1;
					if(!exist)
						lostitems.push_back(c);
				}

/** \brief Core Tracking Function
*
*  DataAssociation is the core tracking function. 
* For every object, we find a particle that is "closest" to its last known position.
* Closest is hereby given by a cost function (GetCost), which is just the distance
* in euclidian space in a minimalist implementation.
* Particles considered as being to costly (for instance because the distance is much 
* higher than the objects maximum speed), are rejected. Then, the trajectory of these 
* objects remain unchanged.
* If there are two objects both chosing the same particle, both objects are marked
* as competitiors.
*/



CvMat* ReduceMatrix(CvMat* matrix,int row, int col)
	{
	int nrows, ncols;

	nrows=matrix->rows;
	ncols=matrix->cols;


	if( nrows-(row>-1)<1  || ncols-(col>-1) < 1)
		{
			cvReleaseMat(&matrix);
			return(0);
		}

	CvMat* rmatrix=cvCreateMat(nrows-(row>-1),ncols-(col>-1),CV_64FC1);

	if(col==-1) col=ncols+1;
	if(row==-1) row=nrows+1;

		for(int i=0; i<nrows; i++)
			{
			for(int j=0; j<ncols; j++)
				{
					if(i!=row && j!=col)
						cvmSet(rmatrix,i-(i>row),j-(j>col),cvmGet(matrix,i,j));
				}
			}

	cvReleaseMat(&matrix);
	return(rmatrix);
	}

int ReduceIndexedMatrix(CvMat** matrix, CvMat** row_index, CvMat** col_index, int row, int col)
	{
		*matrix = ReduceMatrix(*matrix, row, col);
		if(row!=-1) *row_index = ReduceMatrix(*row_index, row, -1);
		if(col!=-1) *col_index = ReduceMatrix(*col_index, col, -1);
		return(1);
	}


void ShowMatrix(CvMat* matrix)
	{
	if(matrix){
	int rrows=matrix->rows;
	int rcols=matrix->cols;

	printf("(%d/%d):\n",rrows,rcols);
		for(int row=0; row<rrows; row++){
			for(int col=0; col<rcols; col++){
				printf("%0.2f ",cvmGet(matrix,row,col));
				}
			printf("\n");
		}
	}
	else
		printf("(0x0)\n -empty matrix\n");
	}

int GetIndex(CvMat* imat,int v)
	{
		return((int) cvmGet(imat,v,0));
	}

int GetNumberOfCandidateSolutionsInRow(CvMat* matrix, int row, int* uniqueindex=0){
	
	int rcols = matrix->cols;
	int nentries=0;
	
	for(int col=0; col<rcols; col++)
		{
		if(cvmGet(matrix,row,col)!=-1)
			{
			nentries++;
			if(uniqueindex) *uniqueindex=col;
			}
		}
	return(nentries);
	}

int GetNumberOfCandidateSolutionsInCol(CvMat* matrix, int col, int* uniqueindex=0){
	
	int rrows = matrix->rows;
	int nentries=0;
	
	for(int row=0; row<rrows; row++)
		{
		if(cvmGet(matrix,row,col)!=-1)
			{
			nentries++;
			if(uniqueindex) *uniqueindex=row;
			}
		}
	return(nentries);
	}


int IsInVector(int t,vector<int>* test)
	{
	for(vector<int>::iterator i=test->begin(); i !=test->end(); i++)
		if(*i==t) return(1);
	return(0);
	}

void FindBestAssociationOverdefined(CvMat* matrix,vector<int>* min_cost_path, vector<int>* tabulist, double* min_cost, double* act_cost,int N,int depth=0)
	{
	
	//vector<int> first_col;
	if(depth==0)  *act_cost=0;
	depth++;
	for(int row=0; row<matrix->rows; row++)
		{
		if( (!IsInVector(row,tabulist)))
			{
			double cost=cvmGet(matrix,row,0);
			if(cost!=-1){
			//	if(depth==1) printf("\n");
				//printf("%d:%0.2f ",depth,cost);
				tabulist->push_back(row);
				*act_cost+=cost;
				CvRect rect;
				rect.x=1;
				rect.y=0;
				rect.width=matrix->cols-1;
				rect.height=matrix->rows;
				if(depth<N){
					CvMat subrect;
					cvGetSubRect(matrix,&subrect,rect);
					FindBestAssociationOverdefined(&subrect,min_cost_path,tabulist,min_cost,act_cost,N,depth);
					}	
				else{
					//printf("=###%0.2f###  (mincost=%0.2f)\n",*act_cost,*min_cost);
					if(*act_cost<*min_cost){
						*min_cost=*act_cost;
						min_cost_path->clear();
						for(vector<int>::iterator i=tabulist->begin(); i !=tabulist->end(); i++) min_cost_path->push_back(*i);
						}
					}
				tabulist->pop_back();
				*act_cost-=cost;
				}
			}
		//else{printf("[sink]\n"); *act_cost=100000;}
		}
	}


void FindBestAssociation(CvMat* matrix,vector<int>* min_cost_path, vector<int>* tabulist, double* min_cost, double* act_cost,int N,int maximum_double=0,int depth=0)
	{
	
	int max_double = N - matrix->rows;  // the maximum number of double assignments is bound by the difference in particles and tracks
	if(maximum_double>max_double) max_double=maximum_double;

	int cur_double = 0;

	if(depth==0)  {
		*act_cost=0; 
		}
	depth++;
	for(int row=0; row<matrix->rows; row++)
		{
		if( !IsInVector(row,tabulist) || cur_double<max_double)
			{

			if(IsInVector(row,tabulist)) cur_double++;

			double cost=cvmGet(matrix,row,0);
			if(cost!=-1){
				if(depth==1) printf("\n");
				//printf("%d:%0.2f ",depth,cost);
				tabulist->push_back(row);
				*act_cost+=cost;
				CvRect rect;
				rect.x=1;
				rect.y=0;
				rect.width=matrix->cols-1;
				rect.height=matrix->rows;
				if(depth<N){
					CvMat subrect;
					cvGetSubRect(matrix,&subrect,rect);
					FindBestAssociation(&subrect,min_cost_path,tabulist,min_cost,act_cost,N,maximum_double,depth);
					}	
				else{
				//	printf("=###%0.2f###  (mincost=%0.2f)\n",*act_cost,*min_cost);
					if(*act_cost<*min_cost){
						*min_cost=*act_cost;
						min_cost_path->clear();
						for(vector<int>::iterator i=tabulist->begin(); i !=tabulist->end(); i++) min_cost_path->push_back(*i);
						}
					}
				tabulist->pop_back();
				*act_cost-=cost;
				}
			}
		}
	}

/*void Tracker::DataAssociation()
	{
		int fp;
		int N;
		int i;


	
		fp=particles->size();    // Number of particles
		N=targets.size();		// Number of trajectories

		CvMat* row_index = cvCreateMat(fp,1,CV_64FC1);
		CvMat* col_index = cvCreateMat(N,1,CV_64FC1);

		for(i=0; i<fp; i++) cvmSet(row_index,i,0,(double) i);
		for(i=0; i<N; i++)  cvmSet(col_index,i,0,(double) i);

		CvMat* matrix = cvCreateMat(fp,N,CV_64FC1);
		int row=0;
		int col=0;

		double max_speed_square = max_speed * max_speed;

		for(row=0; row<particles->size(); row++){
			for(col=0; col<targets.size(); col++){
				
				// Calculate cost for each particle/trajectory pair
				double cost = GetCost(col,particles->at(row).p);
				if(cost < max_speed_square)
					{
						cvmSet(matrix,row,col,sqrt(cost));
					}
				else
					{
						cvmSet(matrix,row,col,-1);
					}
				}
			}
	
	
		// matrix holds now all cost for all possible pairings. Unfeasible pairings are marked as -1 (infinite cost)
	
		int reducable=1;


//		while(reducable && matrix){
//			reducable=0;

		// 1. proceed row-wise and search for rows with a single entry (unique match)			
		while(reducable && matrix) // reduce matrix as long as possible
			{
			reducable=0;
			int rrows=matrix->rows;
			int rcols=matrix->cols;
			
			
			for(row=0; row<rrows; row++) 
				{
				int uniquecol=-1;
				int nentries=GetNumberOfCandidateSolutionsInRow(matrix,row,&uniquecol);
				
				if(nentries==1 && uniquecol!=-1) // for a single entry, check if this is the only entry in this column
					{
						nentries=GetNumberOfCandidateSolutionsInCol(matrix,uniquecol);
						if(nentries==1)
							{
								AddPoint(GetIndex(col_index,uniquecol),particles->at(GetIndex(row_index,row)).p);
								reducable=ReduceIndexedMatrix(&matrix,&row_index,&col_index,row,uniquecol);
								break;
							}
					}
				else if(nentries==0) // particle is not associable at all
					{
						reducable=ReduceIndexedMatrix(&matrix,&row_index,&col_index,row,-1);
						break;
					}
				}
			//ShowMatrix(col_index);
			//ShowMatrix(matrix);
			}


		int emptycolumnfound=0;
		// 2. Check for empty columns (no particle available for a track)
		reducable=1;
		while(matrix && reducable)
			{
			reducable=0;
			int rrows=matrix->rows;
			int rcols=matrix->cols;

			for(col=0; col<rcols; col++)
				{
				  int uniquerow=-1;
				  int nentries=GetNumberOfCandidateSolutionsInCol(matrix,col,&uniquerow);

				  if(nentries==0) // trajectory 'col' is not associable at all
					  {
						emptycolumnfound++;
						reducable=ReduceIndexedMatrix(&matrix,&row_index,&col_index,-1,col);
						break;
					  }
//				  else if(nentries==1 && uniquerow!=-1) // 'col' has only a single trajectory to choose from
//					  {
//					  AddPoint(GetIndex(col_index,col),particles->at(GetIndex(row_index,uniquerow)).p);
//					  reducable=ReduceIndexedMatrix(&matrix,&row_index,&col_index,-1,col);
//					  break;
//					  }
				}
			}
//		}

	
//		printf("Fully reduced system (removed: unique matches, no matches)\n");
//		ShowMatrix(col_index);
//		ShowMatrix(matrix);


		vector<int> min_cost_path;
		vector<int> tabulist;
		double min_cost=10000000;
		double act_cost=0;

			

		if(matrix && matrix->rows>=matrix->cols && !emptycolumnfound){  // if the system is overdefined, i.e. more particles than trajectories
			FindBestAssociationOverdefined(matrix,&min_cost_path,&tabulist,&min_cost,&act_cost,matrix->cols);
			}
		else{
		//	FindBestAssociation(matrix,&min_cost_path,&tabulist,&min_cost,&act_cost,matrix->cols,emptycolumnfound);
			}

		competitors.clear(); // erase the list of competitors;
		


		if(min_cost_path.size()!=N){
			printf("A minimum cost path could not be established, brute force\n");
			for(int id=0;id<nr_objects;id++)   // loop trough all trajectories
				{
				double min_dist=1000000;
				vector<particle>::iterator min_dist_id;
		
				for(vector<particle>::iterator p=particles->begin(); // find the "closest" particle for object 'id'
				p != particles->end();
				p++){
					double dist = GetCost(id,p->p);
					//	printf("%f %f %f\n",p->p.x,p->p.y,dist);
					if(dist < min_dist) 
						{
						min_dist = dist;
						min_dist_id = p; // save pointer to particle
						}
					}
				
				// the closest particle 'min_dist_id' has distance 'min_dist' now
				
				if(sqrt(min_dist) < max_speed){ // if good enough (threshold) take it, otherwise reject
					if(min_dist_id->id==-1){ // if particle is not associated yet
						min_dist_id->id=id; // associate best particle with object id
						
						AddPoint(id,min_dist_id->p);
						}
					else{  // otherwise just take this point and add id to the list of competitors
						
						AddPoint(id,min_dist_id->p);
						if(manual_mode)	AddCompetitor(min_dist_id->id);
						AddCompetitor(id);
						}
					}
				} // end for each id 1
			}
		else{

		printf("Association according to minimum cost path\n");
		for(i=0; i<matrix->cols; i++){
			
			if(particles->at(GetIndex(row_index,min_cost_path.at(i))).id==-1){ // if particle is not associated yet
				particles->at(GetIndex(row_index,min_cost_path.at(i))).id=GetIndex(col_index,i); // associate best particle with object id				
				}
			else{  // otherwise just take this point and add id to the list of competitors
				
				if(manual_mode)	AddCompetitor(particles->at(GetIndex(row_index,min_cost_path.at(i))).id);
				AddCompetitor(GetIndex(col_index,i));
				}		
			AddPoint(GetIndex(col_index,i),particles->at(GetIndex(row_index,min_cost_path.at(i))).p);
			}
		
			}

/*		printf("Cheapest path:\n");
		for(vector<int>::iterator it=min_cost_path.begin(); it != min_cost_path.end(); it++){
			printf("%d ",*it);
			}
		printf("\n");
*/
/*	
				// At this point, two or more trajectories can share one particle. However, all 
		// trajectories that join an already associated particle are kept as competitors
	
	// Now, check which particles have not been associated
	int ap;		// free particles, associated particles	
	
	FindFreeParticles(&fp,&ap);
	
//	printf("Number of free/associated particles: %d/%d\n",fp,ap);
	
	if(fp && ap <nr_objects){ // if there are unassociated particles and less associated particles than objects
		if(!competitors.empty()){
			if(manual_mode){	
				
				
				for(vector<int>::iterator it=competitors.begin(); it != competitors.end(); it++)
					targets.at(*it).DrawTrajectory(style);
				
				for(it=competitors.begin(); it != competitors.end(); it++){
					
					CvPoint m = parent->otGetUserEstimateFor(*it); // this function should ask the user for his estimate
					
					targets.at(*it).trajectory.pop_back();
					targets.at(*it).trajectory.push_back(cvPoint2D32f(m.x,m.y)); // exchange last point by user input
					
					}
				}
			else{
				
				CleanParticles(); // delete all assigned particles from the particle list			
				AssociateParticlesToCompetitors();
				}
			}  // end if !competitors.empty
		} // end if fp && ap<num_objects
	else{
		if(manual_mode){ // if we are in manual mode, we take care of the drawing here
			for(int i=0;i<nr_objects; i++)
				targets.at(i).DrawTrajectory(style);
			}
		}


    //	cvWaitKey(1000);
		if(matrix) cvReleaseMat(&matrix);
		if(row_index) cvReleaseMat(&row_index);
		if(col_index) cvReleaseMat(&col_index);
	}
*/
void Tracker::DataAssociation()
	{
	
	int id;
	
	competitors.clear(); // erase the list of competitors;
	for(id=0;id<nr_objects;id++)   // loop trough all trajectories
		{
		double min_dist=1000000;
		vector<particle>::iterator min_dist_id;
		
		
		for(vector<particle>::iterator p=particles->begin(); // find the "closest" particle for object 'id'
		p != particles->end();
		p++){
			double dist = GetCost(id,p->p);
			//	printf("%f %f %f\n",p->p.x,p->p.y,dist);
			if(dist < min_dist) 
				{
				min_dist = dist;
				min_dist_id = p; // save pointer to particle
				}
			}
		//	int t; scanf("%d",&t);
		
		// the closest particle 'min_dist_id' has distance 'min_dist' now
		
		if(sqrt(min_dist) < max_speed){ // if good enough (threshold) take it, otherwise reject
			if(min_dist_id->id==-1){ // if particle is not associated yet
				min_dist_id->id=id; // associate best particle with object id
				
				AddPoint(id,min_dist_id->p);
				}
			else{  // otherwise just take this point and add id to the list of competitors
				
				AddPoint(id,min_dist_id->p);
				AddCompetitor(id);
				}
			}
		} // end for each id 1
	
		// At this point, two or more trajectories can share one particle. However, all 
		// trajectories that join an already associated particle are kept as competitors
	
	// Now, check which particles have not been associated
	int fp,ap;		// free particles, associated particles	
	
	FindFreeParticles(&fp,&ap);
	
	//printf("Number of free/associated particles: %d/%d\n",fp,ap);
	
	if(fp && ap <nr_objects){ // if there are unassociated particles and less associated particles than objects
		if(!competitors.empty()){
			/*if(manual_mode){	
				
				
				for(vector<int>::iterator it=competitors.begin(); it != competitors.end(); it++)
					targets.at(*it).DrawTrajectory(style);
				
				for(it=competitors.begin(); it != competitors.end(); it++){
					
					CvPoint m = parent->GetUserEstimateFor(*it); // this function should ask the user for his estimate
					
					targets.at(*it).trajectory.pop_back();
					targets.at(*it).trajectory.push_back(cvPoint2D32f(m.x,m.y)); // exchange last point by user input
					
					}
				}
			else{*/
				
				CleanParticles(); // delete all assigned particles from the particle list			
				AssociateParticlesToCompetitors(max_speed);
			//	}
			}  // end if !competitors.empty
		} // end if fp && ap<num_objects
	avg_speed = max_speed/2;

	///////////////////// Determine number and age of shared trajectories //////////////////
	
	vector<int> shared;
	int nrshared = CountSharedTrajectories(&shared);
	
	int maxageexceeded=-1;

	if(nrshared==0){
		for(int i=0; i<nr_objects; i++) sharedage.at(i)=0;
		}
	else{
		for(int i=0; i<nr_objects; i++){
			if(shared.at(i)){ 
				sharedage.at(i)++;
				if(GetCost(i,*GetCritPoint(i))>avg_speed*avg_speed) maxageexceeded=i;
				}
			else
				sharedage.at(i)=0;
				SetCritPoint(i); // remember the actual point as 'last-known-good'
			}
		}


	///////////////////// Determine number and age of resting trajectories ////////////////

	//int nrresting = GetRestingTrajectories(&restingtraj);


	///////////////////////////// Create new trajectories for noise  //////////////////////
	
	CleanParticles();
//	printf("We have now %d particles to assign, and have already %d noise trajectories\n",particles->size(),ptargets.size());
    vector<particle>::iterator p;
	for(p=particles->begin();p != particles->end();p++) // assign particles to existing noise trajectories
		{
		double min_dist=1000000;
		vector<Track>::iterator min_dist_id=ptargets.begin();
		
		
		for(vector<Track>::iterator t=ptargets.begin(); t!=ptargets.end(); t++)
			{
			double dist = GetDist(&(t->trajectory.back()),&(p->p));
	//		printf("%0.2f %0.2f %0.2f %0.2f %0.2f\n",p->p.x,p->p.y,t->trajectory.back().x,t->trajectory.back().y,dist);
			if(dist < min_dist) 
				{
				min_dist = dist;
				min_dist_id = t; // save pointer to closest track
				}
			}
		
		// the closest track 'min_dist_id' has distance 'min_dist' now
		
		if(sqrt(min_dist) < max_speed && p->id==-1){ // if good enough (threshold) take it, otherwise reject
			p->id=min_dist_id->id; // associate best particle with object id
			min_dist_id->AddPoint(p->p);
			}
		else{
			if(min_dist_id!=ptargets.begin()) ptargets.erase(min_dist_id); // erase noise trajectories that did not find anyone
			}
		} // end for each id 1
	
	id=ptargets.size();
	for(p=particles->begin();p != particles->end();p++){ // create new noise trajectories
		if(p->id==-1){
//			printf("Create new noise trajectory (%d)\n",id);
			//Track* tmpTrack = new Track(id,trackingimg,nr_objects);
			ptargets.push_back(Track(id,trackingimg,nr_objects));
			//delete tmpTrack;
			ptargets.back().AddPoint(p->p);
			p->id=id;
			id++;
		}
	}	
	
	int found=0;
	while(!found){ // this loop goes until all particles/trajectories are known
		found=1;
		for(vector<Track>::iterator t=ptargets.begin(); t!=ptargets.end(); t++){ // delete noise trajectories that are not assigned
			found=0;
			for(p=particles->begin();p != particles->end();p++) if(p->id==t->id) found=1;
			if(!found){ ptargets.erase(t); break; }
		}
	}

    ///////////// If a noise trajectory seems to be reasonable, swap with the closest shared trajectory ///////////////////

	found=0;
	for(vector<Track>::iterator t=ptargets.begin(); t!=ptargets.end(); t++){
		double dist = GetDist(&t->trajectory.front(),&t->trajectory.back());
		if(dist > avg_speed * avg_speed && t->trajectory.size()>5){
//			printf("Found promising noise trajectory (%0.2f)\n",sqrt(dist));
// A promising trajectory must have a certain length + certain age to avoid to track on noise bursts			
			double min_dist=1000000;
			int min_dist_id=-1;
			
			for(id=0; id<nr_objects; id++){
				if(shared.at(id)){
					double dist=GetDist(GetPos(id),&t->trajectory.back());
					if(dist < min_dist){
						min_dist=dist;
						min_dist_id=id;
						}
					}
				if(min_dist_id!=-1 && dist < (avg_speed+max_speed)*(avg_speed+max_speed)){
					//printf("Found candidate to swap with!\n");
					shared.at(min_dist_id)=0;
					targets.at(min_dist_id).trajectory.pop_back();
					AddPoint(min_dist_id,t->trajectory.back());
					ptargets.erase(t);
					found=1; // do only one at a time
					break;
					}
				}
			}
		if(found) break;
		}
	}


/** \brief Checks how many particles have been associated or are free respectively 
* \param fp : number of free particles
* \param ap : number of associated particles
*/
void Tracker::FindFreeParticles(int* fp, int* ap)
	{
	*fp=0; *ap=0;
	for(vector<particle>::iterator p=particles->begin();
	p != particles->end();
	p++) 
		if(p->id==-1) 
			(*fp)++;
		else
			(*ap)++;
	}

/** Deletes all used (associated) particles from the list of particles */
void Tracker::CleanParticles()
	{
				vector<particle> temp;
				for(unsigned int i=0; i<particles->size(); i++) temp.push_back(particles->at(i));
				
				particles->clear();
				
				for(vector<particle>::iterator p=temp.begin();
				p!= temp.end();
				p++){									
					if(p->id==-1)
						particles->push_back(*p);
					}
	}

void Tracker::AssociateParticlesToCompetitors(int max_speed){
	for(vector<particle>::iterator p=particles->begin();
				p!= particles->end();
				p++){
					const double INFTY = 1000000000;
					double min_dist= (double) INFTY;
					bool good_competitor_found = false; // True iff a reasonable competitor has been found
					vector<int>::iterator min_dist_id=competitors.begin();
					
					for(vector<int>::iterator it=competitors.begin(); it != competitors.end(); it++){
						double dist = GetCost(*it,p->p);
						if(dist < min_dist) 
							{
								min_dist = dist;
								min_dist_id = it; // save pointer to competitor
							}
						
						}
					
					
					if(sqrt(min_dist) < max_speed)
						{ // take a particle that is not to far away
							targets.at(*min_dist_id).trajectory.pop_back(); // Remove the oldest trajectory point of the trajectory history
							AddPoint(*min_dist_id,p->p); // Add the current point to the trajectory history
							good_competitor_found = true; // In this case say we have found the good competitor
						}
					

					// [2006-10-19-14-08, Clement Hongler:] commented this line because allows potential trajectories overlap
					// if(min_dist_id!=competitors.begin()) competitors.erase(min_dist_id); // ...and delete it
					// Replaced with this one that should do what we expect

					// Iff we have found a good competitor (it has already been added to the particle history
					if (good_competitor_found == true) { 
						competitors.erase(min_dist_id); // We can remove it from the list of the competitors
					}

					} // end for every particle
				}


int Tracker::Step()
	{
	particles = particlefilter->Step();
	if(particles){
		status=RUNNING;
		DataAssociation();	// associate all points with their nearest neighbor
		if(trackingimg->GetDisplay())
			DrawTrajectories();
		if(mask){
			mask->UpdateCount(&targets,(int)(particlefilter->GetProgress(2)));
			mask->DrawMask(trackingimg);
			}
		}
	else
		status=FINISHED;
	return(status);
	}

/** Calculates cost for two points to be associated.
* Here: cost is distance.
*
* \param id : the id of the trajectory
* \param p  : a point
*
* \return Cost between a trajectory and a point
* \todo The cost function used here is very simple. One could imagine to take also other
* attributes, for instance the speed of the object into account.
*/
double Tracker::GetCost(int id,CvPoint2D32f p){
	
	return(
		(targets.at(id).trajectory.back().x-p.x)*
		(targets.at(id).trajectory.back().x-p.x)
		+
		(targets.at(id).trajectory.back().y-p.y)*
		(targets.at(id).trajectory.back().y-p.y)
		);
	}


/** Convert list of contours into particles 
* Allows to filter particles according to tracking/segmentation parameters. Currently,
* the function distinguishes normal and large contours (collisions) but does not use this feature.
* While contours store additional parameters (e.g. area, compactness, ...), particles are given
* only by their position, and the object they are currently associated with.
*
* \param contours : list of contours from segmentation
* \param img : trackingimg to store information relevant to the user (optional)
*
* \todo A possible improvement could be to generate more than one particle from
* contours that are too large.
*/
/*void Tracker::GetParticlesFromContours(vector<resultContour>* contours){
	int numContours = contours->size();
	int numParticles=0;
	
	particles->clear();
	for(int c=0;c<numContours;c++)
		{                             
		if(contours->at(c).area<=max_area){ // Only allow blobs that are not bigger than an object
			particle P;
			P.p=contours->at(c).center;
			P.compactness=contours->at(c).compactness;
			P.orientation=contours->at(c).orientation;
			P.area=contours->at(c).area;
			P.color=contours->at(c).color;
			P.id=-1; // initially the particle is unassociated
			particles->push_back(P);
			numParticles++;
			if(trackingimg->GetDisplay())
				trackingimg->DrawCircle(cvPoint((int)(contours->at(c).center.x),(int)(contours->at(c).center.y)),CV_RGB(0,255,0));
			//trackingimg->DrawRectangle(&contours->at(c).boundingrect,CV_RGB(0,0,255));
				trackingimg->Cover(cvPoint((int)(contours->at(c).center.x),(int)(contours->at(c).center.y)),CV_RGB(255-P.area,0,0),2);
			}
		else{ // ...else add the particle as well and plot with different color
			particle P;
			P.p=contours->at(c).center;
			P.compactness=contours->at(c).compactness;
			P.orientation=contours->at(c).orientation;
			P.area=contours->at(c).area;
			P.color=contours->at(c).color;

			P.id=-1; // initially the particle is unassociated
			particles->push_back(P);
			numParticles++;
			if(trackingimg->GetDisplay())
				trackingimg->DrawCircle(cvPoint((int)(contours->at(c).center.x),(int)(contours->at(c).center.y)),CV_RGB(255,0,0));
			trackingimg->Cover(cvPoint((int)(contours->at(c).center.x),(int)(contours->at(c).center.y)),CV_RGB(255-P.area,0,0),2);
			}
		}
	}*/

/** Add a point to the current track (max track) 
*
* \param i : Identifies the trajectory p will be added to
* \param p : Point to add to trajectory i (subpixel accuracy)
*/
void Tracker::AddPoint(int i, CvPoint2D32f p){
	targets.at(i).AddPoint(p);
	}

/** Initializes the tracker
* As soon as nr_objects particles are distinguished, the tracker locks on them and 
* creates individual trajectories.
*
* \todo This function is not very intelligent. Possible improvement could be to use
* a mask for the arena to prevent noise from being possible candidates.
*/
int Tracker::init()
	{
	int numParticles=0;
		
		particles=particlefilter->Step();
		
		if(particles){
			status=SEARCHING;
			numParticles=particles->size();
			if(numParticles >= nr_objects)				// if there are enough particles
			{
			status = RUNNING;
			for (int i=0;i<nr_objects;i++)         
				{	
				AddPoint(i,particles->at(i).p);
				}
			}
		}
		else{
			return(STOPPED);
		}
      	
	return(status);
	}

/** Yields current position of the object specified by id.
*
* \param id : id of the object which position will be returned.
* \return Pixel coordinate of object specified by id (subpixel accuracy)
*/ 
CvPoint2D32f* Tracker::GetPos(int id)
{
	//printf("%f %f\n",targets.at(0).trajectory.at(0).x,targets.at(0).trajectory.at(0).y);
//	printf("Size %d\n",targets.at(id).trajectory.size());
	return(&targets.at(id).trajectory.back());
}
/** \brief Gives AVI progress
* 
* \param kind : format of the progress
*               - 1 = progress in milliseconds
*               - 2 = progress in frames
*               - 3 = progress in percent
* \result Returns progress in the format specified by 'kind'
*
*/
double Tracker::GetProgress(int kind)
{
	return(particlefilter->GetProgress(kind));
}

void Tracker::SetParameters()
	{
					
	nr_objects=GetIntValByXPath(cfgRoot,"/CFG/TRACKER[@mode='0']/NOBJECTS");
	max_speed=GetIntValByXPath(cfgRoot,"/CFG/TRACKER[@mode='0']/MAXSPEED");
	particlefilter->SetParameters();
	}

/** \brief Changes display options
*
* \param style
* - TRAJCROSS		: Draws a cross and id for every object
* - TRAJNONUMBER	: Draws a cross without id
* - RAJFULL			: Draws a full trajectory with cross and id	
* - TRAJNOCROSS		: Draws only the trajectory
*/
void Tracker::SetVisualisation(int style)
{
this->style=style;
}

/** \brief Wrapper function invoking the drawing routine of all targets
*/
void Tracker::DrawTrajectories()
{
for(int i=0;i<nr_objects; i++)
		targets.at(i).DrawTrajectory(style);
									
}

/** \brief Wrapper function calling	ParticleFilter::GetFPS
*/
double Tracker::GetFPS()
{
 return(particlefilter->GetFPS());
}


/** \brief Calculates the "fitness" of a combination of two particles to belong to the same track
*
* For choosing the right candidate in data association, that is assigning an object to a certain track
* and thus maintaining the identity of an indiviudal, a fitness function might be a good indicator to compare
* different options.
*
* \param pa : pointer to one particle
* \param pb : pointer to another particle
* \return Fitness of the pair a and b
* \todo Take into account color of the particle as well
* \todo FUNCTION NOT IN USE YET
*/
double Tracker::Fitness(particle *pa, particle *pb)
{
	double dist[5];
	double fitness;

	dist[0]=sqrt((pb->p.x-pa->p.x)*(pb->p.x-pa->p.x)+(pb->p.y-pa->p.y)*(pb->p.y-pa->p.y)); // distance between particles
	//dist[1]=fabs(pb.color-pa.color);
	dist[2]=fabs(pb->area-pb->area);
	dist[3]=fabs(pb->compactness-pa->compactness);
	dist[4]=fabs(pb->orientation-pa->orientation);

	fitness = (dist[0] + dist[2] + dist[3] + dist[4]);  
	if(fitness!=0) fitness=1/fitness; // the fitness is better the higher it is
	return(fitness);

}

/** \brief Initializes the Mask Class
*
*/
int Tracker::InitMask()
	{
	if(mask) delete(mask);
	mask = new Mask();
	return(1);
	}

void Tracker::ToggleMaskDisplay()
{
	if(mask) mask->ToggleDisplay();
}

int Tracker::CountSharedTrajectories(vector<int>* shared)
{
	vector<CvPoint2D32f> locs;
	vector<CvPoint2D32f> redundant;

	
	int sht=0;

	for(int i=0;i<nr_objects; i++){
		CvPoint2D32f* act = GetPos(i);
		int found=0;

		for(unsigned int j=0; j<locs.size(); j++){
			if(act->x == locs.at(j).x && act->y == locs.at(j).y){
				int isredundant=0;
				for(unsigned int k=0; k<redundant.size(); k++){
					if(act->x == redundant.at(k).x && act->y==redundant.at(k).y) isredundant=1;
					}
				if(!isredundant) sht++;
				found=1;
				}
			}
		if(!found){
			locs.push_back(*act);
			shared->push_back(0);
			}
		else{
			shared->push_back(sht);
			redundant.push_back(*act);
			}
	}

//	for(i=0;i<nr_objects; i++) printf("%d",shared->at(i));
//	printf("\n");
		
	return(sht);
}

void Tracker::SetCritPoint(int id)
{
	targets.at(id).SetCritPoint(&targets.at(id).trajectory.back());
}

CvPoint2D32f* Tracker::GetCritPoint(int id)
{
	return(&targets.at(id).critpoint);
}


int Tracker::GetRestingTrajectories(vector<int>* restingtraj)
	{
	int found=0;
	
	for(unsigned int i=0; i<targets.size(); i++){
		CvPoint2D32f* p1;
		CvPoint2D32f* p2;
		
		p1 = &targets.at(i).trajectory.back();
		p2 = &targets.at(i).trajectory.at(targets.at(i).trajectory.size()-2);
		
		if(p1->x == p2->x && p1->y == p2->y){ 
			found++;
			restingtraj->at(i)++;
			}
		else
			restingtraj->at(i)=0;
		}
	return(found);
	}

double Tracker::GetDist(CvPoint2D32f *p1, CvPoint2D32f *p2)
{
	return(
		(p1->x-p2->x)*
		(p1->x-p2->x)
		+
		(p1->y-p2->y)*
		(p1->y-p2->y)
		);
}

void Tracker::RefreshCoverage()
{
	particlefilter->RefreshCoverage();
}

void Tracker::SetPos(int id,CvPoint2D32f *p)
{
	targets.at(id).trajectory.pop_back();
	targets.at(id).trajectory.push_back(*p);
					
}


int Tracker::GetNumberofParticles()
{	
		return(particles->size());
}

int Tracker::GetNumberofTracks()
{
	return(targets.size());
}

CvPoint2D32f* Tracker::GetParticlePos(int id)
{
	if((unsigned int) id <= particles->size())
		return(&(particles->at(id).p));
	else
		return(0);
}

CvPoint2D32f* Tracker::GetTargetPos(int id)
{
	return(&(targets.at(id).trajectory.back()));
}
