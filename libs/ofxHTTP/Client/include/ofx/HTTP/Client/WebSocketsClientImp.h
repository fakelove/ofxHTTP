#pragma once

#include <string>

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Logger.h"
#include "Poco/Timer.h"
#include "Poco/NotificationCenter.h"
#include "Poco/Thread.h"
#include "Poco/ThreadTarget.h"
#include "Poco/RunnableAdapter.h"

#include "ofx/HTTP/Client/EventHandler.h"
#include "ofx/HTTP/Client/EventRegistry.h"
#include "ofx/HTTP/Client/EventTarget.h"


namespace ofx {
    namespace HTTP{
        namespace Client {
            
            
            class WebSocketsClientImp: public Poco::Runnable
            {
            private:
                WebSocketsClientImp();
                WebSocketsClientImp(std::string host, int port);
                ~WebSocketsClientImp(void);
                
                std::string _sid;
                int _heartbeat_timeout;
                int _timeout;
                std::string _host;
                int _port;
                std::string _uri;
                bool _connected;
                
                
                Poco::Net::HTTPClientSession *_session;
                Poco::Net::WebSocket *_ws;
                Poco::Timer *_heartbeatTimer;
                //Poco::Logger *_logger;
                Poco::Thread _thread;
                
                int _refCount;
                
            public:
                
                bool handshake();
                bool openSocket();
                bool init();
                
                void release();
                void addref();
                
                static WebSocketsClientImp* connect(std::string host, int port);
                void disconnect(std::string endpoint);
                void connectToEndpoint(std::string endpoint);
                void monitor();
                virtual void run();
                void heartbeat(Poco::Timer& timer);
                bool receive();
                void send(std::string endpoint, std::string s);
                void emit(std::string endpoint, std::string eventname, std::string args);
                
                std::string getUri();
            };
        }
    }
}