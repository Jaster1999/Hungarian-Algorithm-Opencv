#include "Hungarian.hpp"
#include <opencv2/opencv.hpp>

int main(int argc, char **argv)
{
    // Set random seed
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Define the size of the cost matrix
    const int matrixSize = 4; 

    // Create a cost matrix filled with random values between 0 and 100
    cv::Mat costMatrix(matrixSize, matrixSize, CV_32F);
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            costMatrix.at<float>(i, j) = static_cast<float>(std::rand() % 101);  // values between 0 and 100
        }
    }
    //float mydata[] = {7, 96, 10, 88, 47, 58, 51, 11, 27, 44, 58, 59, 97, 96, 96, 8};
    //cv::Mat costMatrix(matrixSize, matrixSize, CV_32F, mydata);

    // Print the generated cost matrix
    std::cout << "Cost Matrix:" << std::endl;
    std::cout << costMatrix << std::endl;

    // Initialize the Hungarian algorithm with the cost matrix
    HungarianAlgorithm hungarian(costMatrix);

    // Solve the assignment problem
    std::vector<int> assignment = hungarian.solve();

    // Print the result
    std::cout << "Optimal Assignment:" << std::endl;
    for (int i = 0; i < assignment.size(); i++) {
        std::cout << "Row " << i << " -> Column " << assignment[i] << std::endl;
    }

    return 0;
}






