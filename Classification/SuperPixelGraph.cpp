#include "SuperPixelGraph.h"

SuperPixelGraph::SuperPixelGraph(void)
{
	numOfSuperPixels = 0;
	numOfClasses = 0;
	pixelLabel = Mat();
	predictConf = Mat();
	superPixelDat = vector<vector<Point > >();
	superPixelContour = vector<vector<Point > >();
	neighborSuperPixelLabel = vector<vector<int> >();
	adjacentEdgeLength = vector<vector<int> >();
	resultLabel = vector<int>();
}
SuperPixelGraph::SuperPixelGraph(vector<vector<Point> >m_superpixelCoords, Mat m_superpixelID, Mat m_predictConf)
{
	numOfSuperPixels = 0;
	numOfClasses = m_predictConf.cols;
	superPixelDat = m_superpixelCoords;
	pixelLabel = m_superpixelID;
	predictConf = m_predictConf;
	superPixelContour = vector<vector<Point > >();
	neighborSuperPixelLabel = vector<vector<int> >();
	adjacentEdgeLength = vector<vector<int> >();
	resultLabel = vector<int>();
}
SuperPixelGraph::~SuperPixelGraph(void)
{
    vector<vector<cv::Point > > emptyPoint = vector<vector<cv::Point > >();
    superPixelDat.swap(emptyPoint);
    vector<vector<cv::Point > > emptyPoint2 = vector<vector<cv::Point > >();
    superPixelContour.swap(emptyPoint2);
    vector<vector<int> > emptyInt = vector<vector<int> >();
    neighborSuperPixelLabel.swap(emptyInt);
    vector<vector<int> > emptyInt2 = vector<vector<int> >();
    adjacentEdgeLength.swap(emptyInt2);
}

/*void SuperPixelGraph::readSuperPixelData(Mat img, string dataDir)
{
	pixelLabel = cv::Mat(img.rows, img.cols, CV_32SC1, cv::Scalar(0));
	//cout<< "size = " << pixelLabel.size() << " rows = "<< pixelLabel.rows << " cols = " << pixelLabel.cols <<endl;
	vector<cv::Point> spRowsDat; //each row in super pixel data
	vector<vector<int> > maskDat; //mask data
	int id, numRowsDat, row, col;
	ifstream readDat(dataDir);
	if(!readDat.good()){
		cout<<"error! open the super pixels data fail."<<endl;
		return ;
	}
	string line;
	getline(readDat, line);
	if( line == "#labels" ){
		getline(readDat, line);
		istringstream ss(line);
		ss>>numOfSuperPixels;
		cout<<numOfSuperPixels<<endl;
		for(int i = 0; i<numOfSuperPixels; i++){
			getline(readDat, line);
			istringstream ss(line);
			ss>>id>>numRowsDat;
			for(int j=0;j<numRowsDat;j++){
				ss>>row>>col;
				pixelLabel.at<int>(row, col) = i;
				//cout<<"pixelLabel = "<<pixelLabel.at<int>(row, col)<<" ";
				spRowsDat.push_back(cv::Point(row,col));
			}
			//cout<<endl;
			superPixelDat.push_back(spRowsDat);
			spRowsDat.swap(vector<cv::Point>());
		}
	}
	readDat.close();
	//cout<<"the pixel label is:"<<endl;
	//for(int i=0; i<img.rows; i++){
	//	for(int j=0; j<img.cols; j++){
	//		cout<<pixelLabel.at<int>(i, j)<<" ";
	//	}
	//	cout<<endl;
	//}
	return ;
}
*/
int SuperPixelGraph::isContour(int curLabel, int row, int col)
{
	if(row<0 || row>=pixelLabel.rows || col<0 || col>=pixelLabel.cols)
		return -1;
	int newLabel = pixelLabel.at<int>(row, col);
	if(curLabel != newLabel)
		return newLabel;
	return -2;
}

bool SuperPixelGraph::superPixelDFS(Mat &flags, int row, int col, vector<Point> &contour, vector<int> &neighborLabel)
{
	//if(row<0 || row>=pixelLabel.rows || col<0 || col>=pixelLabel.cols)
	//	return false;
	if(int(flags.at<uchar>(row, col)) == 1)
		return false;
	flags.at<uchar>(row, col) = uchar(1); //if the label is
	int curLabel = pixelLabel.at<int>(row, col);

	for(int i=-1; i<=1; i++)
		for(int j=-1; j<=1; j++){
			if(abs(i)+abs(j) == 1){
				// judge the pixel is or not a contour;
				int pixelType = isContour(curLabel, row+i, col+j);

				if(pixelType == -2){
					superPixelDFS(flags, row+i, col+j, contour, neighborLabel);
				} else {
					contour.push_back(Point(row+i, col+j));
					neighborLabel.push_back(pixelType);
				}
			}

		}
	return true;
}

