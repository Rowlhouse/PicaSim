#ifndef CLASSESUIMARMELADE_H
#define CLASSESUIMARMELADE_H

#include "ClassesFontMarmelade.h"
#include "ClassesMarmelade.h"
#include "Entete.h"
//#include "PicaDialog.h"
//#include "PicaDialog.h"
#include "Vecteurs.h"
#include <cstdint>
#include <string>
#include <iostream>
#include <algorithm> 
#include <unordered_map>


class CIwUILayout;
class CIwUIPropertySet;



class CIwUIStyle {
public:
    CIwUIStyle() {
        // Initialisation des propriétés par défaut, si nécessaire
    }

    // Méthode pour définir des propriétés en ligne
    void InlinePropertySet() {
        // Cela pourrait initialiser des valeurs par défaut pour ce style
        // mais dans ce cas, nous faisons une simple initialisation vide
    }

    // Retourne un objet de type CIwUIPropertySet pour manipuler les propriétés spécifiques
    CIwUIPropertySet* GetInlinePropertySet() {
        return properties;
    }

private:
    CIwUIPropertySet* properties;  // Ensemble de propriétés liées au style
};

class CIwUIPropertySet {
public:
    CIwUIPropertySet() : mVisible(true)  {}

    // Méthode pour définir une propriété donnée par un nom et une valeur
    void SetProperty(const std::string& key, const IwHashString value) {
        if (key == "drawableType") {
            drawableType = value;
        }
        else {
            std::cout << "La fonction SetProperty de CIwUIPropertySet ne connait pas la property demande" << key << std::endl;
        }
    }

    void SetProperty(const std::string& key, Texture* value) {
        if (key == "texture") {
            texture = value;
        }
        else {
            std::cout << "La fonction SetProperty de CIwUIPropertySet ne connait pas la property demande" << key << std::endl;
        }
    }

    void SetProperty(const std::string& key, Vector2 value) {
        if (key == "aspectRatio") {
            aspectRatio = value;
        }
        else {
            std::cout << "La fonction SetProperty de CIwUIPropertySet ne connait pas la property demande" << key << std::endl;
        }
    }

    void SetProperty(const std::string& key, CIwUIStyle value) {
        if (key == "buttonUp") {
            buttonUp = value;
        }
        else if (key == "buttonDown") {
            buttonDown = value;
        }
        else {
            std::cout << "La fonction SetProperty de CIwUIPropertySet ne connait pas la property demande" << key << std::endl;
        }
    }

    void SetProperty(const std::string& key, CIwPropertyValue value) {
        if (key == "alignH") {
            alignH = value;
        }
        else if (key == "alignV") {
            alignV = value;
        }
        else {
            std::cout << "La fonction SetProperty de CIwUIPropertySet ne connait pas la property demande" << key << std::endl;
        }
    }

    // Affiche toutes les propriétés pour le débogage
    /*void PrintProperties() const {
        for (const auto& pair : properties) {
            std::cout << pair.first << ": " << pair.second << std::endl;
        }
    }*/

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

    void SetColour (Colour Color) { color = Color; }

    void SetFont (CIwGxFont* value) { font = value; }
    CIwGxFont* GetPropertyFont () { return font; }

    void SetTextColour (Colour value) { textColour = value; }

    void SetCaption(std::string Caption) { caption = std::string( Caption); }
    std::string GetCaption() { return caption; }

    void SetStyle (std::string Style) { style = Style; }
    std::string GetStyle () { return style; }

    void SetSizeHint (Vector2 SizeHint) { sizeHint = SizeHint; }
    Vector2 GetSizeHint () { return sizeHint; }

    void SetTexture (Texture * value) { texture = value; }
    Texture* GetTexture () { return texture; }

    void SetEnabled (bool value) { enabled = value; }

    Vector2 GetSizeMin () { return sizeMin; }

private:
    IwHashString drawableType;
    Texture* texture;
    Vector2 aspectRatio;
    CIwUIStyle buttonUp;
    CIwUIStyle buttonDown;
    Vector2 pos=  Vector2(0,0);
    Vector2 size = Vector2(1,1);
    Vector2 sizeMin = Vector2 (1,1);
    Vector2 sizeHint;
    CIwUILayout* layout;
    bool SizeToContent = true;
    bool mVisible;
    bool enabled = true;
    Colour color;
    CIwGxFont* font;
    Colour textColour;
    CIwPropertyValue alignH = IW_UI_ALIGN_CENTRE;
    CIwPropertyValue alignV = IW_UI_ALIGN_CENTRE;
    std::string caption;
    std::string style;
};

class CIwUIStylesheet {
public:
    CIwUIStylesheet(const std::string& name = "") : styleSheetName(name) {}

    std::string GetName() { return styleSheetName; }
private:
    std::string styleSheetName;
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


class CIwUIElementEventHandler{
};

class CIwUIElement : public CIwUIPropertySet {
public:
    CIwUIElement() {}
    virtual ~CIwUIElement() = default;

    void AddChild(CIwUIElement* child) { mChildren.push_back(child); }
    virtual bool HandleEvent(CIwEvent* pEvent) { return false; }

