#ifndef CLASSESMARMELADE_H
#define CLASSESMARMELADE_H


// #include "GameSettings.h"
// #include "ClassesFontMarmelade.h"
#include "Vecteurs.h"
// #include "ClassesFontMarmelade.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <iostream>
//#include <unordered_map>
//#include <variant>
#include "Entete.h"

extern SDL_Renderer* GeneralRender;

class Image {
public : 
    // Définition des formats matériels
    enum Format {
        NULL_FORMAT,
        RGBA_4444,
        RGB_565,
        RGBA_8888,
        ABGR_8888,
        RGB_888,
    };

    static constexpr Uint32 SDL_RGBA_4444 = SDL_PIXELFORMAT_RGBA4444;
    static constexpr Uint32 SDL_RGB_565 = SDL_PIXELFORMAT_RGB565;
    static constexpr Uint32 SDL_RGBA_8888 = SDL_PIXELFORMAT_RGBA8888;
    static constexpr Uint32 SDL_ABGR_8888 = SDL_PIXELFORMAT_ABGR8888;


    //using Format = SDL_PixelFormat*;

    Image() : surface(nullptr), format(nullptr), width(0), height(0) {}
    Image(const char* Filename) {LoadFromFile(Filename);}
    ~Image() { free(); }

    bool LoadFromFile(const char* Filename) {
        free(); // Libérer les ressources existantes, si besoin

        filename = Filename;

        // Charger l'image dans une SDL_Surface
        surface = IMG_Load(filename);
        if (!surface) {
            std::cerr << "Erreur de chargement de l'image : " << IMG_GetError() << std::endl;
            return false;
        }
        format = surface->format;
        height = surface->h;
        width = surface->w;


        return true;
    }

    // Obtenir la largeur et la hauteur de l'image
    int GetWidth() const { return surface ? surface->w : 0; }
    int GetHeight() const { return surface ? surface->h : 0; }
    SDL_Surface* GetSurface() const {return surface;}

    Format GetFormat() const {
        if (!surface) {
            std::cerr << "Surface non chargée." << std::endl;
            return NULL_FORMAT;
        }
        
        return GetEnumFormatFromSDL (surface->format);
    }

    // Obtenir les texels (données de pixels) de l'image
    Uint8* GetTexels() const {
        if (!surface) {
            std::cerr << "Surface non chargée." << std::endl;
            return nullptr;
        }
        return static_cast<Uint8*>(surface->pixels);
    }


