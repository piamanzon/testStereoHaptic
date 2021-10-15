
#ifndef HapticManager_H_
#define HapticManager_H_

// UPDATED: 2016-08-25  (Y. Hu: Microsoft Visual Studio 2015 + Platform toolset in Visual Studio 2015; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)
// UPDATED: 2018-08-02  (Y. Hu: Microsoft Visual Studio 2017 + Platform toolset in Visual Studio 2017; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)

class IFaceManager;

class IHapticManager
{
public:

    static IHapticManager *create();
    static void destroy(IHapticManager *&pInterface);

    virtual void setup(IFaceManager *pFaceManager) = 0;
    virtual void cleanup() = 0;

    virtual void UpdateHapticMapping(void) = 0;
	virtual void DisplayInfo(void)=0;
    virtual void drawCursor() = 0;
    virtual void feelPoints() = 0;
    virtual int getColourNum() = 0;


protected:

    IHapticManager() {}
    virtual ~IHapticManager() {}

};

#endif // HapticManager_H_