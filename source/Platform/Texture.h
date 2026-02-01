#ifndef TEXTURE_H
#define TEXTURE_H

/**
  * Texture - OpenGL texture wrapper using stb_image
  *
  * This class replaces the Marmalade CIwTexture class.
  * It handles loading images via stb_image and creating OpenGL textures.
  */

#include "Platform.h"

// AI generated fix for macos build
#if defined(_WIN32)
    #include <glad/glad.h>
#elif defined(PICASIM_MACOS) || defined(__APPLE__)
    #include <OpenGL/gl.h>
#else
    #include "GLCompat.h"
#endif

#include <string>

// Forward declarations for image format enum
namespace TextureFormat
{
        enum Type
        {
                Unknown = 0,
                RGB_888,
                RGBA_8888,
                BGR_888,
                BGRA_8888,
                RGB_565,
                RGBA_4444,
                Luminance_8,
                LuminanceAlpha_88
        };
}

/**
  * Simple image class for loading and manipulating image data
  * Replaces CIwImage from Marmalade
  */
class Image
{
public:
        Image();
        ~Image();

        /**
          * Load image from file using stb_image
          * @param filename Path to image file (PNG, JPG, BMP, TGA, etc.)
          * @return true on success
          */
        bool LoadFromFile(const char* filename);

        /**
          * Set image dimensions and format (for manual buffer management)
          */
        void SetFormat(TextureFormat::Type format) { mFormat = format; }
        void SetWidth(int width) { mWidth = width; }
        void SetHeight(int height) { mHeight = height; }

        /**
          * Allocate internal buffer or set external buffer
          */
        void SetBuffers(unsigned char* data, int dataSize);

        /**
          * Get image properties
          */
        int GetWidth() const { return mWidth; }
        int GetHeight() const { return mHeight; }
        int GetChannels() const { return mChannels; }
        TextureFormat::Type GetFormat() const { return mFormat; }
        unsigned char* GetTexels() const { return mData; }
        int GetDataSize() const { return mDataSize; }

        /**
          * Convert/resize to another image
          */
        void ConvertToImage(Image* dest) const;

        /**
          * Save as PNG file
          */
        bool SavePng(const char* filename) const;

        /**
          * Free resources
          */
        void Release();

private:
        unsigned char* mData;
        int mWidth;
        int mHeight;
        int mChannels;
        int mDataSize;
        TextureFormat::Type mFormat;
        bool mOwnsData;  // Whether we should free mData on destruction
};

/**
  * OpenGL Texture wrapper
  * Replaces CIwTexture from Marmalade
  */
class Texture
{
public:
        // Texture flags for compatibility with CIwTexture
        enum Flags
        {
                UPLOADED_F = 0x0001,
                MIPMAPPED_F = 0x0002,
                CLAMPED_F = 0x0004
        };

        Texture();
        ~Texture();

        /**
          * Create texture from image data
          * @param image Pointer to loaded image
          */
        void CopyFromImage(const Image* image);

        /**
          * Create texture directly from file
          * @param filename Path to image file
          * @return true on success
          */
        bool LoadFromFile(const char* filename);

        /**
          * Set image data (for compatibility with CIwTexture::SetImage)
          */
        void SetImage(const Image* image);

        /**
          * Upload texture data to GPU
          */
        void Upload();

        /**
          * Bind texture for rendering
          * @param unit Texture unit (default 0)
          */
        void Bind(int unit = 0) const;

        /**
          * Unbind texture
          */
        void Unbind() const;

        /**
          * Get OpenGL texture ID
          */
        GLuint GetTextureID() const { return mTextureID; }

        /**
          * Get texture dimensions
          */
        int GetWidth() const { return mWidth; }
        int GetHeight() const { return mHeight; }

        /**
          * Set texture filtering
          */
        void SetFiltering(bool linear);

        /**
          * Set texture wrapping
          */
        void SetWrap(bool repeat);

        /**
          * Generate mipmaps
          */
        void GenerateMipmaps();

        /**
          * Release GPU resources
          */
        void Release();

        // Compatibility methods for CIwTexture API
        /**
          * Set mip mapping (compatibility)
          */
        void SetMipMapping(bool enable);

        /**
          * Set clamping/wrapping (compatibility)
          */
        void SetClamping(bool clamp);

        /**
          * Set modifiable flag (compatibility - allows texture data to be modified after upload)
          */
        void SetModifiable(bool modifiable) { mModifiable = modifiable; }

        /**
          * Set hardware format hint (compatibility - ignored)
          */
        void SetFormatHW(TextureFormat::Type format) { mFormatHW = format; }

        /**
          * Get texture flags
          */
        uint32_t GetFlags() const { return mFlags; }

        /**
          * Copy texture data from buffer (Marmalade CIwTexture compatibility)
          * @param width Texture width
          * @param height Texture height
          * @param format Pixel format
          * @param pitch Bytes per row (usually width * channels)
          * @param data Pointer to pixel data
          * @param flags Flags (ignored)
          */
        void CopyFromBuffer(int width, int height, TextureFormat::Type format, int pitch, const unsigned char* data, int flags = 0);

        // Public member for compatibility with old code that accesses mHWID directly
        GLuint mHWID;

private:
        GLuint mTextureID;
        int mWidth;
        int mHeight;
        bool mHasMipmaps;
        bool mUploaded;
        bool mModifiable;
        uint32_t mFlags;
        TextureFormat::Type mFormatHW;

        // Cached image data for deferred upload
        unsigned char* mCachedData;
        int mCachedDataSize;
        int mCachedChannels;
};

// Compatibility typedef for old code that uses CIwTexture
typedef Texture CIwTexture;

// Compatibility for CIwImage::Format enum
// Use Image:: namespace instead of CIwImage:: for format access
namespace CIwImageFormat
{
        const TextureFormat::Type RGB_888 = TextureFormat::RGB_888;
        const TextureFormat::Type RGBA_8888 = TextureFormat::RGBA_8888;
        const TextureFormat::Type ABGR_8888 = TextureFormat::BGRA_8888;  // Note: mapped for compatibility
        const TextureFormat::Type BGR_888 = TextureFormat::BGR_888;
        const TextureFormat::Type RGBA_4444 = TextureFormat::RGBA_4444;
        const TextureFormat::Type RGB_565 = TextureFormat::RGB_565;
}

// CIwImage is now an alias for the Image class
// For format enums like CIwImage::RGBA_4444, use CIwImageFormat::RGBA_4444 instead
// Or simply use the Image class directly
class CIwImage : public Image
{
public:
        // Inherit constructors
        using Image::Image;

        // Format constants accessible via CIwImage::RGBA_4444 etc
        static constexpr TextureFormat::Type RGBA_4444 = TextureFormat::RGBA_4444;
        static constexpr TextureFormat::Type RGB_565 = TextureFormat::RGB_565;
        static constexpr TextureFormat::Type RGBA_8888 = TextureFormat::RGBA_8888;
        static constexpr TextureFormat::Type RGB_888 = TextureFormat::RGB_888;
};

#endif // TEXTURE_H
