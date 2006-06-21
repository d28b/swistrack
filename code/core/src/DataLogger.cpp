#include "DataLogger.h"

DataLogger::DataLogger(char* fname, xmlpp::Element* cfgRoot){
/*	wxString ucfname;

	ucfname.Printf("%s",fname);
	ucfname.Replace(".txt","_uncalib.txt");
	
	if(parent->transform) rawcalib = fopen(fname,  "w");
	rawuncalib = fopen(ucfname.GetData(),"w");
	this->parent=parent;
    if(parent->cfg->IsDefined("/CFG/TRACKER/NOBJECTS"))
        ncols = parent->cfg->GetIntParam("/CFG/TRACKER/NOBJECTS");
    else
        throw "Number of objects for Tracker not specified (NOBJECTS)";
*/
	}

void DataLogger::WriteRow(int frame, double time){
/*	if(parent->transform) fprintf(rawcalib,"%d %f ",frame,time);
	fprintf(rawuncalib,"%d %f ",frame,time);
	
	int nins = ncols;
	for(int i=0; i<nins; i++){
		CvPoint2D32f* pi=parent->GetPos(i);
		CvPoint2D32f p=*pi;

		if(parent->transform){
			p.x-=parent->width/2;
			p.y-=parent->height/2;
			CvPoint2D32f c=parent->transform->ImageToWorld(p);
			fprintf(rawcalib,"%f %f ",c.x,c.y);
			}
		fprintf(rawuncalib,"%f %f ",pi->x,pi->y);
		}
	if(parent->transform) fprintf(rawcalib,"\n");
	fprintf(rawuncalib,"\n");
	*/
      }

DataLogger::~DataLogger(){
	/*if(parent->transform) fclose(rawcalib);
	fclose(rawuncalib);*/
	}
