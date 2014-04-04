#ifndef __BACKEND_OPENSOFT_HPP
#define __BACKEND_OPENSOFT_HPP

#include "findBestMatch.hpp"
#include "translateImage.hpp"
#include <bits/stdc++.h>

class Torn_Piece{

    public:

        inline bool inThreshold(const int pixVal);

        /* Generates Binary Image.
         * Input : Input Mat and Output Mat
         * Output : void. (output Mat updated)
         */
        void generateBinary(cv::Mat& input, cv::Mat& binary);

        /* The following method detects Approximated edges for input image before
         * pre processing by CannyThreshold.
         * Input : Mat input, argv name
         * Output : Single pixel width contour in form of Points vector.
         */
        void CannyThreshold(cv::Mat& input, cv::Mat& detected_edges);
        std::vector<cv::Point> contourDetection(cv::Mat& input, char **argv);

        /* Generates features for input contour. Find Feature class in different
         * file.
         * Input : Contour in form of vector of points.
         * Output : Output of Feature vector.
         */
        std::vector <Feature> getFeatures(std::vector <cv::Point>& edgePoints);

};

/* Finds the best matching feature among all the input feature vectors for
 * all the images.
 * Input : vector of feature vector for all contours.
 * Output : Best matching 'Match' as implemented in 'findBestMatch'.
 */
vector <Match> findBestMatch(std::vector <std::vector < Feature> > &shapes);

/* Constants */

const int BG_THRESH = 255;
const int THRESHOLD = 5;
const int kernel_size =3;
const int ratio_ = 2;
const int CANNY_THRESHOLD = 100;

cv::RNG rng(12345);

/* Checks if given pixel is in the range of the background or not */
inline bool Torn_Piece::inThreshold(const int pixVal) {
    return (fabs(pixVal-BG_THRESH) < THRESHOLD);
}


void Torn_Piece::generateBinary(cv::Mat& input, cv::Mat& binary){

    int height, width;
    height = input.rows;
    width = input.cols;

    for(int i=0;i< height/20;i++) {
        for(int j = 0;j<width;j++) {
            input.at<uchar> (i,j) = 255;
        }
    }
    
    for(int i=0;i< height;i++)
        for(int j = 0;j<width;j++)
            if(inThreshold(input.at<uchar>(i,j)))
                binary.at<uchar>(i,j) = 255;


    cv::erode(binary,binary, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
    for(int i=0;i<3;i++)
        cv::dilate(binary,binary, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
  
    //cv::namedWindow("input",CV_WINDOW_NORMAL);
    //cv::imshow("input",binary);
    //cv::waitKey(0);

}


std::vector<cv::Point> Torn_Piece::contourDetection(cv::Mat& input, char **argv) {
    
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> heirarchy;
    cv::Rect bounding_rect;
    
    size_t i=0;
    double largest_area;
    int largest_contour_index;
    
    std::vector<std::vector<cv::Point> > approxContours;

    /* Finds all the contours within the input image. The function format is -
     * findContours(InputOutputArray image, OutputArrayOfArrays contours, OutputArray hierarchy, \
     * int mode, int method, Point offset=Point())
     * The other contour retrieval modes are - CV_RETR_CCOMP, CV_RETR_TREE
     */
    cv::findContours(input, contours, heirarchy, CV_RETR_LIST, \
            CV_CHAIN_APPROX_SIMPLE);

    cv::Mat drawing = cv::Mat::zeros(input.size(),CV_8UC3);

    /* Find the contour with largest area. That is our required contour rest
     * other are contours are filtered.
     */

    for( i = 0; i< contours.size(); i++ ) {
        double a = cv::contourArea( contours[i],false);        //  Find the area of contour
        if(a > largest_area){
            largest_area = a;
            largest_contour_index = i;                     //Store the index of largest contour
            bounding_rect = cv::boundingRect(contours[i]);     // Find the bounding rectangle for biggest contour
        }
    }
    
    // for( i = 0; i< contours.size(); i++ ) {
    //   if(i==largest_contour_index) continue;
    //   bounding_rect = cv::boundingRect(contours[i]);     // Find the bounding rectangle for biggest contour
    //   for(int j = )
    // }


    approxContours.resize( contours.size() );

    cv::Scalar white = cv::Scalar(255,255,255);
    cv::drawContours(drawing, contours, largest_contour_index, white, CV_FILLED, 8, heirarchy, 0, cv::Point());
    //cv::namedWindow("contoursOutput",CV_WINDOW_NORMAL);
    //cv::imshow("contoursOutput", drawing);
    //cv::waitKey(0);

    background.push_back(drawing);

    cv::approxPolyDP(cv::Mat(contours[largest_contour_index]), approxContours[0], 30, true);

    /* DEBUG */
    //std::cout << "No of corner points ~ " << approxContours[0].size() << std::endl;	

    cv::Mat drawing2 = cv::Mat::zeros(input.size(),CV_8UC3);
    drawContours(drawing2, approxContours, 0, white);

    cv::Mat binary;
    cv::cvtColor(drawing2, binary, CV_BGR2GRAY);
    outLines.push_back(binary);

    
    return approxContours[0];
    
}

void Torn_Piece::CannyThreshold(cv::Mat& input, cv::Mat& detected_edges) {

    cv::medianBlur(input,detected_edges, 9);

    // Canny detector OpenCV function
    cv::Canny(detected_edges,detected_edges,CANNY_THRESHOLD,(CANNY_THRESHOLD)*ratio_,kernel_size);

    //enhances white pixels with respect to background
    cv::dilate(detected_edges,detected_edges, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);

    cv::destroyAllWindows();  
}

#endif /* end of include guard: __BACKEND_OPENSOFT_HPP */
