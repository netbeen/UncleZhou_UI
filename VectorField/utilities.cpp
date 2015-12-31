
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

#include "cubicInterpolation.h"
#include "declarations.h"


CvPoint * pts[102400] ;
void drawCurves( IplImage * img , 	std::vector<std::vector<CvPoint>> curves, CvScalar corlor , int thickness = 1) {
	for( int cid = 0; cid < curves.size(); ++cid ){
		if( curves[cid].size() < 2) 
			continue ;
		for(int pid=0; pid<curves[cid].size(); ++pid )
			pts[pid] = &(curves[cid][pid]) ;
		int npts = curves[cid].size() ;
		cvPolyLine( img, pts, &npts,  1, 0,corlor, thickness ) ;
	}
}


double2 point2vector( std::vector<CvPoint> points  )
// estimate the direction vector of discrete points with least square method
// min(k,b) SIGMA( kx+b-y)

{

	// means of x and y
	double x_av = 0.0 ;
	double y_av = 0.0 ;
	for( int i=0; i<points.size(); ++i ){
		x_av += points[i].x ;
		y_av += points[i].y ;
	}
	x_av /= points.size();
	y_av /= points.size();

	// SIGMA xy
	double sigma_xy = 0.0 ;
	for( int i=0; i<points.size(); ++i )
		sigma_xy += points[i].x * points[i].y ;

	// SIGMA x^2
	double sigma_xx = 0.0 ;
	for( int i=0; i<points.size(); ++i )
		sigma_xx += points[i].x * points[i].x ;


	double n = points.size() ;

	double k = ( sigma_xy - n * x_av * y_av ) / std::max( sigma_xx - n * x_av * x_av, 0.01 );



	double2 vec( 1.0, k) ;
	vec.normalize() ;


	return vec ;


}


bool first_random_color = true ;
CvScalar colors[10240];
void drawCurves_randomColor( IplImage * img , std::vector<std::vector<CvPoint>> curves,  int thickness = 1) {

	if( first_random_color ){
		for( int i=0; i<10240; ++i )
			colors[i] = cvScalar( rand()%255,rand()%255,rand()%255 ) ;
		first_random_color = false ;
	}

	for( int cid = 0; cid < curves.size(); ++cid ){

		if( curves[cid].size() < 4) 
			continue ;
		for(int pid=0; pid<curves[cid].size(); ++pid )
			pts[pid] = &(curves[cid][pid]) ;
		int npts = curves[cid].size() ;

		cvPolyLine( img, pts, &npts, 1, 0, colors[cid] , thickness ) ;


	}

}



void drawVectors( IplImage * img , 	LapVectorField &vecfield ) {

	int res = vecfield.resolution ;
	CvSize imgsize = vecfield.imgsize ;
	int majorside = std::max( imgsize.height, imgsize.width ) ;

	int step = std::max( res * 10 / majorside, 1) ; 

	for( int i=0; i<res; i+=step )  for( int j=0; j<res; j+=step ){

		CvPoint p0, p1;
		
		if( imgsize.width < imgsize.height ){
			p0.x = majorside * (double)i / res  - (imgsize.height - imgsize.width)/2 ;
			p0.y = majorside * (double)j / res ; 
		}else{
			p0.x = majorside* (double)i / res ;
			p0.y = majorside * (double)j / res  - (imgsize.width - imgsize.height)/2 ; 

		}
		if( p0.x < 0 || p0.x >= imgsize.width || p0.y <0 || p0.y >= imgsize.height )
			continue;

		double2 p1f = double2(p0.x, p0.y) + vecfield.getVector(p0) * 5 ;
		p1.x = p1f.x ;
		p1.y = p1f.y ;
		
		cvLine( img, p0, p1, cvScalar(0,0,255), 1);
		//cvCircle( img, p0, 1, cvScalar(0,0,255), 1) ;

	}
	

}


void inverseCurve( std::vector<CvPoint> &curve ){
	std::vector<CvPoint> temp = curve ;
	for( int i=0; i<curve.size(); ++i ){
		curve[i] = temp[ curve.size()-1-i] ;
	}

}


