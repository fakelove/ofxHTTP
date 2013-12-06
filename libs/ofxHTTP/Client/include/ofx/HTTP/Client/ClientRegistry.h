#pragma once

#include <map>
#include <string>

namespace ofx {
    namespace HTTP {
        namespace Client{
            class WebSocketsClient;
            class WebSocketsClientImp;
            
            class ClientRegistry
            {
            private:
                ClientRegistry() {};
                ~ClientRegistry() {delete _inst;};
                
                static ClientRegistry *_inst;
                
                std::map<std::string, WebSocketsClient *> _clientMap;
                std::map<std::string, WebSocketsClientImp *> _socketMap;
                
            public:
                static ClientRegistry *instance();
                
                WebSocketsClient *getClient(std::string uri);
                void addClient(WebSocketsClient *client);
                void removeClient(std::string uri);
                
                WebSocketsClientImp *getSocket(std::string uri);
                void addSocket(WebSocketsClientImp *socket, std::string uri);
                void removeSocket(std::string uri);
                
            };
        }
    }
}



