#include "graphcut.h"
#include "gmm.h"
#include <cmath>

GraphCut::GraphCut():scaleFactor(1.0)
{
}

void GraphCut::generateGMMProbability(){
    const int kMeansItCount = 10;
    const int kMeansType = KMEANS_PP_CENTERS;

    this->generateInitGuessMask();
    std::vector<std::vector<cv::Vec3f> > samples;
    //声明各个sample
    for(int i = 0; i < this->CLASS_NUMBER; i++){
        std::vector<cv::Vec3f> singleSample;
        samples.push_back(singleSample);
    }
    //为各个sample添加成员
    for(int y_offset = 0; y_offset < this->rawImage.rows; y_offset++){
        for(int x_offset = 0; x_offset < this->rawImage.cols; x_offset++){
            int currentGuessMask = this->initGuessMask.at<int>(y_offset,x_offset);
            samples.at(currentGuessMask).push_back((cv::Vec3f)this->rawImage.at<cv::Vec3b>(y_offset,x_offset));
        }
    }
    std::cout << "添加sample结束" << std::endl;
    //验证每个sample都不为空
    for(std::vector<cv::Vec3f> elem : samples){
        assert(elem.empty() == false);
    }

    //通过kmeans为每个sample中的成员指定一个label
    std::vector<cv::Mat > mat_samples;
    std::vector<cv::Mat > mat_samples_label;
    for(int i = 0; i < this->CLASS_NUMBER; i++){
        cv::Mat mat_sample((int)samples.at(i).size(), 3, CV_32F, &samples.at(i)[0][0] );
        cv::Mat label;
        cv::kmeans( mat_sample, GMM::componentsCount, label,TermCriteria( CV_TERMCRIT_ITER, kMeansItCount, 0.0), 0, kMeansType );
        mat_samples.push_back(mat_sample);
        mat_samples_label.push_back(label);
    }
    std::cout << "kmeans结束" << std::endl;

    this->GMMProbability = cv::Mat(this->rawImage.rows, this->rawImage.cols, CV_64FC(this->CLASS_NUMBER));
    std::vector<cv::Mat> GMMModels;
    std::vector<GMM> GMMs;
    //初始化GMM
    for(int i = 0; i < this->CLASS_NUMBER; i++){
        cv::Mat tempModel;
        GMM tempGMM(tempModel);
        tempGMM.initLearning();
        GMMModels.push_back(tempModel);
        GMMs.push_back(tempGMM);
    }

    //验证每个sample的样本数与label数相等
    for(int i = 0; i < this->CLASS_NUMBER; i++){
        assert(samples.at(i).size() == static_cast<size_t>(mat_samples_label.at(i).rows));
    }

    //把每个sample加入到GMM中
    for(int i = 0; i < this->CLASS_NUMBER; i++){
        for(size_t sampleIndex = 0; sampleIndex < samples.at(i).size(); sampleIndex++){
            GMMs.at(i).addSample(mat_samples_label.at(i).at<int>(sampleIndex,0), samples.at(i).at(sampleIndex));
        }
    }
    //GMM训练结束
    for(int i = 0; i < this->CLASS_NUMBER; i++){
        GMMs.at(i).endLearning();
    }
    std::cout << "GMM训练结束" << std::endl;

    //为每个GMMProbability位置赋值，值的含义为，当前像素属于类i的概率
    for(int y_offset = 0; y_offset < this->GMMProbability.rows; y_offset++){
        for(int x_offset = 0; x_offset < this->GMMProbability.cols; x_offset++){
            cv::Vec3f currentVec3f = (cv::Vec3f)this->rawImage.at<cv::Vec3b>(y_offset,x_offset);
            for(int i = 0; i < this->CLASS_NUMBER; i++){
                ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                this->GMMProbability.at<cv::Vec<double,3> >(y_offset,x_offset)[i] = GMMs.at(i).operator ()(currentVec3f);
            }
        }
    }
    std::cout << "GMMProbability赋值结束" << std::endl;

}

