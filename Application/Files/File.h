#ifndef FILE_H
#define FILE_H

#include "Entete.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

typedef FILE s3eFile;


// Variable pour suivre la dernière erreur
static thread_local s3eFileError lastError = S3E_FILE_SUCCESS;

s3eFile* s3eFileOpen(const char* filename, const char* mode);

void s3eFileClose(s3eFile* fileStream);

int32_t s3eFileGetSize(s3eFile* fileStream);

int32_t s3eFileRead(void* buffer, int32_t fileSize, int count, s3eFile* fileStream);

s3eFileError s3eFileGetError();


class s3eFileList {
public:
    s3eFileList(const std::string& directory) {
        // Simulation : énumérer les fichiers dans un répertoire
        files = {"file1.jpg", "file2.png", "document.txt"};
    }

    ~s3eFileList() {}

    s3eResult Next(char* filename, int filenameLen) {
        if (currentFileIndex >= files.size()) {
            return S3E_RESULT_ERROR;
        }
        strncpy(filename, files[currentFileIndex].c_str(), filenameLen);
        currentFileIndex++;
        return S3E_RESULT_SUCCESS;
    }

    void Close() {
        // Simulation : libérer les ressources
    }

private:
    std::vector<std::string> files;
    size_t currentFileIndex = 0;
};


// Fonction s3eFileListDirectory
s3eFileList* s3eFileListDirectory(const char* path);

// Fonction s3eFileListNext
s3eResult s3eFileListNext(s3eFileList* fileList, char* filename, int filenameLen) ;

// Fonction s3eFileListClose
void s3eFileListClose(s3eFileList* fileList);

void s3eFileDelete (const char* chaine);


#endif