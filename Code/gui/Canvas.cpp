#include "Canvas.h"
#define THISCLASS Canvas

#include <wx/image.h>
#include <opencv2/core.hpp>

BEGIN_EVENT_TABLE(THISCLASS, wxControl)
	EVT_PAINT(THISCLASS::OnPaint)
	EVT_ERASE_BACKGROUND(THISCLASS::OnEraseBackground)
	EVT_SIZE(THISCLASS::OnSize)
	EVT_LEFT_DOWN(THISCLASS::OnMouseLeftDown)
	EVT_LEFT_UP(THISCLASS::OnMouseLeftUp)
	EVT_RIGHT_DOWN(THISCLASS::OnMouseRightDown)
	EVT_MOTION(THISCLASS::OnMouseMove)
	EVT_MENU(cID_Zoom200, THISCLASS::OnMenuZoom)
	EVT_MENU(cID_Zoom100, THISCLASS::OnMenuZoom)
	EVT_MENU(cID_Zoom50, THISCLASS::OnMenuZoom)
	EVT_MENU(cID_Zoom25, THISCLASS::OnMenuZoom)
	EVT_MENU(cID_Zoom10, THISCLASS::OnMenuZoom)
	EVT_MENU(cID_ZoomFit, THISCLASS::OnMenuZoom)
	EVT_MENU(cID_UpdateRate0, THISCLASS::OnMenuUpdateRate)
	EVT_MENU(cID_UpdateRate1, THISCLASS::OnMenuUpdateRate)
	EVT_MENU(cID_UpdateRate2, THISCLASS::OnMenuUpdateRate)
	EVT_MENU(cID_UpdateRate4, THISCLASS::OnMenuUpdateRate)
	EVT_MENU(cID_UpdateRate8, THISCLASS::OnMenuUpdateRate)
	EVT_MENU(cID_UpdateRate16, THISCLASS::OnMenuUpdateRate)
	EVT_MENU(cID_UpdateRate32, THISCLASS::OnMenuUpdateRate)
	EVT_MENU(cID_UpdateRate64, THISCLASS::OnMenuUpdateRate)
	EVT_MENU(cID_UpdateRate128, THISCLASS::OnMenuUpdateRate)
	EVT_MENU(cID_FlipVertically, THISCLASS::OnMenuFlipVertically)
	EVT_MENU(cID_FlipHorizontally, THISCLASS::OnMenuFlipHorizontally)
	EVT_MENU(cID_SaveViewImageAs, THISCLASS::OnMenuSaveViewImageAs)
	EVT_MENU(cID_SaveOriginalImageAs, THISCLASS::OnMenuSaveOriginalImageAs)
END_EVENT_TABLE()

THISCLASS::Canvas(CanvasPanel * cp):
	wxControl(cp, -1), mCanvasPanel(cp), mPopupMenu(), mDisplayRenderer(),
	mMoveStarted(false), mMoveStartPoint() {

	SetWindowStyle(wxNO_BORDER);

	// Create the popup menu
	mPopupMenu.AppendCheckItem(cID_Zoom200, wxT("Zoom: 200 %"));
	mPopupMenu.AppendCheckItem(cID_Zoom100, wxT("Zoom: 100 %"));
	mPopupMenu.AppendCheckItem(cID_Zoom50, wxT("Zoom: 50 %"));
	mPopupMenu.AppendCheckItem(cID_Zoom25, wxT("Zoom: 25 %"));
	mPopupMenu.AppendCheckItem(cID_Zoom10, wxT("Zoom: 10 %"));
	mPopupMenu.AppendCheckItem(cID_ZoomFit, wxT("Fit zoom"));
	mPopupMenu.AppendSeparator();
	mPopupMenu.AppendCheckItem(cID_UpdateRate1, wxT("Show every frame"));
	mPopupMenu.AppendCheckItem(cID_UpdateRate2, wxT("Show every 2nd frame"));
	mPopupMenu.AppendCheckItem(cID_UpdateRate4, wxT("Show every 4th frame"));
	mPopupMenu.AppendCheckItem(cID_UpdateRate8, wxT("Show every 8th frame"));
	mPopupMenu.AppendCheckItem(cID_UpdateRate16, wxT("Show every 16th frame"));
	mPopupMenu.AppendCheckItem(cID_UpdateRate32, wxT("Show every 32th frame"));
	mPopupMenu.AppendCheckItem(cID_UpdateRate64, wxT("Show every 64th frame"));
	mPopupMenu.AppendCheckItem(cID_UpdateRate128, wxT("Show every 128th frame"));
	mPopupMenu.AppendCheckItem(cID_UpdateRate0, wxT("Don't update display"));
	mPopupMenu.AppendSeparator();
	mPopupMenu.AppendCheckItem(cID_FlipVertically, wxT("Flip vertically"));
	mPopupMenu.AppendCheckItem(cID_FlipHorizontally, wxT("Flip horizontally"));
	mPopupMenu.AppendSeparator();
	mPopupMenu.Append(cID_SaveViewImageAs, wxT("Save displayed image as ..."));
	mPopupMenu.Append(cID_SaveOriginalImageAs, wxT("Save original image as ..."));

	// Set non-bold font
	wxFont f = GetFont();
	f.SetWeight(wxNORMAL);
}

