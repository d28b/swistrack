/**

  This is a text based user interface for SwisTrack

  */


#include "ObjectTracker.h"
#include <cstdio>
#include <libxml++/libxml++.h>



int main(){
    ObjectTracker* ot;
    xmlpp::Element* cfgRoot;
    xmlpp::DomParser* parser;
    xmlpp::Document* document;

    try{
        parser = new xmlpp::DomParser();
        parser->parse_file("example/example.cfg");
        document = parser->get_document();
        cfgRoot = parser->get_document()->get_root_node();
        }
    catch (...){
        throw "Problems encountered when opening default.cfg. Invalid syntax?";
        }

    try{

        ot = new ObjectTracker(cfgRoot);
        ot->Start();
        printf("Status: %d\n",ot->GetStatus());
      
        while(ot->GetStatus() == RUNNING){
            CvPoint2D32f* p=ot->GetPos(0);
            printf("Track 0: %f %f\n",p->x,p->y);
            ot->Step();

            }
        }
    catch( char * str ) {
        printf("Exception raised: %s\n",str);
        }
    
    return(0);    
    }