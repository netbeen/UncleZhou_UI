
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

#include "declarations.h"

using namespace std;


std::vector<double> calAverCurvature( std::vector<std::vector<CvPoint>> curves) ;
std::vector<double>  calLength( std::vector<std::vector<CvPoint>> curves ) ;
std::vector<double> calAverGradient(std::vector<std::vector<CvPoint>>  curves,  Gradients & gramap) ;
std::vector<double> calAverSinAngle(std::vector<std::vector<CvPoint>>  curves,  LapVectorField & vecfield) ;
std::vector<double> calDis2InitCurve(std::vector<std::vector<CvPoint>>  edges, std::vector<std::vector<CvPoint>>  curves ) ;


// this file define the function of detecting short edges

void getEdgels( IplImage *img,  IplImage *edgeImage){


	// convert the image to grey one and smooth it with bilateral filter
	//IplImage *grey = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U, 1 );
	//cvCvtColor(img, grey, CV_RGB2GRAY ) ;

	IplImage *smoothedImage = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U, 3 );


	//cvSmooth( img,smoothedImage, CV_BILATERAL, 0, 0, 100, 3 ) ;

	cvSmooth( img, smoothedImage, CV_GAUSSIAN, 15, 15 , 5 ) ;

	//cvNamedWindow( "grey");
	//cvShowImage( "grey", grey);
	//cvNamedWindow( "smoothedImage");
	//cvShowImage( "smoothedImage", smoothedImage);

	cvCanny( smoothedImage, edgeImage,100, 200 , 5);

	//cvReleaseImage( &grey );
	cvReleaseImage( &smoothedImage );

}


int neibourCounter( IplImage *edgeImage, CvPoint p, std::vector<CvPoint> &neis ){


	int num = 0;
	neis.clear() ;


	int width  = edgeImage->width ;
	int height = edgeImage->height ;

	if( p.y==0 || p.y==height-1 || p.x==0 || p.x==width - 1  )
		return 0;

	if(  CV_IMAGE_ELEM(edgeImage, uchar, p.y-1, p.x-1 )  ) { neis.push_back(cvPoint(p.x-1, p.y-1) );  num++ ; }
	if(  CV_IMAGE_ELEM(edgeImage, uchar, p.y,   p.x-1  )  ) { neis.push_back(cvPoint(p.x-1, p.y  ) );  num++ ; }
	if(  CV_IMAGE_ELEM(edgeImage, uchar, p.y+1, p.x-1 )  ) { neis.push_back(cvPoint(p.x-1, p.y+1) );  num++ ; }
	if(  CV_IMAGE_ELEM(edgeImage, uchar, p.y-1, p.x+1 )  ) { neis.push_back(cvPoint(p.x+1, p.y-1) );  num++ ; }
	if(  CV_IMAGE_ELEM(edgeImage, uchar, p.y,   p.x+1   )  ) { neis.push_back(cvPoint(p.x+1, p.y  ) );  num++ ; }
	if(  CV_IMAGE_ELEM(edgeImage, uchar, p.y+1, p.x+1 )  ) { neis.push_back(cvPoint(p.x+1, p.y+1) );  num++ ; }
	if(  CV_IMAGE_ELEM(edgeImage, uchar, p.y+1, p.x )  ) { neis.push_back(cvPoint(p.x  , p.y+1) );  num++ ; }
	if(  CV_IMAGE_ELEM(edgeImage, uchar, p.y-1, p.x  )  ) { neis.push_back(cvPoint(p.x  , p.y-1) );  num++ ; }

	return num ;
}

