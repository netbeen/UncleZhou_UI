
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "LapVectorField.h"

#ifdef _DEBUG 
#pragma comment(lib, "opencv_core246d.lib") ;
#pragma comment(lib, "opencv_highgui246d.lib") ;
#pragma comment(lib, "opencv_imgproc246d.lib") ;
#else
#pragma comment(lib, "opencv_core246.lib") ;
#pragma comment(lib, "opencv_highgui246.lib") ;
#pragma comment(lib, "opencv_imgproc246.lib") ;
#endif


void curve_interplation( Edges &input, Edges &output ) ;
void getBezier( Edges &input, Edges &output , int step=3) ;
double2 point2vector( std::vector<CvPoint> points  ) ;

void getUniformCubicBSpline( Edges &input,Edges &output );

bool first_time = true ;
double first_value = 0.0 ;

bool consider_vecfield = false ;



double energy_function(std::vector<double> X0, const Edges &ctrpoints,const Edges &edges, Gradients &grads, LapVectorField &vecfild ) {


	// translate ctrpoints
	Edges dstCtrps = ctrpoints ;
	for( int i=0; i<dstCtrps.elems[0].size(); ++i ){
		dstCtrps.elems[0][i].x += X0[ i ] ;
		dstCtrps.elems[0][i].y += X0[ i + dstCtrps.elems[0].size() ] ;
	}


	const std::vector<std::vector<CvPoint>> &edgesEle =  edges.elems ;

	// interpolation
	Edges interCurve ;
	getUniformCubicBSpline(dstCtrps, interCurve) ;

	std::vector<CvPoint> &curve = interCurve.elems[0] ;



	// test if overstepping
	bool overstep = false ;
	for( int i=0; i<curve.size(); ++i ){
		if( curve[i].x <=0 ||curve[i].y<=0 || curve[i].x >= vecfild.imgsize.width - 1 || curve[i].y >= vecfild.imgsize.height - 1  )
			overstep = true ;
	}

	double energy = 0.0 ;
	if( !overstep ){
		// gradient term
		double averGrad = 0.0;
		for( int i=0; i<curve.size(); ++i  ){
			averGrad += grads.magn[curve[i].y][curve[i].x] ;
		}
		averGrad = averGrad/curve.size();

		// smooth term: curvature
		double averCurvature = 0.0 ;
		for( int i=1; i<curve.size()-1; ++i  ){

			double2 p1 = double2( curve[i -1].x, curve[i-1].y) ;
			double2 p2 = double2( curve[i   ].x, curve[i  ].y) ;
			double2 p3 = double2( curve[i +1].x, curve[i+1].y) ;

			averCurvature +=  fabs( acos( (std::min)( (std::max)( (p3-p2)*(p2-p1)/( (p3-p2).norm()*(p2-p1).norm()), -1.0), 1.0 )  ) / (p3-p1).norm()  ) ;
            if( std::isnan(averCurvature) || !finite( averCurvature ) ){
				std::cout << "curvature overflow!" <<std::endl;

				std::cout << "p1 " << p1.x <<" "<<p1.y << "\n" 
					<< "p2 " << p2.x <<" "<<p2.y << "\n"
					<< "p3 " << p3.x <<" "<<p3.y << "\n"
					<< "i" << i <<std::endl;

				system("pause");
			}

		}
		averCurvature = averCurvature/curve.size();
		//std::cout<<"averCurvature: "<<averCurvature<<std::endl;


		// smooth term: discontinuity
		double discon = 0.0 ;
		for( int i=0; i<X0.size()/2-1; ++i )
			discon += pow( X0[i] - X0[i+1], 2.0 ) ;
		for( int i=X0.size()/2; i<X0.size()-1; ++i )
			discon += pow( X0[i] - X0[i+1], 2.0 ) ;
		discon = sqrt( discon ) ;
		discon /= curve.size();


		// vector field term
		double averSinAngle = 0;
		for( int i=2; i<curve.size()-2; ++i  ){

			std::vector<CvPoint> points ;
			points.insert(points.begin(), curve.begin()+i-2, curve.begin()+i+3 );

			double2 dir = point2vector( points ) ;

			double2 vec = vecfild.getVector( curve[i] ) ;

			double cosalpha = (std::min) ( (std::max) (fabs( vec * dir / (vec.norm() * dir.norm())), 0.0), 1.0 ) ;

			averSinAngle += sin( acos( cosalpha ) );
		}
		averSinAngle = averSinAngle/curve.size();


		double h = 15 ;
		// scalar term
		double scalar = 0.0 ;
		for( int i=0; i<curve.size(); ++i  ){

			double2 p0 = double2( curve[i].x, curve[i].y ) ;
			double mindis2eid = 100.0; 
			for( int eid = 0; eid<edgesEle.size(); ++eid ){
				for( int pid = 0; pid<edgesEle[eid].size(); ++pid ){
					double2 p1 = double2( edgesEle[eid][pid].x,edgesEle[eid][pid].y  ) ;
					if( (p0-p1).norm() < mindis2eid )
						mindis2eid = (p0-p1).norm() ;
				}

			}
			scalar += pow( 2.71828, -mindis2eid*mindis2eid / (h * h) ) ;
		}
		scalar /= curve.size() ;

		//std :: cout << "averDistance = " << averDistance << std::endl;

		/*
		// display scalar field
		IplImage *scalarField = cvCreateImage( vecfild.imgsize, 8, 1) ;
		for( int x=0; x<scalarField->width; ++x ) for( int y=0; y<scalarField->height; ++y ){

			double scalar = 0.0 ;
			double2 p0 = double2( x, y ) ;
			double mindis2eid = 100.0; 
			for( int eid = 0; eid<edgesEle.size(); ++eid ){
				for( int pid = 0; pid<edgesEle[eid].size(); ++pid ){
					double2 p1 = double2( edgesEle[eid][pid].x,edgesEle[eid][pid].y  ) ;
					if( (p0-p1).norm() < mindis2eid )
						mindis2eid = (p0-p1).norm() ;
				}

			}
			scalar += pow( 2.71828, -mindis2eid*mindis2eid / (h * h) ) ;
			scalar *= 255  ;

			CV_IMAGE_ELEM( scalarField, uchar, y, x ) = std::min( scalar, 255.0) ;

		} 

		cvNamedWindow( "scalarField");
		cvShowImage("scalarField", scalarField) ;
		while( cvWaitKey(10) == -1 ){}
		*/

		energy =  500-averGrad + 500 * averCurvature  ;
		//energy =  500-averGrad + discon * 10 ;
		if( consider_vecfield ){
			energy += averSinAngle * 500  - scalar*300 ;
			//std::cout<<"\n"<<averGrad<<"\n"<<averCurvature<<"\n"<<averSinAngle<<"\n"<<scalar<<std::endl;
		}
        if( std::isnan(energy) || !finite( energy ) ){
			std::cout<<"\n"<<averGrad<<"\n"<<averCurvature<<"\n"<<averSinAngle<<"\n"<<scalar<<std::endl;
			system("pause");
		}
	}
	else{
		energy =  255-0 + 1000 * 1  ;
		if( consider_vecfield )
			energy += 1 * 300 ;
	}

	if( first_time ){
		first_value = energy ;
		first_time = false ;
	}


	//std::cout<< "f(";
	//for( int i=0; i<X0.size()-1; ++i )
	//	std::cout<<X0[i]<<"," ;
	//std::cout<<X0.back()<<") = "<< (fabs(firts_value) * 2 + energy) / fabs(firts_value) <<std::endl;


	return energy    ;
}

