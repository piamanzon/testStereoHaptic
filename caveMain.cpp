
// UPDATED: 2016-08-25  (Y. Hu: Microsoft Visual Studio 2015 + Platform toolset in Visual Studio 2015; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)
// UPDATED: 2018-08-02  (Y. Hu: Microsoft Visual Studio 2017 + Platform toolset in Visual Studio 2017; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)

#include <stdio.h>

#ifndef WIN32
#include <sys/time.h>
#endif
#include "caveMain.h"
#include "FaceManager.h"
#include "hapticManager.h"

/* Function prototypes */
void init_gl();
void drawScene();
void frame();
void cleanup();
void initScene();
void drawCave(void);
void drawEye(int eye);
void drawInStereo();
void drawOpenGL();

#ifndef WIN32
#include <sys/time.h>
#endif

static IFaceManager *gFaceManager = 0;
static IHapticManager *gHapticManager = 0;
static float rotateAmt;
/* information about the stereoscopy */
static int stereo = 1;


int main(int argc,char **argv)
{
	//for CAVELib compatibility, this function will most likely do nothing...
		CAVEConfigure( &argc, argv, NULL);
		//register your per-context (per-window) initialization function
		CAVEInitApplication( (CAVECALLBACK)init_gl, 0 );
		//register your pre-frame function
		CAVEFrameFunction( (CAVECALLBACK)frame, 0 );
		//register your Display function
		CAVEDisplay( (CAVECALLBACK)drawScene, 0 );

	#ifdef KNAVE_LIB
		//register your cleanup function
		// (Because of the way glut does things under the hood,
		//  we have to register it ahead of time,
		//  to get it called back when we quit. )
		CAVEStopApplication( (CAVECALLBACK)cleanup, 0 );
	#endif
		initScene();
		//get things going!
		CAVEInit();
		// Usually, in a CAVELib App, we wait for the escape key
		// to be hit then call CAVEExit, but KNAVELib is going to
		// handle that for us...
	#ifndef KNAVE_LIB
		while (!CAVEgetbutton(CAVE_ESCKEY))
		{
			// Nap so that this busy loop doesn't waste CPU time 
			// reset timeval struct every time for linux compatibility 
			timeval tval;
			tval.tv_usec = 25000;
			tval.tv_sec = 0;
			select(0,NULL,NULL,NULL,&tval);
			//printf( "App running\n");
		}
    
		//call your cleanup function
		CAVEStopApplication( (CAVECALLBACK)cleanup, 0 );
		CAVEExit();
	
	#endif

	return 1;
}
/*******************************************************************************
Init function for OpenGL
*******************************************************************************/
void init_gl(){
	//glEnable(GL_COLOR_MATERIAL);
}

/*******************************************************************************
CAVE specific function DO NOT TOUCH
*******************************************************************************/
void frame(void)
{
    //printf( "Frame Update.\n" );

    if( CAVEBUTTON1 ) printf( "Button 1 Down\n" );
    if( CAVEBUTTON2 ) printf( "Button 2 Down\n" );
    if( CAVEBUTTON3 ) printf( "Button 3 Down\n" );
	
    //timing for navigation!

    float jx, jy, dt, t;
    
    const float SPEED = 2.0f;

    static float prevtime = 0;

    jx=CAVE_JOYSTICK_X;
    jy=CAVE_JOYSTICK_Y;
    t = CAVEGetTime();
    dt = t - prevtime;

    ////// Frames Per Second Counter //////
    
    if( CAVEMasterDisplay() ){
      
      static int frameCount = 0;
      static float lastFpsTime = t;

      ++frameCount;
      if( t-lastFpsTime > 1.0 ){
	printf( "fps: %d %f %f %f\n", frameCount, dt, t, prevtime );
	frameCount = 0;
	lastFpsTime = t;
      }

    }
    
    prevtime = t;

    if (fabs(jy)>0.1 ){
      float wandFront[3];
      CAVEGetVector(CAVE_WAND_FRONT,wandFront);
      CAVENavTranslate(wandFront[0]*jy*SPEED*dt, wandFront[1]*jy*SPEED*dt,
		       wandFront[2]*jy*SPEED*dt);
    }
    if (fabs(jx)>0.2 )
	CAVENavRot(-jx*25.0f*dt,'y');


	rotateAmt += 180.0 * dt;

}

/*******************************************************************************
Cleanup function to remove everything you are using in this program
*******************************************************************************/
void cleanup(){
	gHapticManager->cleanup();

}
/*******************************************************************************
Init function if you have object to initiate
*******************************************************************************/
void initScene()
{
    gFaceManager = IFaceManager::create();
    gFaceManager->setup();

    gHapticManager = IHapticManager::create();
    gHapticManager->setup(gFaceManager);
}

/*******************************************************************************
 Function called for rendering
*******************************************************************************/
void drawScene(){
	if (stereo==1){
		drawEye(1);
		drawEye(2);
	}
	else{
		drawOpenGL();
	}
}
/*******************************************************************************
 Your openGL function add or remove object in your scene
*******************************************************************************/
void drawOpenGL(){
	drawCave();
	gFaceManager->drawFaces(gHapticManager->getColourNum());
	gHapticManager->feelPoints();
	gHapticManager->drawCursor();
	gHapticManager->UpdateHapticMapping();
	gHapticManager->DisplayInfo();
	
}

