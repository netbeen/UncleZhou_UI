#include <stdlib.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "LapVectorField.h"
#include <bfgs.h> 
#include <float.h>
#include "cubicInterpolation.h"
#include "declarations.h"
#include <opencv2/opencv.hpp>

std::vector<std::vector<CvPoint> > curves ;

bool displayEdge = false ;
bool displayVector = true ;
bool displayCurve = true ;
bool displaySourceimage = true ;
bool displayVecOrSnake = true ;
bool displayEdges = true ;

enum appstate {
	_draw_curves ,
	_show_vectors,
	_edges_linkeded,
	_edges_filtered,
	_new_field,
	_got_result
};

appstate AppState =  _draw_curves;

#define _patch_size 3

int _fissures_thickness = 3 ;
//#define _no_edgexel

IplImage *pCannyImg ;
IplImage *sourceImage ;
IplImage *sourceImage_beforeCanny ;
IplImage *regions ;

#define min( x, y)  (std::min)(x, y)
#define max( x, y)  (std::max)(x, y)

Edges ctrpoints ;
LapVectorField vecfield ;
Gradients gradmap ;

Edges edges ;

double energy_function(std::vector<double> X0, const Edges &ctrpoints, const Edges & edges, Gradients &grads, LapVectorField &vecfild ) ;

class EnergyFunction {
public:
	double operator() (splab::Vector<double> X){
		// copy the input X 
		std::vector<double> X0 ;
		for( int i=0; i<X.size(); ++i )
			X0.push_back(X[i]) ;
		double energy = energy_function(X0, ctrpoints,edges, gradmap, vecfield ) ;
		std::cout<< "f(";
        for( size_t i=0; i<X0.size()-1; ++i )
			std::cout<<X0[i]<<"," ;
		std::cout<<X0.back()<<") = "<< energy <<std::endl;
		return energy ;
	}
	splab::Vector<double> grad(splab::Vector<double> X){
		// copy the input X 
		std::vector<double> cppX ;
		for( int i=0; i<X.size(); ++i )
			cppX.push_back(X[i]) ;
		splab::Vector<double>  g(cppX.size());
		double e =  energy_function(cppX, ctrpoints,edges, gradmap, vecfield ) ;
        for( size_t i=0; i<cppX.size(); ++i ){
			std::vector<double> cppX2 = cppX;
			double step = 3.0  ;
			cppX2[i] += step ;
			g[i] = (energy_function( cppX2,ctrpoints, edges, gradmap, vecfield ) - e ) / step  ;
		}
		std::cout<< "g[] = [";
		for( int i=0; i<g.size()-1; ++i )
			std::cout<<g[i]<<"," ;
		std::cout<<g[X.size()-1]<<"]  "<<  std::endl;
		return g;
	}
};


#include <stdio.h>
#include "lbfgs.h"

static lbfgsfloatval_t evaluate(
    void *instance,
    const lbfgsfloatval_t *x,
    lbfgsfloatval_t *g,
    const int n,
    const lbfgsfloatval_t step
    )
{
	// copy the input X 
	std::vector<double> cppX ;
	for( int i=0; i<n; ++i )
		cppX.push_back(x[i]) ;
	std::cout<< "f(";
    for( size_t i=0; i<cppX.size()-1; ++i )
		std::cout<<cppX[i]<<"," ;
	std::cout<<cppX.back()<<")  "<<  std::endl;
	// evaluate fx
	double e = energy_function(cppX, ctrpoints,edges, gradmap, vecfield ) ;
	// evaluate g
	std::vector<double> cppG(n) ;
	for( int i=0; i<n; ++i ){
		std::vector<double> cppX2 = cppX;
		double step = 3.0  ;
		cppX2[i] += step ;
		cppG[i] = (energy_function( cppX2,ctrpoints, edges, gradmap, vecfield ) - e ) / step  ;
        if( isnan(cppG[i]) || !finite( cppG[i] ) ){
			std::cout<<"\n"<<energy_function( cppX2,ctrpoints, edges, gradmap, vecfield )<<"\n"<<e<<"\n"<<std::endl;
			system("pause");
		}
	}
	// write g
	for( int i=0; i<n; ++i )
		g[i] = cppG[i] ;
	std::cout<< "g[] = [";
    for( size_t i=0; i<cppG.size()-1; ++i )
		std::cout<<cppG[i]<<"," ;
	std::cout<<cppG.back()<<"]  "<<  std::endl;
	return e ;
}