    virtual void Render() {
        std::cout << "Rendering a generic UI element\n";
        if (!IsVisible()) return;
        for (auto& child : mChildren) {
            child->Render();
        }
    }

    void AddEventHandler (CIwUIElementEventHandler* value) {
        eventHandler = value;
    }

    

private:
    std::vector<CIwUIElement*> mChildren;
    CIwUIElementEventHandler* eventHandler = nullptr;
    
};

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

    void* GetRessource () { return ressource; }

private:
    std::string mName;
    resourceType resType;
    void* ressource;
    
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
    void DestroyGroup(CIwResource* group) {
        if (group) {
            std::cout << "Destroyed resource group: " << group->GetName() << std::endl;
            delete group;
        }
    }

    // Récupère une ressource nommée
    void* GetResNamed(const std::string& resourceName, resourceType resType) {
        CIwResource* res = new CIwResource(resourceName, resType);
        resourceList.push_back(res);
        return res->GetRessource();
    }

    void* GetResNamed(const std::string& resourceName, std::string resType) {
        CIwResource* res = new CIwResource(resourceName, resType);
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
    std::vector<void*> resourceList;
};

inline CIwResManager* IwGetResManager() {
    static CIwResManager manager;
    return &manager;
}

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

class CIwUILayout {
public:
    int rows = 1, cols = 1;

    void AddElement(CIwUIElement* element, int i = 0 , CIwPropertyValue alignH = IW_UI_ALIGN_CENTRE, CIwPropertyValue alignV = IW_UI_ALIGN_CENTRE, Vector2 size = Vector2(1,1)) {
        element->SetProperty("alignH",alignH);
        element->SetProperty("alignV",alignV);
        element->SetSize(size);
        mElements[i].push_back(element); 
    }

    void AddElement(CIwUIElement* element, int x , int y) { mElements[x][y] = element; }
    
    void AddElement(CIwUIElement* element, std::string style, int i = 0) {
        element->SetStyle(style);
        GetElements()[i].push_back(element);
    }

    //void RemoveElement(CIwUIElement* element) { mElements.erase(std::remove(mElements.begin(), mElements.end(), element), mElements.end()); }

    virtual void Render() {
        std::cout << "Rendering layout\n";
        for (auto& vectorElements : mElements) {
            for (auto& element : vectorElements) {
                element->Render();
            }
        }
    }

    std::vector<std::vector<CIwUIElement*>> GetElements () { return mElements; }
    void SetSizeToSpace (bool val) {SizeToSpace = val;}

private:
    bool SizeToSpace = false;
    //std::vector<CIwUIElement*> mElements;
    std::vector<std::vector<CIwUIElement*>> mElements = std::vector<std::vector<CIwUIElement*>> (rows, std::vector<CIwUIElement*>(cols, 0));

};

// Classe CIwUILayout pour gérer la disposition des éléments
class CIwUILayoutGrid : public CIwUILayout {
public :
    void AddColumn (int row = 0, float espacement = 0){
        mSpacingColumn = espacement;
        cols+=1;
        for (unsigned int i; i<GetElements ().size(); i++) {
            GetElements ()[i].push_back(0);
        }
    }

    void AddRow (int column = 0, float espacement = 0){
        mSpacingRow = espacement;
        rows+=1;
        GetElements ().push_back(std::vector<CIwUIElement*>(cols,0));
    }

private : 
    float mSpacingColumn = 0;
    float mSpacingRow = 0;
};

class CIwUILayoutVertical : public CIwUILayout {
public:
    CIwUILayoutVertical() : mSpacing(5) {}

    void SetSpacing(int spacing) { mSpacing = spacing; }

    void Render() override {
        int currentY = 0;
        for (auto& vectorElements : GetElements()) {
            for (auto& element : vectorElements) {
                element->SetPosition(Vector2(0, currentY));
                element->Render();
                currentY += element->GetSize().y + mSpacing;
            }
        }
    }

private:
    int mSpacing;
};

class CIwUILayoutHorizontal : public CIwUILayout {
public:
    CIwUILayoutHorizontal() : mSpacing(5) {}

    void SetSpacing(float spacing) { mSpacing = spacing; }

    void Render() override {
        int currentY = 0;
        for (auto& vectorElements : GetElements()) {
            for (auto& element : vectorElements) {
                element->SetPosition(Vector2(0, currentY));
                element->Render();
                currentY += element->GetSize().y + mSpacing;
            }
        }
    }

    void AddColumn(int i, float spacing) {

        mSpacing = spacing;
    }

private:
    float mSpacing;
};

class CIwUIImage : public CIwUIElement {
public:
    CIwUIImage(){}
    CIwUIImage(const std::string& filename, bool use16Bit) : mFilePath(filename), mUse16Bit(use16Bit)/*, mTexture(nullptr)*/ {
        LoadImage();
    }

    ~CIwUIImage() {
        /*if (texture) {
            delete texture;
        }*/
    }

    void LoadImage() {
        std::cout << "Loading image: " << mFilePath << " with 16-bit: " << mUse16Bit << std::endl;
        SetTexture( new Texture(mFilePath, mUse16Bit ));
    }

