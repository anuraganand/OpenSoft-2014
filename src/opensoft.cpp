#include "backendOpensoft.hpp"

vector < cv::Mat > outLines;
vector < cv::Mat > colorImages;
vector < cv::Mat > background;


int main(int argc, char *argv[]) {

  Torn_Piece Piece[argc];

    cv::Mat bin, detected_edges, tmp;
    std::vector<std::vector<cv::Point> > contours(argc);
    std::vector<std::vector<Feature> > shapes;

    if(argc >= 2) {
        for(int i = 1; i < argc; ++i) {
            //cout << i << " " << argc << "\n";
            //std::cout << "Executing file " << argv[i] << " ...." << "\n";
            Mat input;
            tmp = cv::imread(argv[i], CV_LOAD_IMAGE_COLOR);
            resize(tmp, input, Size(), 0.5, 0.5, CV_INTER_LINEAR);
            colorImages.push_back(input);
            Mat tmp2;
            cv::cvtColor(input, tmp2, CV_BGR2GRAY);
            if(!input.data) {
                //std::cerr << "Error loading Image " << argv[i-1] << ".\n";
                continue;
            }
            bin = cv::Mat(input.rows,input.cols,CV_8UC1,cvScalarAll(0)); 
            Piece[i].generateBinary(tmp2,bin);
            Piece[i].CannyThreshold(bin, detected_edges);
            contours[i] = Piece[i].contourDetection(detected_edges, argv);

            //std::cout << "Edge detection done for image - " << argv[i] << " done." << std::endl;
            //std::cout << contours[i].size() << "\n";
            shapes.push_back(getFeatures(contours[i]));
            
        }

        //printf("Initialization Done .....\n");


        assert(outLines[0].channels() == 1);

        while(colorImages.size()>1){

            int k;
            vector <Match> matching = findBestMatch(shapes);
            for(k = 0; k < matching.size(); k++) {
              int sp1 = matching[k].shape1;
              int sp2 = matching[k].shape2;
              int v1 = matching[k].vertex1;
              int v2 = matching[k].vertex2;

              if (getDistance(shapes[sp1][shapes[sp1][v1].prev].cood, getTranslated(shapes[sp2][shapes[sp2][v2].nxt].cood, matching[k])) < DIST_MATCH_THRESH /5 \
                && getDistance(shapes[sp1][shapes[sp1][v1].nxt].cood, getTranslated(shapes[sp2][shapes[sp2][v2].prev].cood, matching[k])) < DIST_MATCH_THRESH /5 ) {
                break;
              }
            }
            if(k == matching.size()) k = 0;
            Match found = matching[k]; 
            if(found.shape1<0 || found.shape2<0) break;           
            Mat new_Image = translatedImage(found);

            int erase1, erase2;
            erase1 = min(found.shape1,found.shape2);
            erase2 = max(found.shape1, found.shape2)-1;

            colorImages.erase(colorImages.begin()+erase1);
            colorImages.erase(colorImages.begin()+erase2);
            colorImages.push_back(new_Image);

            outLines.erase(outLines.begin()+erase1);
            outLines.erase(outLines.begin()+erase2);

            background.erase(background.begin()+erase1);
            background.erase(background.begin()+erase2);

            shapes.erase(shapes.begin()+erase1);
            shapes.erase(shapes.begin()+erase2);

            Mat temp, binry, temp_detect;
            Torn_Piece tempPiece;
            std::vector<cv::Point> temp_contours;

            cv::cvtColor(new_Image,temp, CV_BGR2GRAY);

            binry = cv::Mat(new_Image.rows, new_Image.cols, CV_8UC1, cvScalarAll(0));
            tempPiece.generateBinary(temp,binry);
            tempPiece.CannyThreshold(binry, temp_detect);
            temp_contours = tempPiece.contourDetection(temp_detect,argv);
            shapes.push_back(getFeatures(temp_contours));

        }

        cv::imwrite("Final_Output.jpg",colorImages[0]);

    }
    else if (argc < 2) {
        //std::cerr << "Please provide input Image .." << "\n";
        exit (-1);
    }

    cv::destroyAllWindows();
    return 0;
}
