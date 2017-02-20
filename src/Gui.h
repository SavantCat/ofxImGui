#pragma once

#include "ofImage.h"
#include "ofPixels.h"
#include "ofTexture.h"
#include "ofFbo.h"

#include "BaseEngine.h"
#include "BaseTheme.h"

namespace ofxImGui
{
	class Gui
	{
	public:
		Gui();
		~Gui();

		void setup(BaseTheme* theme = nullptr);
		void begin();
		void end();
		void close();

		BaseEngine* engine;
		float lastTime;

		void setTheme(BaseTheme* theme);
		void openThemeColorWindow();

		BaseTheme* theme;

		void loadImage(ofImage& image, GLuint &TexID);
		void loadImage(string imagePath, GLuint &TexID);

		void loadPixels(string imagePath, GLuint &TexID);
		void loadPixels(ofPixels& pixels, GLuint &TexID);

		void loadTexture(string imagePath, GLuint &TexID);
		void loadTexture(ofTexture& texture, string imagePath, GLuint &TexID);
		
		void loadFbo(ofFbo fbo, GLuint &TexID);
	};
}