void SuperPixelGraph::buildGraph()
{
	Mat flags(pixelLabel.rows, pixelLabel.cols, CV_8U, Scalar(uchar(0)));
	for(int i=0; i<pixelLabel.rows; i++){
		for(int j=0; j<pixelLabel.cols; j++){
			vector<Point> contour = vector<Point>();
			vector<int> neighborLabel = vector<int>();
			if(superPixelDFS(flags, i, j, contour, neighborLabel)){ 
				numOfSuperPixels++;
				superPixelContour.push_back(contour);

				sort(neighborLabel.begin(), neighborLabel.end());
				vector<int> labelType = vector<int>();
				vector<int> numLabel = vector<int>();
				vector<int>::iterator iter=neighborLabel.begin();
				while(*iter < 0) iter++;
				int tmp = *iter, num=0;
				for(; iter!=neighborLabel.end(); iter++){
					if(tmp == *iter){
						num++;
					}else{
						labelType.push_back(tmp);
						numLabel.push_back(num);
						tmp = *iter;
						num = 1;
					}
				}
				labelType.push_back(tmp);
				numLabel.push_back(num);

				neighborSuperPixelLabel.push_back(labelType);
				adjacentEdgeLength.push_back(numLabel);
			}

		}
	}
}
/*
void SuperPixelGraph::outputGraph()
{
	int id=0;
	for(vector<vector<int> >::iterator iter=neighborSuperPixelLabel.begin();iter!=neighborSuperPixelLabel.end();iter++, id++){
		vector<int> pixelLoc = *iter;
		cout<<"the neighborhood of "<<id<<" th superPixel is :  ";
		for(vector<int>::iterator iiter=pixelLoc.begin();iiter!=pixelLoc.end();iiter++){
			int pixelLabel = *iiter;
			cout<<pixelLabel<<"  ";
		}
		cout<<endl;
	}

	cout<<endl<<endl;
	id=0;
	for(vector<vector<int> >::iterator iter=adjacentEdgeLength.begin();iter!=adjacentEdgeLength.end();iter++, id++){
		vector<int> pixelLoc = *iter;
		cout<<"each adjacentEdge length of "<<id<<" th superPixel is :  ";
		for(vector<int>::iterator iiter=pixelLoc.begin();iiter!=pixelLoc.end();iiter++){
			int neighborLen = *iiter;
			cout<<neighborLen<<"  ";
		}
		cout<<endl;
	}

	cout<<"numOfSuperPixels = "<<numOfSuperPixels<<endl;

}
*/

////////////////////////////////////////////////////////////////////////////////
// in this version, set data and smoothness terms using arrays
// grid neighborhood is set up "manually". Uses spatially varying terms. Namely
// V(p1,p2,l1,l2) = w_{p1,p2}*[min((l1-l2)*(l1-l2),4)], with 
// w_{p1,p2} = p1+p2 if |p1-p2| == 1 and w_{p1,p2} = p1*p2 if |p1-p2| is not 1

void SuperPixelGraph::GeneralGraph_DArraySArraySpatVarying(double smoothRatio)
{
	int num_pixels = numOfSuperPixels;
	int num_labels = numOfClasses;
	double dataWeight = 100.0;
	double smoothWeight = dataWeight*smoothRatio;
	double labelWeight = 1;
	// first set up the array for data costs
	int *data = new int[num_pixels*num_labels];
	for ( int i = 0; i < num_pixels; i++ )
		for (int l = 0; l < num_labels; l++ ){
			data[i*num_labels+l] = int((1-predictConf.at<float>(i, l)) * dataWeight);
		}

	// next set up the array for smooth cost
	int *smooth = new int[num_labels*num_labels];
	for ( int l1 = 0; l1 < num_labels; l1++ )
		for (int l2 = 0; l2 < num_labels; l2++ ){
			smooth[l1+l2*num_labels] = l1==l2 ? 0:labelWeight; //labelWeight*((l1-l2)*(l1-l2) <= 4  ? (l1-l2)*(l1-l2):4);
		}

	try{
		GCoptimizationGeneralGraph *gc = new GCoptimizationGeneralGraph(num_pixels,num_labels);
		gc->setDataCost(data);
		gc->setSmoothCost(smooth);


		//now set up the neighborhood system
		Mat flags(num_pixels, num_pixels, CV_8U, Scalar(uchar(0)));
		float len1, len2, adjacentLen;
		int p2, p1=0;
		float weight;
		for(vector<vector<int> >::iterator iter=neighborSuperPixelLabel.begin(); 
			iter!=neighborSuperPixelLabel.end(); iter++,p1++){
				vector<int> neiborLabel = *iter;
				
				int i = 0;
				for(vector<int>::iterator iiter=neiborLabel.begin(); iiter!=neiborLabel.end(); iiter++,i++){
					p2 = *iiter;

					//if(int(flags.at<uchar>(p1,p2)) != 1){
						adjacentLen = adjacentEdgeLength[p1][i];
						len1 = superPixelContour[p1].size();
						len2 = superPixelContour[p2].size();
						weight = 1.0 - 2*adjacentLen / (len1+len2);
						gc->setNeighbors(p1, p2, smoothWeight*weight);
						gc->setNeighbors(p2, p1, smoothWeight*weight);
					//	flags.at<uchar>(p1,p2) = uchar(1);
					//}
				}
		}

		printf("\nBefore optimization energy is %d",gc->compute_energy());
		gc->swap(5);// run expansion for 2 iterations. For swap use gc->swap(num_iterations);
		printf("\nAfter optimization energy is %d\n",gc->compute_energy());

		for ( int  i = 0; i < num_pixels; i++ )
			resultLabel.push_back(gc->whatLabel(i));

		delete gc;
	}
	catch (GCException e){
		e.Report();
	}

	delete [] smooth;
	delete [] data;

}
/*
void SuperPixelGraph::PrintGraphResult()
{
	int id=0;
	cout<<"\n\nthe graph cut result "<<endl;
	for(vector<int>::iterator iter=resultLabel.begin();iter != resultLabel.end();iter++, id++){
		cout<<"the "<<id<<" th superPixels label is : "<<(*iter)<<endl;
	}
}*/