THISCLASS::~Canvas() {
}

void THISCLASS::SetDisplay(Display * display) {
	mDisplayRenderer.SetDisplay(display);
	OnDisplayChanged();
}

void THISCLASS::OnDisplayChanged() {
	UpdateView();
}

void THISCLASS::UpdateView() {
	mDisplayRenderer.DeleteCache();
	mCanvasPanel->UpdateSize();
	Refresh();
}

wxSize THISCLASS::GetMaximumSize() {
	cv::Size size = mDisplayRenderer.GetSize();
	if (size.width < 32) size.width = 32;
	if (size.height < 32) size.height = 32;
	return wxSize(size.width, size.height);
}

void THISCLASS::OnEraseBackground(wxEraseEvent & event) {
	// Overwrite this method to avoid drawing the background
}

void THISCLASS::OnPaint(wxPaintEvent & WXUNUSED(event)) {
	wxPaintDC dc(this);
	bool ok = OnPaintImage(dc);

	// Display an error if no image can be displayed
	if (! ok) {
		wxSize size = GetClientSize();
		dc.SetPen(*wxWHITE_PEN);
		dc.SetBrush(*wxWHITE_BRUSH);
		dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());
		dc.SetFont(GetFont());
		dc.DrawText(wxT("No image."), 4, 4);
	}
}

bool THISCLASS::OnPaintImage(wxPaintDC & dc) {
	cv::Mat img = mDisplayRenderer.GetImage();
	if (img.empty()) return false;

	// Create an image that has the size of the DC
	wxSize dcSize = dc.GetSize();
	int dw = dcSize.GetWidth();
	int dh = dcSize.GetHeight();
	wxImage wximg(dw, dh, false);

	// Prepare the area of interest of the source image
	int sx1 = -mViewOffset.x;
	int sy1 = -mViewOffset.y;
	if (sx1 < 0) sx1 = 0;
	if (sy1 < 0) sy1 = 0;
	if (sx1 > img.cols) sx1 = img.cols;
	if (sy1 > img.rows) sy1 = img.rows;

	int sx2 = -mViewOffset.x + dw;
	int sy2 = -mViewOffset.y + dh;
	if (sx2 < 0) sx2 = 0;
	if (sy2 < 0) sy2 = 0;
	if (sx2 > img.cols) sx2 = img.cols;
	if (sy2 > img.rows) sy2 = img.rows;
	int w = sx2 - sx1;
	//int h=sy2-sy1;	// Not needed

	// Prepare the area of interest of the destination image
	int dx1 = mViewOffset.x;
	int dy1 = mViewOffset.y;
	if (dx1 < 0) dx1 = 0;
	if (dy1 < 0) dy1 = 0;
	if (dx1 > dw) dx1 = dw;
	if (dy1 > dh) dy1 = dh;

	int dx2 = mViewOffset.x + img.cols;
	int dy2 = mViewOffset.y + img.rows;
	if (dx2 < 0) dx2 = 0;
	if (dy2 < 0) dy2 = 0;
	if (dx2 > dw) dx2 = dw;
	if (dy2 > dh) dy2 = dh;

	// Convert the area of interest to a wximg
	unsigned char * cw = wximg.GetData();

	// Top white area
	for (int y = 0; y < dy1; y++) {
		memset(cw, 255, dw * 3);
		cw += dw * 3;
	}

	// Image area
	for (int y = dy1; y < dy2; y++) {
		// White on the left
		memset(cw, 255, dx1 * 3);
		cw += dx1 * 3;

		// Image
		unsigned char * cr = img.ptr(sy1 - dy1 + y) + sx1 * 3;
		memcpy(cw, cr, w * 3);
		cw += w * 3;

		// White on the right
		memset(cw, 255, (dw - dx2) * 3);
		cw += (dw - dx2) * 3;
	}

	// Bottom white area
	for (int y = dy2; y < dh; y++) {
		memset(cw, 255, dw * 3);
		cw += dw * 3;
	}

	// Draw image
	wxBitmap bmp(wximg);
	dc.DrawBitmap(bmp, 0, 0, false);
	return true;
}

void THISCLASS::OnMouseLeftDown(wxMouseEvent & event) {
	mMoveStartPoint = event.GetPosition();
	mMoveStarted = true;
}

void THISCLASS::OnMouseLeftUp(wxMouseEvent & event) {
	mMoveStarted = false;
}