static int progress(
    void *instance,
    const lbfgsfloatval_t *x,
    const lbfgsfloatval_t *g,
    const lbfgsfloatval_t fx,
    const lbfgsfloatval_t xnorm,
    const lbfgsfloatval_t gnorm,
    const lbfgsfloatval_t step,
    int n,
    int k,
    int ls
    )
{
    printf("Iteration %d:\n", k);
    printf("  fx = %f, x[0] = %f, x[1] = %f\n", fx, x[0], x[1]);
    printf("  xnorm = %f, gnorm = %f, step = %f\n", xnorm, gnorm, step);
    printf("\n");
    return 0;
}

void drawCurves( IplImage * img , 	std::vector<std::vector<CvPoint> > curves, CvScalar corlor , int thickness = 1) ;
void drawVectors( IplImage * img , 	LapVectorField &vecfield );
void drawCurves_randomColor( IplImage * img , std::vector<std::vector<CvPoint> > curves,  int thickness = 1) ;
void curve_interplation( Edges &, Edges &) ;
void getUniformCubicBSpline( Edges &input,Edges &output );
void getBezier( Edges &input, Edges &output, int step = 3 );

void mouseCallback(int event, int x, int y, int flag, void *param){
	std::vector<CvPoint> curve ;
	CvPoint p;
	p.x = x; p.y=y;
	switch(event){
	case CV_EVENT_LBUTTONDOWN :
		// add a new curve or fissure
		if( AppState == _draw_curves ){
			curves.push_back(curve) ;
			curves.back().push_back(p);
		}
		break ;
	case CV_EVENT_LBUTTONUP :
		//filter these too short curves or fissures
		if( AppState == _draw_curves ){
			//break ;
			if( curves.size()!= 0 && curves.back().size() < 4 )
				curves.erase(curves.end()-1) ;
		}
		break;
	case CV_EVENT_RBUTTONDOWN:
		if( AppState == _draw_curves ){
			curves.push_back(curve) ;
		}
		break ;
	case CV_EVENT_MOUSEMOVE:
		if( flag & CV_EVENT_FLAG_LBUTTON ){
			CvPoint p;
			p.x = x; p.y=y;
			if( AppState == _draw_curves ){
				if( curves.back().size() == 0 || abs( curves.back().back().x - x) + abs( curves.back().back().y - y) >= 10 )
					curves.back().push_back( p ) ;
			}
		}
	}
}

void getEdgels( IplImage *img,  IplImage *edgeImage) ;
void linkEdge(  IplImage *edgeImage, Edges &edges, int lengthThresh = 5) ;
void filterLinkedEdge( Edges &edges, LapVectorField &vecfild, Gradients &gradmap, std::vector<std::vector<CvPoint> > &curves, double threshhold = 1.0 );
void getGradImage( IplImage *img,IplImage *dst, Gradients  &gradmap  ) ;

