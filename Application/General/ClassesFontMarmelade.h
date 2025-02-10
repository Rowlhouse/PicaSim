#ifndef CLASSESFONTMARMELADE_H
#define CLASSESFONTMARMELADE_H

#include "ClassesMarmelade.h"
// #include "GameSettings.h"
#include <SDL2/SDL_ttf.h>
#include <stdexcept>
#include <string>


enum alignmentHor {
    IW_GX_FONT_ALIGN_LEFT,
    IW_GX_FONT_ALIGN_CENTRE,
    IW_GX_FONT_ALIGN_RIGHT,
};

enum alignmentVer {
    IW_GX_FONT_ALIGN_MIDDLE,
    IW_GX_FONT_ALIGN_UP,
    IW_GX_FONT_ALIGN_DOWN,
    
};


class CIwGxFont  {
public:
    // Constructeur
    CIwGxFont(const std::string& fontName = "", int fontSize = 12) : fontName(fontName), fontSize(fontSize), font(nullptr), colour({255, 255, 255, 255}), AlignmentHor(IW_GX_FONT_ALIGN_LEFT), AlignmentVer(IW_GX_FONT_ALIGN_MIDDLE) {
        if (TTF_Init() == -1) {
            throw std::runtime_error("Erreur lors de l'initialisation de SDL_ttf : " + std::string(TTF_GetError()));
        }

        font = TTF_OpenFont(fontName.c_str(), fontSize);
        if (!font) {
            throw std::runtime_error("Impossible de charger la police : " + fontName + " (" + TTF_GetError() + ")");
        }
    }

    // Destructeur
    ~CIwGxFont() {
        if (font) {
            TTF_CloseFont(font);
        }
        TTF_Quit();
    }

    // Récupérer le nom de la police
    std::string GetFontName() const { return fontName; }

    // Récupérer la taille de la police
    int GetFontSize() const { return fontSize; }

    // Récupérer la hauteur de la police
    uint16_t GetHeight() const {
        if (!font) throw std::runtime_error("La police n'est pas chargée.");
        return static_cast<uint16_t>(TTF_FontHeight(font));
    }

    // Définir la couleur de la police
    void SetColour(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255) {
        colour = {r, g, b, a};
    }

    bool SetColour(unsigned int hexaColor) {
        uint8_t a = (int) ((hexaColor >> 24) & 0xFF);  // Composante Alpha
        uint8_t r = (int) ((hexaColor >> 16) & 0xFF);  // Composante Rouge
        uint8_t g = (int) ((hexaColor >> 8) & 0xFF);   // Composante Verte
        uint8_t b = (int) (hexaColor & 0xFF);
        colour = {r, g, b, a};
        return true;
    }

    bool SetColour(Colour color) {
        colour = color;
        return true;
    }

    // Récupérer la couleur actuelle de la police
    Colour GetColour() const { return colour; }

    // Définir l'alignement horizontal
    void SetAlignmentHor(alignmentHor align) {
        AlignmentHor = align;
    }

    void SetAlignmentVer(alignmentVer align) {
        AlignmentVer = align;
    }

    // Récupérer l'alignement actuel
    alignmentHor GetAlignmentHor() const { return AlignmentHor; }
    alignmentVer GetAlignmentVer() const { return AlignmentVer; }

    // Simuler "IwGxLightingOn" (ajout d'une fonction placeholder)
    bool LightingOn() {
        // Dans ce contexte, cette méthode pourrait activer des paramètres liés au rendu du texte, mais SDL_ttf ne gère pas de lumière.
        // Vous pouvez personnaliser son comportement pour vos besoins spécifiques.
        return true;
    }

    bool LightingOff() {
        // Dans ce contexte, cette méthode pourrait activer des paramètres liés au rendu du texte, mais SDL_ttf ne gère pas de lumière.
        // Vous pouvez personnaliser son comportement pour vos besoins spécifiques.
        return true;
    }