void THISCLASS::OnMouseMove(wxMouseEvent & event) {
	if (! mMoveStarted) {
		wxPoint nowPoint = event.GetPosition();
		int x = (int) ((nowPoint.x - mViewOffset.x) / mDisplayRenderer.GetScalingFactor() + 0.5);
		int y = (int) ((nowPoint.y - mViewOffset.y) / mDisplayRenderer.GetScalingFactor() + 0.5);
		wxString position = wxString::Format(wxT("%d x %d"), x, y);

		Display * display = mCanvasPanel->mCurrentDisplay;
		if (! display) {
			mCanvasPanel->mCanvasAnnotation->SetTextRight(wxT(""));
			return;
		}

		cv::Mat img = display->mMainImage;
		if (x < 0 || x >= img.cols || y < 0 || y >= img.rows) {
			mCanvasPanel->mCanvasAnnotation->SetTextRight(position);
			return;
		}

		unsigned char * pixel = img.ptr<unsigned char>(y, x);
		if (img.channels() == 3) {
			wxColour color(pixel[2], pixel[1], pixel[0]);
			mCanvasPanel->mCanvasAnnotation->SetTextRight(color.GetAsString(wxC2S_HTML_SYNTAX) + wxString::Format(wxT(" RGB %d %d %d @ "), pixel[2], pixel[1], pixel[0]) + position);
		} else {
			mCanvasPanel->mCanvasAnnotation->SetTextRight(wxString::Format(wxT("%d"), pixel[0]) + wxT(" @ ") + position);
		}
		return;
	}

	if (! event.LeftIsDown()) {
		mMoveStarted = false;
		return;
	}

	wxPoint nowPoint = event.GetPosition();
	wxPoint oldViewOffset = mViewOffset;
	mViewOffset.x += nowPoint.x - mMoveStartPoint.x;
	mViewOffset.y += nowPoint.y - mMoveStartPoint.y;
	mMoveStartPoint = nowPoint;

	if (mViewOffset.x > 0) mViewOffset.x = 0;
	if (mViewOffset.y > 0) mViewOffset.y = 0;

	wxSize dcSize = this->GetSize();
	cv::Size imageSize = mDisplayRenderer.GetSize();
	if (mViewOffset.x < dcSize.GetWidth() - imageSize.width)
		mViewOffset.x = dcSize.GetWidth() - imageSize.width;
	if (mViewOffset.y < dcSize.GetHeight() - imageSize.height)
		mViewOffset.y = dcSize.GetHeight() - imageSize.height;

	if (mViewOffset == oldViewOffset) return;
	Refresh(true);
}

void THISCLASS::OnMouseRightDown(wxMouseEvent & event) {
	mPopupMenu.Check(cID_FlipVertically, mDisplayRenderer.GetFlipVertical());
	mPopupMenu.Check(cID_FlipHorizontally, mDisplayRenderer.GetFlipHorizontal());

	mPopupMenu.Check(cID_Zoom200, (mDisplayRenderer.GetScalingFactor() == 2));
	mPopupMenu.Check(cID_Zoom100, (mDisplayRenderer.GetScalingFactor() == 1));
	mPopupMenu.Check(cID_Zoom50, (mDisplayRenderer.GetScalingFactor() == 0.5));

	mPopupMenu.Check(cID_UpdateRate0, (mCanvasPanel->mUpdateRate == 0));
	mPopupMenu.Check(cID_UpdateRate1, (mCanvasPanel->mUpdateRate == 1));
	mPopupMenu.Check(cID_UpdateRate2, (mCanvasPanel->mUpdateRate == 2));
	mPopupMenu.Check(cID_UpdateRate4, (mCanvasPanel->mUpdateRate == 4));
	mPopupMenu.Check(cID_UpdateRate8, (mCanvasPanel->mUpdateRate == 8));
	mPopupMenu.Check(cID_UpdateRate16, (mCanvasPanel->mUpdateRate == 16));
	mPopupMenu.Check(cID_UpdateRate32, (mCanvasPanel->mUpdateRate == 32));
	mPopupMenu.Check(cID_UpdateRate64, (mCanvasPanel->mUpdateRate == 64));
	mPopupMenu.Check(cID_UpdateRate128, (mCanvasPanel->mUpdateRate == 128));

	PopupMenu(&mPopupMenu);
}

void THISCLASS::OnSize(wxSizeEvent & event) {
	Refresh(true);
}

