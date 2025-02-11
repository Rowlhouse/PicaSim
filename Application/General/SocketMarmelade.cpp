#include "SocketMarmelade.h"


std::string s3eInetToString(const s3eInetAddress* address) {
    char buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(address->m_Port.sin_addr), buffer, sizeof(buffer));
    return std::string(buffer);
}

uint16_t s3eInetHtons(uint16_t port) {
    return htons(port);
}

// Création d'un socket
s3eSocket* s3eSocketCreate(int type, int protocol) {
    s3eSocket* sock = new s3eSocket();
    if (sock->Create(type, protocol) != S3E_RESULT_SUCCESS) {
        TRACE("Error creating socket");
        delete sock;
        return nullptr;
    }
    return sock;
}

// Fermeture du socket
void s3eSocketClose(s3eSocket* sock) {
    if (sock) {
        sock->Close();
        delete sock;
    }
}

// Configuration des options du socket
int s3eSocketSetOpt(s3eSocket* sock, s3eSocketMode option, const void* value, socklen_t valueLen) {
    if (!sock) {
        TRACE("Socket is null");
        return S3E_RESULT_ERROR;
    }
    return sock->SetOption(option, value, valueLen);
}

// Association d'une adresse au socket
int s3eSocketBind(s3eSocket* sock, const s3eInetAddress* addr, bool reuseAddr) {
    if (!sock) {
        TRACE("Socket is null");
        return S3E_RESULT_ERROR;
    }
    return sock->Bind(addr, reuseAddr);
}

// Écoute des connexions entrantes
int s3eSocketListen(s3eSocket* sock, int backlog) {
    if (!sock) {
        TRACE("Socket is null");
        return S3E_RESULT_ERROR;
    }
    return sock->Listen(backlog);
}

// Acceptation des connexions entrantes
s3eSocket* s3eSocketAccept(s3eSocket* sock, s3eInetAddress* incomingAddress, int option, void* context) {
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
int s3eInetLookup(const char* hostname, s3eInetAddress* address, void* reserved1, void* reserved2) {
    address->m_Port.sin_family = AF_INET;
    if (inet_pton(AF_INET, hostname, &address->m_Port.sin_addr) <= 0) {
        TRACE("Failed to resolve IP address");
        return -1;  // Erreur
    }
    return 0;  // Succès
}

// Envoie de données via le socket
int s3eSocketSend(s3eSocket* socket, std::string::iterator buffer, size_t length, int flags) {
    if (!socket || socket->GetSocketFD() == -1)
        return -1;

    // Utilisation de la fonction send de POSIX
    return send(socket->GetSocketFD(), &(*buffer), length, flags);
}

// Réception de données via le socket
int s3eSocketRecv(s3eSocket* socket, void* buffer, size_t length, int flags) {
    if (!socket || socket->GetSocketFD() == -1)
        return -1;

    // Utilisation de la fonction recv de POSIX
    return recv(socket->GetSocketFD(), buffer, length, flags);
}

s3eSocketErrors s3eSocketGetError() {
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