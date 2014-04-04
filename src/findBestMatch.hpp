#ifndef __FIND_BEST_MATCH
#define __FIND_BEST_MATCH

#include "edgeFeatures.hpp"

const double DIST_MATCH_THRESH = 100.0;
const double ANG_MATCH_THRESH = 0.15;


/// structure for returning the best possible match according to the algo
/// firstShapeNo is the index of the firstShape and firstVertexNo us the index of vertex
/// similarly, secondshapeNo and secondVertexNo is defined
struct Match {
  int matchVal;
  int shape1;
  int vertex1;
  Point pnt1;
  Point prev1;
  Point next1;

  int shape2;
  int vertex2;
  Point pnt2;
  Point prev2;
  Point next2;

  Match() : matchVal(0), shape1(-1), shape2(-1) {}

  Match(int s1, int v1,int s2,int v2, int match) : matchVal(match), shape1(s1), vertex1(v1), shape2(s2), vertex2(v2) {}
};


/// find the matching value as described in the paper
int getMatchVal(Feature & A, Feature & B, double minPerimeter) {
    /// check the angle
    if(fabs(2 * PI - (A.angle + B.angle)) > ANG_MATCH_THRESH) return 0;
    int countDistMatch = 0;

    /// Check the previous and next side lengths
    countDistMatch += fabs(A.previous - B.next) < DIST_MATCH_THRESH;
    countDistMatch += fabs(A.next - B.previous) < DIST_MATCH_THRESH;
    int retVal = 0;
    if(countDistMatch == 2) retVal = 2;
    else if(countDistMatch == 1) retVal = 1;

    /// check if matchlength is > 0.1 or 0.2 of perimeter length
    // double matchLength = min(A.previous, B.previous) + min(A.next , B.next);
    // if(matchLength > 0.2 * minPerimeter) retVal++;
    // else if(matchLength > 0.1 * minPerimeter) retVal += 2;
    return retVal; 
}

bool comp(Match A, Match B) {
  return A.matchVal > B.matchVal;
}

/// find the best match
vector <Match> findBestMatch(vector < vector <Feature> > & shapes) {

    Match bestMatch;
    vector <Match> ret;
    int totShapes = (int)shapes.size();
    vector <double> perimeters(shapes.size());
    for(int idx = 0; idx < totShapes; idx++) {
        perimeters[idx] = 0;
        for(size_t v = 0; v < shapes[idx].size(); v++) 
            perimeters[idx] += getDistance(shapes[idx][v].cood, \
                    shapes[idx][(v + 1) % shapes[idx].size()].cood);
    }

    for (int shape1 = 0; shape1 < totShapes; shape1++) {
        int totVertices = shapes[shape1].size();

        for (int vertex1 = 0; vertex1 < totVertices; vertex1++) {

            for (int shape2 = 0; shape2 < shape1; shape2++) {
                for (int vertex2 = 0; vertex2 < shapes[shape2].size(); vertex2++) {
                    int curMatch = getMatchVal(shapes[shape1][vertex1], shapes[shape2][vertex2],\
                            min(perimeters[shape1], perimeters[shape2]));

                    bestMatch = Match(shape1, vertex1, shape2, vertex2, curMatch);
                    bestMatch.pnt1 = shapes[shape1][vertex1].cood;
                    bestMatch.prev1 = shapes[shape1][shapes[shape1][vertex1].prev].cood;
                    bestMatch.next1 = shapes[shape1][shapes[shape1][vertex1].nxt].cood;
                    
                    bestMatch.pnt2 = shapes[shape2][vertex2].cood;
                    bestMatch.prev2 = shapes[shape2][shapes[shape2][vertex2].prev].cood;
                    bestMatch.next2 = shapes[shape2][shapes[shape2][vertex2].nxt].cood;

                    ret.push_back(bestMatch);
                }
            }
        }
    }
    sort(ret.begin(), ret.end(), comp);
    return ret;
}

#endif
