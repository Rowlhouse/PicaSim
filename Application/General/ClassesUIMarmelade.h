#ifndef CLASSESUIMARMELADE_H
#define CLASSESUIMARMELADE_H

#include "ClassesFontMarmelade.h"
#include "ClassesMarmelade.h"
#include "Entete.h"
//#include "PicaDialog.h"
#include "Vecteurs.h"
#include <string>
#include <iostream>
#include <algorithm> 


class CIwUILayout;

class CIwUIStylesheet {
public:
    CIwUIStylesheet(const std::string& name = "") : styleSheetName(name) {}

    std::string GetName() { return styleSheetName; }
private:
    std::string styleSheetName;
};


class CIwEvent{
public:
    CIwEvent(CIwEventType id, void* sender) : mID(id), mSender(sender) {}

    CIwEventType GetID() const { return mID; }
    void* GetSender() const { return mSender; }

    /*
    CIwUIEventType GetType() const { return mType; }
    CIwUIElement* GetSource() const { return mSource; }
    */

private:
    CIwEventType mID;       // Type d'événement
    void* mSender;   // Émetteur de l'événement

    /*
    CIwUIEventType mType;
    CIwUIElement* mSource;
    */
};

class CIwUIElementEventHandler {
};

//using Type = std::variant<CIwGxFont*, CIwUIStylesheet*, Texture*, std::nullptr_t>;
template <typename T>
class CIwResource {
public:
    CIwResource(const std::string& name, resourceType ResType) : mName(name), resType(ResType) { CreateRessource(); }
    CIwResource(const std::string& name, std::string ResType) : mName(name) { if (ResType == "CIwTexture") {resType = IW_UI_RESTYPE_TEXTURE;} CreateRessource(); }
    virtual ~CIwResource() {}
    std::string GetName() const { return mName; }
    resourceType GetResType() const { return resType; }

    void CreateRessource () {
        if (resType == IW_GX_RESTYPE_FONT) {
            ressource = new CIwGxFont;
        }
        else if (resType == IW_UI_RESTYPE_STYLESHEET) {
            ressource = new CIwUIStylesheet;
        }
        else if (resType == IW_UI_RESTYPE_TEXTURE) {
            ressource = new Texture;
        }
        else {
            std::cout << "Pas de ressource définies\n";
            ressource = nullptr;
        }

        //return ressource;
    }

    T* GetRessource () { return ressource; }

private:
    std::string mName;
    resourceType resType;
    T* ressource;
    
};

class CIwResGroup {
public:
    CIwResGroup(const std::string& groupName) : groupName(groupName) {}

    void SetAsCurrentGroup() {
        // Simulation : définir le groupe courant
    }

    std::string GetName ()
    {
        return groupName;
    }

private:
    std::string groupName;
};

template <typename T>
class CIwResManager {
public:
    CIwResManager() {}
    ~CIwResManager(){
        for (unsigned int i=0; i<resourceList.size(); i++) {
            delete resourceList[i];
        }
    }

    // Charge un groupe de ressources
    CIwResGroup* LoadGroup(const std::string& groupName) {
        if (mResourceGroups.find(groupName) != mResourceGroups.end()) {
            return mResourceGroups[groupName];
        }

        // Simuler le chargement du groupe
        CIwResGroup* newGroup = new CIwResGroup(groupName);
        mResourceGroups[groupName] = newGroup;
        std::cout << "Loaded resource group: " << groupName << std::endl;
        return newGroup;
    }

    // Détruit un groupe de ressources
    void DestroyGroup(CIwResource<T>* group) {
        if (group) {
            std::cout << "Destroyed resource group: " << group->GetName() << std::endl;
            delete group;
        }
    }

    // Récupère une ressource nommée
    T* GetResNamed(const std::string& resourceName, resourceType resType) {
        CIwResource<T>* res = new CIwResource<T>(resourceName, resType);
        resourceList.push_back(res);
        return res->GetRessource();
    }

    T* GetResNamed(const std::string& resourceName, std::string resType) {
        CIwResource<T>* res = new CIwResource<T>(resourceName, resType);
        resourceList.push_back(res);
        return res->GetRessource();
    }

    void SetCurrentGroup(CIwResGroup* group) {
        mCurrentGroup = group;
        std::cout << "Set current resource group to: " << group->GetName() << std::endl;
    }

private:
    std::unordered_map<std::string, CIwResGroup*> mResourceGroups;
    CIwResGroup* mCurrentGroup = nullptr;
    std::vector<CIwResource<T>*> resourceList;
};

class CIwMaterial {
public:
    enum alpha {
        ALPHA_BLEND,
        ALPHA_NONE,
        ALPHA_ADD,
    };

    CIwMaterial() : mDiffuseColor(255, 255, 255, 255) {}

