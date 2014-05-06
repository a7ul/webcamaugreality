#include <opencv.hpp>
#include <unistd.h>
#include <iostream>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480


using namespace std;
using namespace cv;

//================================================================================
// GLOBAL VARIABLES

// OPENCV GLOBALS
//------------------
CascadeClassifier face_cascade_detector;
//Angle Calculation Variables
int pivotxref=0,pivotyref=0;
float Headangle;
//Current Location Calculation Variables
int cur_x=0,cur_y=0;
//Image from Camera
Mat imageframe;
//Video Capture device
VideoCapture vcap(0); // open the default camera

//OPENGL GLOBALS
//----------------
GLuint vcamtexture;
GLuint HO_Texture;
GLfloat white[] = {1.0, 1.0, 1.0}; //set the light specular to white
GLfloat black[] = {0.0, 0.0, 0.0}; //set the light ambient to black

//END GLOBAL VARIABLES
//================================================================================
//PROTOTYPES
//OPENCV PROTOTYPES
void imageProcess(VideoCapture cap,Mat& image,int& pivotx,int& pivoty,float& angle,CascadeClassifier face_cascade,int&curx ,int&cury);
static void rotate_image180(cv::Mat &dst);
//COMMON PROTOTYPES
void displayVCAM();
//OPENGL PROTOTYPES
void InitGL(int * argc , char ** argv);
void GLdisplay();
void BGplane();
GLuint LoadvcamTexture(Mat image);
void FreeTexture( GLuint texture );
void reshape (int width, int height);
void displayHeadObject(int x ,int y,GLuint headObjTexture);
GLuint LoadimageTexture( const char * filename, int width, int height );

void MyLine( Mat img, Point start, Point endp );
//================================================================================
//MAIN FUNCTION
int main(int argc, char ** argv)
{
    //INITIALIZE OPENCV OBJECTS
    if(!vcap.isOpened())  // check if we succeeded
    {
        cout<<"\nCamera Can't be opened ! Close other camera apps ";
        //usleep(5000000);
        return -1;
    }
    // Load Face cascade (.xml file) for face detection
    face_cascade_detector.load( "haarcascade_frontalface_alt.xml" );

    //Create window to display the original image
    //namedWindow( "Original", 1 );
    //namedWindow( "Detected Face", 1);

    //INITIALIZE OPENGL OBJECTS
    InitGL(&argc,argv);
    glutMainLoop();
    FreeTexture(vcamtexture);
    FreeTexture(HO_Texture);
    return 0;
}

//==========================================================================
//OPENGL + GLUT FUNCTIONS
void InitGL(int * argc , char ** argv)
{
    glutInit(argc,argv);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
    glutInitWindowSize(WIDTH,HEIGHT);
    glutCreateWindow("GLWindow");
    glutDisplayFunc(GLdisplay);
    glutIdleFunc(GLdisplay);
    glutReshapeFunc(reshape);
    HO_Texture=LoadimageTexture("HO_texture.bmp",256,256);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT0,GL_AMBIENT_AND_DIFFUSE,white);
}

void GLdisplay()
{
    glClearColor (0.0, 0.0, 0.0, 0.0);
  //  glShadeModel (GL_SMOOTH);
    glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glColor3f (1.0, 1.0, 1.0);
    glLoadIdentity ();             /* clear the matrix */
    gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    displayVCAM();//Displays background webcam + does head tracking

    displayHeadObject(cur_x,cur_y,HO_Texture);//displays the Object on the head
    glutSwapBuffers();

}


void reshape (int width, int height)
{
    glViewport (0, 0, (GLsizei) width, (GLsizei) height);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glFrustum (-1.0, 1.0, -1.0, 1.0, 1.3, 20.0);
    glMatrixMode (GL_MODELVIEW);



}
float mapthevalue(float y, float y_min, float y_max, float x_min, float x_max)
{
    float x = (y-y_min)/(y_max-y_min)*(x_max-x_min)+x_min;

    return x;

}




void displayHeadObject(int x ,int y,GLuint headObjTexture)
{

    float openglposx,openglposy;
    openglposx=mapthevalue(x,0,WIDTH,-1.0,1.0);
    openglposy=mapthevalue(y,0,HEIGHT,1.0,-1.0);
    cout<<endl<<"cury -> "<<y<<" curx -> "<<x;
    cout<<endl<<"gly -> "<<openglposy<<" glx -> "<<openglposx;

    glPushMatrix();
    glLoadIdentity();
    //glRotatef(Headangle,0.0,-1.0,0.0);
    glTranslatef(openglposx,openglposy,-1.5f);
    glScalef(0.2,0.2,0.2);
    /*
    glRotatef(-90,1,0,0);
    glutSolidCone(1.0f,2.0f,16,16);
    */
    glutSolidTeapot(1.0f);
    glPopMatrix();

}


