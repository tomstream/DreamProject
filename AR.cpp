#include "AR.h"
#include <windows.h>
#include <iostream>
#include <GL/glut.h>

using namespace cv;
using namespace std;

VideoCapture cap(0);
extern int windowWidth;
extern int windowHeight;
static int num_track = 4;

double camD[9] = { 618.526381968738, 0, 310.8963715614199,
0, 619.4548980786033, 248.6374860176724,
0, 0, 1 };
double distCoeffD[5] = { 0.09367405350511771, -0.08731677320554751, 0.002823563134787144, -1.246739177460954e-005, -0.0469061739387372 };
Mat camera_matrix = Mat(3, 3, CV_64FC1, camD);
Mat distortion_coefficients = Mat(5, 1, CV_64FC1, distCoeffD);

vector<Point3f> objP;
Mat objPM;
vector<double> rv(3), tv(3);
Mat rvecm(rv), tvecm(tv);
int thread_quit = 0;

Mat gray, prevGray, image, frame, fixImg;
bool recogonize = false;
vector<Point2f> points[2];
vector<KeyPoint> keypoints;
vector<KeyPoint> initPoints;
vector<Point2f> recoveryPoints;
vector<Point2f> projectedPoints;
vector<Point2f> goodfeatures;
bool initflag = true;
bool needToGetgf = false;
bool needtomap = false;
bool needtokeeptime = false;

const int MAX_COUNT = 500;
size_t trackingpoints = 0;

Mat initdescriptors;

string msg;
int baseLine;
Size textSize;

DWORD t1, t2;
int framenum = 0;

TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 30, 0.01);
Size subPixWinSize(10, 10), winSize(21, 21);


static void help()
{
	cout << "\nHot keys: \n"
		"    Tracking: \n"
		"\tESC - quit the program\n"
		"\tg - get the good corners\n"
		"\tc - delete all the points\n"
		"\tm - choose four points to track and update camera pose then\n"
		"\t\t(To add a tracking point, please click that point)\n"
		"    Generation: \n"
		"\tESC - quit the program\n"
		"\te - change the view mode\n" << endl;

}

void on_mouse(int event, int x, int y, int flag, void *param)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		if (needtomap && points[1].size()<num_track)
		{
			if (points[1].size() == num_track - 1){
				recogonize = true;
			}
			for (size_t i = 0; i<goodfeatures.size(); i++)
			{
				if (abs(goodfeatures[i].x - x) + abs(goodfeatures[i].y - y)<5)
				{
					points[1].push_back(goodfeatures[i]);
					initPoints.push_back(keypoints[i]);
					trackingpoints++;
					break;
				}
			}
			Mat temp;
			image.copyTo(temp);
			for (size_t i = 0; i < points[1].size(); i++)
			{
				circle(temp, points[1][i], 3, Scalar(0, 0, 255), -1, 8);
			}

			msg = format("Resolution: %d * %d.  Corner number: %d.  Tracked points: %d", (int)cap.get(CV_CAP_PROP_FRAME_WIDTH), (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT), goodfeatures.size(), trackingpoints);
			baseLine = 0;
			textSize = getTextSize(msg, 1, 1, 1, &baseLine);
			Point textOrigin(temp.cols - textSize.width - 20, temp.rows - 2 * baseLine - 10);
			putText(temp, msg, textOrigin, 1, 1, Scalar(0, 0, 255));

			imshow("Tracking", temp);
		}
	}
}

void init3DP()
{
	if (!objP.empty())
		objP.clear();
	objP.push_back(Point3f(-0.5, -0.5, 0));    //三维坐标的单位是毫米
	objP.push_back(Point3f(0.5, -0.5, 0));
	objP.push_back(Point3f(0.5, 0.5, 0));
	objP.push_back(Point3f(-0.5, 0.5, 0));
	objPM.setTo(0);
	Mat(objP).convertTo(objPM, CV_32F);
}

bool init()
{
	cap.set(CV_CAP_PROP_FRAME_WIDTH, windowWidth);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, windowHeight);

	if (!cap.isOpened())
	{
		cout << "Could not initialize capturing...\n";
		return false;
	}

	namedWindow("Tracking", 1);
	cvSetMouseCallback("Tracking", on_mouse, NULL);

	init3DP();
	return true;
}