    // Méthode pour définir une texture sur le matériau
    void SetTexture(CIwTexture* texture) {
        mTextures.push_back(texture);
    }

    // Méthode pour définir une couleur diffuse (par exemple, couleur de base du matériau)
    void SetDiffuseColor(int r, int g, int b, int a) {
        mDiffuseColor.Set(r, g, b, a);
    }

    void SetColAmbient(int r, int g, int b, int a) {
        mAmbientColor.Set(r, g, b, a);
    }

    // Méthode pour afficher les informations sur le matériau
    void PrintMaterialInfo() const {
        std::cout << "Material Info:" << std::endl;
        std::cout << "Diffuse Color: (" << mDiffuseColor.r << ", " << mDiffuseColor.g << ", "
                  << mDiffuseColor.b << ", " << mDiffuseColor.a << ")" << std::endl;

        std::cout << "Ambient Color: (" << mAmbientColor.r << ", " << mAmbientColor.g << ", "
                  << mAmbientColor.b << ", " << mAmbientColor.a << ")" << std::endl;

        std::cout << "Textures: ";
        for (const auto& texture : mTextures) {
            std::cout << texture->GetFilename() << " ";
        }
        std::cout << std::endl;
    }

    void SetAlphaMode(alpha mode) {
        mAlphaMode = mode;
        std::cout << "Alpha Mode set to: " << GetAlphaModeString() << std::endl;
    }

private:

    Colour mDiffuseColor;  // La couleur diffuse du matériau
    Colour mAmbientColor;
    std::vector<CIwTexture*> mTextures;  // Liste de textures associées au matériau
    alpha mAlphaMode;  // Mode alpha pour le matériau

    std::string GetAlphaModeString() const {
        switch (mAlphaMode) {
            case ALPHA_BLEND: return "ALPHA_BLEND";
            case ALPHA_NONE: return "ALPHA_NONE";
            case ALPHA_ADD: return "ALPHA_ADD";
            default: return "UNKNOWN";
        }
    }
};

CIwMaterial* GeneralMaterial = nullptr;

inline bool IwGxSetMaterial(CIwMaterial* material) {
    if (material == nullptr)
    {
        std::cerr << "Material non defini pour être utilisé en tant que matériaux général." << std::endl;
        return false;
    }
    GeneralMaterial = material;
    return true;
}

template <typename T>
inline CIwResManager<T> IwGetResManager() {
    static CIwResManager<T> manager;
    return &manager;
}

class CIwUIElement {
public:
    CIwUIElement() : mVisible(true) {}
    virtual ~CIwUIElement() = default;

    void AddChild(CIwUIElement* child) { mChildren.push_back(child); }
    virtual bool HandleEvent(CIwEvent* pEvent) { return false; }

    virtual void Render() {
        std::cout << "Rendering a generic UI element\n";
        if (!mVisible) return;
        for (auto& child : mChildren) {
            child->Render();
        }
    }

    void SetVisible(bool visible) { mVisible = visible; }
    bool IsVisible() const { return mVisible; }

    void SetPosition(Vector2 vec){ pos = vec; }
    Vector2 GetPosition() const { return pos; }

    void SetSize(const Vector2& Size) { size = Size; }
    Vector2 GetSize() { return size; }

    void SetSizeMax(Vector2 Size){ SetSize(Size); }
    void SetSizeMin(Vector2 Size){ SetSize(Size); }

    void SetSizeToContent (bool val) {SizeToContent = val;}

    void SetLayout(CIwUILayout* Layout) { layout = Layout; }
    CIwUILayout* GetLayout () { return layout; }

private:
    bool mVisible;
    std::vector<CIwUIElement*> mChildren;
    Vector2 pos=  Vector2(0,0);
    Vector2 size = Vector2(1,1);
    CIwUILayout* layout;
    bool SizeToContent = true;
};

class CIwUILabel : public CIwUIElement {
public:
    CIwUILabel(const std::string& text) : mText(text) {}
    CIwUILabel() {}


    void SetText(const std::string& text) { mText = text; }
    const std::string& GetText() const { return mText; }

    void Render() override {
        if (IsVisible()) {
            std::cout << "Label: " << mText << std::endl;
        }
    }

    void SetCaption(const char* caption) {mText = std::string( caption);}
    void SetStyle (std::string Style) {style = Style;}

    void SetProperty (std::string Property, CIwPropertyValue value) {
        if (Property == "alignV") {
            property.alignementVertical = value;
        }
    }

private:
    std::string style;
    std::string mText;
    struct properties {
        CIwPropertyValue alignementVertical = IW_UI_ALIGN_CENTER;
    };
    properties property;
};

// Classe CIwUIButton pour un bouton
class CIwUIButton : public CIwUIElement {
public:
    CIwUIButton(const std::string& caption) : mCaption(caption) {}
    CIwUIButton() {}

