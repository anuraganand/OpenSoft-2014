#ifndef __TRANSLATE_IMAGE
#define __TRANSLATE_IMAGE

#include "opencv2/opencv.hpp"
#include "edgeFeatures.hpp"
#include "bits/stdc++.h"
#include "findBestMatch.hpp"

using namespace cv;
using namespace std;

extern vector < cv::Mat > outLines;
extern vector < cv::Mat > colorImages;
extern vector < cv::Mat > background;

const int INF = 1e7;

// check if point(x, y) in img belong to edge
// currently just checking whether its white or not
inline bool isOnEdge(const Mat& img, int x, int y) {
  if(x >= img.rows || x < 0 || y < 0 || y >= img.cols) {
    return false;
  }
  if(img.channels() == 1) return img.at <uchar>(x, y) > 10;
  else if(img.channels() == 3) {
    Vec3b val = img.at <Vec3b> (x, y);
    return (val[0] + val[1] + val[2]) > 100;
  } else {
    assert(0);
    return false;
  }
}

inline cv::Point getTranslated(cv::Point pnt, Match & matched) {
  pnt.x = pnt.x - matched.pnt2.x + matched.pnt1.x;
  pnt.y = pnt.y - matched.pnt2.y + matched.pnt1.y;
  return pnt;
}

inline cv::Point getImage(cv::Point pnt, Match & matched, double theta) {
  int x = pnt.x - matched.pnt2.x , y = pnt.y - matched.pnt2.y;
  pnt.x = x * cos(theta) - y * sin(theta) + matched.pnt1.x;
  pnt.y = y * cos(theta) + x * sin(theta) + matched.pnt1.y;
  return pnt;
}

inline double getTheta(Match & matched) {
  cv::Point left = matched.next1 - matched.pnt1;

  cv::Point right = matched.prev2;
  right = getTranslated(right, matched);
  // right.x = matched.prev2.y;
  // right.y = matched.prev2.x;

  right = right - matched.pnt1;

  double num = left.x * right.x + left.y * right.y;
  double den = sqrt(left.x * left.x + left.y * left.y) * sqrt(right.x * right.x + right.y * right.y);
  double angle1 = acos(num / den);
  int cross = right.x * left.y - right.y * left.x;
  if(cross > 0) angle1 = -angle1;

  left = matched.prev1 - matched.pnt1;
  right = matched.next2;
  right = getTranslated(right, matched);
  right = right - matched.pnt1;
  num = left.x * right.x + left.y * right.y;
  den = sqrt(left.x * left.x + left.y * left.y) * sqrt(right.x * right.x + right.y * right.y);
  double angle2 = acos(num / den);
  cross = right.x * left.y - right.y * left.x;
  if(cross < 0) angle2 = -angle2;
  double angle = (angle1 + angle2) / 2.0;
  return angle;
}

/// This returns an image in which toRotate is translated and rotated to fit mainImage according to the informations of the best match
Mat translatedImage(Match matched) {
  
  Mat & colored1 = colorImages[matched.shape1];
  Mat & colored2 = colorImages[matched.shape2];

  Mat & back1 = background[matched.shape1];
  Mat & back2 = background[matched.shape2];
  
  Mat & mainImage = outLines[matched.shape1];
  Mat & toRotate = outLines[matched.shape2];
  
  int minX = INF, minY = INF, maxX = -INF, maxY = -INF;

  Mat connected = Mat::zeros(max(mainImage.rows, toRotate.rows), mainImage.cols + toRotate.cols, CV_8UC1);

  for(int row = 0; row < mainImage.rows; row++)
    for(int col = 0; col < mainImage.cols; col++)
      if(isOnEdge(mainImage, row, col))
        connected.at <uchar> (row, col) = 255;


  for(int row = 0; row < toRotate.rows; row++)
    for(int col = 0; col < toRotate.cols; col++)
      if(isOnEdge(toRotate, row, col))
        connected.at <uchar> (row, mainImage.cols + col) = 255;


  matched.next2.x += mainImage.cols;
  matched.pnt2.x += mainImage.cols;
  matched.prev2.x += mainImage.cols;
  //tf(matched.prev1, matched.pnt1, matched.next1, matched.next2, matched.pnt2, matched.prev2);

  double theta = getTheta(matched);
  
  /// Find the size of new Image such that after rotation and translation it fits
  for(int row = 0; row < mainImage.rows; row++) {
    for(int col = 0; col < mainImage.cols; col++) if(isOnEdge(mainImage, row, col)) {
      minX = min(minX, row);
      maxX = max(maxX, row);
      minY = min(minY, col);
      maxY = max(maxY, col);
    }
  }

  for(int row = 0; row < mainImage.rows; row++) {
    for(int col = 0; col < mainImage.cols; col++) if(isOnEdge(toRotate, row, col)) {
      cv::Point newPos;
      newPos.x = col + mainImage.cols;
      newPos.y = row;
      newPos = getImage(newPos, matched, theta);
      minY = min(minY, newPos.x);
      maxY = max(maxY, newPos.x);
      minX = min(minX, newPos.y);
      maxX = max(maxX, newPos.y);
    }
  }

  // Set OFFSET for x ad y axis
  int OFFSET_X = -minX + 50 , OFFSET_Y = -minY + 50;
  //int OFFSET_X = 0, OFFSET_Y = 0;
  // Create new blank image
  Mat newImage  = Mat::zeros(maxX - minX + 100, maxY - minY + 100, CV_8UC3);

  Vec3b white;
  white[0] = white[1] = white[2] = 255;
  for(int row = 0; row < newImage.rows; row++) {
    for(int col = 0; col < newImage.cols; col++) {
      newImage.at <Vec3b> (row, col) = white;
    }
  }
  //Mat newImage = Mat::zeros(10000, 10000, CV_8UC1);
  for(int row = 0; row < back1.rows; row++) {
    for(int col = 0; col < back1.cols; col++) if(isOnEdge(back1, row, col)) {
      newImage.at <Vec3b> (row + OFFSET_X, col + OFFSET_Y) = colored1.at <Vec3b> (row, col);

    }
  }

  for(int row = 0; row < back2.rows; row++) {
    for(int col = 0; col < back2.cols; col++) if(isOnEdge(back2, row, col)) {
      cv::Point newPos;
      newPos.x = col + mainImage.cols;
      newPos.y = row;
      newPos = getImage(newPos, matched, theta);
      newImage.at <Vec3b> (newPos.y + OFFSET_X, newPos.x + OFFSET_Y) = colored2.at <Vec3b> (row, col);
    }
  }

  //printf("New Image created\n");
  return newImage;
}


#endif