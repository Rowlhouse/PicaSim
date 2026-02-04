# Parallax Panorama Skybox System

This document describes the technical implementation of the VR parallax panoramic skybox system in PicaSim. The goal is to render a cubemap skybox with depth-based parallax for stereoscopic VR, supporting tiled panoramas with seamless edge transitions between cube faces.

## Overview

The system consists of three main components:

1. **TileExpansion** (`source/Framework/TileExpansion.cpp`) - Adds pixel borders to tiles
2. **Skybox** (`source/Framework/Skybox.cpp`) - Loads, expands, and renders panorama textures
3. **Shader** (`source/Framework/Shaders.cpp`, `skyboxVRParallaxFragmentShaderStr`) - Calculates parallax and handles edge warping

## Source Images

Skybox images are organized by detail level and face:
```
data/SystemData/Panoramas/{PanoramaName}/{detail}/
    up1.jpg, up2.jpg, up3.jpg, up4.jpg      (2x2 grid = 4 tiles)
    front1.jpg, front2.jpg, ...
    left1.jpg, ...
    back1.jpg, ...
    right1.jpg, ...
    down1.jpg, ...
```

- `numPerSide` = sqrt(number of tiles per face) - currently 1 or 2, but could be more
- Tiles are numbered row-by-row: for 2x2, tile 1=top-left, 2=top-right, 3=bottom-left, 4=bottom-right

## Problem Statement

When rendering with depth-based parallax, the shader calculates a UV offset based on the depth at each pixel. This offset can push the sample point outside the [0,1] UV range of the current tile. We need to:

1. Sample the correct pixel from the adjacent tile or cube face
2. Handle the geometric distortion that occurs at cube face boundaries

## Texture Border Expansion (TileExpansion.cpp)

### Purpose

The shader needs to sample beyond the tile's original UV range. Rather than doing complex cross-texture lookups at runtime, we pre-expand each tile texture with border pixels from neighbors.

### Key Parameters

- `panoramaExtension` (e.g., 0.1 = 10%) - Specifies border size as fraction of tile size
- `borderPixels = tileWidth * panoramaExtension` - Actual pixel count for border

### Expansion Process

For each tile, `CreateExpandedTileImage()` creates a new image with dimensions `(width + 2*borderPixels) x (height + 2*borderPixels)`:

1. **Copy center** - The original tile content is placed in the center region
2. **Fill edge borders** - For each edge (left, right, top, bottom):
   - If there's a neighboring tile on the same face, sample from its edge
   - If this is an edge tile (at face boundary), sample from the adjacent cube face
3. **Fill corners** - Use diagonal neighbors or clamp to edge pixel

### Cross-Face Sampling

When a tile is at the edge of a cube face, its border must come from a different face. The `GetAdjacentFace()` function returns:

```cpp
struct AdjacentFaceInfo {
    int face;           // Which cube face to sample from
    int edgeToSample;   // Which edge of that face (0=left, 1=right, 2=top, 3=bottom)
    bool flipU;         // Coordinate flip for orientation matching
    bool flipV;
};
```

The adjacency table encodes the cube topology:
- FRONT's left edge connects to LEFT's right edge
- FRONT's top edge connects to UP's bottom edge
- UP's left edge connects to LEFT's top edge (with potential flip)
- etc.

### Important: No Distortion in Borders

The border pixels are copied **without any geometric distortion**. They represent a simple 1:1 pixel copy from the neighbor. This is intentional - the distortion correction happens in the shader at render time.

## Skybox Loading (Skybox.cpp)

### Init Flow with panoramaExtension > 0

1. **Load all images** into `allFaceImages[NUM_SIDES]` arrays
2. **Expand each tile** using `CreateExpandedTileImage()`
3. **Upload textures** to GPU with:
   - `GL_CLAMP_TO_EDGE` wrapping
   - No mipmapping (would blur the borders)
4. **Store `mPanoramaExtension`** for use during rendering

### Rendering with Tiles

Each face may have multiple tiles arranged in a grid. During render:

