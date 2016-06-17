
#include <opencv2/opencv.hpp>
#include <cassert>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <wiringPi.h>
#include <wiringSerial.h>

#define PIN_RELAY 7
#define PIN_INF 2
#define FULL 5
#define THERE 2
const char  * WINDOW_NAME  = "Face Tracker";
const int CASCADE_NAME_LEN = 2048;
char    CASCADE_NAME[CASCADE_NAME_LEN] = "haarcascade_frontalface_alt2.xml";
using namespace std;
using namespace cv;
class DETECT{
private:
   int where;
   int record[FULL];
   int count[FULL+1];
public:
   DETECT();
   ~DETECT();
   void add(int num);
   void clear();
   int who();
};

int GetMaxNum(int* arr,int len  ,int& num)
{
  int index = arr[0];	
  int i;
  for (i = 0; i < len; i++)	{
    
    if (arr[i]>index)
    {
      index = arr[i];
      num = i;
    }
    //printf("%d\n", num);
  }
  return index;
}


DETECT::DETECT(){
	where=0;
	for(int i=0;i<FULL;i++){
		record[i]=-1;
		count[i]=0;
	}
	count[FULL]=FULL;
}

DETECT::~DETECT(){}

void DETECT::clear()
{
	where=0;
        for(int i=0;i<FULL;i++){
                record[i]=-1;
                count[i]=0;
        }
        count[FULL]=FULL;
}

void DETECT::add(int num)
{
	int temp = record[where];
	record[where++]=num;
	if(where>=FULL) where=0;
	
	if(num==-1) count[FULL]++;
	else count[num]++;

	if(temp==-1) count[FULL]--;
	else count[temp]--;
}

int DETECT::who(){
	int max=-1;
	int index=0;
	for(int i=0;i<FULL+1;i++)
	{
		if(count[i]>max)
		{max=count[i]; index=i;}
	}
	if(index==FULL || max < THERE) return -1;
	else
		return index;
}

void thereisperson()
{
   while(digitalRead(PIN_INF)==0){};
   printf("Hello!\n");
   return;
}
 
void opendoor(int id)
{
   digitalWrite(PIN_RELAY,HIGH);
   printf("Welcome %d!\n",id);
   delay(5000);
   digitalWrite(PIN_RELAY,LOW);
}

int main (int argc, char * const argv[])
{
    wiringPiSetup();
    int handle = serialOpen("/dev/ttyAMA0", 9600);
	if (handle == -1) {
		printf("bluetooth error\n");
		return 1;
	}
    pinMode(PIN_RELAY,OUTPUT);
    digitalWrite(PIN_RELAY,LOW);    
    pinMode(PIN_INF,INPUT);
    const int scale = 2;
    DETECT mydec;
    // create all necessary instances
    printf("Now loading data ...\n");
    CvCapture * camera = cvCreateCameraCapture (CV_CAP_ANY);
    CvHaarClassifierCascade* cascade = (CvHaarClassifierCascade*) cvLoad (CASCADE_NAME, 0, 0, 0);
    CvMemStorage* storage = cvCreateMemStorage(0);
    assert (storage);
    
    // you do own an iSight, don't you ?!?
    if (! camera)
        abort ();

    // did we load the cascade?!?
    if (! cascade)
        abort ();
    
    // get an initial frame and duplicate it for later work
    IplImage *  current_frame = cvQueryFrame (camera);
    IplImage *  draw_image    = cvCreateImage(cvSize (current_frame->width, current_frame->height), IPL_DEPTH_8U, 3);
    IplImage *  gray_image    = cvCreateImage(cvSize (current_frame->width, current_frame->height), IPL_DEPTH_8U, 1);
    IplImage *  small_image   = cvCreateImage(cvSize (current_frame->width / scale, current_frame->height / scale), IPL_DEPTH_8U, 1);
    assert (current_frame && gray_image && draw_image);
    
    
    //add FaceRecognition
    Ptr<FaceRecognizer> model = createEigenFaceRecognizer(100, 1e4);
    model->load("training_data.model");
    
	// as long as there are images ..
    printf("Start Working!\n");


    	char msg[18];
	msg[0] = '{';
	msg[1] = '1';
	msg[16] = '}';
	msg[17] = '\0';

	//time
	time_t now;
	struct tm *timenow;

    while ((current_frame = cvQueryFrame (camera)) != NULL)
    {
        // convert to gray and downsize
        cvCvtColor (current_frame, gray_image, CV_BGR2GRAY);
        cvResize (gray_image, small_image, CV_INTER_LINEAR);
        
        // detect faces
        CvSeq* faces = cvHaarDetectObjects (small_image, cascade, storage,
                                            1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize (30, 30));
        // draw faces
        cvFlip (current_frame, draw_image, 1);
        
        double maxR = 0;
        CvPoint maxP;
        maxP.x = 0;
        maxP.y = 0;
        for (int i = 0; i < (faces ? faces->total : 0); i++)
        {
            CvRect* r = (CvRect*) cvGetSeqElem (faces, i);
            CvPoint center;
            int radius;
            center.x = cvRound((small_image->width - r->width*0.5 - r->x) *scale);
            center.y = cvRound((r->y + r->height*0.5)*scale);
            radius = cvRound((r->width + r->height)*0.25*scale);
            
            //filter other smaller circles
            if (radius > maxR)
            {
                maxR = radius;
                maxP = center;
            }
            //cvCircle (draw_image, center, radius, CV_RGB(0,255,0), 3, 8, 0 );
        }
	
	//printf("GO HERE\n");
        if ((faces ? faces->total : 0) > 0) {
            
            cvCircle (draw_image, maxP, maxR, CV_RGB(0,255,0), 3, 8, 0 );
            
            Mat image(current_frame, 0);
            int min_x = (maxP.x - maxR > 0) ? (maxP.x - maxR) : 0;
            int min_y = (maxP.y - maxR > 0) ? (maxP.y - maxR) : 0;
            min_x = image.cols - min_x - maxR * 2;
            
            Mat src(image, Rect(min_x, min_y, maxR * 2, maxR * 2));
            
            resize(src, src, Size(192, 168));
            
            //add your recognition here... src is the face
            cvtColor(src, src, CV_BGR2GRAY);
            
            equalizeHist(src, src);
            int id;
            id =  model->predict(src);
            mydec.add(id);
	    cout <<"Now id is " <<id << endl;
        }
	else{
		cout << "No Face!" <<endl;
		continue;
	}
//        "Finally:" << mydec.who() <<endl;
	int who = mydec.who();
	if (who > 0) {
		//send msg
		int pid = who;

		sprintf(msg + 2, "%04d", pid);

		time(&now);
		timenow = localtime(&now);

		char tmp[20];
		char date[9];
		sscanf(asctime(timenow), "%s %s %s %s %s", tmp, tmp, tmp, date, tmp);

		memcpy(msg + 6, date, 8);
		
		//result
		msg[14] = '1';

		int check_bit = 0;
		for (int i = 1; i <= 14; i++) {
			check_bit += msg[i];
		}
		check_bit %= 128;

		msg[15] = (char)check_bit;

		printf("send out: %s\n", msg);
		serialPuts(handle, msg);
	
		opendoor(who);
		mydec.clear();
		
		//clear buffer
		cvQueryFrame(camera);
		delay(500);
		cvQueryFrame(camera);
	}
	else {
		
	}
		
        
        // wait a tenth of a second for keypress and window drawing
        int key = cvWaitKey (20);
        if (key == 'q' || key == 'Q')
            break;
    }
    
	serialClose(handle);
    // be nice and return no error
    return 0;
}


