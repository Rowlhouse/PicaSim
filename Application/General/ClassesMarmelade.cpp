#include "ClassesMarmelade.h"

SDL_Renderer* GeneralRender = nullptr;

GLenum ConvertFormatToOpenGL(Image::Format formatHW) {
    if (formatHW == Image::RGBA_4444) {
        return GL_RGBA4;
    }
    else if (formatHW == Image::RGB_565) {
        return GL_RGB565;
    }
    else if (formatHW == Image::RGBA_8888) {
        return GL_RGBA8;
    }
    else if (formatHW == Image::ABGR_8888) {
        return GL_RGBA8; //////////////////////////////////////////////////////////////////////////Faire la conversion
    }
    else {
        std::cerr << "Format non supporté !" << std::endl;
        return GL_RGBA; 
    }
}