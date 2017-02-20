#include "Gui.h"

#include "ofAppRunner.h"

#if defined(TARGET_OPENGLES)
#include "EngineOpenGLES.h"
#else
#include "EngineGLFW.h"
#endif

namespace ofxImGui
{
	//--------------------------------------------------------------
	Gui::Gui()
		: lastTime(0.0f)
		, engine(nullptr)
		, theme(nullptr)
	{}

	//--------------------------------------------------------------
	void Gui::setup(BaseTheme* theme_)
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

	//--------------------------------------------------------------
	void Gui::setTheme(BaseTheme* theme_)
	{
		if (theme)
		{
			delete theme;
			theme = nullptr;
		}
		theme = theme_;
		theme->updateColors();
	}

	//--------------------------------------------------------------
	void Gui::openThemeColorWindow()
	{
		theme->themeColorsWindow(true);
	}

	//--------------------------------------------------------------
	void Gui::loadPixels(ofPixels& pixels, GLuint &TexID)
	{
		glDeleteTextures(1, &TexID);

		pixels.setImageType(OF_IMAGE_COLOR);

		TexID = engine->loadTextureImage2D(pixels.getData(), pixels.getPixelFormat(), pixels.getWidth(), pixels.getHeight());
	}

	//--------------------------------------------------------------
	void Gui::loadPixels(string imagePath, GLuint &TexID)
	{
		if (!engine) return;
		ofPixels pixels;
		ofLoadImage(pixels, imagePath);
		loadPixels(pixels, TexID);
	}

	//--------------------------------------------------------------
	void Gui::loadImage(ofImage& image, GLuint &TexID)
	{
		if (!engine) return;
		loadPixels(image.getPixels(), TexID);
	}

	//--------------------------------------------------------------
	void Gui::loadImage(string imagePath, GLuint &TexID)
	{
		loadPixels(imagePath, TexID);
	}

	//--------------------------------------------------------------
	void Gui::loadTexture(string imagePath, GLuint &TexID)
	{
		ofDisableArbTex();
		ofTexture* texture = new ofTexture();
		ofLoadImage(*texture, imagePath);
		ofEnableArbTex();

		ofPixels pixels;
		texture->readToPixels(pixels);

		loadPixels(pixels, TexID);
	}

	//--------------------------------------------------------------
	void Gui::loadTexture(ofTexture& texture, string imagePath, GLuint &TexID)
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
		TexID = texture.getTextureData().textureID;
	}

	//--------------------------------------------------------------
	void Gui::loadFbo(ofFbo fbo, GLuint &TexID) {
		if (fbo.getTexture().getTextureData().glInternalFormat != GL_TEXTURE_2D) {
			//Reallocate
			ofFbo::Settings settings;
			settings.width = fbo.getWidth();
			settings.height = fbo.getHeight();
			settings.internalformat = fbo.getTexture().getTextureData().glInternalFormat;
			settings.numSamples = 16;
			settings.textureTarget = GL_TEXTURE_2D;
			fbo.allocate(settings);
		}
		TexID = fbo.getTexture().getTextureData().textureID;
	}

	//--------------------------------------------------------------
	void Gui::begin()
	{
		if (!engine)
		{
			ofLogError(__FUNCTION__) << "setup() call required, calling it for you";
			setup();
		}

		ImGuiIO& io = ImGui::GetIO();

		float currentTime = ofGetElapsedTimef();
		if (lastTime > 0.f)
		{
			io.DeltaTime = currentTime - lastTime;
		}
		else
		{
			io.DeltaTime = 1.0f / 60.f;
		}
		lastTime = currentTime;

		// Update settings
		io.MousePos = ImVec2((float)ofGetMouseX(), (float)ofGetMouseY());
		for (int i = 0; i < 5; i++) {
			io.MouseDown[i] = engine->mousePressed[i];

			// Update for next frame; set to false only if the mouse has been released
			engine->mousePressed[i] = !engine->mouseReleased;
		}
		ImGui::NewFrame();
	}

	//--------------------------------------------------------------
	void Gui::end()
	{
		ImGui::Render();
	}

	//--------------------------------------------------------------
	void Gui::close()
	{
		if (engine)
		{
			delete engine;
			engine = nullptr;
		}
		//if(io)
		//{
		//    io->Fonts->TexID = 0;
		//    io = nullptr;
		//}
		if (theme)
		{
			delete theme;
			theme = nullptr;
		}
	}

	//--------------------------------------------------------------
	Gui::~Gui()
	{
		close();
		ImGui::Shutdown();
	}
}