int main2(int argc, char **argv) {
    std::cout<<"  LBFGSERR_MAXIMUMLINESEARCH == "<<   LBFGSERR_MAXIMUMLINESEARCH << std::endl;
	std::string sourceImageName ;
    sourceImageName = std::string("/home/netbeen/桌面/周叔项目/imgSalCurDetector/test.jpg") ;
    sourceImage = cvLoadImage(sourceImageName.c_str(), CV_LOAD_IMAGE_COLOR) ;
	cvNamedWindow( "curveDetect") ;
	cvSetMouseCallback( "curveDetect", mouseCallback ) ;
	// calculate edgels and display it on the source image
	IplImage *edgelsImage = cvCreateImage(cvGetSize(sourceImage),IPL_DEPTH_8U, 1 );
	getEdgels( sourceImage,  edgelsImage ) ;
	// get gradient
	IplImage *gradImage = cvCreateImage(cvGetSize(sourceImage),IPL_DEPTH_8U, 3 );
	getGradImage( sourceImage, gradImage, gradmap );
	Edges dstSnakeCurves ;
	Edges dstVectorCurves ;
	int thresh_t100 = 100;
	cvCreateTrackbar("threshold", "curveDetect",&thresh_t100, 400  ) ;
	Edges edges_bkup ;
	while(1){
		IplImage *screen = cvCloneImage( sourceImage ) ;
		if( !displaySourceimage ){
            std::cout << "NO, 我删掉了一行" << std::endl;
            //cvCopyImage(gradImage,screen);
            //////////////////////////////////////////////////////////////////////我删掉了一行//////////////////////////////////////
		}
		if( displayEdge )
			cvCvtColor(edgelsImage, screen, CV_GRAY2RGB) ;
		if( displayCurve )
			drawCurves( screen , curves, cvScalar(255,0,255),2) ;
		if( 0 && curves.size() && curves[0].size() > 10 ){
			Edges guideCurve, spline ;
			guideCurve.elems = curves ;
			getUniformCubicBSpline(guideCurve, spline) ;
			drawCurves( screen , spline.elems, cvScalar(0,255,0),2) ;
		}
		if( AppState != _draw_curves && displayVector)
			drawVectors( screen, vecfield ) ;
        if( AppState == _got_result )
			if( displayVecOrSnake )
				drawCurves(screen, dstSnakeCurves.elems ,  cvScalar(0,255,255),2) ;
			else
				drawCurves(screen, dstVectorCurves.elems ,  cvScalar(0,255,0),2) ;
		if( AppState >= _edges_linkeded ){
			edges = edges_bkup ;
			filterLinkedEdge(edges, vecfield, gradmap, curves, thresh_t100 * 0.01 ) ;
		}
		if( displayEdges )
			drawCurves_randomColor( screen, edges.elems, 4 ) ;
		cvShowImage("curveDetect", screen );
		unsigned char key = cvWaitKey(50) ;
		if( key !=255)
			std::cout<<(unsigned)key ;
		if( key == 99 ) break; // press c to exit
		if( key == 27 )  {  // press ESC to delete the last curve drawn by user
			if( AppState == _draw_curves && curves.size()!=0)
				curves.erase( curves.end()-1) ;
		}
		if( key == 114 ){ // press r to reset
			curves.clear() ;
			AppState = _draw_curves ;
			getEdgels( sourceImage,  edgelsImage ) ;
			edges.elems.clear();
			dstVectorCurves.elems.clear();
			dstSnakeCurves.elems.clear();
			ctrpoints.elems.clear();
		}
        if( key == 9 ) // press TAB to switch between displaying sourceImage and edges
			displayEdge = !displayEdge ;
        if( key == 113 )  //press q
			displayVector = !displayVector ;
        if( key == 101 )  //press e
			displaySourceimage = !displaySourceimage ;
        if( key == 97 )  //press a
			displayVecOrSnake = !displayVecOrSnake ;
        if( key == 122 )  //press z
			displayEdges = !displayEdges ;
        if( key == 119 ) // press TAB to switch between displaying sourceImage and edges
			displayCurve = !displayCurve ;
		if( key == 115 ){ // press s
			if( AppState == _draw_curves ){ 
				AppState = _show_vectors ;
				 // calculate vector field
				std::cout<<"begin calculate the vector field" << std::endl;
                vecfield.compute( curves, cvSize( sourceImage->width,sourceImage->height), 400 ) ;
				std::cout<<"vector field calculated" << std::endl;
                std::cout << "验证，是main2"  << std::endl;
                //cvZero( bandMsk ) ;
                //drawCurves( bandMsk, curves, cvScalar(1, 1,1), 80 ) ;
                //cvMul(bandMsk, edgelsImage, edgelsImage ) ;
            }
            /*else if( AppState == _show_vectors ){
				AppState = _edges_linkeded ;
				linkEdge(edgelsImage, edges, 20) ;
				edges_bkup = edges ;
            }
            else if( AppState == _edges_linkeded ){
				AppState = _edges_filtered ;
				filterLinkedEdge( edges,  vecfield, gradmap, curves, thresh_t100 * 0.01 ) ;
            }
            else if( AppState == _edges_filtered ){
				AppState = _new_field ;
                std::vector<std::vector<CvPoint> > allCurves = edges.elems;
                for( size_t i=0; i<curves.size(); ++i )
					allCurves.push_back(curves[i] ) ;
				// smooth edges
				std::cout<<"begin calculate the new vector field" << std::endl;
				vecfield.compute( edges.elems, cvSize( sourceImage->width,sourceImage->height), 400 ) ;
				std::cout<<"new vector field calculated" << std::endl;
            }
            else if( AppState == _new_field ){
				AppState = _got_result ;
				// calculate ctrpoints
				ctrpoints.elems = curves ;
				CvPoint prepoint = ctrpoints.elems[0][0] ;
                for( size_t j=1; j<ctrpoints.elems[0].size()-1; ++j ){
					if( sqrt( (double)((prepoint.x - ctrpoints.elems[0][j].x) *(prepoint.x - ctrpoints.elems[0][j].x) 
						+ (prepoint.y - ctrpoints.elems[0][j].y) *(prepoint.y - ctrpoints.elems[0][j].y)) ) > 20  ){
							prepoint = ctrpoints.elems[0][j] ;
					}
					else{
						ctrpoints.elems[0].erase( ctrpoints.elems[0].begin() + j ) ;
						j-- ;
					}
                }
                #define _lbfgs 0
				if( _lbfgs ){
					// Snakes
					extern bool consider_vecfield ;
					consider_vecfield = false ;
					std::cout<<"begin optimization: consider_vecfield = false"<<std::endl;
					int ret = 0;
					lbfgsfloatval_t fx;
					lbfgsfloatval_t *x = lbfgs_malloc( ctrpoints.elems[0].size() * 2 );
					lbfgs_parameter_t param;
					if (x == NULL) {
						printf("ERROR: Failed to allocate a memory block for variables.\n");
						return 1;
					}
                    // Initialize the variables.
                    for (size_t i = 0;i < ctrpoints.elems[0].size() * 2 ; i++)
						x[i] = 0;
					lbfgs_parameter_init(&param);
					param.max_linesearch = 100 ;
					ret = lbfgs(ctrpoints.elems[0].size() * 2, x, &fx, evaluate, progress, NULL, &param);
					printf("L-BFGS optimization terminated with status code = %d\n", ret);
					printf("  fx = %f, x[0] = %f, x[1] = %f\n", fx, x[0], x[1]);
					// apply the optimization result to ctrpoints
					Edges dstctrpoints = ctrpoints ;
                    for( size_t i=0; i<dstctrpoints.elems[0].size(); ++i ){
						dstctrpoints.elems[0][i].x += x[ i ] ;
						dstctrpoints.elems[0][i].y += x[ i + dstctrpoints.elems[0].size() ] ;
					}
					curve_interplation(dstctrpoints, dstSnakeCurves ) ;
					// field guided snakes
					consider_vecfield = true ;
					std::cout<<"begin optimization: consider_vecfield = true"<<std::endl;
                    for (size_t i = 0;i < ctrpoints.elems[0].size() * 2 ; i++)
						x[i] = 0;
					lbfgs_parameter_init(&param);
					param.max_linesearch = 100 ;
					ret = lbfgs(ctrpoints.elems[0].size() * 2, x, &fx, evaluate, progress, NULL, &param);
					printf("L-BFGS optimization terminated with status code = %d\n", ret);
					printf("  fx = %f, x[0] = %f, x[1] = %f\n", fx, x[0], x[1]);
					// apply the optimization result to ctrpoints
					dstctrpoints = ctrpoints ;
                    for( size_t i=0; i<dstctrpoints.elems[0].size(); ++i ){
						dstctrpoints.elems[0][i].x += x[ i ] ;
						dstctrpoints.elems[0][i].y += x[ i + dstctrpoints.elems[0].size() ] ;
					}
					curve_interplation(dstctrpoints, dstVectorCurves ) ;
					lbfgs_free(x);
				}
				else{
					// Snakes
					extern bool consider_vecfield ;
					consider_vecfield = false ;
					std::cout<<"begin optimization: consider_vecfield = false"<<std::endl;
					splab::BFGS<double, EnergyFunction> solver;
					splab::Vector<double> x0;
					x0.resize(ctrpoints.elems[0].size() * 2) ;
					for( int i=0; i<x0.size(); ++i )
						x0[i] = 0 ;
					EnergyFunction ef ;
					solver.optimize(ef, x0, 1e-10, 1000 ) ;
					x0 = solver.getOptValue() ;
					std::cout<<"func["<<x0<<"] = "<<solver.getFuncMin()<<std::endl;
					std::vector<double> X;
					for( int i=0; i<x0.size(); ++i )
						X.push_back(x0[i]);
					// apply the optimization result to ctrpoints
					Edges dstctrpoints = ctrpoints ;
                    for( size_t i=0; i<dstctrpoints.elems[0].size(); ++i ){
						dstctrpoints.elems[0][i].x += X[ i ] ;
						dstctrpoints.elems[0][i].y += X[ i + dstctrpoints.elems[0].size() ] ;
					}
					getUniformCubicBSpline(dstctrpoints, dstSnakeCurves ) ;
					// field guided Snakes
					consider_vecfield = true ;
					std::cout<<"begin optimization: consider_vecfield = true"<<std::endl;
					for( int i=0; i<x0.size(); ++i )
						x0[i] = 0 ;
					solver.optimize(ef, x0, 1e-10, 1000 ) ;
					x0 = solver.getOptValue() ;
					std::cout<<"func["<<x0<<"] = "<<solver.getFuncMin()<<std::endl;
					// apply the optimization result to ctrpoints
                    for( size_t i=0; i<ctrpoints.elems[0].size(); ++i ){
						ctrpoints.elems[0][i].x += x0[ i ] ;
						ctrpoints.elems[0][i].y += x0[ i + ctrpoints.elems[0].size() ] ;
					}
					getUniformCubicBSpline(ctrpoints, dstVectorCurves) ;
				}
            }*/
		}
	}
	return 0;
}