void getGradImage( IplImage *img,IplImage *dst, Gradients  &gradmap  ){

	//// convert the image to grey one and smooth it with bilateral filter
	//IplImage *grey = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U, 1 );
	//cvCvtColor(img, grey, CV_RGB2GRAY ) ;


	IplImage *smoothedImage = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U, 3 );
	cvSmooth( img, smoothedImage, CV_GAUSSIAN, 31, 31 , 5 ) ;



	Gradients grads(smoothedImage);

	int width  = img->width ;
	int height = img->height ;

	// compute gradient value
	for( int h=0; h < img->height; ++h ) for( int w=0; w < img->width; ++w){

		if( h==0 || h==height-1 || w==0 || w==width - 1  )
			grads.elems[h][w] = double2(0,0) ;
		else{
			double3 dev(0,0,0);

			dev.x =  CV_IMAGE_ELEM(smoothedImage, uchar, h, (w + 1)*3 + 0 ) -  CV_IMAGE_ELEM(smoothedImage, uchar, h, (w-1) * 3 + 0) ;  
			dev.y =  CV_IMAGE_ELEM(smoothedImage, uchar, h, (w + 1)*3 + 1 ) -  CV_IMAGE_ELEM(smoothedImage, uchar, h, (w-1) * 3 + 1) ;  
			dev.z =  CV_IMAGE_ELEM(smoothedImage, uchar, h, (w + 1)*3 + 2 ) -  CV_IMAGE_ELEM(smoothedImage, uchar, h, (w-1) * 3 + 2) ;  
			grads.elems[h][w].x = dev.norm();

			dev.x =  CV_IMAGE_ELEM(smoothedImage, uchar, h+1, (w )*3 + 0 ) -  CV_IMAGE_ELEM(smoothedImage, uchar, h-1, (w) * 3 + 0) ;  
			dev.y =  CV_IMAGE_ELEM(smoothedImage, uchar, h+1, (w )*3 + 1 ) -  CV_IMAGE_ELEM(smoothedImage, uchar, h-1, (w) * 3 + 1) ;  
			dev.z =  CV_IMAGE_ELEM(smoothedImage, uchar, h+1, (w )*3 + 2 ) -  CV_IMAGE_ELEM(smoothedImage, uchar, h-1, (w) * 3 + 2) ;  
			grads.elems[h][w].y = dev.norm();

		}

		grads.elems[h][w] = grads.elems[h][w]/2 ;

	}

	// scale the gradient to [0,255]
	double maxValue = 0 ;
	for( int h=0; h < img->height; ++h ) for( int w=0; w < img->width; ++w)
		if( grads.elems[h][w].norm() > maxValue ) maxValue = grads.elems[h][w].norm() ;

	for( int h=0; h < img->height; ++h ) for( int w=0; w < img->width; ++w)
		grads.magn[h][w] = (std::min)( grads.elems[h][w].norm()/maxValue , 1.0)  * 255 ;


	// convert the magnitudes of gradients to image
	IplImage *gradImage = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U, 1 );
	for( int h=0; h < img->height; ++h ) for( int w=0; w < img->width; ++w)
		CV_IMAGE_ELEM( gradImage, uchar, h, w ) = grads.magn[h][w] ;

	// save gradients
	gradmap = grads ;

	cvCvtColor( gradImage, dst, CV_GRAY2RGB);

	cvReleaseImage( &smoothedImage);
	cvReleaseImage( &gradImage);
}


// compute totalGradient

std::vector<double> calAverGradient(std::vector<std::vector<CvPoint>>  curves,  Gradients & gramap) {

	std::vector<double> res ;


	for( int cid=0; cid<curves.size(); ++cid ){
		double total_gra = 0;
		std::vector<CvPoint> curve = curves[cid] ;
		for( int id =0; id<curve.size(); ++id )
			total_gra += gramap.magn[ curve[id].y][curve[id].x] ;

		res.push_back(total_gra/curve.size()) ;
	}

	return res ;


} 



// calculate lengths
std::vector<double>  calLength( std::vector<std::vector<CvPoint>> curves ) {

	std::vector<double> res ;


	for( int cid=0; cid<curves.size(); ++cid ){

		std::vector<CvPoint> curve = curves[cid] ;

		double2 p1 = double2( curve[0].x,      curve[0].y ) ;
		double2 p2 = double2( curve.back().x , curve.back().y ) ;

		res.push_back(   (p1-p2).norm() ) ;
	}


	return res;

}



