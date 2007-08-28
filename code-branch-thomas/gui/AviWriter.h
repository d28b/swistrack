#ifndef _aviwriter_H
#define _aviwriter_H

#include "cv.h"
#include "highgui.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "SwisTrack.h"

enum aviwriter_code {RAWIMAGE, OVERLAY};
/** \class AviWriter
*
* \brief Provides avi writting functions
*
*/
class AviWriter
	{
	public:
		AviWriter(wxString* fname, SwisTrack* parent,int type);
		void WriteFrame();
		~AviWriter();
	private:
		CvVideoWriter* AVIWriter;
		SwisTrack* parent;
		int aviwriter_type;
	};

#endif
