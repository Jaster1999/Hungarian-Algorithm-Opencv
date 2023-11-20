#include <iostream>
#include <vector>
#include <limits>
#include <opencv2/opencv.hpp>

class HungarianAlgorithm {
public:
    HungarianAlgorithm(const cv::Mat& costMatrix);

    std::vector<int> solve();

private:
    cv::Mat m_matrix;
    cv::Mat m_mask_matrix;
    std::vector<bool> coveredRows, coveredCols;
    cv::Point zeroLocation;

    void step_one();
    void step_two();
    int step_three();
    int step_four();
    int step_five();
};