// calculate curvatures 
void getBernsteinCoefficient( std::vector<double> &bc, int n   ) ;
std::vector<double> calAverCurvature( std::vector<std::vector<CvPoint>> curves ) {

	std::vector<std::vector<CvPoint>> bezierCurvess ;
	
	std::vector<double> res ;
	bezierCurvess.clear();
	for( int cid=0; cid<curves.size(); ++cid ){

		double total_curvature = 0;

		std::vector<double2> curve ;
		for( int i=0; i<curves[cid].size(); ++i)
			curve.push_back( double2( curves[cid][i].x,curves[cid][i].y )  ) ;

		int n = curve.size();
		std::vector<double> bc ;
		getBernsteinCoefficient(bc, n ) ;

		// store the Bezier curve
		std::vector<double2> bezierCurves ;
		for( double u=0; u<=1.0; u+=0.01 ){
			double2 p ;
			for( int k = 0; k<n; ++k ){
				p.x += bc[k] * pow(u, k) * pow( 1-u, n-1-k) * curve[k].x ;
				p.y += bc[k] * pow(u, k) * pow( 1-u, n-1-k) * curve[k].y ;
			}	

			bezierCurves.push_back( p ) ;
		}



		for( int id =1; id<bezierCurves.size()-1; ++id ){
			double2 p1 = bezierCurves[id-1] ;
			double2 p2 = bezierCurves[id  ] ;
			double2 p3 = bezierCurves[id+1] ;

			total_curvature +=  fabs( acos( (std::min)( (std::max)( (p3-p2)*(p2-p1)/( (p3-p2).norm()*(p2-p1).norm()), -1.0), 1.0 )  ) / (p3-p1).norm()  ) ;


            if( std::isnan(total_curvature) || !finite( total_curvature ) ){
				std::cout<<"calAverCurvature\n"<<p1.x<<" "<<p1.y<<"\n"
												<<p2.x<<" "<<p2.y<<"\n"
												<<p3.x<<" "<<p3.y<<std::endl;
				system("pause");
			}

		}

		res.push_back(total_curvature/curve.size()) ;


		std::vector<CvPoint> cvBezierCurve ;
		for( int i=0; i<bezierCurves.size(); ++i )
			cvBezierCurve.push_back( cvPoint(bezierCurves[i].x, bezierCurves[i].y));
		bezierCurvess.push_back( cvBezierCurve) ;
	}

	return res ;

}


std::vector<double> calAverSinAngle(std::vector<std::vector<CvPoint>>  curves,  LapVectorField & vecfield){
	
	std::vector<double> res ;

	for( int i=0; i< curves.size(); ++i ){


		double sinDegree = 0.0;
		for( int j=1; j<curves[i].size()-1; ++j ){

			std::vector<CvPoint> points ;
			points.insert(points.begin(), curves[i].begin()+j-1, curves[i].begin()+j+2 );

			double2 dir = point2vector( points ) ;
			//std::cout<<points.size()<<std::endl;
			double2 vec = vecfield.getVector(curves[i][j] ) ;

			sinDegree += sin( acos( std::min( std::max( fabs( vec * dir / (vec.norm() * dir.norm())), 0.0 ), 1.0 )  ) ) ;


		}

		if(  curves[i].size() > 3 )
			res.push_back(  sinDegree / (curves[i].size()-2) );
		else
			res.push_back(1.0) ;


	}


	return res;

}


std::vector<double> calDis2InitCurve(std::vector<std::vector<CvPoint>>  edges, std::vector<std::vector<CvPoint>>  curves ){

	std::vector<double> res ;

	for( int i=0; i< edges.size(); ++i ){

		std::vector<CvPoint> c1 = edges[i] ;
		std::vector<CvPoint> c2 = curves[0] ;
		double dis = 0.0 ;
		for( int j=0; j<c1.size(); ++j ){
			double mindis = 100.0 ;
			double2 p0 = double2( c1[j].x, c1[j].y ) ;

			for( int k=0; k<c2.size(); ++k ){
				double2 p1 = double2( c2[k].x, c2[k].y ) ;
				if( (p0-p1).norm() < mindis )
					mindis = (p0-p1).norm() ;
			}

			dis += mindis ;
		}

		dis /= c1.size() ;

		res.push_back( dis ) ;
	}


	return res;

}