    void SetFormat(Format newFormat) {

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

    // Définir la largeur et la hauteur
    void SetWidth(int newWidth) {
        width = newWidth;  // Modifier la largeur de la surface
        if (surface)
        {
            surface->w = width;
        }
    }

    void SetHeight(int newHeight) {
        height = newHeight;  // Modifier la hauteur de la surface
        if (surface)
        {
            surface->h = height;
        }
    }

    void ConvertToImage(Image* newImage) {
        if (!newImage || !surface) return;

        SDL_PixelFormat* formatNewImage = newImage->format;
        if (!newImage->format) {
            std::cerr << "Format de l'image de destination non défini." << std::endl;
            return;
        }

        newImage->free();

        newImage->surface = SDL_ConvertSurface(surface, formatNewImage, 0);
        if (!newImage->surface) {
            std::cerr << "Erreur de conversion de l'image : " << SDL_GetError() << std::endl;
        }
        if (newImage->height != 0 && newImage->width != 0)
        {
            newImage->surface->h = newImage->height; 
            newImage->surface->w = newImage->width;
        }
        else 
        {
            newImage->height = newImage->surface->h;
            newImage->width = newImage->surface->w;
        }
        formatNewImage = nullptr;
    }

    bool SavePng(const char* filename) const {
        if (!surface) {
            std::cerr << "Surface non chargée." << std::endl;
            return false;
        }
        if (IMG_SavePNG(surface, filename) != 0) {
            std::cerr << "Erreur de sauvegarde de l'image en PNG : " << IMG_GetError() << std::endl;
            return false;
        }
        return true;
    }

    void SetBuffers(unsigned char* buffer, size_t bufferSize) {
        if (!surface) {
            std::cerr << "Surface non chargée." << std::endl;
            return;
        }

        // Vérifier si la taille du buffer correspond à la taille attendue de l'image
        if (bufferSize != width * height * 4) {
            std::cerr << "Taille du buffer incorrecte." << std::endl;
            return;
        }

        // Copier les données du buffer dans la surface
        memcpy(surface->pixels, buffer, bufferSize);
    }
private :
    SDL_Surface* surface;
    SDL_PixelFormat* format;
    const char* filename;
    int width;
    int height;

    // Libérer les ressources
    void free() {
        if (surface) {
            SDL_FreeSurface(surface);
            surface = nullptr;
        }
        format= nullptr;
    }

    SDL_PixelFormat* GetSDLFormatFromEnum(Format format) {
        switch (format) {
            case RGBA_4444:
                return SDL_AllocFormat(SDL_PIXELFORMAT_RGBA4444);
            case RGB_565:
                return SDL_AllocFormat(SDL_PIXELFORMAT_RGB565);
            case RGBA_8888:
                return SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
            case ABGR_8888: 
                return SDL_AllocFormat(SDL_PIXELFORMAT_ABGR8888);
            default:
                std::cerr << "Format non supporté dans GetSDLFormatFromEnum." << std::endl;
                return nullptr;
        }
    }

    Format GetEnumFormatFromSDL (SDL_PixelFormat* Format) const {
        switch (Format->format) {
            case SDL_RGBA_4444:
                return RGBA_4444;
            case SDL_RGB_565:
                return RGB_565;
            case SDL_RGBA_8888:
                return RGBA_8888;
            default:
                std::cerr << "Format non supporté : " << Format->format << std::endl;
                return NULL_FORMAT;
        }
    }

};

GLenum ConvertFormatToOpenGL(Image::Format formatHW);

class Texture {
public : 
private:
    SDL_Texture* texture;
    int width;
    int height;
    std::string filename;
    bool use16Bit;
    Image* image;
    bool mipmaping;
    bool filtering;
    bool clamping;
    Image::Format formatHW;
    int flags;
    bool modifiable = true;

    void FreeResources() {
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
        if (image) {
            delete image;
            image = nullptr;
        }
        width = 0;
        height = 0;
    }

public:
    GLuint m_HWID;
    enum Flags {
        UPLOADED_F = 1 << 0, // Indique que la texture a été uploadée
    };

    // Constructeur
    Texture() : texture(nullptr), width(0), height(0), filename(""), use16Bit(false), image(nullptr), mipmaping(true), filtering(false), clamping(false), flags(0) {}

    Texture(const std::string& filename, bool use16Bit = false, bool Mipmaping = true, bool filtering = true, bool clamping = true) 
        : texture(nullptr), width(0), height(0), filename(filename), use16Bit(use16Bit), mipmaping(Mipmaping), filtering(filtering), clamping(clamping), flags(0) {
        LoadFromFile(filename.c_str());
    }

    // Destructeur
    ~Texture() {
        FreeResources();
    }

    bool CopyFromImage(Image* Image) {
        // Utilisation du renderer global
        SDL_Renderer* renderer = GeneralRender;
        if (!renderer) {
            std::cerr << "Erreur : Renderer non initialisé." << std::endl;
            return false;
        }

        image = Image;
        image->SetHeight(height);
        image->SetWidth(width);

        // Convertir la surface en texture pour le rendu
        texture = SDL_CreateTextureFromSurface(renderer, image->GetSurface());
        if (!texture) {
            std::cerr << "Erreur de création de texture : " << SDL_GetError() << std::endl;
            return false;
        }

        // Obtenir les dimensions
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
        return true;
    }

    bool CopyFromBuffer (int width, int height, Image::Format format, int pitch, const void* buffer, int flags) {
        if (!buffer) {
            std::cerr << "Erreur : Buffer invalide." << std::endl;
            return false;
        }
    
        FreeResources();  // Libérer les ressources existantes
    
        SDL_Renderer* renderer = GeneralRender;
        if (!renderer) {
            std::cerr << "Erreur : Renderer non initialisé." << std::endl;
            return false;
        }
    
        // Déterminer le format SDL équivalent
        Uint32 sdlFormat = SDL_PIXELFORMAT_RGB24; // Par défaut pour RGB_888
        if (format == Image::RGBA_8888) {
            sdlFormat = SDL_PIXELFORMAT_RGBA8888;
        } else if (format == Image::RGB_888) {
            sdlFormat = SDL_PIXELFORMAT_RGB24;
        }
    
        // Créer une surface SDL à partir du buffer
        SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(
            (void*)buffer, width, height, 24, pitch, sdlFormat);
    
        if (!surface) {
            std::cerr << "Erreur de création de surface : " << SDL_GetError() << std::endl;
            return false;
        }
    
        // Convertir la surface en texture
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            std::cerr << "Erreur de création de texture : " << SDL_GetError() << std::endl;
            SDL_FreeSurface(surface);
            return false;
        }
    
