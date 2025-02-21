#ifndef CLASSESFONTMARMELADE_H
#define CLASSESFONTMARMELADE_H

#include "ClassesMarmelade.h"
// #include "ClassesUIMarmelade.h"
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
    IW_GX_FONT_ALIGN_BOTTOM,
    IW_GX_FONT_ALIGN_TOP,    
};


class CIwGxFont : public CIwResource {
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


extern CIwGxFont* GeneralFont;

bool IwGxFontSetFont(CIwGxFont* font);

Colour IwGxFontGetCol();

bool IwGxFontSetCol(unsigned int hexaColour);

bool IwGxFontSetCol(Colour color);

bool IwGxLightingOn();

bool IwGxLightingOff();

bool IwGxFontSetAlignmentHor(alignmentHor Alignement);

bool IwGxFontSetAlignmentVer(alignmentVer Alignement);

bool IwGxFontSetRect(CIwRect rect);

bool IwGxFontDrawText(const std::string& text);
//----------------------------------------------------------------------------------------------------------------------
/*CIwGxFont* GetFont(const GameSettings& gameSettings)
{
  CIwGxFont* font = (CIwGxFont*) IwGetResManager()->GetResNamed("normal", IW_GX_RESTYPE_FONT);
  return font;
}*/

#endif