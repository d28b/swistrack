#include "DataLogger.h"

DataLogger::DataLogger(ObjectTracker* parent, xmlpp::Element* cfgRoot){

	if(!IsAttrByXPath(cfgRoot,"/CFG/COMPONENTS/OUTPUT","mode"))
        throw "[Datalogger::Datalogger] Datalogger mode undefined (/CFG/COMPONENTS/OUTPUT)";

    mode=GetIntAttrByXPath(cfgRoot,"/CFG/COMPONENTS/OUTPUT","mode");
    
    switch(mode){
        case 0 : /////////////// Raw Data //////////////////////////
            CreateExceptionIfEmpty(cfgRoot,"/CFG/OUTPUT[@mode='0']");
            CreateExceptionIfEmpty(cfgRoot,"/CFG/OUTPUT[@mode='0']/OUTPUTFNAME");					
			break;
        default : throw "[Datalogger::Datalogger] mode invalid";
        };

	
	switch(mode){
		case 0:
			output = fopen(GetValByXPath(cfgRoot,"/CFG/OUTPUT[@mode='0']/OUTPUTFNAME"),"w");
			break;
		default : throw "[Datalogger::Datalogger] mode invalid";
	};

	this->parent=parent;
	ncols = parent->GetNumberofTracks();
	}

void DataLogger::WriteRow(){
	
	double frame   =  parent->GetProgress(2);
	double time = parent->GetProgress(1);

	fprintf(output,"%2.0f %f ",frame,time);
		
	for(int i=0; i<ncols; i++){
		CvPoint2D32f* pi=parent->GetPos(i);
		CvPoint2D32f p=*pi;

		fprintf(output,"%f %f ",pi->x,pi->y);
		}
	fprintf(output,"\n");
      }

DataLogger::~DataLogger(){	
	fclose(output);
	}
