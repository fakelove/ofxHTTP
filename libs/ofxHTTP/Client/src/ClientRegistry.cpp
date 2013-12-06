#include "ClientRegistry.h"
#include "WebSocketsClient.h"
namespace ofx {
    namespace HTTP {
        namespace Client{
            ClientRegistry *ClientRegistry::_inst = NULL;
            
            ClientRegistry *ClientRegistry::instance() {
                
                if(!_inst)
                    _inst = new ClientRegistry();
                
                return _inst;
                
            }
            
            WebSocketsClient *ClientRegistry::getClient(std::string uri) {
                
                WebSocketsClient *c = NULL;
                
                std::map<std::string,WebSocketsClient *>::iterator it = _clientMap.find(uri);
                if(it != _clientMap.end())
                {
                    
                    c = it->second;
                    
                }
                
                return c;
                
            }
            
            void ClientRegistry::addClient(WebSocketsClient *client) {
                
                _clientMap[client->getUri()] = client;
                
            }
            
            void ClientRegistry::removeClient(std::string uri) {
                _clientMap.erase(uri);
            }
            
            WebSocketsClientImp *ClientRegistry::getSocket(std::string uri) {
                
                WebSocketsClientImp *c = NULL;
                
                std::map<std::string,WebSocketsClientImp *>::iterator it = _socketMap.find(uri);
                if(it != _socketMap.end())
                {
                    
                    c = it->second;
                    
                }
                
                return c;
                
            }
            
            void ClientRegistry::addSocket(WebSocketsClientImp *socket, std::string uri) {
                
                _socketMap[uri] = socket;
                
            }
            
            void ClientRegistry::removeSocket(std::string uri) {
                _socketMap.erase(uri);
            }
        }
    }
}