    void Render() override {
        if (IsVisible() && GetTexture()) {
            std::cout << "Rendering image from: " << mFilePath << std::endl;
        }
    }

    /*void SetPos(Vector2 Pos) { pos = Pos; }
    Vector2 GetPos() const { return pos; }

    void SetSizeToContent(bool val) {    ////////////////////////////////////////////////////////// If true the the whole screen is vertically centred. False places it at the top
        sizeToContent = val;
    }*/

    //void SetColour (Colour col) { colour = col;}
    void SetModulatesColour (bool val) { modulateColor = val;}

    void SetLayout (CIwUILayout* val) { layout = val; }

private:
    std::string mFilePath;
    bool mUse16Bit;
    //Texture* mTexture; 
    //Vector2 pos;
    //bool sizeToContent = true;
    //Colour colour = Colour (255,255,255,255);
    bool modulateColor = false;
    CIwUILayout* layout = nullptr;
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

private:
    std::string mText;
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
            GenerateRect();
            // Définir la couleur en fonction du focus
            if (mFocused)
            {
                SDL_SetRenderDrawColor(GeneralRenderManager.getRenderer(), 255, 255, 0, 255); // Jaune si focus
            }
            else
            {
                SDL_SetRenderDrawColor(GeneralRenderManager.getRenderer(), 128, 128, 128, 255); // Gris sinon
            }

            // Dessiner le bouton
            SDL_RenderFillRect(GeneralRenderManager.getRenderer(), &mRect);

            // Dessiner le texte (simplifié)
            SDL_Color textColor = {255, 255, 255, 255}; // Blanc
            SDL_Surface *textSurface = TTF_RenderText_Solid(GetPropertyFont()->GetTTF_Font(), mCaption.c_str(), textColor);
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(GeneralRenderManager.getRenderer(), textSurface);
            SDL_Rect textRect = {mRect.x + 10, mRect.y + 10, textSurface->w, textSurface->h};

            SDL_RenderCopy(GeneralRenderManager.getRenderer(), textTexture, nullptr, &textRect);

            // Nettoyer la mémoire
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }
    }

    void AddEventHandler(CIwUIElement* handler) { mHandlers.push_back(handler); }
    void RemoveEventHandler(CIwUIElement* handler) {
        auto it = std::remove(mHandlers.begin(), mHandlers.end(), handler);
        mHandlers.erase(it, mHandlers.end());
    }

    void RequestFocus() {
        mFocused = true;
    }

    bool IsMouseOver(int mouseX, int mouseY) {
        return (mouseX >= mRect.x && mouseX <= (mRect.x + mRect.w) &&
                mouseY >= mRect.y && mouseY <= (mRect.y + mRect.h));
    }

    void GenerateRect () {
        mRect = CIwRect (GetPosition(), GetSize());
    }


private:
    std::string mStyle;
    std::string mCaption;
    std::vector<CIwUIElement*> mHandlers;
    bool mFocused = false;
    CIwRect mRect;
};

class CIwUISlider : public CIwUIElement {

};

class CIwUICheckbox : public CIwUIElement {

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

    int GetNumElements () {
        return mElements.size();
    }

    bool Update(int i) {
        return true;
    }

    void DestroyElements() {
        for (CIwUIElement* element : mElements) {
            if (element) {
                delete element;
            }
        }
        mElements.clear();
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

class CIwUITextField : public CIwUIElement {

};

class CIwUITabBar : public CIwUIElement {
private:
    struct Tab {
        std::string label;
        CIwUIElement* content;
    };

    std::vector<Tab> tabs;
    int16_t selectedTab;

public:
    CIwUITabBar() : selectedTab(0) {}

    void AddTab(const std::string& title, CIwUIElement* content) {
        tabs.push_back({title, content});
    }

    void SetRadioButtonStyle(int16_t tabIndex, const std::string& style) {
        if (tabIndex >= 0 && tabIndex < tabs.size()) {
            std::cout << "Style for tab " << tabIndex << " set to: " << style << std::endl;
        }
    }

    void SetRadioButtonCaption(int16_t tabIndex, const std::string& caption) {
        if (tabIndex >= 0 && tabIndex < tabs.size()) {
            tabs[tabIndex].label = caption;
        }
    }

    void SetSelected(int16_t tabIndex) {
        if (tabIndex >= 0 && tabIndex < tabs.size()) {
            selectedTab = tabIndex;
        }
    }

    void Render() override {
        for (size_t i = 0; i < tabs.size(); ++i) {
            std::cout << "Rendering tab: " << tabs[i].label << std::endl;
            if (i == selectedTab) {
                std::cout << "Tab " << i << " is selected!" << std::endl;
            }
        }
    }

    int16_t GetSelected() {
        return selectedTab;
    }
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

struct Element {
    int x, y, width, height; // Dimensions de l'éléments
};
// Recréation de la fonction IwUIDebugRender
inline void IwUIDebugRender(SDL_Renderer* renderer, int flags) {
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

using CIwUIColour = CIwColour;
#endif