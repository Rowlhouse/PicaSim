#ifndef TILE_EXPANSION_H
#define TILE_EXPANSION_H

#include "../Platform/Texture.h"
#include <vector>

// Cubemap face indices for tile expansion
enum CubemapFace {
    FACE_UP = 0,
    FACE_FRONT,
    FACE_LEFT,
    FACE_BACK,
    FACE_RIGHT,
    FACE_DOWN,
    NUM_CUBEMAP_FACES
};

/**
 * Create an expanded tile image with borders from neighboring tiles.
 *
 * This function takes a tile from a tiled cubemap panorama and expands it
 * by adding border pixels sampled from adjacent tiles. This allows parallax
 * UV offsets to sample beyond the original tile boundaries without artifacts.
 *
 * @param faceImages      All tile images for the current face
 * @param allFaceImages   All tile images for all 6 cubemap faces (for edge crossings)
 * @param face            Current face index (0-5, use CubemapFace enum)
 * @param tileIndex       Which tile within the face to expand
 * @param numPerSide      Grid dimension (e.g., 2 for a 2x2 grid of tiles)
 * @param borderPixels    Number of border pixels to add on each side
 * @return                New Image with dimensions (width + 2*borderPixels) x (height + 2*borderPixels)
 *                        Caller is responsible for deleting the returned Image.
 */
Image* CreateExpandedTileImage(
    const std::vector<Image*>& faceImages,
    const std::vector<Image*> allFaceImages[NUM_CUBEMAP_FACES],
    int face,
    int tileIndex,
    int numPerSide,
    int borderPixels);

#endif // TILE_EXPANSION_H