void GraphCut::GridGraph_Individually(int width,int height,int num_pixels,int num_labels)
{

    int *result = new int[num_pixels];   // stores result of optimization

    try{
        GCoptimizationGridGraph *gc = new GCoptimizationGridGraph(width,height,num_labels);

        std::vector<cv::Vec3b>::iterator it;
        for ( int i = 0; i < num_pixels; i++ ){
            int currentRow = i/this->rawImage.cols;
            int currentCol = i%this->rawImage.cols;

            //////////////////////////直接使用initGuess，start
            cv::Vec3b currentValue = this->initGuess.at<cv::Vec3b>(currentRow,currentCol);
            it = std::find(this->label2Value.begin(), this->label2Value.end(), currentValue);
            int label = it - this->label2Value.begin();
            for(int labelIndex = 0; labelIndex < this->CLASS_NUMBER; labelIndex++){
                if(label == labelIndex){
                    gc->setDataCost(i,labelIndex,1);
                }else{
                    gc->setDataCost(i,labelIndex,4);
                }
            }
            //////////////////////////直接使用initGuess，end


            //////////////////////////使用GMM，start
            /*for(int labelIndex = 0; labelIndex < this->CLASS_NUMBER; labelIndex++){
                ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                double gmmProbability = this->GMMProbability.at<cv::Vec<double,3> >(currentRow,currentCol)[labelIndex];
                gc->setDataCost(i,labelIndex,1*-log(gmmProbability));
            }*/
            //////////////////////////使用GMM，end

        }

        // next set up smoothness costs individually
        for ( int l1 = 0; l1 < num_labels; l1++ ){
            for (int l2 = 0; l2 < num_labels; l2++ ){
                int cost = (l1-l2)*(l1-l2) <= 32  ? 2*(l1-l2)*(l1-l2):32;
                gc->setSmoothCost(l1,l2,5*cost);
            }
        }

        printf("\nBefore optimization energy is %d",static_cast<int>(gc->compute_energy()));
        //gc->expansion(5);// run expansion for 2 iterations. For swap use gc->swap(num_iterations);
        gc->swap(5);
        printf("\nAfter optimization energy is %d\n",static_cast<int>(gc->compute_energy()));

        for ( int  i = 0; i < num_pixels; i++ ){
            result[i] = gc->whatLabel(i);
            this->resultLabel.at<cv::Vec3b>(i/this->resultLabel.cols, i%this->resultLabel.cols) = this->label2Value.at(result[i]);
        }

        delete gc;
    }
    catch (GCException e){
        e.Report();
    }
    delete [] result;
}

void GraphCut::generateInitGuessMask(){
    assert(this->initGuess.empty() == false);
    assert(this->initGuess.size == this->rawImage.size);
    assert(this->label2Value.empty() == false);

    this->initGuessMask = cv::Mat(this->rawImage.size(), CV_32S);
    for(int y_offset = 0; y_offset < this->initGuess.rows; y_offset++){
        for(int x_offset = 0; x_offset < this->initGuess.cols; x_offset++){
            cv::Vec3b currentColor = this->initGuess.at<cv::Vec3b>(y_offset,x_offset);
            for(size_t i = 0; i < this->label2Value.size(); i++){
                if(this->label2Value.at(i) == currentColor){
                    this->initGuessMask.at<int>(y_offset,x_offset) = i;
                    break;
                }
            }
        }
    }
}

/**
 * @brief GraphCut::main
 * @brief 使用预先进行分类的分类图作为最初猜想
 */
