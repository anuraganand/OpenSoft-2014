#ifndef __EDGE_FEATURES_HPP
#define __EDGE_FEATURES_HPP

#include "opencv2/opencv.hpp"
#include <bits/stdc++.h>

using namespace std;
using namespace cv;

const double EPS = 1e-4;
const double PI = acos(-1.0);

/// struct for storing features of a vertex
/// We may next and previous to int and store squares of the distance for easier calculations
struct Feature {
    double angle;    /// exterior angle
    double next;     /// length of the side towards head
    double previous; /// length of the side towards tail
    cv::Point cood;  /// co-ordinates of the vertex
    int prev;
    int nxt;
};

// returs distance of two points
inline double getDistance(const cv::Point& a, const cv::Point& b) {
    double dSq = (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
    return sqrt(dSq);
}

/// returs angle at b given distance between (a, b), (b, c) and (a, c)
/// Uses cosine formula cost(theta) = (a^2 + b^2 - c^2) / (2ab)
inline double getAngle(cv::Point left, cv::Point right) {
    double num = left.x * right.x + left.y * right.y;
    double den = sqrt(left.x * left.x + left.y * left.y) * sqrt(right.x * right.x + right.y * right.y);
    return acos(num / den);
}

cv::Point getCentroid(vector <cv::Point>& edgePoints) {
  long long sumX = 0, sumY = 0;
  for(int i = 0; i < edgePoints.size(); i++) {
    sumX += edgePoints[i].x;
    sumY += edgePoints[i].y;
  }
  sumX /= edgePoints.size();
  sumY /= edgePoints.size();

  return cv::Point((int)sumX, (int)sumY);
}

/// takes the vector of points on the edge (in order) as arguments
/// returns the vector of features of the vertices
vector <Feature> getFeatures(vector <cv::Point>& edgePoints) {
    vector <Feature> features;
    cv::Point center = getCentroid(edgePoints);
    int totPnts = edgePoints.size();

    for(int pntNo = 0; pntNo < totPnts; pntNo++) {
        int prevPnt = (pntNo - 1);
        if(prevPnt == -1) prevPnt = totPnts - 1;
        int nextPnt = (pntNo + 1);
        if(nextPnt == totPnts) nextPnt = 0;

        /// Now prevPnt is the index of the vertex previous to the current vertex 
        /// and nextPnt is the index of the next vertex

        Feature curFeature;
        /// Calculate the parameters
        curFeature.previous = getDistance(edgePoints[pntNo], edgePoints[prevPnt]);
        curFeature.next = getDistance(edgePoints[pntNo], edgePoints[nextPnt]);
        curFeature.cood = edgePoints[pntNo];
        curFeature.prev = prevPnt;
        curFeature.nxt = nextPnt;
        double distNextPrev = getDistance(edgePoints[prevPnt], edgePoints[nextPnt]);
        cv::Point left = edgePoints[prevPnt] - edgePoints[pntNo];
        cv::Point cent = center - edgePoints[pntNo];
        cv::Point right = edgePoints[nextPnt] - edgePoints[pntNo];
        curFeature.angle = 2 * PI - getAngle(left, cent) - getAngle(cent, right);
        features.push_back(curFeature);
    }
    return features;
}

#endif
