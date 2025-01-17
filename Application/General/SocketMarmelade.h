#ifndef SOCKET_MARMELADE_H
#define SOCKET_MARMELADE_H

#include "Trace.h"
#include "Entete.h"
//#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
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
inline std::string s3eInetToString(const s3eInetAddress* address) {
    char buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(address->m_Port.sin_addr), buffer, sizeof(buffer));
    return std::string(buffer);
}

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
inline uint16_t s3eInetHtons(uint16_t port) {
    return htons(port);
}

// Création d'un socket
inline s3eSocket* s3eSocketCreate(int type, int protocol) {
    s3eSocket* sock = new s3eSocket();
    if (sock->Create(type, protocol) != S3E_RESULT_SUCCESS) {
        TRACE("Error creating socket");
        delete sock;
        return nullptr;
    }
    return sock;
}

// Fermeture du socket
inline void s3eSocketClose(s3eSocket* sock) {
    if (sock) {
        sock->Close();
        delete sock;
    }
}

// Configuration des options du socket
inline int s3eSocketSetOpt(s3eSocket* sock, s3eSocketMode option, const void* value, socklen_t valueLen) {
    if (!sock) {
        TRACE("Socket is null");
        return S3E_RESULT_ERROR;
    }
    return sock->SetOption(option, value, valueLen);
}

// Association d'une adresse au socket
inline int s3eSocketBind(s3eSocket* sock, const s3eInetAddress* addr, bool reuseAddr) {
    if (!sock) {
        TRACE("Socket is null");
        return S3E_RESULT_ERROR;
    }
    return sock->Bind(addr, reuseAddr);
}

// Écoute des connexions entrantes
inline int s3eSocketListen(s3eSocket* sock, int backlog) {
    if (!sock) {
        TRACE("Socket is null");
        return S3E_RESULT_ERROR;
    }
    return sock->Listen(backlog);
}

// Acceptation des connexions entrantes
inline s3eSocket* s3eSocketAccept(s3eSocket* sock, s3eInetAddress* incomingAddress, int option, void* context) {
    if (!sock) {
        TRACE("Socket is null");
        return nullptr;
    }

    // Acceptation de la connexion
    int newSock = sock->Accept(incomingAddress);
    if (newSock < 0) {
        TRACE("Error accepting connection");
        return nullptr;
    }

    // Traitement des options supplémentaires (par exemple, configuration du socket)
    if (option == 0) {
        // Par exemple, vous pouvez configurer des options spécifiques sur le socket
        // SetOption(option, context) pourrait être une fonctionnalité spécifique
        TRACE("Option 0 selected, no special settings");
    }
    else if (option == 1) {
        // Si vous souhaitez traiter une autre option, vous pouvez ajouter ici
        TRACE("Option 1 selected, custom handling");
    }

    // Création d'un nouveau socket pour la connexion acceptée
    s3eSocket* newSocket = new s3eSocket();
    if (newSocket->Create(SOCK_STREAM, 0) != S3E_RESULT_SUCCESS) {
        TRACE("Error creating new socket");
        delete newSocket;
        return nullptr;
    }

    // Vous pouvez utiliser le `context` ici si nécessaire (par exemple, pour une gestion spécifique)
    if (context != nullptr) {
        TRACE("Context is provided, performing custom actions");
        // Traitez le contexte si besoin
    }

    TRACE("Accepted connection from %s", s3eInetToString(incomingAddress).c_str());
    return newSocket;
}

// Recherche d'une adresse IP
inline int s3eInetLookup(const char* hostname, s3eInetAddress* address, void* reserved1, void* reserved2) {
    address->m_Port.sin_family = AF_INET;
    if (inet_pton(AF_INET, hostname, &address->m_Port.sin_addr) <= 0) {
        TRACE("Failed to resolve IP address");
        return -1;  // Erreur
    }
    return 0;  // Succès
}

// Envoie de données via le socket
inline int s3eSocketSend(s3eSocket* socket, std::string::iterator buffer, size_t length, int flags) {
    if (!socket || socket->GetSocketFD() == -1)
        return -1;

    // Utilisation de la fonction send de POSIX
    return send(socket->GetSocketFD(), &(*buffer), length, flags);
}

// Réception de données via le socket
inline int s3eSocketRecv(s3eSocket* socket, void* buffer, size_t length, int flags) {
    if (!socket || socket->GetSocketFD() == -1)
        return -1;

    // Utilisation de la fonction recv de POSIX
    return recv(socket->GetSocketFD(), buffer, length, flags);
}

inline s3eSocketErrors s3eSocketGetError() {
    // Retourner l'erreur selon le type de socket (en utilisant errno de POSIX)
    switch (errno) {
        //case EAGAIN: 
        case EWOULDBLOCK: 
            return S3E_SOCKET_ERR_WOULDBLOCK;    // EAGAIN et EWOULDBLOCK peuvent être traitées ensemble
        case ECONNRESET:
            return S3E_SOCKET_ERR_CONNRESET;     // Connexion réinitialisée
        case EINPROGRESS:
            return S3E_SOCKET_ERR_INPROGRESS;    // Opération en cours
        default:
            return S3E_SOCKET_ERR_UNKNOWN;       // Erreur inconnue
    }
}


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