void getPlanarSurface(vector<Point2f>& imgP){

	solvePnP(objPM, Mat(imgP), camera_matrix, distortion_coefficients, rvecm, tvecm);

	projectedPoints.clear();
	projectPoints(objPM, rvecm, tvecm, camera_matrix, distortion_coefficients, projectedPoints);
	fixImg = image.clone();
	for (unsigned int i = 0; i < projectedPoints.size(); ++i)
	{
		circle(image, projectedPoints[i], 3, Scalar(255, 0, 0), -1, 8);
	}
}


int tracking_update()
{
	ORB orb;
	for (;;)
	{
		if (thread_quit){
			thread_quit++;
			if (thread_quit == 3)break;
		}
		if (needtomap && goodfeatures.size()>0)
		{
			needToGetgf = false;
			if (trackingpoints<num_track)
			{
				char c = waitKey(2);
				if (c == 'c')
				{
					points[0].clear();
					points[1].clear();
					trackingpoints = 0;
					goodfeatures.clear();
					needToGetgf = false;
					needtomap = false;
					initflag = true;
					initPoints.clear();
					init3DP();
				}
				if (c == 27)
					break;
				continue;
			}
			if (initflag && initPoints.size() == num_track)
			{
				ORB orb;
				cvtColor(image, gray, COLOR_BGR2GRAY);
				orb.compute(gray, initPoints, initdescriptors);
				initflag = false;
			}
			if (recogonize)
				frame = fixImg;
			else{
				cap >> frame;
				fixImg = frame.clone();
			}
			if (frame.empty())
				break;
			if (!recogonize)frame.copyTo(fixImg);
			frame.copyTo(image);
			cvtColor(image, gray, COLOR_BGR2GRAY);

			if (!points[0].empty())
			{
				vector<uchar> status;
				vector<float> err;
				if (prevGray.empty())
					gray.copyTo(prevGray);
				calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err);
				size_t i, k;
				int missingPointIndex = 0;
				for (i = k = 0; i < points[1].size(); i++)
				{
					if (!status[i])
					{
						missingPointIndex = i;
						continue;
					}
					points[1][k++] = points[1][i];
					circle(image, points[1][i], 3, Scalar(0, 0, 255), -1, 8);
				}
				if (k == num_track)
				{
					getPlanarSurface(points[0]);
				}
				else
				{
				}
			}
			framenum++;
		}
		else
		{
			//cap >> frame;
			if (recogonize)
				frame = fixImg;
			else{
				cap >> frame;
				fixImg = frame.clone();
			}
			if (frame.empty())
				break;

			frame.copyTo(image);
			if (needToGetgf)
			{
				cvtColor(image, gray, COLOR_BGR2GRAY);

				// automatic initialization
				//				goodFeaturesToTrack(gray, goodfeatures, MAX_COUNT, 0.01, 10);
				orb.detect(gray, keypoints);
				goodfeatures.clear();
				for (size_t i = 0; i < keypoints.size(); i++) {
					goodfeatures.push_back(keypoints[i].pt);
				}
				cornerSubPix(gray, goodfeatures, subPixWinSize, Size(-1, -1), termcrit);
				for (size_t i = 0; i < goodfeatures.size(); i++)
				{
					circle(image, goodfeatures[i], 3, Scalar(0, 255, 0), -1, 8);
				}
			}
		}

		msg = format("Resolution: %d * %d.  Corner number: %d.  Tracked points: %d",
			(int)cap.get(CV_CAP_PROP_FRAME_WIDTH), (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT), goodfeatures.size(), trackingpoints);
		baseLine = 0;
		textSize = getTextSize(msg, 1, 1, 1, &baseLine);
		Point textOrigin(image.cols - textSize.width - 20, image.rows - 2 * baseLine - 10);
		putText(image, msg, textOrigin, 1, 1, Scalar(0, 0, 255));

		imshow("Tracking", image);

		char c = (char)waitKey(5);
		if (c == 27)
		{
			cap.release();
			return 0;
		}
		if (thread_quit == 2){
			waitKey(0);
			while (1){}
		}

		switch (c)
		{
		case 'g':
			needToGetgf = true;
			needtomap = false;
			break;
		case 'c':
			points[0].clear();
			points[1].clear();
			trackingpoints = 0;
			goodfeatures.clear();
			needToGetgf = false;
			needtomap = false;
			initflag = true;
			initPoints.clear();
			init3DP();
			break;
		case 'm':
			trackingpoints = 0;
			needtomap = true;
			break;
		case 't':
			needtokeeptime = true;
			framenum = 0;
			t1 = GetTickCount();
			break;
		}

		std::swap(points[1], points[0]);
		cv::swap(prevGray, gray);
		framenum++;
	}
	if (needtokeeptime)
	{
		t2 = GetTickCount();
		//cout << endl << "fps:" << framenum / ((t2 - t1)*1.0 / 1000) << "\n";
	}
	return 0;
}

