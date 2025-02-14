cmake_minimum_required(VERSION 3.10) # Version minimale recommandée pour CMake
project(PicaSim)                    # Nom du projet

# Définitions globales pour le compilateur
set(CMAKE_CXX_STANDARD 17)          # Version de C++
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -g -DBT_NO_PROFILE -DIW_HTTP_SSL -DIW_GXFONT_TTF") # Flags du compilateur -Werror 

# Inclure les répertoires des headers
include_directories(
    Application/Challenge
    Application/Menus
    Application/Files
    Application/General
    Application/Graphics
    Application/Plane
    Application/Internet
    Application/Physics
    Application/Terrain
    Application/Heightfield
    Application/Languages
    Application/Controller
    Application/Controller/android
    Application/Controller/generic
    Application/Controller/h
    Application/Controller/iphone
    Application/Controller/win10
    Application/Controller/windows
    Application/AIController
    Application/UI
    Application/Bullet/BulletCollision
    Application/Bullet/BulletDynamics
    Application/Bullet/LinearMath
    Application/Bullet
    Application/tinyxml
    Application/MapTrace
    Application/Other
    Application
)

# Rechercher automatiquement tous les fichiers .cpp dans les répertoires spécifiés
file(GLOB_RECURSE SRCS Application/*.cpp)

list(APPEND SRCS 
    #Application/Controller/CustomController.cpp
)

# Ajout des bibliothèques externes
set(LIBRARIES 
    GL
    SDL2
    EGL
    GLESv2
    GLEW
    X11
    OpenCL
    OpenGL
    glut
    GL
    SDL2_ttf 
    SDL2_image
    tiff
    GLU
)

# Ajout des chemins pour les bibliothèques statiques ou dynamiques
link_directories(
    Application/Bullet/BulletCollision
    Application/Bullet/BulletDynamics
    Application/Bullet/LinearMath
    Application/Bullet
    Application/tinyxml
)

# Recherche et intégration de libcurl
find_package(CURL REQUIRED) # Recherche libcurl sur le système
include_directories(${CURL_INCLUDE_DIRS}) # Ajouter les répertoires d'en-têtes
list(APPEND LIBRARIES ${CURL_LIBRARIES}) # Ajouter libcurl aux bibliothèques à lier

# Définir l'exécutable
add_executable(${PROJECT_NAME} ${SRCS})

# Lier les bibliothèques au projet
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})