        // Stocker les infos
        this->width = width;
        this->height = height;
        this->formatHW = format;
        this->flags = flags;
    
        SDL_FreeSurface(surface);
        return true;
    }
    

    void SetModifiable(bool val) {modifiable = val;}

    // Méthode pour dessiner la texture
    void render(SDL_Renderer* renderer, int x, int y) {
        if (texture) {
            SDL_Rect renderQuad = {x, y, width, height};
            SDL_RenderCopy(renderer, texture, nullptr, &renderQuad);
        }
    }

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetFlags() const { return flags; }

    // Obtenir le nom du fichier associé
    std::string GetFilename() const { return filename; }

    // Savoir si la texture est en 16 bits
    bool Is16Bit() const { return use16Bit; }

    // Charger une texture à partir d'un fichier
    bool LoadFromFile(const char* filename) {
        // Nettoyer les ressources existantes si besoin
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
            width = 0;
            height = 0;
        }

        SDL_Renderer* renderer = GeneralRender;
        if (!renderer) {
            std::cerr << "Erreur : Renderer non initialisé." << std::endl;
            return false;
        }

        if (image)
        {
            delete image;
        }
        image = new Image (filename);

        if (!CopyFromImage(image)) {
            delete image;
            image = nullptr;
            return false;
        }

        // Libérer la surface temporaire
        //SDL_FreeSurface(image.GetSurface());
        return true;
    }

    bool SetImage(Image* Image) {
        // Utilisation du renderer global
        SDL_Renderer* renderer = GeneralRender;
        if (!renderer) {
            std::cerr << "Erreur : Renderer non initialisé." << std::endl;
            return false;
        }

        image = Image;

        // Convertir la surface en texture pour le rendu
        texture = SDL_CreateTextureFromSurface(renderer, image->GetSurface());
        if (!texture) {
            std::cerr << "Erreur de création de texture : " << SDL_GetError() << std::endl;
            return false;
        }

        // Obtenir les dimensions
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
        return true;
    }

    void SetMipMapping(bool val) { mipmaping = val; }
    void SetFiltering(bool val) { filtering = val; }
    void SetClamping(bool val) { clamping = val; }

    bool SetFormatHW(Image::Format format)  {
        if (!format) {
            std::cerr << "Erreur : pas de format HW spécifié" << std::endl;
            return false;
        }
        formatHW = format;

        return true;
    }

    void Upload() {
        if (!image || !texture) {
            std::cerr << "Erreur : Pas d'image ou de texture à uploader." << std::endl;
            return;
        }

        SDL_Renderer* renderer = GeneralRender;
        if (!renderer) {
            std::cerr << "Erreur : Renderer non initialisé pour l'upload." << std::endl;
            return;
        }
        GLuint textureID;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Définir les paramètres de texture
        if (mipmaping) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamping ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamping ? GL_CLAMP_TO_EDGE : GL_REPEAT);

        // Transférer les données des pixels à OpenGL
        GLenum format = ConvertFormatToOpenGL(formatHW);
        GLenum pixelType = GL_UNSIGNED_BYTE; // Par défaut, mais vous pouvez l'adapter selon les besoins

        glTexImage2D(GL_TEXTURE_2D, 0, format, image->GetSurface()->w, image->GetSurface()->h, 0, format, pixelType, image->GetSurface()->pixels);

        m_HWID = textureID;

        // Mettre le flag UPLOADED_F
        flags |= UPLOADED_F;

        // Debug
        std::cout << "Upload réussi : texture OpenGL créée avec ID " << textureID << "." << std::endl;
    }
};

class Colour {
public :
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;

  Colour(uint8_t R = 255, uint8_t G = 255, uint8_t B = 255, uint8_t A = 255) : r(R), g(G), b(B), a(A)
  {
  }

