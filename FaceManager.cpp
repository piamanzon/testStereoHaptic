
// UPDATED: 2016-08-25  (Y. Hu: Microsoft Visual Studio 2015 + Platform toolset in Visual Studio 2015; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)
// UPDATED: 2018-08-02  (Y. Hu: Microsoft Visual Studio 2017 + Platform toolset in Visual Studio 2017; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)

#include "caveMain.h"

#include "FaceManager.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <float.h>
#include <assert.h>

#include <vector>
#include <gl/glut.h>   

using namespace std;

namespace
{

static const int kNumFaces = 6;



class FaceManager : public IFaceManager
{
public:

	struct FaceStruct
	{
		hduVector3Df p1;
		hduVector3Df p2;
		hduVector3Df p3;
		hduVector3Df p4;
		hduVector3Df n1;
	};

	typedef vector<FaceStruct> FaceList;
    
    FaceManager();
    ~FaceManager();

    void setup();
    void cleanup();
    //void drawFaces(float red, float green, float blue);
	void drawFaces(int colourNum);
	void drawFace(int i);
	int getNumFaces();
	void drawFaceOpenGL(int i);
   

private:

    FaceList m_facesList;

    GLuint m_nFaceDisplayList;

};

} // anonymous namespace

/******************************************************************************
 Factory method for creating an instance of PointManager.
******************************************************************************/
IFaceManager *IFaceManager::create(void)
{
    return new FaceManager;
}

/*******************************************************************************
 Factory method for destroying an instance of FaceManager.
*******************************************************************************/
void IFaceManager::destroy(IFaceManager *&fInterface)
{
    if (fInterface)
    {
        FaceManager *fImp = static_cast<FaceManager *>(fInterface);
        delete fImp;
        fInterface = 0;
    }
}

/*******************************************************************************
 PointManager constructor.
*******************************************************************************/
FaceManager::FaceManager() :
    m_nFaceDisplayList(0)
{
}

/*******************************************************************************
 FaceManager destructor.
*******************************************************************************/
FaceManager::~FaceManager()
{
}

/*******************************************************************************
 Initialization of the cube
*******************************************************************************/
void FaceManager::setup()
{
    m_facesList.reserve(kNumFaces);

	hduVector3Df p1= hduVector3Df(1.0, 2., -1.);
	hduVector3Df p2= hduVector3Df(1.0, 0., -1.);
	hduVector3Df p3= hduVector3Df(3.0, 0., -1.);
	hduVector3Df p4= hduVector3Df(3.0, 2., -1.);
	hduVector3Df p5= hduVector3Df(3.0, 2., -3.);
	hduVector3Df p6= hduVector3Df(3.0, 0., -3.);
	hduVector3Df p7= hduVector3Df(1.0, 2., -3.);
	hduVector3Df p8= hduVector3Df(1.0, 0., -3.);

	FaceStruct face;

    face.p1 = p1;
	face.p2 = p2;
	face.p3 = p3;
	face.p4 = p4;
	face.n1 = normalize(crossProduct(p1-p2,p3-p2));
	//face.shape = hlGenShapes(1);
    m_facesList.push_back(face);
	face.p1 = p1;
	face.p2 = p2;
	face.p3 = p8;
	face.p4 = p7;
	//face.shape = hlGenShapes(1);
	face.n1 = normalize(crossProduct(p1-p2,p8-p2));
    m_facesList.push_back(face);
	face.p1 = p4;
	face.p2 = p3;
	face.p3 = p6;
	face.p4 = p5;
	//face.shape = hlGenShapes(1);
	face.n1 = normalize(crossProduct(p4-p3,p6-p3));
    m_facesList.push_back(face);
	face.p1 = p1;
	face.p2 = p4;
	face.p3 = p5;
	face.p4 = p7;
	//face.shape = hlGenShapes(1);
	face.n1 = normalize(crossProduct(p1-p4,p5-p4));
    m_facesList.push_back(face);
	face.p1 = p4;
	face.p2 = p3;
	face.p3 = p6;
	face.p4 = p8;
	//face.shape = hlGenShapes(1);
	face.n1 = normalize(crossProduct(p4-p3,p6-p3));
    m_facesList.push_back(face);
	face.p1 = p5;
	face.p2 = p6;
	face.p3 = p8;
	face.p4 = p7;
	//face.shape = hlGenShapes(1);
	face.n1 = normalize(crossProduct(p5-p6,p8-p6));
    m_facesList.push_back(face);
}

/*******************************************************************************
Clean up function
*******************************************************************************/
void FaceManager::cleanup()
{
    m_facesList.clear();
}

/*******************************************************************************
	Draw every face
*******************************************************************************/
void FaceManager::drawFaces(int colourNum)
{
	/*for (int i = 0; i < kNumFaces; i++)
    {
		drawFace(i);
	}*/
	glPushMatrix(); glTranslatef(-1.5, 1.0, -2.0);
	if (colourNum == 1)
		glColor3f(0.5f, 0.1f, 0.7f);
	else
		glColor3f(0.1f, 0.7f, 0.1f);

	glutSolidSphere(1.0, 20, 16);
	glPopMatrix();
	glFlush();
}



/*******************************************************************************
	Draw one face
*******************************************************************************/
void FaceManager::drawFace(int i){
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	/*color of the cube*/
	GLfloat mat_specular[] = { 0.3, 0.13, 0.54, 1 };
	GLfloat mat_shininess[] = { 3.0 };
	GLfloat mat_ambient[] = { 0.3,0.13,0.54,1 };
	GLfloat mat_diffuse[] = { 0.15,0.35,0.41,1 };
	
	float light_pos0[]={0,0,0,1};
	glEnable(GL_LIGHT0);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
	glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    
    glColor3f(1.0, 0.0, 0.0);
    glLineWidth(2.0);
	glBegin(GL_QUADS);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	drawFaceOpenGL(i);
	glEnd();

    glPopAttrib();
}
/*******************************************************************************
	OpenGL code for each face
*******************************************************************************/
void FaceManager::drawFaceOpenGL(int i)
{
	glNormal3f(m_facesList.at(i).n1[0], m_facesList.at(i).n1[1], m_facesList.at(i).n1[2]);
	glVertex3f(m_facesList.at(i).p1[0], m_facesList.at(i).p1[1], m_facesList.at(i).p1[2]);
	glVertex3f(m_facesList.at(i).p2[0], m_facesList.at(i).p2[1], m_facesList.at(i).p2[2]);
	glVertex3f(m_facesList.at(i).p3[0], m_facesList.at(i).p3[1], m_facesList.at(i).p3[2]);
	glVertex3f(m_facesList.at(i).p4[0], m_facesList.at(i).p4[1], m_facesList.at(i).p4[2]);      
    
}

int FaceManager::getNumFaces(){
	return kNumFaces;
}