void GraphCut::main(){
    this->rawImage = cv::imread("/home/netbeen/桌面/周叔项目/img5.jpg");
    this->initGuess = cv::imread("/home/netbeen/桌面/周叔项目/leaf-trg_label.png");

    //this->rawImage = cv::imread("/home/netbeen/桌面/周叔项目/trg (2).png");
    //this->initGuess = cv::imread("/home/netbeen/桌面/周叔项目/trg_label.png");

    this->rawImage = this->rawImage(cv::Rect(0,0,this->initGuess.cols, this->initGuess.rows));
    this->resultLabel = cv::Mat(this->rawImage.rows,this->rawImage.cols,CV_8UC3);

    this->CLASS_NUMBER = 3;

    if(this->checkUserMarkValid(this->initGuess) == false){
        std::cout << "checkUserMarkValid false!" <<std::endl;
        exit(1);
    }else{
        std::cout << "checkUserMarkValid true!" <<std::endl;
    }

    int num_pixels = this->rawImage.cols*this->rawImage.rows;

    this->generateGMMProbability();

    // smoothness and data costs are set up one by one, individually
    std::cout << "GridGraph_Individually start" << std::endl;
    this->GridGraph_Individually(this->rawImage.cols,this->rawImage.rows,num_pixels,this->CLASS_NUMBER);

    /*this->initGuessGray = this->resultLabelGray;
    this->GridGraph_Individually(this->rawImage.cols,this->rawImage.rows,num_pixels,this->CLASS_NUMBER);*/

    cv::imshow("resultLabelGray",this->resultLabel);
    cv::imshow("initGuessGray",this->initGuess);
    cv::imshow("rawImage",this->rawImage);
    cv::imwrite("output.png",this->resultLabel);
    cv::waitKey();
}


void GraphCut::main(cv::Mat& sourceImage, cv::Mat& initGuess){
    this->rawImage = sourceImage;
    this->initGuess = initGuess;

    this->rawImage = this->rawImage(cv::Rect(0,0,this->initGuess.cols, this->initGuess.rows));
    this->resultLabel = cv::Mat(this->rawImage.rows,this->rawImage.cols,CV_8UC3);

    this->CLASS_NUMBER = 2;

    /*if(this->checkUserMarkValid(this->initGuess) == false){
        std::cout << "checkUserMarkValid false!" <<std::endl;
        exit(1);
    }else{
        std::cout << "checkUserMarkValid true!" <<std::endl;
    }*/

    int num_pixels = this->rawImage.cols*this->rawImage.rows;

    //this->generateGMMProbability();

    // smoothness and data costs are set up one by one, individually
    std::cout << "GridGraph_Individually start" << std::endl;
    this->GridGraph_Individually(this->rawImage.cols,this->rawImage.rows,num_pixels,this->CLASS_NUMBER);

    /*this->initGuessGray = this->resultLabelGray;
    this->GridGraph_Individually(this->rawImage.cols,this->rawImage.rows,num_pixels,this->CLASS_NUMBER);*/

    initGuess = this->resultLabel;
    /*cv::imshow("resultLabelGray",this->resultLabel);
    cv::imshow("initGuessGray",this->initGuess);
    cv::imshow("rawImage",this->rawImage);
    cv::imwrite("output.png",this->resultLabel);
    cv::waitKey();*/
}


bool GraphCut::checkUserMarkValid(const cv::Mat& userMark){
    for(int offset_y = 0; offset_y < userMark.rows; offset_y++){
        for(int offset_x = 0; offset_x < userMark.cols; offset_x++){
            cv::Vec3b currentMark = userMark.at<cv::Vec3b>(offset_y,offset_x);
            std::vector<cv::Vec3b>::iterator it = std::find(this->label2Value.begin(),this->label2Value.end(),currentMark);
            if(it == this->label2Value.end()){
                this->label2Value.push_back(currentMark);
                std::cout << "Find new mark: " << static_cast<int>(currentMark[0]) <<" " << static_cast<int>(currentMark[1])<<  " "<< static_cast<int>(currentMark[2]) << std::endl;
                if(this->label2Value.size() > static_cast<size_t>(this->CLASS_NUMBER)){
                    return false;
                }
            }
        }
    }
    if(this->label2Value.size() == static_cast<size_t>(this->CLASS_NUMBER)){
        return true;
    }else{
        return false;
    }
}