    void SetStyle(const std::string& style) { mStyle = style; }
    void SetCaption(const std::string& caption) { mCaption = caption; }
    const std::string& GetCaption() const { return mCaption; }

    void TriggerEvent(CIwEventType evType)
    {
        CIwEvent ev(evType, this);
        for (CIwUIElement* handler : mHandlers)
        {
            handler->HandleEvent(&ev);
        }
    }


    void Click()
    {
        std::cout << "Button clicked: " << mCaption << "\n";
        TriggerEvent(IWUI_EVENT_BUTTON);
    }

    void Render() override {
        if (IsVisible()) {
            std::cout << "Rendering button: " << mCaption << std::endl;
        }
    }

    void AddEventHandler(CIwUIElement* handler) { mHandlers.push_back(handler); }
    void RemoveEventHandler(CIwUIElement* handler) {
        auto it = std::remove(mHandlers.begin(), mHandlers.end(), handler);
        mHandlers.erase(it, mHandlers.end());
    }



private:
    std::string mStyle;
    std::string mCaption;
    std::vector<CIwUIElement*> mHandlers;
};

// Classe CIwUILayout pour gérer la disposition des éléments
class CIwUILayout {
public:
    void AddElement(CIwUIElement* element) { mElements.push_back(element); }

    //void RemoveElement(CIwUIElement* element) { mElements.erase(std::remove(mElements.begin(), mElements.end(), element), mElements.end()); }

    virtual void Render() {
        std::cout << "Rendering layout\n";
        for (auto& element : mElements) {
            element->Render();
        }
    }

    void SetSizeToSpace (bool val) {SizeToSpace = val;}

    std::vector<CIwUIElement*> GetElements () { return mElements; }

private:
    std::vector<CIwUIElement*> mElements;
    bool SizeToSpace = false;
};

class CIwUIImage : public CIwUIElement {
public:
    CIwUIImage(){}
    CIwUIImage(const std::string& filename, bool use16Bit) : mFilePath(filename), mUse16Bit(use16Bit), mTexture(nullptr) {
        LoadImage();
    }

    ~CIwUIImage() {
        if (mTexture) {
            delete mTexture;
        }
    }

    void LoadImage() {
        std::cout << "Loading image: " << mFilePath << " with 16-bit: " << mUse16Bit << std::endl;
        mTexture = new Texture(mFilePath, mUse16Bit );
    }

    void SetTexture(Texture* texture) {mTexture = texture;}
    Texture* GetTexture() { return mTexture; }

    void Render() override {
        if (IsVisible() && mTexture) {
            std::cout << "Rendering image from: " << mFilePath << std::endl;
        }
    }

    void SetPos(Vector2 Pos) { pos = Pos; }
    Vector2 GetPos() const { return pos; }

    void SetSizeToContent(bool val) {    ////////////////////////////////////////////////////////// If true the the whole screen is vertically centred. False places it at the top
        sizeToContent = val;
    }

    void SetColour (Colour col) { colour = col;}
    void SetModulatesColour (bool val) { modulateColor = val;}

private:
    std::string mFilePath;
    bool mUse16Bit;
    Texture* mTexture; 
    Vector2 pos;
    bool sizeToContent = true;
    Colour colour = Colour (255,255,255,255);
    bool modulateColor = false;
};

class CIwUIScrollableView : public CIwUIElement {
public:
    CIwUIScrollableView() : mVerticalScrollPosition(0), mHorizontalScrollPosition(0) {}

    void AddChild(CIwUIElement* child) { mChildren.push_back(child); }

    void SetVerticalScrollPosition(int position) {
        mVerticalScrollPosition = position;
        std::cout << "Vertical Scroll Position set to: " << mVerticalScrollPosition << "\n";
    }

    void SetHorizontalScrollPosition(int position) {
        mHorizontalScrollPosition = position;
        std::cout << "Horizontal Scroll Position set to: " << mHorizontalScrollPosition << "\n";
    }

    void Render() override {
        std::cout << "Rendering scrollable view. Scroll positions: (" << mHorizontalScrollPosition << ", " << mVerticalScrollPosition << ")\n";
        if (IsVisible()) {
            for (auto& child : mChildren) {
                child->Render();
            }
        }
    }

    // Obtenir la position actuelle du défilement
    Vector2 GetScrollPosition() const {
        return Vector2(mHorizontalScrollPosition, mVerticalScrollPosition);
    }

    // Définir une nouvelle position de défilement
    void ScrollToPosition(const CIwVec2& position) {
        mHorizontalScrollPosition = position.x;
        mVerticalScrollPosition = position.y;
        std::cout << "Scrolled to position: ";
        position.Print();
    }

private:
    int mVerticalScrollPosition;
    int mHorizontalScrollPosition;
    std::vector<CIwUIElement*> mChildren;
};

