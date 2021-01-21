#ifndef gasdev_h
#define gasdev_h
/******************************** DECLARATION MODULE ***********************************************/
/*
// Filename : gasdev.h
//
// Author   : Thierry Zimmerman Vision Group IMT/DMT EPFL
//
// Date     : April 1997
//
// Purpose  : header file for gasdev.c.
//             gsdev.c was found on the net and used for tsai model camera calibration.
//             It didn't have any header file so a lot of warnings where generated at
//              copile time, pretty anoying.
//
*/
/*************************************************************************************************/
/******************************** Inclusions *****************************************************/
/*************************************************************************************************/
#include <math.h>
/*************************************************************************************************/
/******************************** Functions ******************************************************/
/*************************************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

double ran1 (int *idum);
double gasdev (int *idum);

#ifdef __cplusplus
}
#endif


#endif
