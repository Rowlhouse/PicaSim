#include "TileExpansion.h"

#include <algorithm>
#include <cstring>

//======================================================================================================================
// Get the index of the neighboring tile within the same face
// Returns -1 if the neighbor is outside this face (edge tile)
static int GetNeighborTileIndex(int tileIndex, int numPerSide, int dx, int dy)
{
    int tileX = tileIndex % numPerSide;
    int tileY = tileIndex / numPerSide;
    int neighborX = tileX + dx;
    int neighborY = tileY + dy;

    if (neighborX < 0 || neighborX >= numPerSide ||
        neighborY < 0 || neighborY >= numPerSide)
    {
        return -1; // Outside this face
    }

    return neighborY * numPerSide + neighborX;
}

//======================================================================================================================
// Adjacent face mapping for cubemap edge crossings
// Returns the adjacent face and which edge of that face to sample from
struct AdjacentFaceInfo
{
    int face;           // Adjacent face index (FACE_UP, FACE_FRONT, etc.)
    int edgeToSample;   // 0=left, 1=right, 2=top, 3=bottom edge of adjacent face
    bool flipU;         // Whether to flip U coordinate when sampling
    bool flipV;         // Whether to flip V coordinate when sampling
};

static AdjacentFaceInfo GetAdjacentFace(int face, int edge)
{
    // edge: 0=left, 1=right, 2=top, 3=bottom
    // Returns which face is adjacent and how to sample its edge
    static const AdjacentFaceInfo adjacency[NUM_CUBEMAP_FACES][4] = {
        // UP face: left=LEFT, right=RIGHT, top=BACK, bottom=FRONT
        {{FACE_LEFT, 2, false, false}, {FACE_RIGHT, 2, false, true}, {FACE_BACK, 2, true, false}, {FACE_FRONT, 2, false, false}},
        // FRONT face: left=LEFT, right=RIGHT, top=UP, bottom=DOWN
        {{FACE_LEFT, 1, false, false}, {FACE_RIGHT, 0, false, false}, {FACE_UP, 3, false, false}, {FACE_DOWN, 2, false, false}},
        // LEFT face: left=BACK, right=FRONT, top=UP, bottom=DOWN
        {{FACE_BACK, 1, false, false}, {FACE_FRONT, 0, false, false}, {FACE_UP, 0, false, false}, {FACE_DOWN, 0, false, true}},
        // BACK face: left=RIGHT, right=LEFT, top=UP, bottom=DOWN
        {{FACE_RIGHT, 1, false, false}, {FACE_LEFT, 0, false, false}, {FACE_UP, 2, true, false}, {FACE_DOWN, 3, true, false}},
        // RIGHT face: left=FRONT, right=BACK, top=UP, bottom=DOWN
        {{FACE_FRONT, 1, false, false}, {FACE_BACK, 0, false, false}, {FACE_UP, 1, false, true}, {FACE_DOWN, 1, false, false}},
        // DOWN face: left=LEFT, right=RIGHT, top=FRONT, bottom=BACK
        {{FACE_LEFT, 3, false, true}, {FACE_RIGHT, 3, false, false}, {FACE_FRONT, 3, false, false}, {FACE_BACK, 3, true, false}},
    };
    return adjacency[face][edge];
}

