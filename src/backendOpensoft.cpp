#include "backendOpensoft.hpp"

/* Constants */

const int BG_THRESH = 255;
const int THRESHOLD = 5;
const int kernel_size =3;
const int ratio = 2;
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
  
    cv::namedWindow("input",CV_WINDOW_NORMAL);
    cv::imshow("input",binary);
    cv::waitKey(0);

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
    //cv::Mat drawing1 = cv::Mat::zeros(input.size(),CV_8UC3);

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
    
    approxContours.resize( contours.size() );

    cv::Scalar white = cv::Scalar(255,255,255);
    cv::drawContours(drawing, contours, largest_contour_index, white, 1, 8, heirarchy, 0, cv::Point());
    cv::namedWindow("contoursOutput",CV_WINDOW_NORMAL);
    cv::imshow("contoursOutput", drawing);
    cv::waitKey(0);

    cv::imwrite(std::string(argv[1])+"_canny_output.jpg",drawing);

    cv::approxPolyDP(cv::Mat(contours[largest_contour_index]), approxContours[0], 20, true);

    /* DEBUG */
    // std::cout << approxContours.size() << std::endl;
    std::cout << "No of corner points ~ " << approxContours[0].size() << std::endl;	
    /* */

    drawing = cv::Mat::zeros(input.size(),CV_8UC3);
    drawContours(drawing, approxContours, 0, white);
    cv::imwrite(std::string(argv[1])+"_appPoly_output.jpg",drawing);
    
    return contours[largest_contour_index];
    
}

void Torn_Piece::CannyThreshold(cv::Mat& input, cv::Mat& detected_edges) {

    cv::medianBlur(input,detected_edges, 9);
    cv::namedWindow("Canny Result",CV_WINDOW_NORMAL);

    // Canny detector OpenCV function
    cv::Canny(detected_edges,detected_edges,CANNY_THRESHOLD,(CANNY_THRESHOLD)*ratio,kernel_size);

    //enhances white pixels with respect to background
    cv::dilate(detected_edges,detected_edges, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
    cv::imshow("Canny Result", detected_edges);
    cv::waitKey(0);

    cv::destroyAllWindows();  
}