class CIwUIStyleManager {
public:
    void SetStylesheet(CIwUIStylesheet** stylesheet) {
        styleSheet = stylesheet;
        //std::cout << "Setting stylesheet: " << stylesheet->GetName() << std::endl;
    }
private :
    CIwUIStylesheet** styleSheet;
};

class CIwUIView {
public:
    CIwUIView() { std::cout << "CIwUIView created." << std::endl; }
    virtual ~CIwUIView() { 
        for (unsigned int i=0; i<mElements.size(); i++) {
            delete mElements[i];
        }
        std::cout << "CIwUIView destroyed." << std::endl; 
    }
    
    void Render() {
        for (auto& element : mElements) {
            element->Render();
        }
    }

    void AddElement(CIwUIElement* element) {mElements.push_back(element);}
    void AddElementToLayout (CIwUIElement* element) {element->GetLayout()->AddElement(element);}

    void RemoveElement (CIwUIElement* element) {
        auto it = std::remove(mElements.begin(), mElements.end(), element);
        mElements.erase(it, mElements.end());
    }

    bool Update(int i) {
        return true;
    }
private: 
    std::vector<CIwUIElement*> mElements;
};

class CIwUIController {
public:
    CIwUIController() { std::cout << "CIwUIController created." << std::endl; }
    virtual ~CIwUIController() { std::cout << "CIwUIController destroyed." << std::endl; }

    bool Update() {
        return true;
    }
};

class CIwUITextInput {
public:
    enum EditorMode {
        eInlineKeyboard,
        ePopupKeyboard
    };

    CIwUITextInput() { std::cout << "CIwUITextInput created." << std::endl; }

    void SetEditorMode(EditorMode mode) {
        std::cout << "Setting editor mode to: " << mode << std::endl;
        mEditorMode = mode;
    }

    EditorMode GetEditorMode() const {
        return mEditorMode;
    }

    virtual ~CIwUITextInput() { std::cout << "CIwUITextInput destroyed." << std::endl; }

private:
    EditorMode mEditorMode;
};

class CIwUILayoutVertical : public CIwUILayout {
public:
    CIwUILayoutVertical() : mSpacing(5) {}

    void SetSpacing(int spacing) { mSpacing = spacing; }

    void Render() override {
        int currentY = 0;
        for (auto& element : GetElements()) {
            element->SetPosition(Vector2(0, currentY));
            element->Render();
            currentY += element->GetSize().y + mSpacing;
        }
    }

    void SetSizeToSpace(bool val){
        sizeToSpace = val;
    }

private:
    int mSpacing;
    bool sizeToSpace = false;
};

class CIwUILayoutHorizontal : public CIwUILayout {
public:
    CIwUILayoutHorizontal() : mSpacing(5) {}

    void SetSpacing(int spacing) { mSpacing = spacing; }

    void Render() override {
        int currentX = 0;
        for (auto& element : GetElements()) {
            element->SetPosition(Vector2(currentX, 0));
            element->Render();
            currentX += element->GetSize().x + mSpacing;
        }
    }

private:
    int mSpacing;
};


CIwUITextInput* UITextInput;
CIwUIStyleManager* UIManager;
CIwUIController* UIController;
CIwUIView* UIView;
//CIwUILayout* UILayout;

inline CIwUITextInput* IwGetUITextInput() {
    //static CIwUITextInput textInput;
    return UITextInput;
}

inline CIwUIStyleManager* IwGetUIStyleManager() {
    //static CIwUIStyleManager manager;
    return UIManager;
}

inline CIwUIController* IwGetUIController() {
    return UIController;
}

inline CIwUIView* IwGetUIView() {
    return UIView;
}

inline void Iw2DSetColour(uint32_t hex) {
    float a = (hex >> 24) & 0xFF; // Rouge
    float b = (hex >> 16) & 0xFF; // Vert
    float g = (hex >> 8) & 0xFF;  // Bleu
    float r = hex & 0xFF;
    glClearColor(r/256.0f, g/256.0f, b/256.0f, a/256.0f);
}

inline void Iw2DDrawRect(Vector2 posiInit, Vector2 dimensions){
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

inline void Iw2DFillPolygon(const Vector2* verts, int NUM_VERTS) {
    // Activer le mode de dessin du polygone
    glBegin(GL_POLYGON);
    
    // Boucler à travers chaque sommet et le définir
    for (int i = 0; i < NUM_VERTS; ++i) {
        glVertex2f(verts[i].x, verts[i].y);  // verts[i] contient les coordonnées x, y
    }

    // Terminer le dessin du polygone
    glEnd();
}

using CIwUIColour = CIwColour;
#endif