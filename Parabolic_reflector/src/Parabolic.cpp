#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <geometry_msgs/Twist.h>

using namespace cv;
using namespace std;

static const std::string OPENCV_WINDOW = "Image window";

class ImageConverter
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;

  public:
     ImageConverter()
         : it_(nh_)
  	{
    		// Subscribe to input video feed and publish output video feed
    		image_sub_ = it_.subscribe("usb_cam/image_raw", 1,
      		&ImageConverter::imageCb, this);
    		image_pub_ = it_.advertise("/image_converter/output_video", 1);

    		cv::namedWindow(OPENCV_WINDOW,CV_WINDOW_AUTOSIZE);
  	}

    ~ImageConverter()
       {
        	cv::destroyWindow(OPENCV_WINDOW);
       }
   
   void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {
       geometry_msgs::Twist twist;

       cv_bridge::CvImagePtr cv_ptr;
    	try
   	 {
      	    cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    	 }
    	catch (cv_bridge::Exception& e)
    	 {
      	    ROS_ERROR("cv_bridge exception: %s", e.what());
            return;
         }


        int iLowH = 58;
 	int iHighH = 110;

 	int iLowS = 154;
 	int iHighS = 255;

 	int iLowV = 0;
 	int iHighV = 255;

        Mat image=cv_ptr->image;
        Mat imgHSV;
        Mat imgThresholded;


        rectangle();//function which draws rectangle on screen
        cvtColor(frame, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
        
	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

	//morphological opening (remove small objects from the foreground)
  	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
  	dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

  	//morphological closing (fill small holes in the foreground)
  	dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
  	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
  
   	//Calculate the moments of the thresholded image
  	Moments oMoments = moments(imgThresholded);

  	double dM01 = oMoments.m01;
  	double dM10 = oMoments.m10;
  	double dArea = oMoments.m00;
   
  	posX = dM10 / dArea;
  	posY = dM01 / dArea;
  
  	cout <<"posX:\n"<<posX<<endl; 
  	cout <<"posY:\n"<<posY<<endl; 
         
 	circle( frame, Point(posX, posY), 2, Scalar( 255, 255, 255 ), 2, 8 );
	
	//function which detect whether object passes the line or not
        detect_line();

	imshow("Thresholded Image", imgThresholded); //show the thresholded image
        imshow("Original", frame); //show the original image

        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
       {
            cout << "esc key is pressed by user" << endl;
            break; 
       }
 }
   
   void detect_line()
{
        if(posX < x2 && posX > x1)
         {
           cout<<"Detected\n"<<endl;
         }       
}

   void rectangle()
{

  	x1=frame.cols/2 - 30;
  	double y1=0;
  	x2=frame.cols/2 + 30;
  	int y2=frame.rows; 
  	rectangle( frame,Point(x1,y1),Point(x2,y2),Scalar( 0, 0, 255 ),1,8 );

}


     
 