```cpp
float imageScale = 1.0f / numPerSide;
esScalef(1.0f, imageScale, imageScale);  // Scale geometry to tile size

for (each tile) {
    float y = scale * (numPerSide - (i * 2 + 1.0f));  // Tile offset in Y
    float z = scale * (numPerSide - (j * 2 + 1.0f));  // Tile offset in Z
    esTranslatef(0.0f, y, z);
    // ... draw quad
}
```

The shader receives:
- `u_tilesPerSide` = numPerSide
- `u_tileOffset` = (y, z) translation values
- `u_tileEdgeFlags` = which edges of this tile are at face boundaries

## Parallax Shader (skyboxVRParallaxFragmentShaderStr)

### Uniforms

```glsl
uniform sampler2D u_skyboxTexture;
uniform sampler2D u_depthTexture;
uniform float u_eyeOffset;      // -1.0 for left eye, +1.0 for right eye
uniform float u_ipd;            // Interpupillary distance in world units
uniform float u_nearPlane, u_farPlane;
uniform vec2 u_screenSize;
uniform vec3 u_eyeRightLocal;   // Eye separation direction in face-local coords
uniform float u_tilesPerSide;      // numPerSide
uniform vec2 u_tileOffset;      // Tile translation (y, z)
uniform vec2 u_tanFovMin, u_tanFovMax;  // FOV tangents for depth correction
uniform float u_panoramaExtension;
uniform vec4 u_tileEdgeFlags;   // (isLeftEdge, isRightEdge, isTopEdge, isBottomEdge)
```

### Algorithm

#### Step 1: Sample Depth

```glsl
vec2 screenUV = gl_FragCoord.xy / u_screenSize;
float depthSample = texture2D(u_depthTexture, screenUV).r;

// Convert to linear depth, then correct for radial distance
float linearDepth = u_nearPlane * u_farPlane /
                    (u_farPlane - depthSample * (u_farPlane - u_nearPlane));
vec2 viewRay = mix(u_tanFovMin, u_tanFovMax, screenUV);
float cosAngle = 1.0 / sqrt(1.0 + dot(viewRay, viewRay));
linearDepth = linearDepth / cosAngle;  // Perpendicular -> radial distance
```

#### Step 2: Calculate Parallax Offset

For non-sky pixels (depthSample < 0.9999):

```glsl
// Reconstruct actual skybox position from tile geometry
vec3 skyboxPos;
skyboxPos.x = v_position.x;  // Forward direction (always 'scale')
skyboxPos.y = (v_position.y + u_tileOffset.x) / u_tilesPerSide;
skyboxPos.z = (v_position.z + u_tileOffset.y) / u_tilesPerSide;

// Parallax shift: larger for closer objects
float parallaxShift = (u_ipd * 0.5) * u_eyeOffset * (skyboxPos.x / linearDepth);

// UV Jacobian: how world-space movement maps to UV change
// UV = (0.5 - y/(2x), 0.5 - z/(2x)) for face-local coords
float dU = (skyboxPos.y * u_eyeRightLocal.x - skyboxPos.x * u_eyeRightLocal.y) / (2.0 * x2);
float dV = (skyboxPos.z * u_eyeRightLocal.x - skyboxPos.x * u_eyeRightLocal.z) / (2.0 * x2);

vec2 uvOffset = vec2(dU, dV) * parallaxShift * u_tilesPerSide;

// Distance correction: points away from face center are further
float correction = length(skyboxPos) / skyboxPos.x;
uvOffset *= correction;
```

#### Step 3: Sample with Offset

```glsl
vec2 rawUV = v_texCoord + uvOffset;
```

At this point, `rawUV` may be outside [0,1] for close objects with large parallax.

#### Step 4: Border Distortion Correction

When sampling in the border region (rawUV outside [0,1]) **at a cube face boundary** (indicated by u_tileEdgeFlags), we need to apply projective warping:

```glsl
bool inLeftBorder = rawUV.x < 0.0 && u_tileEdgeFlags.x > 0.5;
bool inRightBorder = rawUV.x > 1.0 && u_tileEdgeFlags.y > 0.5;
bool inTopBorder = rawUV.y < 0.0 && u_tileEdgeFlags.z > 0.5;
bool inBottomBorder = rawUV.y > 1.0 && u_tileEdgeFlags.w > 0.5;

if (inLeftBorder || inRightBorder || inTopBorder || inBottomBorder)
{
    // Convert UV back to direction ratios
    // UV mapping: U = 0.5 - y/(2x), V = 0.5 - z/(2x)
    // Inverse: y/x = 1 - 2*U, z/x = 1 - 2*V
    float y_over_x = 1.0 - 2.0 * rawUV.x;
    float z_over_x = 1.0 - 2.0 * rawUV.y;

    // Apply projective warp using dominant coordinate
    float maxCoord = max(abs(y_over_x), abs(z_over_x));
    if (maxCoord > 1.0)
    {
        float warpFactor = 1.0 / maxCoord;
        float y_warped = y_over_x * warpFactor;
        float z_warped = z_over_x * warpFactor;
        rawUV = vec2((1.0 - y_warped) * 0.5, (1.0 - z_warped) * 0.5);
    }
}
```

**Why this works**: The border pixels were copied linearly, but cube faces have different projections. When a ray direction would actually hit an adjacent face (y/x > 1 or z/x > 1), we need to "bend" the UV coordinates to sample from where the border pixels actually came from.

#### Step 5: Remap to Expanded Texture

Finally, map the [0,1] UV range to the center of the expanded texture:

```glsl
float borderFraction = u_panoramaExtension / (1.0 + 2.0 * u_panoramaExtension);
vec2 texCoord = borderFraction + rawUV * (1.0 - 2.0 * borderFraction);
gl_FragColor = texture2D(u_skyboxTexture, texCoord);
```

If original texture was 1024x1024 and panoramaExtension = 0.1:
- Expanded texture is 1228x1228 (1024 + 2*102)
- borderFraction = 0.1 / 1.2 = 0.0833
- UV 0.0 maps to texCoord 0.0833, UV 1.0 maps to texCoord 0.9167

## Coordinate Systems

### World Space
- X = East, Y = North, Z = Up
- The skybox rotates by `mOffset` degrees around Z

### Face-Local Coordinates
- Each cube face has its own coordinate system where `x` is forward (into the face)
- `y` and `z` align with the UV axes
- `CalculateFaceEyeRightLocal()` transforms the eye separation direction from world to face-local

### UV Space
- (0,0) = top-left of tile
- (1,1) = bottom-right of tile
- The UV mapping formula: `U = 0.5 - y/(2x)`, `V = 0.5 - z/(2x)`

## Key Files

| File | Purpose |
|------|---------|
| `source/Framework/TileExpansion.h` | Header with `CreateExpandedTileImage()` declaration |
| `source/Framework/TileExpansion.cpp` | Border expansion logic, cross-face adjacency tables |
| `source/Framework/Skybox.h` | Skybox class with `mPanoramaExtension` member |
| `source/Framework/Skybox.cpp` | Loading, expansion, and rendering |
| `source/Framework/Shaders.cpp` | `skyboxVRParallaxFragmentShaderStr` (line 672+) |
| `source/Framework/Shaders.h` | `SkyboxVRParallaxShader` class (line 150+) |

## Settings

The `panoramaExtension` parameter is passed to `Skybox::Init()`. Typical values:
- 0.0 = No expansion (VR parallax won't work at face edges)
- 0.1 = 10% border (recommended for VR)
- Higher values use more GPU memory but allow larger parallax offsets

## Debugging

Enable `DEBUG_SAVE_EXPANDED_TEXTURES` in `Skybox.cpp` to save expanded textures as PNG files:
```cpp
#define DEBUG_SAVE_EXPANDED_TEXTURES 1
```
Files are saved as `expanded_{face}{tile}.png` in the working directory.