  void Set (uint8_t R, uint8_t G, uint8_t B, uint8_t A=255) {r=R; g=G; b=B; a=A;}
  bool Set (unsigned int hexaColor)
  {
    a = (int) ((hexaColor >> 24) & 0xFF);  // Composante Alpha
    r = (int) ((hexaColor >> 16) & 0xFF);  // Composante Rouge
    g = (int) ((hexaColor >> 8) & 0xFF);   // Composante Verte
    b = (int) (hexaColor & 0xFF);
    return true;
  }

  SDL_Colour GetSDLColour ()
  {
    return SDL_Colour ({r,g,b,a});
  }
};

class Plane {
public :
  Vector3 normalVector;
  float constant;

  Plane() : normalVector(Vector3()), constant(0.0f) {}
  Plane(const Vector3& v, float k) : normalVector(v), constant(k) {}


  void Normalise() {float invM = 1.0f / normalVector.GetLength(); normalVector *= invM; constant *= invM;}
  float GetDistanceToPoint(const Vector3& pt) const { return normalVector * pt + constant;}
};

typedef Colour CIwColour;
typedef Texture CIwTexture;
typedef Image CIwImage;

typedef void CIwManaged ;


class CIwRect : public SDL_Rect {
public:
    // Constructeur par défaut
    CIwRect() {
        x = 0;
        y = 0;
        w = 0;
        h = 0;
    }

    // Constructeur avec initialisation des valeurs
    CIwRect(int x, int y, int w, int h) {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }

    CIwRect(Vector2 pos, Vector2 size) {
        this->x = pos.x;
        this->y = pos.y;
        this->w = size.x;
        this->h = size.y;
    }

    // Méthode pour définir les valeurs du rectangle
    void Set(int x, int y, int w, int h) {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }

    // Méthode pour obtenir la largeur
    int GetWidth() const {
        return w;
    }

    // Méthode pour obtenir la hauteur
    int GetHeight() const {
        return h;
    }

    // Méthode pour obtenir la position x
    int GetX() const {
        return x;
    }

    // Méthode pour obtenir la position y
    int GetY() const {
        return y;
    }

    // Méthode pour vérifier si le rectangle est valide (largeur et hauteur non nulles)
    bool IsValid() const {
        return w > 0 && h > 0;
    }

    // Méthode pour afficher les informations du rectangle
    void Print() const {
        std::cout << "Rectangle: [x: " << x << ", y: " << y << ", w: " << w << ", h: " << h << "]" << std::endl;
    }

    // Fonction statique pour créer un rectangle à partir d'un SDL_Rect
    static CIwRect FromSDLRect(const SDL_Rect& sdlRect) {
        return CIwRect(sdlRect.x, sdlRect.y, sdlRect.w, sdlRect.h);
    }

    // Conversion vers un SDL_Rect
    SDL_Rect ToSDLRect() const {
        SDL_Rect sdlRect = {x, y, w, h};
        return sdlRect;
    }

    // Méthode pour savoir si un point est à l'intérieur du rectangle
    bool Contains(int px, int py) const {
        return (px >= x && px <= (x + w) && py >= y && py <= (y + h));
    }

    bool Contains(const CIwRect& other) const {
        return other.x >= x && other.x + other.w <= x + w &&
               other.y >= y && other.y + other.h <= y + h;
    }

    // Méthode pour déplacer le rectangle
    void Move(int dx, int dy) {
        x += dx;
        y += dy;
    }

    // Méthode pour redimensionner le rectangle
    void Resize(int dw, int dh) {
        w += dw;
        h += dh;
    }

    // Méthode pour déplacer et redimensionner en une seule opération
    void SetPositionAndSize(int nx, int ny, int nw, int nh) {
        x = nx;
        y = ny;
        w = nw;
        h = nh;
    }

    bool Intersects(const CIwRect& other) const {
        return !(x + w <= other.x || x >= other.x + other.w ||
                 y + h <= other.y || y >= other.y + other.h);
    }

    int Area() const {
        return w * h;
    }
};

class IwHashString {
public:
    IwHashString() : str() {}
    IwHashString(const std::string& str) : str(str) {}

    const std::string& GetString() const { return str; }

private:
    std::string str;
};






#endif