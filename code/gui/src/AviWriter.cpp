#include "AviWriter.h"

AviWriter::AviWriter(wxString* fname, SwisTrack* parent, int type=RAWIMAGE)
{
    aviwriter_type  = type;
    AVIWriter       = cvCreateAVIWriter(
                        fname->GetData(),
                        -1,
                        parent->GetDisplaySpeed(),
                        cvSize(parent->width,parent->height)
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