void THISCLASS::OnMenuSaveOriginalImageAs(wxCommandEvent & event) {
	// Get the current image
	Display * display = mCanvasPanel->mCurrentDisplay;
	if (! display) return;
	cv::Mat img = display->mMainImage;
	if (img.empty()) return;

	// Copy the current image (since the file dialog will allow other threads to run, which may modify the image)
	cv::Mat imgCopy = CopyToBGR(img);

	// Show the file save dialog
	wxFileDialog * dlg = new wxFileDialog(this, wxT("Save original image"), wxT(""), wxT(""), wxT("Bitmap (*.bmp)|*.bmp|PNG images (*.png)|*.png|JPEG images (*.jpeg)|*.jpeg|All files|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
	if (dlg->ShowModal() != wxID_OK) return;

	// Save the image
	wxString filename = dlg->GetPath();
	if (! cv::imwrite(filename.ToStdString(), imgCopy)) {
		wxMessageDialog dlg(this, wxT("The file could not be saved!"), wxT("Save original image"), wxOK);
		dlg.ShowModal();
		return;
	}
}

void THISCLASS::OnMenuSaveViewImageAs(wxCommandEvent & event) {
	// Copy the current image (since the file dialog will allow other threads to run)
	cv::Mat img = mDisplayRenderer.GetImage();
	if (img.empty()) return;

	// Copy the current image (since the file dialog will allow other threads to run, which may modify the image) and convert it from RGB to BGR
	cv::Mat imgCopy = cv::Mat(img.size(), CV_8UC3);
	cv::cvtColor(img, imgCopy, cv::COLOR_RGB2BGR);

	// Show the file save dialog
	wxFileDialog * dlg = new wxFileDialog(this, wxT("Save displayed image"), wxT(""), wxT(""), wxT("Bitmap (*.bmp)|*.bmp|PNG images (*.png)|*.png|JPEG images (*.jpeg)|*.jpeg|All files|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
	if (dlg->ShowModal() != wxID_OK) return;

	// Save the image
	wxString filename = dlg->GetPath();
	if (! cv::imwrite(filename.ToStdString(), imgCopy)) {
		wxMessageDialog dlg(this, wxT("The file could not be saved!"), wxT("Save displayed image"), wxOK);
		dlg.ShowModal();
		return;
	}
}

cv::Mat THISCLASS::CopyToBGR(cv::Mat src) {
	if (src.channels() == 3) return src.clone();

	if (src.channels() == 1) {
		cv::Mat dest = cv::Mat(src.size(), CV_8UC3);
		cv::cvtColor(src, dest, cv::COLOR_GRAY2BGR);
		return dest;
	}

	cv::Mat dest = cv::Mat(src.size(), CV_8UC3);
	int fromTo[] = {0, 0, 0, 1, 0, 2};
	cv::mixChannels(&src, 1, &dest, 1, fromTo, 3);
	return dest;
}

void THISCLASS::OnMenuFlipVertically(wxCommandEvent & event) {
	mDisplayRenderer.SetFlipVertical(event.IsChecked());
}

void THISCLASS::OnMenuFlipHorizontally(wxCommandEvent & event) {
	mDisplayRenderer.SetFlipHorizontal(event.IsChecked());
}

void THISCLASS::OnMenuZoom(wxCommandEvent & event) {
	if (event.GetId() == cID_Zoom200) {
		mDisplayRenderer.SetScalingFactor(2);
	} else if (event.GetId() == cID_Zoom100) {
		mDisplayRenderer.SetScalingFactor(1);
	} else if (event.GetId() == cID_Zoom50) {
		mDisplayRenderer.SetScalingFactor(0.5);
	} else if (event.GetId() == cID_Zoom25) {
		mDisplayRenderer.SetScalingFactor(0.25);
	} else if (event.GetId() == cID_Zoom10) {
		mDisplayRenderer.SetScalingFactor(0.1);
	} else {
		mDisplayRenderer.SetScalingFactorMax(cv::Size(mCanvasPanel->mAvailableSpace.GetWidth(), mCanvasPanel->mAvailableSpace.GetHeight()));
	}

	UpdateView();
}

void THISCLASS::OnMenuUpdateRate(wxCommandEvent & event) {
	if (event.GetId() == cID_UpdateRate0) {
		mCanvasPanel->mUpdateRate = 0;
	} else if (event.GetId() == cID_UpdateRate2) {
		mCanvasPanel->mUpdateRate = 2;
	} else if (event.GetId() == cID_UpdateRate4) {
		mCanvasPanel->mUpdateRate = 4;
	} else if (event.GetId() == cID_UpdateRate8) {
		mCanvasPanel->mUpdateRate = 8;
	} else if (event.GetId() == cID_UpdateRate16) {
		mCanvasPanel->mUpdateRate = 16;
	} else if (event.GetId() == cID_UpdateRate32) {
		mCanvasPanel->mUpdateRate = 32;
	} else if (event.GetId() == cID_UpdateRate64) {
		mCanvasPanel->mUpdateRate = 64;
	} else if (event.GetId() == cID_UpdateRate128) {
		mCanvasPanel->mUpdateRate = 128;
	} else {
		mCanvasPanel->mUpdateRate = 1;
	}
}
