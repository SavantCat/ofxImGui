#pragma once

#include "ofMain.h"
#include "imgui.h"

#include "BaseTheme.h"

#if defined(TARGET_OPENGLES)
    #include "EngineOpenGLES.h"
#else
    #include "EngineGLFW.h"
#endif



class ofxImGui
{
public:

    ofxImGui();
    ~ofxImGui();
    void setup(BaseTheme* theme=NULL);
    void begin();
    void end();
    void close();
    
    void openThemeColorWindow();
    GLuint loadImage(ofImage& image);
    GLuint loadImage(string imagePath);
    GLuint loadImage(ofImage& image, GLuint texID);
    
    GLuint loadPixels(string imagePath);
    GLuint loadPixels(ofPixels& pixels);
    GLuint loadPixels(ofPixels& pixels, GLuint TexID);
    
    GLuint loadTexture(string imagePath);
    GLuint loadTexture(ofTexture& texture, string imagePath);
    
    GLuint loadFbo(ofFbo &buffer);
    
    BaseTheme* theme;
    void setTheme(BaseTheme* theme);

    float lastTime;
    vector<ofTexture*> loadedTextures;
    
    GLuint lastID;
    ofPixels pixels;
    
    ofBufferObject pixelBufferBack, pixelBufferFront;
    
#if defined(TARGET_OPENGLES)
    EngineOpenGLES* engine;
#else  
    EngineGLFW* engine;
#endif
};
