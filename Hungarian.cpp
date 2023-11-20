#include "Hungarian.hpp"

HungarianAlgorithm::HungarianAlgorithm(const cv::Mat& costm_matrix) 
{
    // Expand m_matrix to square if necessary
    int maxSize = std::max(costm_matrix.rows, costm_matrix.cols);
    m_matrix = cv::Mat::zeros(maxSize, maxSize, CV_32S);
    m_mask_matrix = cv::Mat::zeros(maxSize, maxSize, CV_32S);

    costm_matrix.copyTo(m_matrix(cv::Rect(0, 0, costm_matrix.cols, costm_matrix.rows)));

    coveredRows.assign(maxSize, false);
    coveredCols.assign(maxSize, false);
}

std::vector<int> HungarianAlgorithm::solve() 
{
    int step = 1;

    while (step) 
    {
        std::cout << "Current step: " << step << std::endl; // Debugging output for the current step
        switch (step) 
        {
            case 1:
                step_one();
                step = 2;
                break;
            case 2:
                step_two();
                step = 3;
                break;
            case 3:
                step = step_three();
                break;
            case 4:
                step = step_four();
                break;
            case 5:
                step = step_five();
                break;
        }
    }

    std::vector<int> assignment;
    for (int row = 0; row < m_mask_matrix.rows; ++row) {
        for (int col = 0; col < m_mask_matrix.cols; ++col) {
            if (m_mask_matrix.at<int>(row, col) == 1) {
                assignment.push_back(col);
                break;
            }
        }
    }
    return assignment;
}

void HungarianAlgorithm::step_one() 
{
    // Subtract the smallest value in each row from all values in that row
    for (int i = 0; i < m_matrix.rows; ++i) {
        double minVal;
        cv::minMaxIdx(m_matrix.row(i), &minVal);
        m_matrix.row(i) -= minVal;
    }
}

void HungarianAlgorithm::step_two() 
{
    // Subtract the smallest value in each col from all values in that col
    for (int i = 0; i < m_matrix.cols; ++i) {
        double minVal;
        cv::minMaxIdx(m_matrix.col(i), &minVal);
        m_matrix.col(i) -= minVal;
    }
}

int HungarianAlgorithm::step_three() 
{   
    // Step 1: Star each zero (Z) in the m_matrix if there are no other m_mask_matrix zeros in its row or column
    for (int i = 0; i < m_matrix.rows; ++i) 
    {
        const int* mat_row_ptr = m_matrix.ptr<int>(i);
        int* mask_row_ptr = m_mask_matrix.ptr<int>(i);
        for (int j = 0; j < m_matrix.cols; ++j) 
        {
            if (mat_row_ptr[j] == 0 && !coveredRows[i] && !coveredCols[j]) 
            {
                mask_row_ptr[j] = 1;
                coveredRows[i] = true;
                coveredCols[j] = true;
            }
        }
    }

    return 4;
}

