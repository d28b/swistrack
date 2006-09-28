#include "AviWriter.h"

AviWriter::AviWriter(wxString* fname, SwisTrack* parent, int type=RAWIMAGE)
{
	if(!parent->ot) throw "[AviWriter::AviWriter] ObjectTracker not initialized";   
	aviwriter_type  = type;
    AVIWriter       = cvCreateAVIWriter(
                        fname->GetData(),
                        -1,
                        parent->GetDisplaySpeed(),
                        cvSize(parent->ot->GetImagePointer()->width,parent->ot->GetImagePointer()->height)
                      );
    this->parent    = parent;
}

void AviWriter::WriteFrame()
{
    switch (aviwriter_type)
	{
    case OVERLAY : 
        cvWriteToAVI(AVIWriter,parent->ot->GetImagePointer()); break;
    case RAWIMAGE: 
        cvWriteToAVI(AVIWriter,parent->ot->GetRawImagePointer()); break;
	}
}

AviWriter::~AviWriter()
{
    cvReleaseVideoWriter(&AVIWriter);
}
