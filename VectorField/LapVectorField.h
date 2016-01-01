


#ifndef _lap_vector_field_h_
#define _lap_vector_field_h_ 

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <vector>
//#include <Windows.h>
//#include <Winuser.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

/*#ifdef _DEBUG
#pragma comment(lib, "opencv_core246d.lib") ;
#pragma comment(lib, "opencv_highgui246d.lib") ;
#pragma comment(lib, "opencv_imgproc246d.lib") ;
#else
#pragma comment(lib, "opencv_core246.lib") ;
#pragma comment(lib, "opencv_highgui246.lib") ;
#pragma comment(lib, "opencv_imgproc246.lib") ;
#endif*/

#include "cholmod_matrix.h"

#include "declarations.h"
#include <string>

//extern int consider_fitted_curve ;

typedef Cholmod_matrix<double> CMatrix;

void computeVectorField( std::vector<double2>  &vector_field, std::vector<std::vector<double2> > &curves_flt, int resolution  ) ;


class LapVectorField{
public:
	LapVectorField() {};
	int resolution ;
	std::vector<double2>  vector_field ;
	CvSize imgsize ;

    void compute(  std::vector<std::vector<CvPoint> > curves_pix, CvSize imgsize, int res  ) ;

	double2 getVector( CvPoint p );

};

#endif