void linkEdge(  IplImage *edgeImage, Edges &edges, int lengthThresh = 5) { // edgels are presented by white pixels


	//cvNamedWindow( "edgeImage") ;
	//cvShowImage("edgeImage", edgeImage) ;
	//while( cvWaitKey(10) == -1 ){}

	int width  = edgeImage->width ;
	int height = edgeImage->height ;

	std::vector<CvPoint> neis ;
	
	
	// trace the contour

	bool there_is_edgels = true ;

	while( there_is_edgels ){

		there_is_edgels = false ;

		int seed_i, seed_j ;

		// find an edgel
		for( int h=1; h < height-1; ++h ) for( int w=1; w < width-1; ++w){

			if(  CV_IMAGE_ELEM(edgeImage, uchar, h, w ) ){
				there_is_edgels = true ;
				seed_i = w ;
				seed_j = h ;
				break ;
			}

		}

		// if edgel not found. terminate
		if( !there_is_edgels )
			break ;

		// set (seed_i,seed_j) as seed point, trace a contour until no neighbor edgels can be found OR there are more than 1 neighbor
		 
		if( neibourCounter(edgeImage,cvPoint(seed_i,seed_j), neis ) == 1 ){

			edges.elems.resize( edges.elems.size() + 1 ) ;
			std::vector<CvPoint> &contour = edges.elems.back() ;
			
			contour.push_back( cvPoint(seed_i,seed_j) ) ;
			contour.push_back( neis[0] ) ;
			CV_IMAGE_ELEM(edgeImage, uchar, seed_j, seed_i ) = 0;
			CV_IMAGE_ELEM(edgeImage, uchar, neis[0].y, neis[0].x ) = 0;


			while( 1 ){
				int nnum = neibourCounter(edgeImage,neis[0], neis ) ; 
				if( nnum == 1 ){
					contour.push_back( neis[0] ) ;
					CV_IMAGE_ELEM(edgeImage, uchar, neis[0].y, neis[0].x ) = 0;
				}else if( nnum == 2 ){

					if( abs(neis[0].x - neis[1].x) +  abs(neis[0].y - neis[1].y) == 1 ){
						if( abs( contour.back().x - neis[1].x) +  abs( contour.back().y - neis[1].y) == 1 )
							neis[0] = neis[1];

						contour.push_back( neis[0] ) ;
						CV_IMAGE_ELEM(edgeImage, uchar, neis[0].y, neis[0].x ) = 0;
					}
					else
						break;


				}else
					break;
			}


		}else  if( neibourCounter(edgeImage,cvPoint(seed_i,seed_j), neis) == 2 ){

			edges.elems.resize( edges.elems.size() + 1 ) ;
			std::vector<CvPoint> &contour = edges.elems.back() ;

			contour.push_back( cvPoint(seed_i,seed_j) ) ;
			contour.push_back( neis[0] ) ;
			contour.insert( contour.begin(), neis[1] ) ;
			CV_IMAGE_ELEM(edgeImage, uchar, seed_j, seed_i ) = 0;
			CV_IMAGE_ELEM(edgeImage, uchar, neis[0].y, neis[0].x ) = 0;
			CV_IMAGE_ELEM(edgeImage, uchar, neis[1].y, neis[1].x ) = 0;

			
			// search forward
			while( 1 ){
				int nnum = neibourCounter(edgeImage,neis[0], neis ) ; 
				if( nnum == 1 ){
					contour.push_back( neis[0] ) ;
					CV_IMAGE_ELEM(edgeImage, uchar, neis[0].y, neis[0].x ) = 0;
				}else if( nnum == 2 ){

					if( abs(neis[0].x - neis[1].x) +  abs(neis[0].y - neis[1].y) == 1 ){
						if( abs( contour.back().x - neis[1].x) +  abs( contour.back().y - neis[1].y) == 1 )
							neis[0] = neis[1];

						contour.push_back( neis[0] ) ;
						CV_IMAGE_ELEM(edgeImage, uchar, neis[0].y, neis[0].x ) = 0;
					}
					else
						break;


				}else
					break;
			}

			neis.clear() ;
			neis.push_back(contour[0] ) ;

			// search backward
			while( 1 ){
				int nnum = neibourCounter(edgeImage,neis[0], neis ) ; 
				if( nnum == 1 ){
					contour.insert( contour.begin(), neis[0] ) ;;
					CV_IMAGE_ELEM(edgeImage, uchar, neis[0].y, neis[0].x ) = 0;
				}else if( nnum == 2 ){

					if( abs(neis[0].x - neis[1].x) +  abs(neis[0].y - neis[1].y) == 1 ){
						if( abs( contour[0].x - neis[1].x) +  abs( contour[0].y - neis[1].y) == 1 )
							neis[0] = neis[1];

						contour.insert( contour.begin(), neis[0] ) ;
						CV_IMAGE_ELEM(edgeImage, uchar, neis[0].y, neis[0].x ) = 0;
					}
					else
						break;


				}else
					break;
			}


		}else{

			CV_IMAGE_ELEM(edgeImage, uchar, seed_j, seed_i ) = 0;
		}



	}

	// delete too short edge

	for( int i=0; i<edges.elems.size() ; ++i )
		if( edges.elems[i].size() < lengthThresh  ){
			 edges.elems.erase(  edges.elems.begin() + i ) ;
			 i--;
		}

}