double energy_function_v2(std::vector<double> X0, Edges &ctrpoints, Gradients &grads, LapVectorField &vecfild ) {

	// translate ctrpoints
	Edges dstCtrps = ctrpoints ;
	for( int i=0; i<dstCtrps.elems[0].size(); ++i ){
		dstCtrps.elems[0][i].x += X0[ i ] ;
		dstCtrps.elems[0][i].y += X0[ i + dstCtrps.elems[0].size() ] ;
	}


	// interpolation
	Edges interCurve ;
	curve_interplation(dstCtrps, interCurve) ;

	std::vector<CvPoint> &curve = interCurve.elems[0] ;



	// test if overstepping
	bool overstep = false ;
	for( int i=0; i<curve.size(); ++i ){
		if( curve[i].x <=0 ||curve[i].y<=0 || curve[i].x >= vecfild.imgsize.width - 1 || curve[i].y >= vecfild.imgsize.height - 1  )
			overstep = true ;
	}

	double energy = 0.0 ;
	if( !overstep ){
		// gradient term
		double averGrad = 0.0;
		for( int i=1; i<curve.size()-1; ++i  ){


			double2 p1 = double2( curve[i -1].x, curve[i-1].y) ;
			double2 p2 = double2( curve[i   ].x, curve[i  ].y) ;
			double2 p3 = double2( curve[i +1].x, curve[i+1].y) ;

			std::vector<CvPoint> points ;
			points.insert(points.begin(), curve.begin()+i-1, curve.begin()+i+2 );
			double2 dir = point2vector( points ) ;
			double2 vec = vecfild.getVector( curve[i] ) ;

			double cosalpha = (std::min) ( (std::max) (fabs( vec * dir / (vec.norm() * dir.norm())), 0.0), 1.0 ) ;

			//double sinalpha = sin( acos( cosalpha ) );
			if( consider_vecfield )
				averGrad += grads.magn[curve[i].y][curve[i].x]  * cosalpha;
			else
				averGrad += grads.magn[curve[i].y][curve[i].x]  ;
		}
		averGrad = averGrad/curve.size();

		// smooth term
		double averCurvature = 0 ;
		for( int i=1; i<curve.size()-1; ++i  ){

            double2 p1 = double2( curve[i -1].x, curve[i-1].y) ;
            double2 p2 = double2( curve[i   ].x, curve[i  ].y) ;
            double2 p3 = double2( curve[i +1].x, curve[i+1].y) ;

			averCurvature +=  fabs( acos( (std::min)( (std::max)( (p3-p2)*(p2-p1)/( (p3-p2).norm()*(p2-p1).norm()), -1.0), 1.0 )  ) / (p3-p1).norm()  ) ;
		}
		averCurvature = averCurvature/curve.size();

		//std::cout<<"averCurvature: "<<averCurvature<<std::endl;



		energy =  255-averGrad + 1000 * averCurvature  ;
	}
	else{
		energy =  255 + 1000 * 1  ;
	}

	if( first_time ){
		first_value = energy ;
		first_time = false ;
	}

	//std::cout<< "f(";
	//for( int i=0; i<X0.size()-1; ++i )
	//	std::cout<<X0[i]<<"," ;
	//std::cout<<X0.back()<<") = "<< (fabs(firts_value) * 2 + energy) / fabs(firts_value) <<std::endl;


	return (energy )/ first_value;
}
