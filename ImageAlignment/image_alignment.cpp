#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;
using namespace std;

Mat GetGradient(Mat src_gray)
{
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;

    int scale = 1;
    int delta = 0;
    int ddepth = CV_32FC1; ;

    // Calculate the x and y gradients using Sobel operator
    Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_x, abs_grad_x);

    Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y);

    // Combine the two gradients
    Mat grad;
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

    return grad;

}
 
void ImageAlignment(const Mat& src) 
{
        // Find the width and height of the color image
    Size sz = src.size();
    int height = sz.height / 3;
    int width = sz.width;

    // Extract the three channels from the gray scale image
    vector<Mat>channels;
    channels.emplace_back(src(Rect(0, 0, width, height)));
    channels.emplace_back(src(Rect(0, height, width, height)));
    channels.emplace_back(src(Rect(0, 2 * height, width, height)));

    // Merge the three channels into one color image
    Mat im_color;
    merge(channels, im_color);

    // Set space for aligned image.
    vector<Mat> aligned_channels;
    aligned_channels.emplace_back(Mat(height, width, CV_8UC1));
    aligned_channels.emplace_back(Mat(height, width, CV_8UC1));

    // The blue and green channels will be aligned to the red channel.
    // So copy the red channel
    aligned_channels.emplace_back(channels[2].clone());

    // Define motion model
    const int warp_mode = MOTION_AFFINE;


    // Set the warp matrix to identity.
    Mat warp_matrix;
    if (warp_mode == MOTION_HOMOGRAPHY)
        warp_matrix = Mat::eye(3, 3, CV_32F);
    else
        warp_matrix = Mat::eye(2, 3, CV_32F);

    // Set the stopping criteria for the algorithm.
    int number_of_iterations = 5000;
    double termination_eps = 1e-10;

    TermCriteria criteria(TermCriteria::COUNT + TermCriteria::EPS,
        number_of_iterations, termination_eps);

    // Warp the blue and green channels to the red channel
    for (int i = 0; i < 2; i++)
    {
        double cc = findTransformECC(
            GetGradient(channels[2]),
            GetGradient(channels[i]),
            warp_matrix,
            warp_mode,
            criteria
            );

        cout << "CC " << cc << endl;
        if (cc == -1)
        {
            cerr << "The execution was interrupted. The correlation value is going to be minimized." << endl;
            cerr << "Check the warp initialization and/or the size of images." << endl << flush;
        }


        if (warp_mode == MOTION_HOMOGRAPHY)
            // Use Perspective warp when the transformation is a Homography
            warpPerspective(channels[i], aligned_channels[i], warp_matrix, aligned_channels[0].size(), INTER_LINEAR + WARP_INVERSE_MAP);
        else
            // Use Affine warp when the transformation is not a Homography
            warpAffine(channels[i], aligned_channels[i], warp_matrix, aligned_channels[0].size(), INTER_LINEAR + WARP_INVERSE_MAP);

}

    // Merge the three channels
    Mat im_aligned;
    merge(aligned_channels, im_aligned);

    // Show final output
    imshow("Color Image", im_color);
    imshow("Aligned Image", im_aligned);
    waitKey(0);
}

void BlobDetection(const Mat& src) 
{

    // Setup SimpleBlobDetector parameters.
    SimpleBlobDetector::Params params;

    // Change thresholds
    params.minThreshold = 10;
    params.maxThreshold = 200;

    // Filter by Area.
    params.filterByArea = true;
    params.minArea = 1500;

    // Filter by Circularity
    params.filterByCircularity = true;
    params.minCircularity = 0.1;

    // Filter by Convexity
    params.filterByConvexity = true;
    params.minConvexity = 0.87;

    // Filter by Inertia
    params.filterByInertia = true;
    params.minInertiaRatio = 0.01;


    // Storage for blobs
    vector<KeyPoint> keypoints;


    // Set up detector with params
    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);

    // Detect blobs
    detector->detect(src, keypoints);


    // Draw detected blobs as red circles.
    // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
    // the size of the circle corresponds to the size of blob

    Mat im_with_keypoints;
    drawKeypoints(src, keypoints, im_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    // Show blobs
    imshow("keypoints", im_with_keypoints);
    waitKey(0);

}

int main() 
{
    

    
    // Image Alignment test
    //Mat im = imread("/images/emir.jpg", IMREAD_GRAYSCALE);
    
    // blob test
    Mat im = imread("/images/blob.jpg", IMREAD_GRAYSCALE);
    if (im.empty())
    {
        cerr << "Error opening image \n";
        return EXIT_FAILURE;
        
    }
    else
    {
        //ImageAlignment(im);
        BlobDetection(im);
    }

    

    return 0;

    
}