void kangxueCalcVectorField(IplImage* sourceImage,IplImage* screen, std::vector<std::vector<CvPoint> > curves) {
    cvSetMouseCallback( "curveDetect", mouseCallback ) ;
    // calculate edgels and display it on the source image
    IplImage *edgelsImage = cvCreateImage(cvGetSize(sourceImage),IPL_DEPTH_8U, 1 );
    getEdgels( sourceImage,  edgelsImage ) ;
    // get gradient
    IplImage *gradImage = cvCreateImage(cvGetSize(sourceImage),IPL_DEPTH_8U, 3 );
    getGradImage( sourceImage, gradImage, gradmap );
    Edges dstSnakeCurves ;
    Edges dstVectorCurves ;
    int thresh_t100 = 100;
    cvCreateTrackbar("threshold", "curveDetect",&thresh_t100, 400  ) ;
    Edges edges_bkup ;

    //screen = cvCloneImage( sourceImage ) ;        //已经注释掉了，需要在外部进行cpoy，防止指针出错
    vecfield.compute( curves, cvSize( sourceImage->width,sourceImage->height), 400 ) ;
    //drawCurves( screen , curves, cvScalar(255,0,255),2) ;
    drawVectors( screen, vecfield ) ;
    return;

    while(1){
        screen = cvCloneImage( sourceImage ) ;
        if( displayEdge )
            cvCvtColor(edgelsImage, screen, CV_GRAY2RGB) ;
        if( displayCurve )
            drawCurves( screen , curves, cvScalar(255,0,255),2) ;
        if( 0 && curves.size() && curves[0].size() > 10 ){
            Edges guideCurve, spline ;
            guideCurve.elems = curves ;
            getUniformCubicBSpline(guideCurve, spline) ;
            drawCurves( screen , spline.elems, cvScalar(0,255,0),2) ;
        }
        if( AppState != _draw_curves && displayVector)
            drawVectors( screen, vecfield ) ;
        if( AppState == _got_result )
            if( displayVecOrSnake )
                drawCurves(screen, dstSnakeCurves.elems ,  cvScalar(0,255,255),2) ;
            else
                drawCurves(screen, dstVectorCurves.elems ,  cvScalar(0,255,0),2) ;
        if( AppState >= _edges_linkeded ){
            edges = edges_bkup ;
            filterLinkedEdge(edges, vecfield, gradmap, curves, thresh_t100 * 0.01 ) ;
        }
        if( displayEdges )
            drawCurves_randomColor( screen, edges.elems, 4 ) ;
        cvShowImage("curveDetect", screen );
        unsigned char key = cvWaitKey(50) ;
        if( key == 99 ) break; // press c to exit
        if( key == 114 ){ // press r to reset
            curves.clear() ;
            AppState = _draw_curves ;
            getEdgels( sourceImage,  edgelsImage ) ;
            edges.elems.clear();
            dstVectorCurves.elems.clear();
            dstSnakeCurves.elems.clear();
            ctrpoints.elems.clear();
        }
        if( key == 115 ){ // press s
            if( AppState == _draw_curves ){
                AppState = _show_vectors ;
                 // calculate vector field
                std::cout<<"begin calculate the vector field" << std::endl;
                vecfield.compute( curves, cvSize( sourceImage->width,sourceImage->height), 400 ) ;
                std::cout<<"vector field calculated" << std::endl;
            }
        }
    }
}
