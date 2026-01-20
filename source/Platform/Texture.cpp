#include "Texture.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// stb_image implementation
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_THREAD_LOCALS
#include <stb_image.h>

// stb_image_write for PNG saving
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

// stb_image_resize for image resizing
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize2.h>

//==============================================================================
// Image implementation
//==============================================================================

Image::Image()
    : mData(nullptr)
    , mWidth(0)
    , mHeight(0)
    , mChannels(0)
    , mDataSize(0)
    , mFormat(TextureFormat::Unknown)
    , mOwnsData(false)
{
}

Image::~Image()
{
    Release();
}

bool Image::LoadFromFile(const char* filename)
{
    Release();

    // Load image with stb_image
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);

    if (!data)
    {
        fprintf(stderr, "Failed to load image: %s - %s\n", filename, stbi_failure_reason());
        return false;
    }

    mData = data;
    mWidth = width;
    mHeight = height;
    mChannels = channels;
    mDataSize = width * height * channels;
    mOwnsData = true;

    // Set format based on channels
    switch (channels)
    {
    case 1:
        mFormat = TextureFormat::Luminance_8;
        break;
    case 2:
        mFormat = TextureFormat::LuminanceAlpha_88;
        break;
    case 3:
        mFormat = TextureFormat::RGB_888;
        break;
    case 4:
        mFormat = TextureFormat::RGBA_8888;
        break;
    default:
        mFormat = TextureFormat::Unknown;
        break;
    }

    return true;
}

void Image::SetBuffers(unsigned char* data, int dataSize)
{
    // Don't free existing data if we owned it - caller is taking over
    if (mOwnsData && mData && mData != data)
    {
        stbi_image_free(mData);
    }

    mData = data;
    mDataSize = dataSize;
    mOwnsData = false;  // Caller owns the buffer
}

void Image::ConvertToImage(Image* dest) const
{
    if (!mData || !dest || dest->mWidth <= 0 || dest->mHeight <= 0)
        return;

    int destChannels = mChannels;
    int destDataSize = dest->mWidth * dest->mHeight * destChannels;

    // Allocate destination buffer
    unsigned char* destData = (unsigned char*)malloc(destDataSize);
    if (!destData)
        return;

    // Use stb_image_resize for high-quality resizing
    stbir_resize_uint8_linear(
        mData, mWidth, mHeight, mWidth * mChannels,
        destData, dest->mWidth, dest->mHeight, dest->mWidth * destChannels,
        (stbir_pixel_layout)mChannels
    );

    dest->mData = destData;
    dest->mChannels = destChannels;
    dest->mDataSize = destDataSize;
    dest->mFormat = mFormat;
    dest->mOwnsData = true;
}

bool Image::SavePng(const char* filename) const
{
    if (!mData || mWidth <= 0 || mHeight <= 0)
        return false;

    int result = stbi_write_png(filename, mWidth, mHeight, mChannels, mData, mWidth * mChannels);
    return result != 0;
}

void Image::Release()
{
    if (mOwnsData && mData)
    {
        stbi_image_free(mData);
    }
    mData = nullptr;
    mWidth = 0;
    mHeight = 0;
    mChannels = 0;
    mDataSize = 0;
    mFormat = TextureFormat::Unknown;
    mOwnsData = false;
}

//==============================================================================
// Texture implementation
//==============================================================================

Texture::Texture()
    : mTextureID(0)
    , mWidth(0)
    , mHeight(0)
    , mHasMipmaps(false)
    , mUploaded(false)
    , mModifiable(false)
    , mFlags(0)
    , mFormatHW(TextureFormat::RGBA_8888)
    , mCachedData(nullptr)
    , mCachedDataSize(0)
    , mCachedChannels(0)
    , mHWID(0)
{
}

Texture::~Texture()
{
    Release();
}

void Texture::CopyFromImage(const Image* image)
{
    if (!image || !image->GetTexels())
        return;

    // Store image data for deferred upload
    mWidth = image->GetWidth();
    mHeight = image->GetHeight();
    mCachedChannels = image->GetChannels();
    mCachedDataSize = image->GetDataSize();

    // Free any existing cached data
    if (mCachedData)
    {
        free(mCachedData);
    }

    // Copy the image data
    mCachedData = (unsigned char*)malloc(mCachedDataSize);
    if (mCachedData)
    {
        memcpy(mCachedData, image->GetTexels(), mCachedDataSize);
    }

    mUploaded = false;

    // Auto-upload
    Upload();
}

