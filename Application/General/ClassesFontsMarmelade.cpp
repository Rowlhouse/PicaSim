#include "ClassesFontMarmelade.h"

CIwGxFont* GeneralFont;


bool IwGxFontSetFont(CIwGxFont* font) {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }
    GeneralFont = font;
    return true;
}

Colour IwGxFontGetCol() {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return Colour();
    }
    return GeneralFont->GetColour();
}

bool IwGxFontSetCol(unsigned int hexaColour) {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }
    return GeneralFont->SetColour(hexaColour);
}

bool IwGxFontSetCol(Colour color) {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }
    return GeneralFont->SetColour(color);
}

bool IwGxLightingOn() {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }
    return GeneralFont->LightingOn();
}

bool IwGxLightingOff() {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }
    return GeneralFont->LightingOff();
}

bool IwGxFontSetAlignmentHor(alignmentHor Alignement) {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }
    GeneralFont->SetAlignmentHor(Alignement);
    return true;
}

bool IwGxFontSetAlignmentVer(alignmentVer Alignement) {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }
    GeneralFont->SetAlignmentVer(Alignement);
    return true;
}

bool IwGxFontSetRect(CIwRect rect) {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }

    // Utiliser le rectangle pour définir la zone de rendu (à l'intérieur de CIwGxFont)
    // Ici, nous allons l'enregistrer dans la classe CIwGxFont en ajoutant une variable membre pour le rectangle.
    GeneralFont->SetRenderRect(rect);
    return true;
}


bool IwGxFontDrawText(const std::string& text) {
    if (!GeneralFont) {
        std::cerr << "GeneralFont non créé pour être utilisé" << std::endl;
        return false;
    }

    // Si un rectangle de rendu a été défini, nous pouvons l'utiliser pour positionner le texte.
    CIwRect rect = GeneralFont->GetRenderRect();
    SDL_Surface* surface = GeneralFont->DrawText(text);

    // Créer une texture à partir de la surface pour l'affichage
    SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surface);
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
    SDL_RenderCopy(gRenderer, texture, nullptr, &dstRect);

    // Libérer la texture après usage
    SDL_DestroyTexture(texture);

    return true;
}