void filterLinkedEdge( Edges &edges, LapVectorField &vecfild, Gradients &gradmap, std::vector<std::vector<CvPoint>> &curves, double threshhold = 1.0 ){

	if( edges.elems.size() < 3 )
		return ;

	std::vector<std::vector<CvPoint>> &eds = edges.elems ;
	// calculate 4 terms for sorting edges
	std::vector<double> averGradient = calAverGradient(eds, gradmap) ;
	std::vector<double> sinAgl = calAverSinAngle(eds, vecfild) ;
	std::vector<double> lengths = calLength( eds ) ;
	std::vector<double> averCurvatures = calAverCurvature( eds) ;
	std::vector<double> distances = calDis2InitCurve(eds, curves ) ;

	// scale them
	double maxGra = averGradient[0] ;
	double maxLen = lengths[0] ;
	double maxCur = averCurvatures[0] ;
	double maxDis = distances[0] ;
	for( int i=1; i<eds.size(); ++i ){
		if( averGradient[i] > maxGra) maxGra = averGradient[i] ;
		if( lengths[i] > maxLen) maxLen = lengths[i] ;
		if( averCurvatures[i] > maxCur) maxCur = averCurvatures[i] ;
		if( distances[i] > maxDis) maxDis = distances[i] ;
	}

	for( int i=0; i<eds.size(); ++i ){
		averGradient[i] /= maxGra ;
		lengths[i] /= maxLen ;
		averCurvatures[i] /= maxCur ;
		distances[i] /= maxDis ;
	}


	// score them
	std::vector<double> scores ;
	for( int i=0; i<eds.size(); ++i ){
		double scr =  averGradient[i] -  sinAgl[i] + lengths[i] - averCurvatures[i] - distances[i] + 1.0 ;

		if( eds[i].size() <3 )
			scr = 0.0;
		scores.push_back( scr ) ;

		

        if( std::isnan(scr) || !finite( scr ) ){
			std::cout<<"\n"<<averGradient[i]<<"\n"<<sinAgl[i]<<"\n"<<averCurvatures[i]<<"\n"<<distances[i]<<std::endl;
			system("pause");
		}

	}

	// resort edges
	for( int i=0; i<scores.size(); ++i){
		for( int j=i+1; j<scores.size(); ++j){
			if( scores[j] > scores[i] ){
				double tem = scores[j] ;
				scores[j] = scores[i];
				scores[i] = tem ;

				std::vector<CvPoint> temEdge = eds[j] ;
				eds[j] = eds[i];
				eds[i] = temEdge ;
			}
		}
	}

	//// detect where to break
	//int breakpoint_id = 0 ;
	//double gap = scores[0]-scores[1];
	//for( int i = 1; i<eds.size(); ++i ){
	//	if( scores[i] - scores[i+1] > gap   ){
	//		gap = scores[i] - scores[i+1] ;
	//		breakpoint_id = i ;
	//	}
	//}

	//eds.erase( eds.begin() + breakpoint_id + 1, eds.end() );

	for( int i=0; i<scores.size(); ++i ){
		if(  scores[i] < threshhold  ){
			eds.erase( eds.begin() + i , eds.end());
			break;
		}
	}

	//for( int i=0; i<scores.size(); ++i )
	//	std::cout << scores[i] << " " ;
	//std::cout<<std::endl;
 

		// delete too short edge
	for( int i=0; i<edges.elems.size() ; ++i )
		if( edges.elems[i].size() < 10  ){
			edges.elems.erase(  edges.elems.begin() + i ) ;
			i--;
		}

	// inverse edges to make them consistent with the direction of vector field
	void inverseCurve( std::vector<CvPoint> &curve ) ;
	for( int i=0; i< edges.elems.size(); ++i ){
		double2 p0 = double2( edges.elems[i][0].x, edges.elems[i][0].y) ;
		double2 p1 = double2( edges.elems[i].back().x, edges.elems[i].back().y ) ;

		double2 dir = p1 - p0 ;
		double2 vec = vecfild.getVector(edges.elems[i][edges.elems[i].size()/2] ) ;

		if(  vec * dir < 0 )
			inverseCurve( edges.elems[i] ) ;

	}


	// reduce edges 
	for( int i=0; i< edges.elems.size(); ++i ){
		for( int j=0; j<edges.elems[i].size(); ++j ){

			if( j%3 != 0 )
				edges.elems[i][j].x = edges.elems[i][j].y = 0;
		}
	}

	for( int i=0; i< edges.elems.size(); ++i )
		for( int j=0; j<edges.elems[i].size(); ++j )
			if( edges.elems[i][j].x ==0 && edges.elems[i][j].y==0 ){
				edges.elems[i].erase( edges.elems[i].begin()+j ) ;
				j--;
			}




}

