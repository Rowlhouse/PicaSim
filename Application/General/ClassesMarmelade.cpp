#include "ClassesMarmelade.h"

CIwResource::~CIwResource() {}  // Implémentation vide









Image::Format Image::GetFormat() const {
    if (!surface) {
        std::cerr << "Surface non chargée." << std::endl;
        return NULL_FORMAT;
    }
    
    return GetEnumFormatFromSDL (surface->format);
}

void Image::SetFormat(Format newFormat) {

    format = GetSDLFormatFromEnum(newFormat);
    if (surface && newFormat) {
        // Convertir la surface actuelle au nouveau format
        SDL_Surface* newSurface = SDL_ConvertSurfaceFormat(surface, format->format, 0);
        if (newSurface) {
            SDL_FreeSurface(surface);  // Libérer l'ancienne surface
            surface = newSurface;
        } else {
            std::cerr << "Erreur de conversion de format : " << SDL_GetError() << std::endl;
        }
    }
}
















GLenum ConvertSDLFormatToOpenGL(Uint32 sdlFormat) {
    switch (sdlFormat) {
        case SDL_PIXELFORMAT_RGB24:   return GL_RGB;
        case SDL_PIXELFORMAT_BGR24:   return GL_BGR;
        case SDL_PIXELFORMAT_RGB888:  return GL_RGB;
        case SDL_PIXELFORMAT_BGR888:  return GL_BGR;
        case SDL_PIXELFORMAT_RGBA8888:return GL_RGBA;
        case SDL_PIXELFORMAT_ABGR8888:return GL_ABGR_EXT; // Extension OpenGL si disponible
        case SDL_PIXELFORMAT_ARGB8888:return GL_BGRA; // OpenGL utilise souvent GL_BGRA pour ARGB
        default:
            std::cerr << "Format SDL non supporté pour OpenGL !" << std::endl;
            return GL_RGB; // Valeur par défaut
    }
}


SDL_PixelFormat* GetSDLFormatFromEnum(Image::Format format) {
    switch (format) {
        case Image::RGBA_4444:
            return SDL_AllocFormat(SDL_PIXELFORMAT_RGBA4444);
        case Image::RGB_565:
            return SDL_AllocFormat(SDL_PIXELFORMAT_RGB565);
        case Image::RGBA_8888:
            return SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
        case Image::ABGR_8888: 
            return SDL_AllocFormat(SDL_PIXELFORMAT_ABGR8888);
        default:
            std::cerr << "Format non supporté dans GetSDLFormatFromEnum." << std::endl;
            return nullptr;
    }
}




Image::Format GetEnumFormatFromSDL (SDL_PixelFormat* Format)  {
    switch (Format->format) {
        case SDL_PIXELFORMAT_RGBA4444:
            return Image::RGBA_4444;
        case SDL_PIXELFORMAT_RGB565:
            return Image::RGB_565;
        case SDL_PIXELFORMAT_RGBA8888:
            return Image::RGBA_8888;
        default:
            std::cerr << "Format non supporté dans la conversion de l'enum vers le SDL : " << Format->format << std::endl;
            return Image::NULL_FORMAT;
    }
}