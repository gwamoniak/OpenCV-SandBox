#include <opencv2/img_inputproc/img_inputproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

// Draw a single point
static void draw_point( Mat& img_input, Point2f fp, Scalar color )
{
    circle( img_input, fp, 2, color, CV_FILLED, CV_AA, 0 );
}

// Draw delaunay triangles
static void draw_delaunay( Mat& img_input, Subdiv2D& subdiv, Scalar delaunay_color )
{

    vector<Vec6f> triangleList;
    subdiv.getTriangleList(triangleList);
    vector<Point> pt(3);
    Size size = img_input.size();
    Rect rect(0,0, size.width, size.height);

    for( size_t i = 0; i < triangleList.size(); i++ )
    {
        Vec6f t = triangleList[i];
        pt[0] = Point(cvRound(t[0]), cvRound(t[1]));
        pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
        pt[2] = Point(cvRound(t[4]), cvRound(t[5]));
        
        // Draw rectangles completely inside the image.
        if ( rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2]))
        {
            line(img_input, pt[0], pt[1], delaunay_color, 1, CV_AA, 0);
            line(img_input, pt[1], pt[2], delaunay_color, 1, CV_AA, 0);
            line(img_input, pt[2], pt[0], delaunay_color, 1, CV_AA, 0);
        }
    }
}

//Draw voronoi diagram
static void draw_voronoi( Mat& img_input, Subdiv2D& subdiv )
{
    vector<vector<Point2f> > facets;
    vector<Point2f> centers;
    subdiv.getVoronoiFacetList(vector<int>(), facets, centers);

    vector<Point> ifacet;
    vector<vector<Point> > ifacets(1);

    for( size_t i = 0; i < facets.size(); i++ )
    {
        ifacet.resize(facets[i].size());
        for( size_t j = 0; j < facets[i].size(); j++ )
            ifacet[j] = facets[i][j];

        Scalar color;
        color[0] = rand() & 255;
        color[1] = rand() & 255;
        color[2] = rand() & 255;
        fillConvexPoly(img_input, ifacet, color, 8, 0);

        ifacets[0] = ifacet;
        polylines(img_input, ifacets, true, Scalar(), 1, CV_AA, 0);
        circle(img_input, centers[i], 3, Scalar(), CV_FILLED, CV_AA, 0);
    }
}


int main( )
{

    // Define window names
    string win_delaunay = "Delaunay Triangulation";
    string win_voronoi = "Voronoi Diagram";
    
    // Turn on animation while drawing triangles
    bool animate = true;

    Scalar delaunay_color(255,255,255), points_color(0, 0, 255);   

    Mat img_input = imread("images/barack.jpg");
    
    Mat img_input_origin = img_input.clone();
    
    // Rectangle to be used with Subdiv2D
    Size size = img_input.size();
    Rect rect(0, 0, size.width, size.height);

    // Create an instance of Subdiv2D
    Subdiv2D subdiv(rect);
    
    // Create a vector of points.
    vector<Point2f> points;

    // Read in the points from a text file
    ifstream inputTxt("points.txt");
    int x, y;
    while(inputTxt >> x >> y)
    {
        points.push_back(Point2f(x,y));
    }
    
    for( auto it = points.begin(); it != points.end(); it++)
    {
        subdiv.insert(*it);
        // Show animation
        if (animate)
        {
            Mat img_input_copy = img_input_origin.clone();
            // Draw delaunay triangles
            draw_delaunay( img_input_copy, subdiv, delaunay_color );
            imshow(win_delaunay, img_input_copy);
            waitKey(100);
        }
        
    }
    
    // Draw delaunay triangles
    draw_delaunay( img_input, subdiv, delaunay_color );

    // Draw points
    for( auto it = points.begin(); it != points.end(); it++)
    {
        draw_point(img_input, *it, points_color);
    }
    
    // Allocate space for Voronoi Diagram
    Mat img_input_voronoi = Mat::zeros(img_input.rows, img_input.cols, CV_8UC3);
    
    // Draw Voronoi diagram
    draw_voronoi( img_input_voronoi, subdiv );

    // Show results.
    imshow( win_delaunay, img_input);
    imshow( win_voronoi, img_input_voronoi);
    waitKey(0);

    return 0;
}