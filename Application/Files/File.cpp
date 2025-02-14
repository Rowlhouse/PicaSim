#include "File.h"
#include <fstream>
#include <sys/stat.h>


s3eFile* s3eFileOpen(const char* filename, const char* mode)
{
    
    if (!filename || !mode) {
        lastError = S3E_FILE_ERROR_NULL_POINTER;
        return nullptr;
    }
    s3eFile* fileStream = fopen(filename, mode);
    if (!fileStream) { 
        lastError = S3E_FILE_ERROR_OPEN_FAILED;
        return nullptr;
    }

    lastError = S3E_FILE_SUCCESS;
    return fileStream;
}


void s3eFileClose(s3eFile* fileStream)
{
    if (fileStream) {
        fclose(fileStream);
        fileStream = nullptr;
    }
    else
    {
        lastError = S3E_FILE_ERROR_NULL_POINTER;
    }

    lastError = S3E_FILE_SUCCESS;
    //fileSize = 0;
}


int32_t s3eFileGetSize(s3eFile* fileStream)
{
    if (!fileStream) {
        lastError = S3E_FILE_ERROR_NOT_OPEN;
        return -1;
    }

    fseek(fileStream, 0, SEEK_END);
    int32_t fileSize = ftell(fileStream);
    fseek(fileStream, 0, SEEK_SET);

    lastError = S3E_FILE_SUCCESS;
    return fileSize;
}


int32_t s3eFileRead(void* buffer, int32_t fileSize, int count, s3eFile* fileStream)
{
    if (!fileStream) {
        lastError = S3E_FILE_ERROR_NULL_POINTER;
        return -1;
    }
    int32_t bytesRead = fread(buffer, fileSize, count, fileStream);
    if (bytesRead == 0 && ferror(fileStream)) {
        lastError = S3E_FILE_ERROR_NOT_OPEN;
        return -1;
    }

    lastError = S3E_FILE_SUCCESS;
    return bytesRead;
}


s3eFileError s3eFileGetError() 
{
    return lastError;
}


s3eFileList* s3eFileListDirectory(const char* path)
{
    // Créer un nouvel objet s3eFileList avec le répertoire passé
    return new s3eFileList(path);
}

s3eResult s3eFileListNext(s3eFileList* fileList, char* filename, int filenameLen) 
{
    if (fileList == nullptr) {
        return S3E_RESULT_ERROR;
    }
    return fileList->Next(filename, filenameLen);
}

void s3eFileListClose(s3eFileList* fileList) 
{
    if (fileList != nullptr) {
        fileList->Close();
        delete fileList;  // Libérer la mémoire allouée pour l'objet fileList
    }
}

bool s3eFileCheckExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}


void s3eFileDelete (const char* chaine){
    ////////////////////////////////////////////////////////////////////////////////////////////Compléter
}



// Fonction pour créer un répertoire de manière portable
int s3eFileMakeDirectory(const char *path) {
    // Sur Windows, utiliser CreateDirectory
    #ifdef _WIN32
        if (CreateDirectory(path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
            return 1; // Répertoire créé ou existe déjà
        }
        return 0; // Erreur lors de la création du répertoire
    #else
        // Sur Unix-like (Linux/macOS), utiliser mkdir
        if (mkdir(path, 0777) == 0 || errno == EEXIST) {
            return 1; // Répertoire créé ou existe déjà
        }
        return 0; // Erreur lors de la création
    #endif
}