/*******************************************************************************
 Function responsible for the stereoscopy (NO NEED TO TOUCH)
*******************************************************************************/
void drawEye(int eye){
	//glMatrixMode( GL_PROJECTION );
	//glLoadIdentity();
	double dx0 = -0.070 / 4.0; 
	double dx1 = 0.40 / 4.0; 

/* information for glFrustum DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING*/
//	float frustumTop = 1.2;
//	float frustumRight = 1;
	double frustumTop = 1.0;
	double frustumRight = 1.7777; //Monitor aspect ratio is 16:9 = 1.7777	
	double CAVENear = 1.0;
	double CAVEFar = 10000.0;

	glDrawBuffer(GL_BACK_LEFT);
	if (eye==2){
		glDrawBuffer (GL_BACK_RIGHT);
		dx0=-dx0;
		dx1=-dx1;
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-frustumRight-dx0, frustumRight-dx0, -frustumTop, frustumTop,CAVENear,CAVEFar);
	glTranslatef(0+dx1,0,0);
	glMatrixMode(GL_MODELVIEW);
	glShadeModel(GL_SMOOTH);
	drawOpenGL();
    glFlush();
}

/*******************************************************************************
CAVE specific function DO NOT TOUCH
*******************************************************************************/
void drawCave(void)
{

	//printf( "Drawing.\n" );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

	float headPos[3];
	float headVec[3];

	CAVEGetPosition( CAVE_HEAD, headPos );

	glPushMatrix();

	glTranslatef( headPos[0], headPos[1], headPos[2] );

	glDisable( GL_LIGHTING );

	//headLines
	CAVEGetVector( CAVE_HEAD_FRONT, headVec );
	glBegin( GL_LINES );
		glColor3f( 1,1,1 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_HEAD_BACK, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.5,0.5,0.5 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_HEAD_LEFT, headVec );
	glBegin( GL_LINES );
		glColor3f( 1,1,1 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_HEAD_RIGHT, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.5,0.5,0.5 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_HEAD_UP, headVec );
	glBegin( GL_LINES );
		glColor3f( 1,1,1 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_HEAD_DOWN, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.5,0.5,0.5 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();

	glPopMatrix();

	//wand lines
	CAVEGetPosition( CAVE_WAND, headPos );
	glPushMatrix();
	glTranslatef( headPos[0], headPos[1], headPos[2] );

	CAVEGetVector( CAVE_WAND_FRONT, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.99,0.99,0.99 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_WAND_BACK, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.5,0.5,0.5 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_WAND_LEFT, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.99,0.99,0.99 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_WAND_RIGHT, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.5,0.5,0.5 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_WAND_UP, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.99,0.99,0.99 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_WAND_DOWN, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.5,0.5,0.5 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();

	glPopMatrix();


	CAVENavTransform();

	glBegin( GL_QUADS );

	glColor3f( 0.81, 0.81, 0.81 );

	for( int i=-50; i<50;++i ){
		int shift = i%2;
		for( int j=-50; j<50;j+=2 ){
			
			glVertex3f( j+shift+1, 0,  i   );
			glVertex3f( j+shift,   0,  i   );
			glVertex3f( j+shift,   0,  i+1 );
			glVertex3f( j+shift+1, 0,  i+1 );
			
		}
	}

	glEnd();

	//navigated head-lines
	CAVEGetPosition( CAVE_HEAD_NAV, headPos );
	glPushMatrix();
	glTranslatef( headPos[0], headPos[1], headPos[2] );

	//Begin test of head/wand vectors in navigated coords
	CAVEGetVector( CAVE_HEAD_FRONT_NAV, headVec );
	glBegin( GL_LINES );
		glColor3f( 1,1,1 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_HEAD_BACK_NAV, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.5,0.5,0.5 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_HEAD_LEFT_NAV, headVec );
	glBegin( GL_LINES );
		glColor3f( 1,1,1 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_HEAD_RIGHT_NAV, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.5,0.5,0.5 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_HEAD_UP_NAV, headVec );
	glBegin( GL_LINES );
		glColor3f( 1,1,1 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_HEAD_DOWN_NAV, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.5,0.5,0.5 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();glPopMatrix();

	//wand lines
	CAVEGetPosition( CAVE_WAND_NAV, headPos );
	glPushMatrix();
	glTranslatef( headPos[0], headPos[1], headPos[2] );

	CAVEGetVector( CAVE_WAND_FRONT_NAV, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.99,0.99,0.99 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_WAND_BACK_NAV, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.5,0.5,0.5 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_WAND_LEFT_NAV, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.99,0.99,0.99 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_WAND_RIGHT_NAV, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.5,0.5,0.5 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_WAND_UP_NAV, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.99,0.99,0.99 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();
	CAVEGetVector( CAVE_WAND_DOWN_NAV, headVec );
	glBegin( GL_LINES );
		glColor3f( 0.5,0.5,0.5 );
		glVertex3f( 0, 0, 0 );
		glVertex3f(headVec[0], headVec[1], headVec[2] );
	glEnd();


	glPopMatrix();
}