#include "ClassesUIMarmelade.h"

CIwMaterial* GeneralMaterial = nullptr;


CIwResManager* IwGetResManager() {
    static CIwResManager manager;
    return &manager;
}

bool IwGxSetMaterial(CIwMaterial* material) {
    if (material == nullptr)
    {
        std::cerr << "Material non defini pour être utilisé en tant que matériaux général." << std::endl;
        return false;
    }
    GeneralMaterial = material;
    return true;
}

CIwUITextInput* IwGetUITextInput() {
    //static CIwUITextInput textInput;
    return UITextInput;
}

CIwUIStyleManager* IwGetUIStyleManager() {
    //static CIwUIStyleManager manager;
    return UIManager;
}

CIwUIController* IwGetUIController() {
    return UIController;
}

CIwUIView* IwGetUIView() {
    return UIView;
}

void Iw2DSetColour(uint32_t hex) {
    float a = (hex >> 24) & 0xFF; // Rouge
    float b = (hex >> 16) & 0xFF; // Vert
    float g = (hex >> 8) & 0xFF;  // Bleu
    float r = hex & 0xFF;
    glClearColor(r/256.0f, g/256.0f, b/256.0f, a/256.0f);
}

void Iw2DDrawRect(Vector2 posiInit, Vector2 dimensions){
    glColor3f(1.0f, 0.0f, 0.0f); // Couleur rouge, les valeurs sont entre 0.0 et 1.0

    // Début du dessin d'un quadrilatère
    glBegin(GL_QUADS); // Déclare que vous allez dessiner un quadrilatère

    // Définir les 4 coins du rectangle (x0, y0) = coin inférieur gauche, w, h = largeur et hauteur
    glVertex2f(posiInit.x, posiInit.y);             // Coin inférieur gauche
    glVertex2f(posiInit.x + dimensions.x, posiInit.y);         // Coin inférieur droit
    glVertex2f(posiInit.x + dimensions.x, posiInit.y + dimensions.y);     // Coin supérieur droit
    glVertex2f(posiInit.x, posiInit.y + dimensions.y);         // Coin supérieur gauche

    // Fin du dessin du quadrilatère
    glEnd();
}

void Iw2DFillPolygon(const Vector2* verts, int NUM_VERTS) {
    // Activer le mode de dessin du polygone
    glBegin(GL_POLYGON);
    
    // Boucler à travers chaque sommet et le définir
    for (int i = 0; i < NUM_VERTS; ++i) {
        glVertex2f(verts[i].x, verts[i].y);  // verts[i] contient les coordonnées x, y
    }

    // Terminer le dessin du polygone
    glEnd();
}

// Recréation de la fonction IwUIDebugRender
void IwUIDebugRender(SDL_Renderer* renderer, int flags) {
    // Exemple de hiérarchie d'éléments
    Element element = {50, 50, 200, 150};

    // Si le drapeau IW_UI_DEBUG_LAYOUT_OUTLINE_F est activé
    if (flags & 0x01) { // Supposons que IW_UI_DEBUG_LAYOUT_OUTLINE_F == 0x01
        // Couleur pour le contour (rouge pour l'exemple)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        // Dessiner le contour de l'élément
        SDL_Rect rect = {element.x, element.y, element.width, element.height};
        SDL_RenderDrawRect(renderer, &rect);
    }

    // D'autres flags peuvent être gérés ici
}