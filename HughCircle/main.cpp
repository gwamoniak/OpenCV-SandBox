#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/core/core.hpp>




using namespace cv;
using namespace std;

int main() {

	Mat frame;
	Mat grayframe;

	VideoCapture stream1(0);   //0 is the id of video device.0 if you have only one camera.

	if (!stream1.isOpened()) { //check if video device has been initialised
		cout << "cannot open camera";
	}
	vector<Vec3f> circles;
	int radius = 1;
	Point center;
	//unconditional loop
	while (cv::waitKey(1) != 27) // 27 = ascii value of ESC
	{
		stream1.read(frame);
		cvtColor(frame, grayframe, COLOR_BGR2GRAY);
		GaussianBlur(grayframe, grayframe, Size(9, 9), 2, 2);


		/// Apply the Hough Transform to find the circles
		HoughCircles(grayframe, circles, HOUGH_GRADIENT, 1, 30, 100, 50, 0, 0);

		/// Draw the circles detected
		for (size_t i = 0; i < circles.size(); i++)
		{
			center.x = cvRound(circles[i][0]);
			center.y = cvRound(circles[i][1]);
			radius = cvRound(circles[i][2]);
			// circle center
			circle(frame, center, 3, Scalar(0, 255, 0), 3, LINE_AA);
			// circle outline
			circle(frame, center, radius, Scalar(0, 0, 255), 3, LINE_AA);

			std::string buffer = "Radius: " + std::to_string(radius);

			cv::putText(frame, buffer, cv::Point(0, 32), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255, 255));
			cv::line(frame, center, cv::Point(center.x + radius, center.y), cv::Scalar(255, 255, 255, 255));


		}

		namedWindow("Hough Circle Transform Demo", WINDOW_AUTOSIZE);
		imshow("Hough circles", frame);

	}
	circles.clear();
	return 0;
}