bool Texture::LoadFromFile(const char* filename)
{
    Image image;
    if (!image.LoadFromFile(filename))
        return false;

    CopyFromImage(&image);
    return true;
}

void Texture::SetImage(const Image* image)
{
    CopyFromImage(image);
}

void Texture::Upload()
{
    if (mUploaded || !mCachedData)
        return;

    // Generate texture ID if needed
    if (mTextureID == 0)
    {
        glGenTextures(1, &mTextureID);
    }

    glBindTexture(GL_TEXTURE_2D, mTextureID);

    // Determine GL format based on channels
    GLenum format = GL_RGBA;
    GLenum internalFormat = GL_RGBA;
    switch (mCachedChannels)
    {
    case 1:
        format = GL_LUMINANCE;
        internalFormat = GL_LUMINANCE;
        break;
    case 2:
        format = GL_LUMINANCE_ALPHA;
        internalFormat = GL_LUMINANCE_ALPHA;
        break;
    case 3:
        format = GL_RGB;
        internalFormat = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        internalFormat = GL_RGBA;
        break;
    }

    // Set pixel alignment
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, mWidth, mHeight, 0, format, GL_UNSIGNED_BYTE, mCachedData);

    // Set default filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set default wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    mUploaded = true;
    mFlags |= UPLOADED_F;
    mHWID = mTextureID;  // Sync for compatibility

    // Free cached data after upload
    free(mCachedData);
    mCachedData = nullptr;
    mCachedDataSize = 0;
}

void Texture::Bind(int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetFiltering(bool linear)
{
    if (mTextureID == 0)
        return;

    glBindTexture(GL_TEXTURE_2D, mTextureID);

    if (linear)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mHasMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mHasMipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetWrap(bool repeat)
{
    if (mTextureID == 0)
        return;

    glBindTexture(GL_TEXTURE_2D, mTextureID);

    GLenum mode = repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::GenerateMipmaps()
{
    if (mTextureID == 0)
        return;

    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    mHasMipmaps = true;
}

void Texture::Release()
{
    if (mTextureID != 0)
    {
        glDeleteTextures(1, &mTextureID);
        mTextureID = 0;
        mHWID = 0;
    }

    if (mCachedData)
    {
        free(mCachedData);
        mCachedData = nullptr;
    }

    mWidth = 0;
    mHeight = 0;
    mHasMipmaps = false;
    mUploaded = false;
    mFlags = 0;
    mCachedDataSize = 0;
    mCachedChannels = 0;
}

void Texture::SetMipMapping(bool enable)
{
    if (enable && mUploaded && !mHasMipmaps)
    {
        GenerateMipmaps();
    }
    if (enable)
    {
        mFlags |= MIPMAPPED_F;
    }
    else
    {
        mFlags &= ~MIPMAPPED_F;
    }
}

void Texture::SetClamping(bool clamp)
{
    SetWrap(!clamp);
    if (clamp)
    {
        mFlags |= CLAMPED_F;
    }
    else
    {
        mFlags &= ~CLAMPED_F;
    }
}

void Texture::CopyFromBuffer(int width, int height, TextureFormat::Type format, int pitch, const unsigned char* data, int flags)
{
    // Release existing data
    if (mCachedData)
    {
        free(mCachedData);
        mCachedData = nullptr;
    }

    mWidth = width;
    mHeight = height;
    mUploaded = false;
    mFlags &= ~UPLOADED_F;

    // Determine number of channels from format
    int channels = 4;
    switch (format)
    {
    case TextureFormat::RGB_888:
    case TextureFormat::BGR_888:
        channels = 3;
        break;
    case TextureFormat::RGBA_8888:
    case TextureFormat::BGRA_8888:
    case TextureFormat::RGBA_4444:
        channels = 4;
        break;
    case TextureFormat::Luminance_8:
        channels = 1;
        break;
    case TextureFormat::LuminanceAlpha_88:
        channels = 2;
        break;
    case TextureFormat::RGB_565:
        channels = 3;
        break;
    default:
        channels = 4;
        break;
    }

    mCachedChannels = channels;

    // Calculate expected size and allocate
    int dataSize = width * height * channels;
    mCachedData = (unsigned char*)malloc(dataSize);
    mCachedDataSize = dataSize;

    if (!mCachedData)
        return;

    // Copy row by row to handle pitch differences
    int expectedRowSize = width * channels;
    for (int y = 0; y < height; y++)
    {
        memcpy(mCachedData + y * expectedRowSize, data + y * pitch, expectedRowSize);
    }

    mFormatHW = format;
}
