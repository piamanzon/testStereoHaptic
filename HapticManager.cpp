
// UPDATED: 2016-08-25  (Y. Hu: Microsoft Visual Studio 2015 + Platform toolset in Visual Studio 2015; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)
// UPDATED: 2018-08-02  (Y. Hu: Microsoft Visual Studio 2017 + Platform toolset in Visual Studio 2017; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)

#include "caveMain.h"
#include "FaceManager.h"
#include "HapticManager.h"

#include <HDU/hduVector.h>

#include <HD/hd.h>
#include <HL/hl.h>
#include <HDU/hduMatrix.h>
#include <HDU/hduError.h>
#include <iostream>
#include <stdarg.h>
#include <string>
#include <HLU/hlu.h>
#include <GL/glut.h>
#include <vector>
using namespace std;
namespace 
{

class HapticManager : public IHapticManager
{
public:

    HapticManager();
    virtual ~HapticManager();

    virtual void setup(IFaceManager *pFaceManager);
    virtual void cleanup();
    virtual void drawCursor();
    virtual void feelPoints();
	virtual void DisplayInfo(void);
    virtual int getColourNum();
    int colourNum;


protected:

   
	void UpdateHapticMapping(void);
    int getPointIndexFromTouchId(HLuint id) const;

    static void HLCALLBACK hlTouchCB(HLenum event, HLuint object, HLenum thread,
                                     HLcache *cache, void *userdata);
    static void HLCALLBACK hlButtonDownCB(HLenum event, HLuint object, HLenum thread,
                                          HLcache *cache, void *userdata);
    static void HLCALLBACK hlButtonUpCB(HLenum event, HLuint object, HLenum thread,
                                        HLcache *cache, void *userdata);
    static void HLCALLBACK hlMotionCB(HLenum event, HLuint object, HLenum thread,
                                      HLcache *cache, void *userdata);

private:

    HHD m_hHD;
    HHLRC m_hHLRC;

    IFaceManager *m_pFaceManager;

    int m_nManipFace;
    
    HLuint m_nFaceNameBase;

    double m_cursorScale;
    GLuint m_nCursorDisplayList;
	GLuint gCursorDisplayList;
    HLuint m_effectName;       
	double gCursorScale;
	HLuint gMyShapeId;
	vector<HLuint> myShapesId;
    HLuint gMyShapeId2;
    //HLuint gMyInteractionId1;
  

	void DrawBitmapString(GLfloat x, GLfloat y, void *font, char *format,...);
};

} // anonymous namespace


/*******************************************************************************
 Creates a HapticManager.                                                                                
*******************************************************************************/
IHapticManager *IHapticManager::create()
{
    return new HapticManager;
}


/*******************************************************************************
 Destroys the HapticManager.
*******************************************************************************/
void IHapticManager::destroy(IHapticManager *&pInterface)
{
    HapticManager *pImp = static_cast<HapticManager *>(pInterface);
    if (pImp)
    {
        delete pImp;
        pInterface = 0;
    }
}


/*******************************************************************************
 Constructor.
*******************************************************************************/
HapticManager::HapticManager() : 
    m_pFaceManager(0),
    m_nManipFace(-1),
    m_nFaceNameBase(0),
    m_effectName(0),
    m_cursorScale(0),
    m_nCursorDisplayList(0),
	gCursorDisplayList(0),   //initialization updated on 2015-07-21
	gCursorScale(0),         //initialization updated on 2015-07-21
	gMyShapeId(0), 
    gMyShapeId2(0),//initialization updated on 2015-07-21
	myShapesId(0),           //initialization updated on 2015-07-21
	m_hHD(HD_INVALID_HANDLE),
    colourNum(1),
    m_hHLRC(NULL)
{

}


/*******************************************************************************
 Destructor.
*******************************************************************************/
HapticManager::~HapticManager()
{
}


/*******************************************************************************
 Sets up the haptics.
*******************************************************************************/
void HapticManager::setup(IFaceManager *pFaceManager)
{        

    m_pFaceManager = pFaceManager;

    HDErrorInfo error;
    m_hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        //hduPrintError(stderr, &error, "Failed to initialize haptic device");
        //fprintf(stderr, "Press any key to exit");
        //getchar();
        exit(1);
    }   
    
    // Create a haptic context for the device.  The haptic context maintains 
    // the state that persists between frame intervals and is used for
    // haptic rendering.
    m_hHLRC = hlCreateContext(m_hHD);
    hlMakeCurrent(m_hHLRC); 

    int numFaces = pFaceManager->getNumFaces();

	/*for (int i=0; i<6; i++){
		myShapesId.push_back(hlGenShapes(1));
	}*/
	
	gMyShapeId=hlGenShapes(1);

    hlAddEventCallback(HL_EVENT_TOUCH, HL_OBJECT_ANY,
            HL_CLIENT_THREAD, hlTouchCB, this);
    hlAddEventCallback(HL_EVENT_UNTOUCH, HL_OBJECT_ANY,
            HL_CLIENT_THREAD, hlTouchCB, this);
    hlAddEventCallback(HL_EVENT_1BUTTONDOWN, HL_OBJECT_ANY,
            HL_CLIENT_THREAD, hlButtonDownCB, this);   
    //}

    hlAddEventCallback(HL_EVENT_1BUTTONUP, HL_OBJECT_ANY,
        HL_CLIENT_THREAD, hlButtonUpCB, this);

    //Codes for setting up force feedback from the sphere drawn in Step 2

    gMyShapeId2 = hlGenShapes(7);
    hlAddEventCallback(HL_EVENT_TOUCH, HL_OBJECT_ANY, HL_CLIENT_THREAD, hlTouchCB, this); 
    hlAddEventCallback(HL_EVENT_UNTOUCH, HL_OBJECT_ANY, HL_CLIENT_THREAD, hlTouchCB, this);   
    hlAddEventCallback(HL_EVENT_1BUTTONDOWN, HL_OBJECT_ANY, HL_CLIENT_THREAD, hlButtonDownCB, this);     
    hlAddEventCallback(HL_EVENT_1BUTTONUP, HL_OBJECT_ANY, HL_CLIENT_THREAD, hlButtonUpCB, this);    
    // End of the addition  
    
}


