
#ifndef __declaration_h
#define  __declaration_h
#include <cmath>
#include <vector>
#include <iostream>
#include <opencv/cv.h>
class double2{
public:
	double x ;
	double y ;
	
	double norm(){
		return sqrt( x*x+y*y) ;
	}
	double2 normalize(){
		double n = norm() ;
		x /= n;
		y /= n ;

		return *this ;
	}
	double2(){ x = y = 0.0;}
	double2(double _x, double _y ): x(_x), y(_y) {}

	double2 operator+(double2 a){
		return double2(x+a.x, y+a.y) ;
	}
	double operator*(double2 a){
		return x*a.x + y*a.y ;
	}
	double2 operator*(double a){
		return double2(a*x, a*y) ;
	}
	double2 operator/(double a){
		return double2(x/a, y/a) ;
	}
	double2 operator-(double2 a){
		return double2(x-a.x, y-a.y) ;
	}

	double2 operator-(){
		return double2(-x, -y) ;
	}

	bool operator==(double2 a){
		return x==a.x&&y==a.y ;
	}
};


class int2{
public:
	int2():first(0),second(0){}
	int2(int a, int b):first(a), second(b) {}
	
	union{
		int first ;
		int x ;
	} ;

	union{

		int second ;
		int y ;
	};


	int &operator[](int a){
		if( a == 0)
			return x ;
		else if( a==1)
			return y;
		else{
			std::cerr<<"invalid index of int2" ;
			exit(1) ;
		}
	}
};
class double3{
public:
	double x, y, z;
	double3(double x0, double y0, double z0):x(x0), y(y0), z(z0){}

	double3 operator/(double a){
		return double3(x/a, y/a, z/a) ;
	}
	double3 operator*(double a){
		return double3(x*a, y*a, z*a) ;
	}
	double3 operator-(double3 &a){
		return double3(x-a.x, y-a.y, z-a.z) ;
	}
	double3 operator+(double3 &a){
		return double3(x+a.x, y+a.y, z+a.z) ;
	}
	double norm(){
		return sqrt( x*x+y*y+z*z) ;
	}

};

struct smnode{
	int row ;
	double val ; 
};
class sparse_matrix{
public:
	sparse_matrix( int cols){
		data.resize(cols) ;
	}

	std::vector<std::vector<smnode> > data ;

	inline double getValue( int col, int row ) {

		for( size_t i=0; i<data[col].size(); ++i)
			if( data[col][i].row == row)
				return  data[col][i].val ;

		return 0;
	}

	inline void pluse(int col, int row, double val ) {
		
		for( size_t i=0; i<data[col].size(); ++i){
			if( data[col][i].row == row){
				data[col][i].val += val ;
				return ;
			}
		}

		smnode node ;
		node.row = row ;
		node.val = val ;

		data[col].push_back(node) ;
			
	}


};

class Edges{
public:
	std::vector<std::vector<CvPoint> > elems ;
	Edges(){}
};

class Gradients{
public:
	std::vector<std::vector<double2> > elems ;
	std::vector<std::vector<double> > magn ;
	Gradients(){}
	Gradients(IplImage *img){
		elems.resize( img->height ) ;
		magn.resize(img->height ) ;

		for( size_t i=0; i < elems.size(); ++i)
			elems[i].resize(img->width) ;
		for( size_t i=0; i < magn.size(); ++i)
			magn[i].resize(img->width) ;
	}
};

void drawCurves( IplImage * img , 	std::vector<std::vector<CvPoint> > curves, CvScalar corlor , int thickness ) ;
//typedef std::pair<double2, double>  Circle;


#endif
