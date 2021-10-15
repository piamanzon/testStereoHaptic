
#ifndef FaceManager_H_
#define FaceManager_H_

// UPDATED: 2016-08-25  (Y. Hu: Microsoft Visual Studio 2015 + Platform toolset in Visual Studio 2015; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)
// UPDATED: 2018-08-02  (Y. Hu: Microsoft Visual Studio 2017 + Platform toolset in Visual Studio 2017; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)

/******************************************************************************
 IPointManager Interface
******************************************************************************/
class IFaceManager
{
public:
    static IFaceManager *create(void);
    static void destroy(IFaceManager *&fInterface);
    
    virtual void setup() = 0;
    virtual void cleanup() = 0;
	//virtual void drawFaces(float red, float green, float blue)=0;
   
    virtual void drawFaces(int colourNum) = 0;
    virtual void drawFace(int i)=0;
	virtual int getNumFaces()=0;

protected:
    IFaceManager() {}
    virtual ~IFaceManager() {}

}; 

#endif // FaceManager_H_