/*******************************************************************************
 Cleans up the haptics.
*******************************************************************************/
void HapticManager::cleanup()
{   
    if (m_hHLRC != NULL)
    {

        hlBeginFrame();
        hlStopEffect(m_effectName);
        hlEndFrame();    

        hlDeleteEffects(m_effectName, 1);

        int numFaces = m_pFaceManager->getNumFaces();

        for (int i = 0; i < numFaces; i++)
        {
            HLuint nFaceName = m_nFaceNameBase + i;

            // Unregister event callbacks for all of the points.
            hlRemoveEventCallback(HL_EVENT_TOUCH, nFaceName,
                                  HL_CLIENT_THREAD, hlTouchCB);
            hlRemoveEventCallback(HL_EVENT_UNTOUCH, nFaceName,
                                  HL_CLIENT_THREAD, hlTouchCB);
            hlRemoveEventCallback(HL_EVENT_1BUTTONDOWN, nFaceName,
                                  HL_CLIENT_THREAD, hlButtonDownCB);                            
        }    

        hlRemoveEventCallback(HL_EVENT_1BUTTONUP, HL_OBJECT_ANY,
                              HL_CLIENT_THREAD, hlButtonUpCB);                            

        hlDeleteShapes(m_nFaceNameBase, numFaces);

        hlMakeCurrent(NULL);
        hlDeleteContext(m_hHLRC);
    }

    if (m_hHD != HD_INVALID_HANDLE)
        hdDisableDevice(m_hHD);
}




/*******************************************************************************
 draw the cursor
*******************************************************************************/
void HapticManager::drawCursor(void)
	{
		//cout<<"redraw"<<endl;
		static const double kCursorRadius = 0.5;
		static const int kCursorTess = 15;
		HLdouble proxytransform[16];

		GLUquadricObj *qobj = 0;

		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
		glPushMatrix();

		if (!gCursorDisplayList)
		{
			//cout<<"draw cursor display list"<<endl;
			gCursorDisplayList = glGenLists(1);
			glNewList(gCursorDisplayList, GL_COMPILE);
			qobj = gluNewQuadric();
			glPushMatrix();
			glColor3f(1.,0,0); 
			gluSphere(qobj, kCursorRadius*2 * 4, kCursorTess, kCursorTess);
			glColor3f(0,0,1.);
			gluCylinder(qobj,kCursorRadius * 4,kCursorRadius * 4,40,100,5);
			glPopMatrix();
			gluDeleteQuadric(qobj);
			glEndList();
		}  

		// Apply the local position/rotation transform of the haptic device proxy.
		hlGetDoublev(HL_PROXY_TRANSFORM, proxytransform);
		glMultMatrixd(proxytransform);
        
		// Apply the local cursor scale factor.
		glScaled(gCursorScale, gCursorScale, gCursorScale);
	
		glEnable(GL_NORMALIZE);
		glEnable(GL_COLOR_MATERIAL);
		glColor3f(0.0, 0.5, 1.0);
		glCallList(gCursorDisplayList);
		glPopMatrix(); 
		glPopAttrib();
}



/*******************************************************************************
 draw what the haptic device must reder
*******************************************************************************/
void HapticManager::feelPoints()
{      
    hlBeginFrame();
	int i=0;
    int nFaces = m_pFaceManager->getNumFaces();
    //for (int i = 0; i < nFaces; i++)
    //{   
		hlBeginShape(HL_SHAPE_DEPTH_BUFFER, gMyShapeId);//myShapesId.at(i));
		hlTouchableFace(HL_FRONT_AND_BACK);
		m_pFaceManager->drawFace(i);
        hlEndShape();
    //}
        // Codes for getting force feedback from the sphere drawn in Step 2
        hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, gMyShapeId2);       
        hlTouchableFace(HL_FRONT_AND_BACK);           
        m_pFaceManager->drawFaces(colourNum);       
        hlEndShape(); // End of the addition 

    // Call any event callbacks that have been triggered.*/
    hlCheckEvents();
    hlEndFrame();
	//glDisable(GL_LIGHTING);
  //  colourNum = 1;

}

