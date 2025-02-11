#ifndef SOCKET_MARMELADE_H
#define SOCKET_MARMELADE_H

#include "Trace.h"
#include "Entete.h"
//#include <iostream>
// #include <sys/socket.h>
// #include <netinet/in.h>
#include <netinet/tcp.h>  // Pour TCP_NODELAY
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <curl/curl.h>

enum s3eSocketMode {
    S3E_SOCKET_TCP = IPPROTO_TCP,        // Niveau pour les options TCP
    S3E_SOCKET_NODELAY = TCP_NODELAY,   // Désactiver le Nagle Algorithm
};


// Structure représentant une adresse IP
struct s3eInetAddress {
    sockaddr_in m_Port;

    s3eInetAddress() {
        std::memset(&m_Port, 0, sizeof(m_Port));
    }
};

// Fonction utilitaire pour convertir une adresse en chaîne de caractères
std::string s3eInetToString(const s3eInetAddress* address);

// Classe pour gérer un socket
class s3eSocket {
public:
    s3eSocket() : sock_fd(-1) {}
    
    ~s3eSocket() {
        if (sock_fd != -1) {
            close(sock_fd);
        }
    }

    // Création d'un socket
    int Create(int type, int protocol) {
        sock_fd = socket(AF_INET, type, protocol);
        if (sock_fd < 0) {
            TRACE("Error creating socket");
            return S3E_RESULT_ERROR;
        }
        return S3E_RESULT_SUCCESS;
    }

    // Fermeture du socket
    void Close() {
        if (sock_fd != -1) {
            close(sock_fd);
            sock_fd = -1;
        }
    }

    // Configuration des options du socket
    int SetOption(s3eSocketMode option, const void* value, socklen_t valueLen) {
        int level = IPPROTO_TCP; // Niveau pour les options comme NODELAY
        return setsockopt(sock_fd, level, static_cast<int>(option), value, valueLen);
    }

    // Association d'une adresse au socket
    int Bind(const s3eInetAddress* addr, bool reuseAddr) {
        if (reuseAddr) {
            int opt = 1;
            if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
                TRACE("Error setting SO_REUSEADDR");
                return S3E_RESULT_ERROR;
            }
        }

        if (bind(sock_fd, reinterpret_cast<const sockaddr*>(&addr->m_Port), sizeof(addr->m_Port)) < 0) {
            TRACE("Error binding socket");
            return S3E_RESULT_ERROR;
        }
        return S3E_RESULT_SUCCESS;
    }

    // Écoute des connexions entrantes
    int Listen(int backlog) {
        return listen(sock_fd, backlog);
    }

    // Acceptation des connexions entrantes
    int Accept(s3eInetAddress* incomingAddress) {
        sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        int newSock = accept(sock_fd, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);

        if (newSock < 0) {
            TRACE("Error accepting connection");
            return S3E_RESULT_ERROR;
        }

        if (incomingAddress) {
            incomingAddress->m_Port = clientAddr;
        }

        TRACE("Accepted connection from %s", s3eInetToString(incomingAddress).c_str());
        return newSock;
    }

    int GetSocketFD() const { return sock_fd; }

private:
    int sock_fd; // Descripteur de socket
};

// Fonction utilitaire pour la conversion de port
uint16_t s3eInetHtons(uint16_t port);

// Création d'un socket
s3eSocket* s3eSocketCreate(int type, int protocol);

// Fermeture du socket
void s3eSocketClose(s3eSocket* sock);

// Configuration des options du socket
int s3eSocketSetOpt(s3eSocket* sock, s3eSocketMode option, const void* value, socklen_t valueLen);

// Association d'une adresse au socket
int s3eSocketBind(s3eSocket* sock, const s3eInetAddress* addr, bool reuseAddr);

// Écoute des connexions entrantes
int s3eSocketListen(s3eSocket* sock, int backlog);

// Acceptation des connexions entrantes
s3eSocket* s3eSocketAccept(s3eSocket* sock, s3eInetAddress* incomingAddress, int option, void* context);

// Recherche d'une adresse IP
int s3eInetLookup(const char* hostname, s3eInetAddress* address, void* reserved1, void* reserved2);

// Envoie de données via le socket
int s3eSocketSend(s3eSocket* socket, std::string::iterator buffer, size_t length, int flags);

// Réception de données via le socket
int s3eSocketRecv(s3eSocket* socket, void* buffer, size_t length, int flags);

s3eSocketErrors s3eSocketGetError();


class CIwHTTP {
public:
    CIwHTTP() : curl(curl_easy_init()), status(CURLE_OK), contentLength(0) {}
    ~CIwHTTP() {
        if (curl) {
            curl_easy_cleanup(curl);
        }
    }

    int Get(const char* url, int32_t (*headersCallback)(void*, void*), void* userdata) {
        if (!curl) {
            TRACE("CURL initialization failed");
            return CURLE_FAILED_INIT;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headersCallback);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, userdata);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &contentBuffer);
        status = curl_easy_perform(curl);

        if (status != CURLE_OK) {
            TRACE("CURL request failed: %s", curl_easy_strerror(status));
            return S3E_RESULT_ERROR;
        }

        double cl;
        curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &cl);
        contentLength = (cl > 0) ? static_cast<uint32_t>(cl) : 0;

        TRACE("HTTP GET success, content length: %u", contentLength);
        return S3E_RESULT_SUCCESS;
    }

    uint32_t ContentLength() const { return contentLength; }
    uint32_t ContentReceived() const { return contentBuffer.size(); }
    uint32_t ContentExpected() const { return contentLength; }
    int GetStatus() const { return status == CURLE_OK ? S3E_RESULT_SUCCESS : S3E_RESULT_ERROR; }

    void ReadContent(char* buffer, size_t size, int32_t (*dataCallback)(void*, void*), void* userdata = NULL) {
        memcpy(buffer, contentBuffer.c_str(), size);
        dataCallback(nullptr, userdata);
    }

private:
    CURL* curl;
    CURLcode status;
    uint32_t contentLength;
    std::string contentBuffer;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* out) {
        size_t totalSize = size * nmemb;
        out->append(static_cast<char*>(contents), totalSize);
        return totalSize;
    }
};


#endif