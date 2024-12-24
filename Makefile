# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -g -DBT_NO_PROFILE -DIW_HTTP_SSL -DIW_GXFONT_TTF

# Include paths
INCLUDES = -Isource/tinyxml -Isource/Heightfield -Isource/Framework -Isource/bullet -I/usr/include/glm -I/usr/include/SDL2

# Source files and object files
SRC_DIRS = source/PicaSim source/PicaSim/Languages source/PicaSim/Menus source/Framework source/Heightfield source/tinyxml
SRCS = $(wildcard $(addsuffix /*.cpp, $(SRC_DIRS)))
OBJS = $(SRCS:.cpp=.o)

# Library paths and libraries
LDFLAGS = -Lsource/bullet -lBulletDynamics -lBulletCollision -lLinearMath -lGL -lGLEW -lSDL2

# Target executable
TARGET = PicaSim

# Rules
all: clean $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean


#Explication des sections du Makefile :

#     Variables de compilation :
#         CXX : Le compilateur à utiliser (g++ pour les fichiers C++).
#         CXXFLAGS : Les flags du compilateur, incluant les options de préprocesseur définies dans votre script mkb.

#     Chemins d'inclusion :
#         INCLUDES : Les chemins vers les répertoires contenant les fichiers .h.

#     Fichiers sources et objets :
#         SRC_DIRS : Les répertoires contenant les fichiers sources.
#         SRCS : Tous les fichiers .cpp dans les répertoires spécifiés.
#         OBJS : Les fichiers objets correspondants.

#     Cible d'exécutable :
#         TARGET : Le nom de l'exécutable final.

#     Règles de compilation :
#         all : Règle pour construire l'exécutable final.
#         $(TARGET) : Lie tous les fichiers objets pour créer l'exécutable.
#         %.o : Règle pour compiler chaque fichier source en fichier objet.
#         clean : Règle pour nettoyer les fichiers objets et l'exécutable.

# Utilisation :

# Pour compiler le projet, naviguez dans le répertoire contenant votre Makefile et exécutez :

# make

# Pour nettoyer les fichiers objets et l'exécutable, utilisez :

# make clean
