#include "MySharkML.h"
#include <shark/Algorithms/Trainers/RFTrainer.h> //the random forest trainer
#include <shark/ObjectiveFunctions/Loss/ZeroOneLoss.h> //zero one loss for evaluation

#include <iostream>

CMySharkML::CMySharkML(void)
{
	m_numLabels = 0;
}

CMySharkML::CMySharkML(int numTree)
{
	m_numTrees = numTree;
	m_numLabels = 0;
}

CMySharkML::~CMySharkML(void)
{
}

void CMySharkML::RFClassification(cv::Mat &trainingFeat, std::vector<int> &trainingLabel, cv::Mat &TestFeat, std::vector<int> &OutputLabel, cv::Mat &OutputConf)
{
	ClassificationDataset data, dataTest;
	Features2SharkData(data, trainingFeat, trainingLabel); //Training set
	std::vector<int> testLabel(TestFeat.rows, 0);
	Features2SharkData(dataTest, TestFeat, testLabel); //Test Set

	m_numLabels = numberOfClasses(data);
	if(m_numLabels < 2) {
		cout<<"ERROR in training Labels"<<endl;
		return;
	}

	cout << "Training set - number of data points: " << data.numberOfElements()
		<< " number of classes: " << m_numLabels
		<< " input dimension: " << inputDimension(data) << endl;

	cout << "Test set - number of data points: " << dataTest.numberOfElements()
		<< " number of classes: " << numberOfClasses(dataTest)
		<< " input dimension: " << inputDimension(dataTest) << endl;

	
	clock_t start_time_RF=clock();

	//Generate a random forest
	//###begin<train>
	RFTrainer trainer;
	trainer.setNTrees(m_numTrees);

	RFClassifier model;
	trainer.train(model, data);
	//###end<train>

	Data<RealVector> prediction = model(dataTest.inputs());
	//cout << "Random Forest on test set accuracy:     " << 1. - loss.eval(dataTest.labels(), prediction) << endl;
	//###end<eval>

	clock_t end_time_RF=clock();
	cout<< "Random Forest running time is: "<<static_cast<double>(end_time_RF-start_time_RF)/CLOCKS_PER_SEC<<"ms"<<endl;

	// output
	GetPredictionLabelandConfidence(OutputLabel, OutputConf, prediction);
}

void CMySharkML::Features2SharkData(LabeledData<RealVector, unsigned int> &dataset,	cv::Mat &features, std::vector<int> &v_label)
{
	//copy rows of the file into the dataset
	std::size_t rows = features.rows;
	std::size_t dimensions = features.cols;
	std::vector<std::size_t> batchSizes = shark::detail::optimalBatchSizes(rows, 256);

	// Test data
	dataset = LabeledData<RealVector, unsigned int>(batchSizes.size());
	std::size_t currentRow = 0;
	for(std::size_t b = 0; b != batchSizes.size(); ++b) {
		RealMatrix& inputs = dataset.batch(b).input;
		UIntVector& labels = dataset.batch(b).label;
		inputs.resize(batchSizes[b], dimensions);
		labels.resize(batchSizes[b]);
		//copy the rows into the batch
		for(std::size_t i = 0; i != batchSizes[b]; ++i,++currentRow){
			int rawLabel = v_label[currentRow];
			labels[i] = rawLabel; 
			for(std::size_t j = 0; j != dimensions; ++j){
				inputs(i,j) = features.at<float>(currentRow, j);
			}
		}
	}

}

void CMySharkML::GetPredictionLabelandConfidence(std::vector<int> &predictLable, cv::Mat &predictConf, Data<RealVector> &predictions)
{
	predictLable.clear();
	predictConf.release();

	std::size_t numElements = predictions.numberOfElements();

	predictLable.assign(numElements, 0);
	predictConf = cv::Mat(numElements, m_numLabels, CV_32F);
	memset(predictConf.data, 0, sizeof(float)*numElements*m_numLabels);

	for(int k = 0; k < numElements; ++k) {

		predictions.element(k);

		RealVector::iterator iter_p = predictions.element(k).begin(); //Predict.begin();
		float maxP = *iter_p;
		int L = 0;
		predictConf.at<float>(k, 0) = *iter_p;

		std::size_t numInputs = predictions.element(k).size(); //Predict.size();
		for(int i=1; i<numInputs; ++i){
			iter_p++;

			predictConf.at<float>(k, i) = *iter_p;
			if(*iter_p > maxP) {
				maxP = *iter_p;
				L = i;
			}
		}

		predictLable[k] = L;
	}
}