#include "ClassesUIMarmelade.h"
#include "Trace.h"
// #include "ClassesFontMarmelade.h"

CIwMaterial* GeneralMaterial = nullptr;
CIwUITextInput* UITextInput = nullptr;
CIwUIStyleManager* UIManager = nullptr;
CIwUIController* UIController = nullptr;
CIwUIView* UIView = nullptr;


void CIwResManager::CreateRessource() {
    switch (resType) {
        case IW_GX_RESTYPE_FONT:
            ressource = new CIwGxFont;
            break;
        case IW_UI_RESTYPE_STYLESHEET:
            ressource = new CIwUIStylesheet;
            break;
        case IW_UI_RESTYPE_TEXTURE:
            ressource = new Texture;
            break;
        default:
            std::cerr << "Erreur: Type de ressource inconnu" << std::endl;
            ressource = nullptr;
    }
}

void* CIwResManager::GetResNamed(const std::string& resourceName, std::string ResType) {
    if (ResType == "CIwTexture") {
        resType = IW_UI_RESTYPE_TEXTURE;
    }
    CreateRessource();
    resourceList.push_back(ressource);
    return ressource;
}

void* CIwResManager::GetResNamed(const std::string& resourceName, resourceType ResType) {
    resType = ResType;
    CreateRessource();
    resourceList.push_back(ressource);
    return ressource;
}


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
    if (!UITextInput) {
        TRACE_FILE_IF(1) TRACE("UITextInput not init ou free before the end");
    }
    //static CIwUITextInput textInput;
    return UITextInput;
}

CIwUIStyleManager* IwGetUIStyleManager() {
    if (!UIManager) {
        TRACE_FILE_IF(1) TRACE("UIManager not init ou free before the end");
    }
    //static CIwUIStyleManager manager;
    return UIManager;
}

CIwUIController* IwGetUIController() {
    if (!UIController) {
        TRACE_FILE_IF(1) TRACE("UIController not init ou free before the end");
    }
    return UIController;
}

CIwUIView* IwGetUIView() {
    if (!UIView) {
        TRACE_FILE_IF(1) TRACE("UIView not init ou free before the end");
    }
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