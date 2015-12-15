#include "SuperPixelGraph.h"


/*int main()
{

	map<int, int> mapNeighborLabelToLen;
	mapNeighborLabelToLen.insert(pair<int, int>(2, 3));
	mapNeighborLabelToLen.insert(map<int, int>::value_type(3, 5));
	mapNeighborLabelToLen[5] = 6;
	map<int, int>::iterator I_iter = mapNeighborLabelToLen.find(4);
	if(I_iter != mapNeighborLabelToLen.end())
		cout<<"find the corresponding map value "<<mapNeighborLabelToLen[3]<<endl;
	else
		cout<<"not find the corresponding map value "<<mapNeighborLabelToLen[4]<<endl;
	for(map<int, int>::iterator iter=mapNeighborLabelToLen.begin(); iter!=mapNeighborLabelToLen.end(); iter++)
		cout<<"1: "<<(*iter).first<<", 2: "<<(*iter).second<<endl;

	int num_superPixels = 2012;
	int num_labels = 4;
	
	Mat img = imread("data/src.png");

	SuperPixelGraph spGraph;

	spGraph.readSuperPixelData(img, "data/superPixels.txt");
	spGraph.buildGraph();
	spGraph.outputGraph();
	
	spGraph.GeneralGraph_DArraySArraySpatVarying(num_superPixels,num_labels);
	
	printf("\n  Finished %d (%d) clock per sec %d",clock()/CLOCKS_PER_SEC,clock(),CLOCKS_PER_SEC);

	return 0;
}*/
