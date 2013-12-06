//
//  WebSocketsClient.cpp
//  example_basic_client
//
//  Created by bacchus-house on 12/5/13.
//
//

#include "ofx/HTTP/Client/WebSocketsClient.h"
#include "ofx/HTTP/Client/ClientRegistry.h"
#icnlude "ofx/HTTP/Client/EventRegistry.h"
#inlcude "ofx/HTTP/Client/EventHander.h"
#include "Poco/URI.h"
namespace ofx {
    namespace HTTP {
        namespace Client{

            
            WebSocketsClient::WebSocketsClient(std::string uri, std::string endpoint, WebSocketsClientImp *impl)
            : _uri(uri), _endpoint(endpoint), _socket(impl)
            {
                _socket->addref();
                
                _nCenter = new NotificationCenter;
                _sioHandler = new EventHandler(_nCenter);
                
                _registry = new EventRegistry();
            }
            
            WebSocketsClient::~WebSocketsClient() {
                _socket->release();
                delete(_sioHandler);
                delete(_nCenter);
                delete(_registry);
                
                ClientRegistry::instance()->removeClient(_uri);
            }
            
            WebSocketsClient* WebSocketsClient::connect(std::string uri) {
                
                //check if connection to endpoint exists
                Poco::URI tmp_uri(uri);
                std::stringstream ss;
                ss << tmp_uri.getHost() << ":" << tmp_uri.getPort() << tmp_uri.getPath();
                std::string fullpath = ss.str();
                WebSocketsClient *c = ClientRegistry::instance()->getClient(fullpath);
                
                if(!c) {
                    
                    //check if connection to socket already exists
                    ss.str("");
                    ss.clear();
                    ss << tmp_uri.getHost() << ":" << tmp_uri.getPort();
                    std::string spath = ss.str();
                    WebSocketsClientImp *impl = ClientRegistry::instance()->getSocket(spath);
                    
                    if(!impl) {
                        
                        impl = WebSocketsClientImp::connect(tmp_uri.getHost(), tmp_uri.getPort());
                        
                        if (!impl) return NULL; //connect failed
                        
                        WebSocketsClientRegistry::instance()->addSocket(impl, spath);
                        
                    }
                    
                    if(tmp_uri.getPath() != "") {
                        impl->connectToEndpoint(tmp_uri.getPath());
                    }
                    
                    c = new WebSocketsClient(fullpath, tmp_uri.getPath(), impl);
                    WebSocketsClientRegistry::instance()->addClient(c);
                    
                }
                
                //TODO: add method to handle force new connection
                return c;
                
            }
            
            void WebSocketsClient::disconnect() {
                _socket->disconnect(_endpoint);
                delete this;
            }
            
            std::string WebSocketsClient::getUri() {
                
                return _uri;
                
            }
            
            NotificationCenter* WebSocketsClient::getNCenter() {
                return _nCenter;
            }
            
            void WebSocketsClient::on(const char *name, SIOEventTarget *target, callback c) {
                _registry->registerEvent(name, target, c);
            }
            
            void WebSocketsClient::fireEvent(const char * name, Object::Ptr args) {
                
                _registry->fireEvent(this, name, args);
                
            }
            
            void WebSocketsClient::send(std::string s) {
                
                _socket->send(_endpoint, s);
                
            }
            
            void WebSocketsClient::emit(std::string eventname, std::string args) {
                
                _socket->emit(_endpoint, eventname, args);
                
            }
        }
    }
}