void BGplane()
{
    glPushMatrix();
    glTranslatef(0.0f,0.0f,-5.0f);
    glScalef(7.0f,7.0f,0.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,vcamtexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f( 1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f( 1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-1.0f,  1.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}
GLuint LoadvcamTexture(Mat image)
{
    GLuint mTexture;
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glEnable(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    rotate_image180(image);
    cv::Size s = image.size();
    //Generate the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, s.width, s.height, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data); //USE GL_BGR for the Image Data as OpenCV:Mat stores them in BGR format

    return mTexture; //return whether it was successfull
}


GLuint LoadimageTexture( const char * filename, int width, int height )
{
    GLuint texture;
    unsigned char * data;
    FILE * file;
    //The following code will read in our image file (BMP)
    file = fopen( filename, "rb" );
    if ( file == NULL ) return 0;
    data = (unsigned char *)malloc( width * height * 3 );
    fread( data, width * height * 3, 1, file );
    fclose( file );

    glGenTextures( 1, &texture ); //generate the texture with the loaded data
    glBindTexture( GL_TEXTURE_2D, texture ); //bind the texture  to it’s array
    glEnable(GL_TEXTURE_2D);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE ); //set texture environment parameters
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );

    //Generate the texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height,GL_RGB, GL_UNSIGNED_BYTE, data);
    free( data ); //free the texture
    return texture; //return whether it was successfull
}




void FreeTexture( GLuint texture )
{
    glDeleteTextures( 1, &texture );
}
//============================================================================
//OPENCV + OPENGL -- COMMON

void displayVCAM()
{
    imageProcess(vcap,imageframe,pivotxref,pivotyref,Headangle,face_cascade_detector,cur_x,cur_y);
//    cin>>cur_x;
//    cin>>cur_y;
//    vcap>>imageframe;
    vcamtexture=LoadvcamTexture(imageframe);//Will Rotate the image by 180 degree , so do image processing before this
    BGplane();

}



//============================================================================
//OPENCV - IMAGE PROCESSING FUNCTIONS


void imageProcess(VideoCapture cap,Mat& image,int& pivotx,int& pivoty,float& angle,CascadeClassifier face_cascade,int &curx ,int &cury)
{

    cap >> image; // get a new frame from camera
    //imshow( "Original", image ); //Display original image
    resize(image,image,Size(WIDTH,HEIGHT),0,0,INTER_LINEAR);

    // Detect faces
    std::vector<Rect> faces;
    face_cascade.detectMultiScale( image, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

    // Draw circles on the detected faces
    for( int i = 0; i < faces.size(); i++ )
    {
        //cout<<endl<<"x - "<<faces[i].x<<" y - "<<faces[i].y<<" ";
        Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
       // ellipse( image, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

        //---------------------------------------------
        //ANGLE CALCULATION FOR HEAD
        angle=atan(((faces[i].y+faces[i].width*0.5)-pivoty)/(faces[i].x-pivotx))*180.0/3.14159265;
        //Correction to the angle to make y axis as reference instead of x axis
        (angle>0)?(angle=(90-angle)):(angle=(-90-angle));
        //Reinitialize for next angle detection if angle > 40
        //This means the person has moved his/her position
        //rather than only head as head cannot rotate more than 40 degrees
        //under straight posture
        if(abs(angle)>39)
        {
            pivotx=faces[i].x + faces[i].width*0.5;
            pivoty=faces[i].y - faces[i].height*0.5f;
        }
        //FINAL ANGLE VALUE FOR HEAD
        //cout<<"angle in degree: "<<angle;
        //----------------------------------------------
        //FINAL OBJECT POSITION ON HEAD
        curx=WIDTH-center.x;
        cury=center.y-faces[i].height;
        //cout<<"\nHEAD POS X,Y "<<curx<<" "<<cury;

        break; //Basically run the loop only once as we are dealing with only 1 face.
    }

    //imshow( "Detected Face", image ); //Display image with detection ellipse
    waitKey(1); //Wait for 1 millisec

}
static void rotate_image180(cv::Mat &dst)
{
    cv::transpose(dst, dst);
    cv::flip(dst, dst, 1);
    cv::transpose(dst, dst);
    cv::flip(dst, dst, 1);
}

//END
//==========================================================================