double2 point2vector( std::vector<CvPoint> points  ) ;

void filterLinkedEdge_v2( Edges &edges, LapVectorField &vecfild ){

	// filter the edges with average intersecting angle with the vector field

	for( int i=0; i< edges.elems.size(); ++i ){


		double sinDegree = 0;
		//for( int j=1; j<edges.elems[i].size()-1; ++j ){

		//	std::vector<CvPoint> points ;
		//	points.insert(points.begin(), edges.elems[i].begin()+j-1, edges.elems[i].begin()+j+2 );

		//	double2 dir = point2vector( points ) ;
		//	//std::cout<<points.size()<<std::endl;
		//	double2 vec = vecfild.getVector(edges.elems[i][j] ) ;

		//	sinDegree += sin( acos( std::min( std::max( fabs( vec * dir / (vec.norm() * dir.norm())), 0.0 ), 1.0 )  ) ) ;


		//	//std::cout<<"dir: "<<dir.x<<","<<dir.y<<std::endl;


		//}



		double2 dir = point2vector( edges.elems[i] ) ;
		//std::cout<<points.size()<<std::endl;
		double2 vec = vecfild.getVector(edges.elems[i][ edges.elems[i].size()/2 ] ) ;

		sinDegree = sin( acos( (std::min) ( (std::max) ( fabs( vec * dir / (vec.norm() * dir.norm())), 0.0 ), 1.0 )  ) ) ;


		// if the average included angle between vector field and the curve are greater than 30 degree, then delete it
		if(  sinDegree  > sin( 3.14 * 30 /180) ){

			edges.elems.erase( edges.elems.begin()+i );
			i--;
		}


	}

	// delete too short edge
	for( int i=0; i<edges.elems.size() ; ++i )
		if( edges.elems[i].size() < 10  ){
			edges.elems.erase(  edges.elems.begin() + i ) ;
			i--;
		}

	// inverse edges to make them consistent with the direction of vector field
	void inverseCurve( std::vector<CvPoint> &curve ) ;
	for( int i=0; i< edges.elems.size(); ++i ){
		double2 p0 = double2( edges.elems[i][0].x, edges.elems[i][0].y) ;
		double2 p1 = double2( edges.elems[i].back().x, edges.elems[i].back().y ) ;

		double2 dir = p1 - p0 ;
		double2 vec = vecfild.getVector(edges.elems[i][edges.elems[i].size()/2] ) ;

		if(  vec * dir < 0 )
			inverseCurve( edges.elems[i] ) ;

	}


	// reduce edges 
	for( int i=0; i< edges.elems.size(); ++i ){
		for( int j=0; j<edges.elems[i].size(); ++j ){

			if( j%3 != 0 )
				edges.elems[i][j].x = edges.elems[i][j].y = 0;
		}
	}

	for( int i=0; i< edges.elems.size(); ++i )
		for( int j=0; j<edges.elems[i].size(); ++j )
			if( edges.elems[i][j].x ==0 && edges.elems[i][j].y==0 ){
				edges.elems[i].erase( edges.elems[i].begin()+j ) ;
				j--;
			}

}


void filterLinkedEdge_v1( Edges &edges, LapVectorField &vecfild ){




	for( int i=0; i< edges.elems.size(); ++i ){

		for( int j=2; j<edges.elems[i].size()-2; ++j ){

			std::vector<CvPoint> points ;
			points.insert(points.begin(), edges.elems[i].begin()+j-2, edges.elems[i].begin()+j+2 );

			double2 dir = point2vector( points ) ;

			double2 vec = vecfild.getVector(edges.elems[i][j] ) ;

			// if the included angle between vector field and the curve at the point <i,j> are greater than 30 degree, then break it at this point, and put the broken edge at the end
			if(  acos( fabs( vec * dir / (vec.norm() * dir.norm())) )   > 3.14 * 30 /180){
				std::vector<CvPoint> backedge ;

				backedge.insert(backedge.begin(), edges.elems[i].begin()+j, edges.elems[i].end() );
				edges.elems[i].erase(edges.elems[i].begin()+j, edges.elems[i].end());
				edges.elems.push_back(backedge) ;
			}

		}

	}





}
