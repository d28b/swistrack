#include "Utility.h"
#define THISCLASS Utility
#include <stdio.h>
#include <math.h>

double THISCLASS::SquareDistance(cv::Point2f p1, cv::Point2f p2) {
	double dx = p1.x - p2.x;
	double dy = p1.y - p2.y;
	return dx * dx + dy * dy;
}

double THISCLASS::Distance(cv::Point2f p1, cv::Point2f p2) {
	return pow(SquareDistance(p1, p2), 0.5);
}

cv::Rect THISCLASS::RectByCenter(int centerX, int centerY, int width, int height) {
	int lowerLeftx = centerX - width / 2;
	int lowerLefty = centerY - width / 2;
	return cv::Rect(lowerLeftx, lowerLefty, width, height);
}

double THISCLASS::toMillis(wxDateTime ts) {
	return ts.GetTicks() * 1000ULL + ts.GetMillisecond();
}

wxString THISCLASS::toMillisString(wxDateTime ts) {
	wxString millis;
	millis << ts.GetMillisecond();
	millis.Pad(3 - millis.Length(), '0', false);

	wxString out;
	out << ts.GetTicks();
	out << millis;
	return out;
}