int HapticManager::getColourNum() {
    return colourNum;
}
/*******************************************************************************
 Highlights the touched object.
*******************************************************************************/
void HLCALLBACK HapticManager::hlTouchCB(HLenum event, HLuint object,
                                         HLenum thread, HLcache *cache, 
                                         void *userdata)
{
    HapticManager *tempP = (HapticManager*)userdata;
    //userdata->drawFaces(1.0f, 0.0f, 0.0f);
    if (tempP->colourNum == 1)
        tempP->colourNum = 2;
    else
        tempP->colourNum = 1;
}


/*******************************************************************************
 Starts manipulation upon button press.
*******************************************************************************/
void HLCALLBACK HapticManager::hlButtonDownCB(HLenum event, HLuint object,
                                              HLenum thread, HLcache *cache,
                                              void *userdata)
{
   Beep( 750, 300 );
}


/*******************************************************************************
 Stops manipulation upon button release.
*******************************************************************************/
void HLCALLBACK HapticManager::hlButtonUpCB(HLenum event, HLuint object,
                                            HLenum thread, HLcache *cache, 
                                            void *userdata)
{
  
}


/*******************************************************************************
 Called whenever the device position changes.
*******************************************************************************/
void HLCALLBACK HapticManager::hlMotionCB(HLenum event, HLuint object,
                                          HLenum thread, HLcache *cache,
                                          void *userdata)
{

}
/*******************************************************************************
 OpenGL function for Displaying string on the screen.
*******************************************************************************/
void HapticManager::DrawBitmapString(GLfloat x, GLfloat y, void *font, char *format,...)
	{
		int len, i;
		va_list args;
		char string[256];

		// special C stuff to interpret a dynamic set of arguments specified by "..."
		va_start(args, format);
		vsprintf(string, format, args);
		va_end(args);

		glRasterPos2f(x, y);
		len = (int) strlen(string);
	
		for (i = 0; i < len; i++)
		{
			glutBitmapCharacter(font, string[i]);
		}
	}

/*******************************************************************************
 Information that will be displayed on the screen
*******************************************************************************/
void HapticManager::DisplayInfo(void)
{
	int mWindW=600;
	int mWindH=600;
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

    glPushMatrix();
    glLoadIdentity();

	// switch to 2d orthographic mMode for drawing text
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, mWindW, mWindH, 0);
	glMatrixMode(GL_MODELVIEW);
	
	glColor3f(1.0, 1.0, 1.0);

	int textRowDown = 0; // lines of text already drawn downwards from the top
	int textRowUp = 0; // lines of text already drawn upwards from the bottom
	HLdouble proxyRot[3];
	hdGetDoublev(HD_CURRENT_POSITION, proxyRot);	
	DrawBitmapString(5, 20 + textRowDown * 15, GLUT_BITMAP_HELVETICA_10, "position:");
	textRowDown++;
	DrawBitmapString(5, 20 + textRowDown * 15, GLUT_BITMAP_HELVETICA_10 /*GLUT_BITMAP_9_BY_15*/, "%4.1f,%4.1f,%4.1f", proxyRot[0],proxyRot[1],proxyRot[2]);
	textRowDown++;
	HLdouble proxyRot1[3];
	hlGetDoublev(HL_PROXY_POSITION, proxyRot1); 
	DrawBitmapString(5, 20 + textRowDown * 15, GLUT_BITMAP_HELVETICA_10 /*GLUT_BITMAP_9_BY_15*/, "%4.1f,%4.1f,%4.1f", proxyRot1[0],proxyRot1[1],proxyRot1[2]);
	textRowDown++;
	HLdouble proxyRot2[3];
	hlGetDoublev(HL_DEVICE_POSITION, proxyRot2);	
	DrawBitmapString(5, 20 + textRowDown * 15, GLUT_BITMAP_HELVETICA_10 /*GLUT_BITMAP_9_BY_15*/, "%4.1f,%4.1f,%4.1f", proxyRot2[0],proxyRot2[1],proxyRot2[2]);
	textRowDown++;
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
    glPopMatrix();	
	// turn depth and lighting back on for 3D rendering
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}
/*******************************************************************************
 function for updating the mapping between opengl anf the haptic device (DO NOT TOUCH)
*******************************************************************************/
void HapticManager::UpdateHapticMapping(void)
{
	
	GLdouble modelview[16];
    GLdouble projection[16];
    GLint viewport[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
  
    hlMatrixMode(HL_TOUCHWORKSPACE);
    hlLoadIdentity();
	
	HLdouble minPoint[3], maxPoint[3];
	
	
	minPoint[0]=-10; minPoint[1]=-10; minPoint[2]=1;
	maxPoint[0]=20; maxPoint[1]=20; maxPoint[2]=-20;
	hluFitWorkspaceBox(modelview, minPoint, maxPoint); 

    // Compute cursor scale.
    gCursorScale = hluScreenToModelScale(modelview, projection, viewport);
    gCursorScale *= 20;

}