//======================================================================================================================
// Create an expanded image with borders from neighboring tiles
// The result is a new image with dimensions (width + 2*borderPixels) x (height + 2*borderPixels)
Image* CreateExpandedTileImage(
    const std::vector<Image*>& faceImages,
    const std::vector<Image*> allFaceImages[NUM_CUBEMAP_FACES],
    int face,
    int tileIndex,
    int numPerSide,
    int borderPixels)
{
    if (faceImages.empty() || tileIndex >= (int)faceImages.size())
        return nullptr;

    Image* srcImage = faceImages[tileIndex];
    int srcWidth = srcImage->GetWidth();
    int srcHeight = srcImage->GetHeight();
    int channels = srcImage->GetChannels();
    unsigned char* srcData = srcImage->GetTexels();

    int newWidth = srcWidth + 2 * borderPixels;
    int newHeight = srcHeight + 2 * borderPixels;

    // Allocate new image data
    Image* expandedImage = new Image();
    int dataSize = newWidth * newHeight * channels;
    unsigned char* expandedData = new unsigned char[dataSize];

    // Initialize to black (fallback)
    memset(expandedData, 0, dataSize);

    // Copy the main tile content to the center
    for (int y = 0; y < srcHeight; ++y)
    {
        memcpy(expandedData + ((y + borderPixels) * newWidth + borderPixels) * channels,
               srcData + (y * srcWidth) * channels,
               srcWidth * channels);
    }

    // Helper to get pixel from a tile (with clamping)
    auto getPixel = [channels](Image* img, int x, int y, unsigned char* out) {
        if (!img) { memset(out, 0, channels); return; }
        x = std::max(0, std::min(x, img->GetWidth() - 1));
        y = std::max(0, std::min(y, img->GetHeight() - 1));
        memcpy(out, img->GetTexels() + (y * img->GetWidth() + x) * channels, channels);
    };

    int tileX = tileIndex % numPerSide;
    int tileY = tileIndex / numPerSide;

    // Fill the border regions from neighboring tiles
    // Left border
    {
        int neighborIdx = GetNeighborTileIndex(tileIndex, numPerSide, -1, 0);
        Image* neighbor = (neighborIdx >= 0 && neighborIdx < (int)faceImages.size()) ? faceImages[neighborIdx] : nullptr;
        AdjacentFaceInfo adj = {-1, 0, false, false};

        // If no neighbor within face, try adjacent face
        if (!neighbor && tileX == 0)
        {
            adj = GetAdjacentFace(face, 0); // left edge
            const std::vector<Image*>& adjFace = allFaceImages[adj.face];
            if (!adjFace.empty())
            {
                // Determine which tile from adjacent face based on edge orientation
                int adjIdx = -1;
                switch (adj.edgeToSample)
                {
                case 0: // left edge of neighbor - rightmost column of tiles
                    adjIdx = (adj.flipV ? (numPerSide - 1 - tileY) : tileY) * numPerSide + (numPerSide - 1);
                    break;
                case 1: // right edge of neighbor - leftmost column of tiles
                    adjIdx = (adj.flipV ? (numPerSide - 1 - tileY) : tileY) * numPerSide + 0;
                    break;
                case 2: // top edge of neighbor - bottom row of tiles
                    adjIdx = (numPerSide - 1) * numPerSide + (adj.flipU ? (numPerSide - 1 - tileY) : tileY);
                    break;
                case 3: // bottom edge of neighbor - top row of tiles
                    adjIdx = 0 * numPerSide + (adj.flipU ? (numPerSide - 1 - tileY) : tileY);
                    break;
                }
                if (adjIdx >= 0 && adjIdx < (int)adjFace.size())
                    neighbor = adjFace[adjIdx];
            }
        }

        for (int y = 0; y < srcHeight; ++y)
        {
            for (int x = 0; x < borderPixels; ++x)
            {
                unsigned char pixel[4];
                if (neighbor)
                {
                    int sampleX, sampleY;
                    if (adj.face >= 0)
                    {
                        // Cross-face: sample based on which edge of neighbor
                        switch (adj.edgeToSample)
                        {
                        case 0: // left edge - sample column near x=0
                            sampleX = borderPixels - 1 - x;
                            sampleY = adj.flipV ? (srcHeight - 1 - y) : y;
                            break;
                        case 1: // right edge - sample column near x=width
                        default:
                            sampleX = srcWidth - borderPixels + x;
                            sampleY = adj.flipV ? (srcHeight - 1 - y) : y;
                            break;
                        case 2: // top edge - sample row near y=0
                            sampleX = adj.flipV ? (srcWidth - 1 - y) : y;
                            sampleY = borderPixels - 1 - x;
                            break;
                        case 3: // bottom edge - sample row near y=height
                            sampleX = adj.flipV ? (srcWidth - 1 - y) : y;
                            sampleY = srcHeight - borderPixels + x;
                            break;
                        }
                    }
                    else
                    {
                        // Same face: sample from right of neighbor tile
                        sampleX = srcWidth - borderPixels + x;
                        sampleY = y;
                    }
                    getPixel(neighbor, sampleX, sampleY, pixel);
                }
                else
                    getPixel(srcImage, 0, y, pixel); // Clamp to edge

                memcpy(expandedData + ((y + borderPixels) * newWidth + x) * channels, pixel, channels);
            }
        }
    }

    // Right border
    {
        int neighborIdx = GetNeighborTileIndex(tileIndex, numPerSide, 1, 0);
        Image* neighbor = (neighborIdx >= 0 && neighborIdx < (int)faceImages.size()) ? faceImages[neighborIdx] : nullptr;
        AdjacentFaceInfo adj = {-1, 0, false, false};

        if (!neighbor && tileX == numPerSide - 1)
        {
            adj = GetAdjacentFace(face, 1); // right edge
            const std::vector<Image*>& adjFace = allFaceImages[adj.face];
            if (!adjFace.empty())
            {
                // Determine which tile from adjacent face based on edge orientation
                int adjIdx = -1;
                switch (adj.edgeToSample)
                {
                case 0: // left edge of neighbor - rightmost column of tiles
                    adjIdx = (adj.flipV ? (numPerSide - 1 - tileY) : tileY) * numPerSide + (numPerSide - 1);
                    break;
                case 1: // right edge of neighbor - leftmost column of tiles
                    adjIdx = (adj.flipV ? (numPerSide - 1 - tileY) : tileY) * numPerSide + 0;
                    break;
                case 2: // top edge of neighbor - bottom row of tiles
                    adjIdx = (numPerSide - 1) * numPerSide + (adj.flipU ? (numPerSide - 1 - tileY) : tileY);
                    break;
                case 3: // bottom edge of neighbor - top row of tiles
                    adjIdx = 0 * numPerSide + (adj.flipU ? (numPerSide - 1 - tileY) : tileY);
                    break;
                }
                if (adjIdx >= 0 && adjIdx < (int)adjFace.size())
                    neighbor = adjFace[adjIdx];
            }
        }

        for (int y = 0; y < srcHeight; ++y)
        {
            for (int x = 0; x < borderPixels; ++x)
            {
                unsigned char pixel[4];
                if (neighbor)
                {
                    int sampleX, sampleY;
                    if (adj.face >= 0)
                    {
                        // Cross-face: sample based on which edge of neighbor
                        switch (adj.edgeToSample)
                        {
                        case 0: // left edge - sample column near x=0
                        default:
                            sampleX = x;
                            sampleY = adj.flipV ? (srcHeight - 1 - y) : y;
                            break;
                        case 1: // right edge - sample column near x=width
                            sampleX = srcWidth - 1 - x;
                            sampleY = adj.flipV ? (srcHeight - 1 - y) : y;
                            break;
                        case 2: // top edge - sample row near y=0
                            sampleX = adj.flipV ? (srcWidth - 1 - y) : y;
                            sampleY = x;
                            break;
                        case 3: // bottom edge - sample row near y=height
                            sampleX = adj.flipV ? (srcWidth - 1 - y) : y;
                            sampleY = srcHeight - 1 - x;
                            break;
                        }
                    }
                    else
                    {
                        // Same face: sample from left of neighbor tile
                        sampleX = x;
                        sampleY = y;
                    }
                    getPixel(neighbor, sampleX, sampleY, pixel);
                }
                else
                    getPixel(srcImage, srcWidth - 1, y, pixel);

                memcpy(expandedData + ((y + borderPixels) * newWidth + srcWidth + borderPixels + x) * channels, pixel, channels);
            }
        }
    }

    // Top border
    {
        int neighborIdx = GetNeighborTileIndex(tileIndex, numPerSide, 0, -1);
        Image* neighbor = (neighborIdx >= 0 && neighborIdx < (int)faceImages.size()) ? faceImages[neighborIdx] : nullptr;
        AdjacentFaceInfo adj = {-1, 0, false, false};

        if (!neighbor && tileY == 0)
        {
            adj = GetAdjacentFace(face, 2); // top edge
            const std::vector<Image*>& adjFace = allFaceImages[adj.face];
            if (!adjFace.empty())
            {
                // Determine which tile from adjacent face based on edge orientation
                int adjIdx = -1;
                switch (adj.edgeToSample)
                {
                case 0: // left edge of neighbor - rightmost column of tiles
                    adjIdx = (adj.flipV ? (numPerSide - 1 - tileX) : tileX) * numPerSide + (numPerSide - 1);
                    break;
                case 1: // right edge of neighbor - leftmost column of tiles
                    adjIdx = (adj.flipV ? (numPerSide - 1 - tileX) : tileX) * numPerSide + 0;
                    break;
                case 2: // top edge of neighbor - bottom row of tiles
                    adjIdx = (numPerSide - 1) * numPerSide + (adj.flipU ? (numPerSide - 1 - tileX) : tileX);
                    break;
                case 3: // bottom edge of neighbor - top row of tiles
                    adjIdx = 0 * numPerSide + (adj.flipU ? (numPerSide - 1 - tileX) : tileX);
                    break;
                }
                if (adjIdx >= 0 && adjIdx < (int)adjFace.size())
                    neighbor = adjFace[adjIdx];
            }
        }

        for (int y = 0; y < borderPixels; ++y)
        {
            for (int x = 0; x < srcWidth; ++x)
            {
                unsigned char pixel[4];
                if (neighbor)
                {
                    int sampleX, sampleY;
                    if (adj.face >= 0)
                    {
                        // Cross-face: sample based on which edge of neighbor
                        switch (adj.edgeToSample)
                        {
                        case 0: // left edge - sample column near x=0
                            sampleX = borderPixels - 1 - y;
                            sampleY = adj.flipV ? (srcHeight - 1 - x) : x;
                            break;
                        case 1: // right edge - sample column near x=width
                            sampleX = srcWidth - borderPixels + y;
                            sampleY = adj.flipV ? (srcHeight - 1 - x) : x;
                            break;
                        case 2: // top edge - sample row near y=0
                            sampleX = adj.flipU ? (srcWidth - 1 - x) : x;
                            sampleY = borderPixels - 1 - y;
                            break;
                        case 3: // bottom edge - sample row near y=height
                        default:
                            sampleX = adj.flipU ? (srcWidth - 1 - x) : x;
                            sampleY = srcHeight - borderPixels + y;
                            break;
                        }
                    }
                    else
                    {
                        // Same face: sample from bottom of neighbor tile
                        sampleX = x;
                        sampleY = srcHeight - borderPixels + y;
                    }
                    getPixel(neighbor, sampleX, sampleY, pixel);
                }
                else
                    getPixel(srcImage, x, 0, pixel);

                memcpy(expandedData + (y * newWidth + x + borderPixels) * channels, pixel, channels);
            }
        }
    }

    // Bottom border
    {
        int neighborIdx = GetNeighborTileIndex(tileIndex, numPerSide, 0, 1);
        Image* neighbor = (neighborIdx >= 0 && neighborIdx < (int)faceImages.size()) ? faceImages[neighborIdx] : nullptr;
        AdjacentFaceInfo adj = {-1, 0, false, false};

        if (!neighbor && tileY == numPerSide - 1)
        {
            adj = GetAdjacentFace(face, 3); // bottom edge
            const std::vector<Image*>& adjFace = allFaceImages[adj.face];
            if (!adjFace.empty())
            {
                // Determine which tile from adjacent face based on edge orientation
                int adjIdx = -1;
                switch (adj.edgeToSample)
                {
                case 0: // left edge of neighbor - rightmost column of tiles
                    adjIdx = (adj.flipV ? (numPerSide - 1 - tileX) : tileX) * numPerSide + (numPerSide - 1);
                    break;
                case 1: // right edge of neighbor - leftmost column of tiles
                    adjIdx = (adj.flipV ? (numPerSide - 1 - tileX) : tileX) * numPerSide + 0;
                    break;
                case 2: // top edge of neighbor - bottom row of tiles
                    adjIdx = (numPerSide - 1) * numPerSide + (adj.flipU ? (numPerSide - 1 - tileX) : tileX);
                    break;
                case 3: // bottom edge of neighbor - top row of tiles
                    adjIdx = 0 * numPerSide + (adj.flipU ? (numPerSide - 1 - tileX) : tileX);
                    break;
                }
                if (adjIdx >= 0 && adjIdx < (int)adjFace.size())
                    neighbor = adjFace[adjIdx];
            }
        }

        for (int y = 0; y < borderPixels; ++y)
        {
            for (int x = 0; x < srcWidth; ++x)
            {
                unsigned char pixel[4];
                if (neighbor)
                {
                    int sampleX, sampleY;
                    if (adj.face >= 0)
                    {
                        // Cross-face: sample based on which edge of neighbor
                        switch (adj.edgeToSample)
                        {
                        case 0: // left edge - sample column near x=0
                            sampleX = y;
                            sampleY = adj.flipV ? (srcHeight - 1 - x) : x;
                            break;
                        case 1: // right edge - sample column near x=width
                            sampleX = srcWidth - 1 - y;
                            sampleY = adj.flipV ? (srcHeight - 1 - x) : x;
                            break;
                        case 2: // top edge - sample row near y=0
                            sampleX = adj.flipU ? (srcWidth - 1 - x) : x;
                            sampleY = y;
                            break;
                        case 3: // bottom edge - sample row near y=height
                        default:
                            sampleX = adj.flipU ? (srcWidth - 1 - x) : x;
                            sampleY = srcHeight - 1 - y;
                            break;
                        }
                    }
                    else
                    {
                        // Same face: sample from top of neighbor tile
                        sampleX = x;
                        sampleY = y;
                    }
                    getPixel(neighbor, sampleX, sampleY, pixel);
                }
                else
                    getPixel(srcImage, x, srcHeight - 1, pixel);

                memcpy(expandedData + ((srcHeight + borderPixels + y) * newWidth + x + borderPixels) * channels, pixel, channels);
            }
        }
    }

    // Corner regions - use diagonal neighbors or clamp
    // Top-left corner
    {
        int diagIdx = GetNeighborTileIndex(tileIndex, numPerSide, -1, -1);
        Image* diag = (diagIdx >= 0 && diagIdx < (int)faceImages.size()) ? faceImages[diagIdx] : nullptr;

        for (int y = 0; y < borderPixels; ++y)
        {
            for (int x = 0; x < borderPixels; ++x)
            {
                unsigned char pixel[4];
                if (diag)
                    getPixel(diag, srcWidth - borderPixels + x, srcHeight - borderPixels + y, pixel);
                else
                    getPixel(srcImage, 0, 0, pixel);

                memcpy(expandedData + (y * newWidth + x) * channels, pixel, channels);
            }
        }
    }

    // Top-right corner
    {
        int diagIdx = GetNeighborTileIndex(tileIndex, numPerSide, 1, -1);
        Image* diag = (diagIdx >= 0 && diagIdx < (int)faceImages.size()) ? faceImages[diagIdx] : nullptr;

        for (int y = 0; y < borderPixels; ++y)
        {
            for (int x = 0; x < borderPixels; ++x)
            {
                unsigned char pixel[4];
                if (diag)
                    getPixel(diag, x, srcHeight - borderPixels + y, pixel);
                else
                    getPixel(srcImage, srcWidth - 1, 0, pixel);

                memcpy(expandedData + (y * newWidth + srcWidth + borderPixels + x) * channels, pixel, channels);
            }
        }
    }

    // Bottom-left corner
    {
        int diagIdx = GetNeighborTileIndex(tileIndex, numPerSide, -1, 1);
        Image* diag = (diagIdx >= 0 && diagIdx < (int)faceImages.size()) ? faceImages[diagIdx] : nullptr;

        for (int y = 0; y < borderPixels; ++y)
        {
            for (int x = 0; x < borderPixels; ++x)
            {
                unsigned char pixel[4];
                if (diag)
                    getPixel(diag, srcWidth - borderPixels + x, y, pixel);
                else
                    getPixel(srcImage, 0, srcHeight - 1, pixel);

                memcpy(expandedData + ((srcHeight + borderPixels + y) * newWidth + x) * channels, pixel, channels);
            }
        }
    }

    // Bottom-right corner
    {
        int diagIdx = GetNeighborTileIndex(tileIndex, numPerSide, 1, 1);
        Image* diag = (diagIdx >= 0 && diagIdx < (int)faceImages.size()) ? faceImages[diagIdx] : nullptr;

        for (int y = 0; y < borderPixels; ++y)
        {
            for (int x = 0; x < borderPixels; ++x)
            {
                unsigned char pixel[4];
                if (diag)
                    getPixel(diag, x, y, pixel);
                else
                    getPixel(srcImage, srcWidth - 1, srcHeight - 1, pixel);

                memcpy(expandedData + ((srcHeight + borderPixels + y) * newWidth + srcWidth + borderPixels + x) * channels, pixel, channels);
            }
        }
    }

    // Set up the expanded image
    expandedImage->SetWidth(newWidth);
    expandedImage->SetHeight(newHeight);
    expandedImage->SetChannels(channels);
    expandedImage->SetFormat(srcImage->GetFormat());
    expandedImage->SetBuffers(expandedData, dataSize);

    return expandedImage;
}
