//
//  WebSocketClient.h
//  example_basic_client
//
//  Created by bacchus-house on 12/5/13.
//
//

#pragma once

#include "ofx/HTTP/Client/WebSocketsClientImp.h"
#include "ofx/HTTP/Client/EventHandler.h"
#include "ofx/HTTP/Client/EventRegistry.h"

namespace ofx {
    namespace HTTP {
        namespace Client {

            
            class WebSocketsClient
            {
            private:
                ~WebSocketsClient();
                
                WebSocketsClientImp *_socket;
                
                std::string _uri;
                std::string _endpoint;
                
//                NotificationCenter* _nCenter;
                
                EventRegistry *_registry;
                EventHandler *_sioHandler;
                
            public:
                WebSocketsClient(std::string uri, std::string endpoint, WebSocketsClientImp *impl);
                
                bool init();
                
                static WebSocketsClient* connect(std::string uri);
                void disconnect();
                void send(std::string s);
                void emit(std::string eventname, std::string args);
                std::string getUri();
 //               NotificationCenter* getNCenter();
                
                typedef void (EventTarget::*callback)(const void*, Object::Ptr&);
                
                void on(const char *name, SIOEventTarget *target, callback c);
                
                void fireEvent(const char * name, Object::Ptr args);
            };
        }
    }
}