int HungarianAlgorithm::step_four()
{
    // reset the covers to act as lines
    std::fill(coveredRows.begin(), coveredRows.end(), false);
    std::fill(coveredCols.begin(), coveredCols.end(), false);

    // Cover each column containing a starred zero
    for (int j = 0; j < m_matrix.cols; ++j) 
    {
        for (int i = 0; i < m_matrix.rows; ++i) 
        {
            if (m_mask_matrix.at<int>(i, j) == 1) 
            {
                coveredCols[j] = true;
                break;
            }
        }
    }

    if(std::count(coveredCols.begin(), coveredCols.end(), true)==m_matrix.cols)
    {
        return 0;
    }

    while(true)
    {
        int i, j, col=-1, row=-1;
        bool uncovered_zero = false;
        // Find out if theres an uncovered zero and prime it
        for(i=0; i<m_matrix.rows && !uncovered_zero; i++)
        {
            for(j=0; j<m_matrix.cols && !uncovered_zero; j++)
            {
                if(m_matrix.at<int>(i,j) == 0 && !coveredCols[j] && !coveredRows[i])
                {
                    m_mask_matrix.at<int>(i,j) = 2;
                    col = j;
                    row = i;
                    uncovered_zero = true;
                }
            }
        }

        if(uncovered_zero)
        // uncovered zero was found
        {   
            // figure out if there is a starred zero on the same row as the uncovered one thats now primed
            bool starred_found = false;
            for(j=0; j<m_matrix.cols; j++)
            {
                if(m_mask_matrix.at<int>(row,j) == 1)
                {
                    coveredRows[row] = true;
                    coveredCols[j] = false;
                    starred_found = true;
                    // there is a starred zero along this row
                    break;
                }
            }

            if(starred_found)
            // if a starred zero was found we want to try find another uncovered zero
            {
                // do nothing
            }
            else
            // if one wasnt however we want to see if there is a starred zero along the column instead
            {
                std::vector<std::pair<int, int>> path;
                path.push_back({row, col});
                bool done = false;
                while(!done)
                {
                    // look for a starred zero in the current column
                    int star_row = -1;
                    for(i=0; i<m_mask_matrix.rows; i++)
                    {
                        if(m_mask_matrix.at<int>(i,col) == 1)
                        {
                            star_row = i;
                            break;
                        }
                    }

                    // if we found a starred zero look for a primed in the starred row
                    if(star_row != -1)
                    {
                        path.push_back({star_row, col});

                        for(j=0; j<m_mask_matrix.cols; j++)
                        {
                            if(m_mask_matrix.at<int>(star_row,j) == 2)
                            {
                                path.push_back({star_row, j});
                                col = j;
                            }
                        }
                    }
                    else
                    {
                        done = true;
                    }

                    // look for a primed zero in the previously found starred row
                }

                // Unstar each starred zero of the series, star each primed zero of the series
                for(auto& p : path) 
                {
                    if(m_mask_matrix.at<int>(p.first, p.second) == 1) 
                    {
                        m_mask_matrix.at<int>(p.first, p.second) = 0;
                    } 
                    else 
                    {
                        m_mask_matrix.at<int>(p.first, p.second) = 1;
                    }
                }
                
                const int n = m_mask_matrix.rows;
                // Clear covers and erase all primes
                for(int r = 0; r < n; r++) 
                {
                    for(int c = 0; c < n; c++) 
                    {
                        if(m_mask_matrix.at<int>(r, c) == 2) 
                        {
                            m_mask_matrix.at<int>(r, c) = 0;
                        }
                    }
                }
                // reset all lines
                std::fill(coveredRows.begin(), coveredRows.end(), false);
                std::fill(coveredCols.begin(), coveredCols.end(), false);

                // check if we have an optimal solution, i.e. number of starred zeroes equals the matrix size
                int star_count = 0;
                for(int r=0; r<n; r++)
                {
                    const int* mask_row_ptr = m_mask_matrix.ptr<int>(r);
                    for(int c=0; c<n; c++)
                    {
                        if(mask_row_ptr[c] == 1)
                        {
                            star_count++;
                        }
                    }
                }

                // if it is optimal return 0 and cease the algorithm
                if(star_count == n)
                {
                    return 0;
                    break;
                }

            }
        }
        else
        // no uncovered zero was found matrix adjustment is required.
        {
            return 5;
            break;
        }
    }
}

int HungarianAlgorithm::step_five()
{
    int n = m_matrix.rows;
    
    // 1. Find the smallest uncovered value
    int minUncoveredValue = std::numeric_limits<int>::max();
    for(int row = 0; row < n; row++) 
    {
        for(int col = 0; col < n; col++) 
        {
            if(!coveredRows[row] && !coveredCols[col] && m_matrix.at<int>(row, col) < minUncoveredValue) 
            {
                minUncoveredValue = m_matrix.at<int>(row, col);
            }
        }
    }

    // 2. Subtract the smallest value from every uncovered element
    for(int row = 0; row < n; row++) 
    {
        for(int col = 0; col < n; col++) 
        {
            if(!coveredRows[row] && !coveredCols[col]) 
            {
                m_matrix.at<int>(row, col) -= minUncoveredValue;
            }
        }
    }

    // 3. Add the smallest value to every element that is covered by two lines
    for(int row = 0; row < n; row++) 
    {
        for(int col = 0; col < n; col++) 
        {
            if(coveredRows[row] && coveredCols[col]) 
            {
                m_matrix.at<int>(row, col) += minUncoveredValue;
            }
        }
    }

    return 4;
}
