#include "Texture.h"

GLuint	loadTextureFromFile(const string&	filename)
{
	SDL_Surface	*imageSurface = IMG_Load(filename.c_str());
	if (!imageSurface){

		cout << "Can't Load	image " << filename << " " << IMG_GetError();
		return	0;
	}

	GLuint textureID = convertSDLSurfaceToTexture(imageSurface);
	SDL_FreeSurface(imageSurface);

	return textureID;
}

GLuint	loadTextureFromFont(const string& fontFilename, int	pointSize, const string& text)
{
	TTF_Font	*	font = TTF_OpenFont(fontFilename.c_str(), pointSize);
	if (!font)
	{
		cout << "Unable	to load font	" << fontFilename << "	" << TTF_GetError();
		return	0;
	}
	SDL_Surface	*textSurface = TTF_RenderText_Blended(font, text.c_str(), { 255, 255, 255 });

	GLuint textureID = convertSDLSurfaceToTexture(textSurface);
	SDL_FreeSurface(textSurface);

	TTF_CloseFont(font);

	return textureID;
}

GLuint convertSDLSurfaceToTexture(SDL_Surface * surface)
{
	GLuint textureID = 0;
	GLint		nOfColors = surface->format->BytesPerPixel;

	GLenum	textureFormat = GL_RGB;
	GLenum	internalFormat = GL_RGB8;

	if (nOfColors == 4)					//	contains	an	alpha	channel
	{
		if (surface->format->Rmask == 0x000000ff){
			textureFormat = GL_RGBA;
			internalFormat = GL_RGBA8;
		}
		else{
			textureFormat = GL_BGRA;
			internalFormat = GL_RGBA8;
		}
	}
	else if (nOfColors == 3)					//	no	alpha	channel
	{
		if (surface->format->Rmask == 0x000000ff){
			textureFormat = GL_RGB;
			internalFormat = GL_RGB8;
		}
		else{
			textureFormat = GL_BGR;
			internalFormat = GL_RGB8;
		}
	}
	else{
		cout << "warning: the image is not truecolor.. this will	probably break";
		return 0;
	}

	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, surface->w, surface->h, 0, textureFormat,
		GL_UNSIGNED_BYTE, surface->pixels);

	return textureID;
}