    // Rendre du texte sur une surface SDL
    SDL_Surface* DrawText(const std::string& text) {
        if (!font) {
            throw std::runtime_error("La police n'a pas été chargée.");
        }

        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), colour.GetSDLColour());
        if (!textSurface) {
            throw std::runtime_error("Erreur lors du rendu du texte : " + std::string(TTF_GetError()));
        }

        return textSurface; // L'utilisateur est responsable de libérer la surface avec SDL_FreeSurface.
    }

    // Méthodes pour définir et obtenir le rectangle de rendu
    void SetRenderRect(const CIwRect& rect) {
        renderRect = rect;
    }

    CIwRect GetRenderRect() const {
        return renderRect;
    }

    TTF_Font* GetTTF_Font () const {
        return font;
    }

private:
    std::string fontName;    // Nom du fichier de la police
    int fontSize;            // Taille de la police
    TTF_Font* font;          // Pointeur vers la police SDL_ttf
    Colour colour;        // Couleur actuelle de la police
    alignmentHor AlignmentHor;     // Alignement horizontal
    alignmentVer AlignmentVer;
    CIwRect renderRect;
};


CIwGxFont* GeneralFont;


inline bool IwGxFontSetFont(CIwGxFont* font) {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }
    GeneralFont = font;
    return true;
}

inline Colour IwGxFontGetCol() {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return Colour();
    }
    return GeneralFont->GetColour();
}

inline bool IwGxFontSetCol(unsigned int hexaColour) {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }
    return GeneralFont->SetColour(hexaColour);
}

inline bool IwGxFontSetCol(Colour color) {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }
    return GeneralFont->SetColour(color);
}

inline bool IwGxLightingOn() {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }
    return GeneralFont->LightingOn();
}

inline bool IwGxLightingOff() {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }
    return GeneralFont->LightingOff();
}

inline bool IwGxFontSetAlignmentHor(alignmentHor Alignement) {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }
    GeneralFont->SetAlignmentHor(Alignement);
    return true;
}

inline bool IwGxFontSetAlignmentVer(alignmentVer Alignement) {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }
    GeneralFont->SetAlignmentVer(Alignement);
    return true;
}

inline bool IwGxFontSetRect(CIwRect rect) {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }

    // Utiliser le rectangle pour définir la zone de rendu (à l'intérieur de CIwGxFont)
    // Ici, nous allons l'enregistrer dans la classe CIwGxFont en ajoutant une variable membre pour le rectangle.
    GeneralFont->SetRenderRect(rect);
    return true;
}


inline bool IwGxFontDrawText(const std::string& text) {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }

    // Si un rectangle de rendu a été défini, nous pouvons l'utiliser pour positionner le texte.
    CIwRect rect = GeneralFont->GetRenderRect();
    SDL_Surface* surface = GeneralFont->DrawText(text);

    // Créer une texture à partir de la surface pour l'affichage
    SDL_Texture* texture = SDL_CreateTextureFromSurface(RendererManager::getRenderer(), surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "Erreur lors de la création de la texture pour le texte : " << SDL_GetError() << std::endl;
        return false;
    }

    // Récupérer la largeur et la hauteur du texte
    int textWidth = surface->w;
    int textHeight = surface->h;

    // Appliquer l'alignement du texte, en fonction de l'alignement défini dans CIwGxFont
    int x = rect.GetX();
    if (GeneralFont->GetAlignmentHor() == IW_GX_FONT_ALIGN_CENTRE) {
        x = rect.GetX() + (rect.GetWidth() - textWidth) / 2;
    } else if (GeneralFont->GetAlignmentHor() == IW_GX_FONT_ALIGN_RIGHT) {
        x = rect.GetX() + rect.GetWidth() - textWidth;
    }
    else 
    {
        std::cerr << "Alignement non défini" << std::endl;
    }

    // Dessiner la texture à l'écran
    SDL_Rect dstRect = {x, rect.GetY(), textWidth, textHeight};
    SDL_RenderCopy(RendererManager::getRenderer(), texture, nullptr, &dstRect);

    // Libérer la texture après usage
    SDL_DestroyTexture(texture);

    return true;
}
//----------------------------------------------------------------------------------------------------------------------
/*CIwGxFont* GetFont(const GameSettings& gameSettings)
{
  CIwGxFont* font = (CIwGxFont*) IwGetResManager()->GetResNamed("normal", IW_GX_RESTYPE_FONT);
  return font;
}*/

#endif