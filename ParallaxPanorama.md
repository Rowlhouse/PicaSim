# Parallax Panorama Skybox System

This document describes the VR parallax panoramic skybox system in PicaSim. The goal is to render a cubemap skybox with depth-based parallax for stereoscopic VR, supporting tiled panoramas with seamless transitions across cube face boundaries.

## The Problem

A standard cubemap skybox has no depth, so both eyes see the same image and the world looks flat in VR. To create depth, we shift each eye's view based on the actual scene depth: nearby objects get a larger shift (parallax), distant objects get less.

This works straightforwardly in the interior of a cube face, but at face boundaries the parallax offset can push the sample point off the edge of one face and onto an adjacent face. The two faces have different projections, so a naive sample would produce a visible seam.

## Solution: Pre-expanded Tiles + Shader Correction

The solution has two parts:

1. **CPU-side (TileExpansion)**: Before uploading textures, each tile is expanded with a border of pixels copied from its neighbours (including across cube faces). The copy is a simple linear pixel transfer with no geometric correction.

2. **GPU-side (Shader)**: When the parallax offset lands in a border region at a cube face boundary, the shader applies a projective warp to account for the geometric distortion between faces. This corrects the fact that the border pixels were copied linearly from a face with a different projection.

## Components

### Source Images

Skybox images are organized by detail level and face under `data/SystemData/Panoramas/{name}/{detail}/`. Each face can be split into a grid of tiles (e.g. 2x2), numbered row-by-row: tile 1 = top-left, 2 = top-right, 3 = bottom-left, 4 = bottom-right.

### Tile Expansion (`source/Framework/TileExpansion.cpp`)

`CreateExpandedTileImage()` takes each tile and creates a new image padded with border pixels. The border size is controlled by `panoramaExtension` (e.g. 0.1 = 10% of tile width).

For interior edges (between tiles on the same face), the border pixels come from the adjacent tile. For boundary edges (at the edge of a cube face), they come from the corresponding edge of the adjacent face. The `GetAdjacentFace()` function encodes the full cube topology — which face connects to which, and whether the coordinates need flipping to match orientation.

Corner pixels are filled from diagonal neighbours where possible, or clamped to the nearest edge pixel.

### Skybox Rendering (`source/Framework/Skybox.cpp`)

During init (when `panoramaExtension > 0`), all face images are loaded into memory first (so neighbours are available), then each tile is expanded, uploaded to the GPU with `GL_CLAMP_TO_EDGE` wrapping (no mipmaps — they would blur the borders), and the originals freed.

`RenderVRParallax()` renders each cube face as a separate draw call, passing the depth texture from the main scene render. For tiled faces, each tile is rendered individually with uniforms that describe its position in the grid (`u_tileOffset`, `u_tilesPerSide`) and which of its edges are at face boundaries (`u_tileEdgeFlags`).

The eye separation direction is transformed from world space into each face's local coordinate system (`u_eyeRightLocal`) so the shader can compute UV offsets correctly per face.

### Parallax Shader (`source/Framework/Shaders.cpp`)

The fragment shader (`skyboxVRParallaxFragmentShaderStr`) works in five steps:

1. **Sample depth** at the current screen position. Convert the non-linear depth buffer value to a linear radial distance, correcting for the view ray angle (perpendicular depth from the frustum is not the same as radial distance from the eye).

2. **Calculate the parallax UV offset**. Reconstruct the skybox world position from the tile geometry (reversing the scale and translation applied during rendering). The parallax shift is proportional to `IPD / distance` — closer objects shift more. A UV Jacobian converts the world-space eye offset into a UV-space offset, accounting for the cube face projection.

3. **Apply the offset** to the tile's texture coordinates. The resulting UV may be outside [0,1] for nearby objects with large parallax — this is where the border pixels are needed.

4. **Projective warp for face boundaries**. When the offset UV is outside [0,1] *and* the tile edge is a face boundary (indicated by `u_tileEdgeFlags`), the shader converts UV back to direction ratios (`y/x` and `z/x`), checks if the ray has crossed onto an adjacent face (ratio exceeds 1.0), and applies a projective warp (`1/maxRatio`) to map back into the border region where the pixels actually live. This corrects the linear-vs-projective mismatch from the CPU-side border copy.

5. **Remap to expanded texture coordinates**. The shader maps [0,1] UV onto the centre portion of the expanded texture, leaving the border region accessible for parallax samples that go beyond the original tile edge.

## Coordinate Systems

- **World space**: X = East, Y = North, Z = Up. The skybox rotates by `mOffset` degrees around Z.
- **Face-local space**: Each cube face has `x` pointing inward (perpendicular to the face), with `y` and `z` aligned to the UV axes.
- **UV space**: (0,0) = top-left, (1,1) = bottom-right. Mapping: `U = 0.5 - y/(2x)`, `V = 0.5 - z/(2x)`.

## Settings

`panoramaExtension` is passed to `Skybox::Init()`:
- 0.0 = No expansion (VR parallax won't work at face edges)
- 0.1 = 10% border (sufficient for typical VR scenes)
- Higher values use more GPU memory but allow larger parallax offsets

## Debugging

Set `DEBUG_SAVE_EXPANDED_TEXTURES` to 1 in `Skybox.cpp` to save expanded textures as PNG files (`expanded_{face}{tile}.png` in the working directory).
