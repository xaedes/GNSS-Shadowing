#ifndef _sgp4ext_
#define _sgp4ext_
/*     ----------------------------------------------------------------
*
*                                 sgp4ext.h
*
*    this file contains extra routines needed for the main test program for sgp4.
*    these routines are derived from the astro libraries.
*
*                            companion code for
*               fundamentals of astrodynamics and applications
*                                    2007
*                              by david vallado
*
*       (w) 719-573-2600, email dvallado@agi.com
*
*    current :
*              20 apr 07  david vallado
*                           misc documentation updates
*    changes :
*              14 aug 06  david vallado
*                           original baseline
*               27 jun 15 grady hillhouse
                            commented out asinh function
*       ----------------------------------------------------------------      */

#include <string.h>
#include <math.h>

#include "sgp4unit.h"


// ------------------------- function declarations -------------------------

double  sgn
        (
          double x
        );

double  mag
        (
          const double x[3]
        );

void    cross
        (
          const double vec1[3], const double vec2[3], double outvec[3]
        );

double  dot
        (
          const double x[3], const double y[3]
        );

double  angle
        (
          const double vec1[3],
          const double vec2[3]
        );

void    newtonnu
        (
          double ecc, double nu,
          double& e0, double& m
        );

/*double  asinh
        (
          double xval
        );
*/
void    rv2coe
        (
          const double r[3], const double v[3], double mu,
          double& p, double& a, double& ecc, double& incl, double& omega, double& argp,
          double& nu, double& m, double& arglat, double& truelon, double& lonper
        );

void    jday
        (
          int year, int mon, int day, int hr, int minute, double sec,
          double& jd
        );

void    days2mdhms
        (
          int year, double days,
          int& mon, int& day, int& hr, int& minute, double& sec
        );

void    invjday
        (
          double jd,
          int& year, int& mon, int& day,
          int& hr, int& minute, double& sec
        );

#endif
