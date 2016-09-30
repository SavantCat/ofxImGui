#include "ofxImGui.h"

ofxImGui::ofxImGui()
{
    lastTime = 0.0f;
    engine = NULL;
    theme = NULL;
}

void ofxImGui::setup(BaseTheme* theme_)
{
    ImGuiIO& io = ImGui::GetIO();
    
    io.DisplaySize = ImVec2((float)ofGetWidth(), (float)ofGetHeight());
    io.MouseDrawCursor = false;
    
#if defined(TARGET_OPENGLES)
    engine = new EngineOpenGLES();
#else
    engine = new EngineGLFW();
#endif
    
    engine->setup();
    
    if (theme_)
    {
        setTheme(theme_);
    }
    else
    {
        setTheme(new BaseTheme());
    }
}

void ofxImGui::setTheme(BaseTheme* theme_)
{
    if(theme)
    {
        delete theme;
        theme = NULL;
    }
    theme = theme_;
    theme->updateColors();
}

void ofxImGui::openThemeColorWindow()
{
    theme->themeColorsWindow(true);
}

GLuint ofxImGui::loadPixels(ofPixels& pixels)
{
    static GLuint lastID;
    
    glDeleteTextures(1, &lastID);
    
    lastID = engine->loadTextureImage2D(pixels.getData(),
                                        pixels.getWidth(),
                                        pixels.getHeight());
    
    return lastID;
}

GLuint ofxImGui::loadPixels(ofPixels& pixels, GLuint TexID)
{
    glDeleteTextures(1, &TexID);
    
    TexID = engine->loadTextureImage2D(pixels.getData(),
                                        pixels.getWidth(),
                                        pixels.getHeight());
    
    return TexID;
}



GLuint ofxImGui::loadPixels(string imagePath)
{
    if(!engine) return -1;
    ofPixels pixels;
    ofLoadImage(pixels, imagePath);
    return loadPixels(pixels);
}

GLuint ofxImGui::loadImage(ofImage& image)
{
    if(!engine) return -1;
    return loadPixels(image.getPixels());
}

GLuint ofxImGui::loadImage(ofImage& image, GLuint texID)
{
    if(!engine) return -1;
    
    image.setImageType(OF_IMAGE_COLOR_ALPHA);
    return loadPixels(image.getPixels(), texID);
}

GLuint ofxImGui::loadImage(string imagePath)
{
    return loadPixels(imagePath);
}

GLuint ofxImGui::loadTexture(string imagePath)
{
    ofDisableArbTex();
    ofTexture* texture  = new ofTexture();
    ofLoadImage(*texture, imagePath);
    ofEnableArbTex();
    loadedTextures.push_back(texture);
    return texture->getTextureData().textureID;
}

GLuint ofxImGui::loadTexture(ofTexture& texture, string imagePath)
{
    bool isUsingArb = ofGetUsingArbTex();
    if (isUsingArb)
    {
        ofDisableArbTex();
        
    }
    ofLoadImage(texture, imagePath);
    if (isUsingArb)
    {
        ofEnableArbTex();
    }
    return texture.getTextureData().textureID;
}

GLuint ofxImGui::loadFbo(ofFbo &buffer){
    static GLuint _lastID;
    static ofVec2f size;
    unsigned char * pixelData;
    
    if(buffer.isAllocated() == false)
        return _lastID;
    
//    //TODO check GL_RGBA
//
//    if(buffer.getWidth() != size.x || buffer.getHeight() != size.y){
//        size = ofVec2f(buffer.getWidth(),buffer.getHeight());
//        pixelBufferBack.allocate(buffer.getWidth()*buffer.getHeight()*4,GL_DYNAMIC_READ);
//        pixelBufferFront.allocate(buffer.getWidth()*buffer.getHeight()*4,GL_DYNAMIC_READ);
//    }
//    
//    //Get pixel data
//    buffer.getTexture().copyTo(pixelBufferBack);
//    pixelBufferFront.bind(GL_PIXEL_UNPACK_BUFFER);
//    pixelData = pixelBufferFront.map<unsigned char>(GL_READ_ONLY);
//    pixelBufferFront.unmap();
//    pixelBufferFront.unbind(GL_PIXEL_UNPACK_BUFFER);
//    swap(pixelBufferBack,pixelBufferFront);
//    
//    glDeleteTextures(1, &_lastID);
//    
//    _lastID = engine->loadTextureImage2D(pixelData,
//                                        buffer.getWidth(),
//                                        buffer.getHeight());
    
    
    //buffer.readToPixels(<#ofPixels &pixels#>)
    
    ofTextureData texData;
    
    texData = buffer.getTexture().getTextureData();
    _lastID = texData.textureTarget;
    
    return _lastID;
}

void ofxImGui::begin()
{
    if(!engine)
    {
        ofLogError(__FUNCTION__) << "setup call required - calling it for you";
        setup();
    }
    
    ImGuiIO& io = ImGui::GetIO();
    
    float currentTime = ofGetElapsedTimef();
    if(lastTime > 0.f)
    {
        io.DeltaTime = currentTime - lastTime;
    }
    else
    {
        io.DeltaTime = 1.0f / 60.f;
    }
    lastTime = currentTime;
    
    io.MousePos = ImVec2((float)ofGetMouseX(), (float)ofGetMouseY());
    ImGui::NewFrame();
}

void ofxImGui::end()
{
    ImGui::Render();
}

void ofxImGui::close()
{
    if(engine)
    {
        delete engine;
        engine = NULL;
    }
    //if(io)
    //{
    //    io->Fonts->TexID = 0;
    //    io = nullptr;
    //}
    if(theme)
    {
        delete theme;
        theme = NULL;
    }
    for(size_t i=0; i<loadedTextures.size(); i++)
    {
        delete loadedTextures[i];
    }
    loadedTextures.clear();
}

ofxImGui::~ofxImGui()
{
    close();
    ImGui::Shutdown();
}