void tracking_thread()
{
	help();

	if (!init())
		return;
	cout << "haha";
	try{
		tracking_update();
	}
	catch (const exception& e){
		cout << e.what();
	}
}


const GLfloat PI = 3.14;

/// record the state of mouse
GLboolean mousedown = GL_FALSE;
GLboolean drawmap = GL_TRUE;
/// when a mouse-key is pressed, record current mouse position
static GLint mousex = 0, mousey = 0;

static GLfloat center[3] = { 0.0f, 0.0f, 0.0f }; /// center position
static GLfloat eye[3]; /// eye's position

static GLfloat yrotate = PI / 2; /// angle between y-axis and look direction
static GLfloat xrotate = PI / 2; /// angle between x-axis and look direction
static GLfloat celength = 15.0f;/// lenght between center and eye

// GLfloat light_ambient[] = {1.0, 1.0, 1.0, 1.0};  /* Red diffuse light. */
// GLfloat light_diffuse[] = {1.0, 0.0, 0.0, 1.0};  /* Red diffuse light. */
// GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};  /* Infinite light location. */

Mat texttmp(windowWidth, windowHeight, CV_8UC3);
double fovx, fovy, focalLength, aspectRatio, z_near = 1.0, z_far = 1000.0;
Point2d principalPt;

void draw_map(std::function<void()> draw)
{
	if (frame.data != NULL)
	{
		cvtColor(frame, texttmp, CV_BGR2RGB);
		flip(texttmp, texttmp, 0);
		glEnable(GL_TEXTURE_2D);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texttmp.data);

		glPushMatrix();

		glScaled(1.0 / windowWidth, 1.0 / windowHeight, 1.0);
		glScaled(1.55, 1.55, 1);
		glTranslated(-windowWidth / 2, -windowHeight / 2, 0.0);
		glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex2i(0, 0);
		glTexCoord2i(1, 0); glVertex2i(windowWidth, 0);
		glTexCoord2i(1, 1); glVertex2i(windowWidth, windowHeight);
		glTexCoord2i(0, 1); glVertex2i(0, windowHeight);
		glEnd();

		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}

	if (needtomap && trackingpoints == num_track)
	{

		/* Use depth buffering for hidden surface elimination. */
		glEnable(GL_DEPTH_TEST);
		Mat rotM(3, 3, CV_64FC1);
		Rodrigues(rvecm, rotM);

		glPushMatrix();
		double model_view_matrix[16] = {
			rotM.at<double>(0, 0), -rotM.at<double>(1, 0), -rotM.at<double>(2, 0), 0,
			rotM.at<double>(0, 1), -rotM.at<double>(1, 1), -rotM.at<double>(2, 1), 0,
			rotM.at<double>(0, 2), -rotM.at<double>(1, 2), -rotM.at<double>(2, 2), 0,
			tv[0], -tv[1], -tv[2], 1
		};
		glLoadMatrixd(model_view_matrix);

		glRotated(180.0, 1.0, 0.0, 0.0);

		draw();

		/* Use depth buffering for hidden surface elimination. */
		glDisable(GL_DEPTH_TEST);
		glPopMatrix();
		if (thread_quit == 0)
			thread_quit = 1;
	}
}


void initAR()
{
	calibrationMatrixValues(camera_matrix, Size(windowWidth, windowHeight), 0.0, 0.0, fovx, fovy, focalLength, principalPt, aspectRatio);
}

