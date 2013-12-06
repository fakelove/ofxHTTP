#include "WebSocketsClientImp.h"

#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPMessage.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/StreamCopier.h"
#include "Poco/Format.h"
#include <iostream>
#include <sstream>
#include <limits>
#include "Poco/StringTokenizer.h"
#include "Poco/String.h"
#include "Poco/Timer.h"
#include "Poco/RunnableAdapter.h"
#include "Poco/URI.h"

#include "ofx/HTTP/Client/ClientNotifications.h"
#include "ofx/HTTP/Client/ClientRegistry.h"
#include "ofx/HTTP/Client/WebSocketsClient.h"


namespace ofx {
    namespace HTTP {
        namespace Client {
            
            WebSocketsClientImp::WebSocketsClientImp() {
                WebSocketsClientImp("localhost", 3000);
            }
            
            WebSocketsClientImp::WebSocketsClientImp(std::string host, int port) :
            _port(port),
            _host(host),
            _refCount(0)
            {
                std::stringstream s;
                s << host << ":" << port;
                _uri = s.str();
                _ws = NULL;
                
            }
            
            WebSocketsClientImp::~WebSocketsClientImp(void) {
                
                _thread.join();
                
                disconnect("");
                
                _ws->shutdown();
                delete(_ws);
                
                delete(_heartbeatTimer);
                delete(_session);
                
                ClientRegistry::instance()->removeSocket(_uri);
            }
            
            bool WebSocketsClientImp::init() {
                
                
                if(handshake())
                {
                    
                    if(openSocket()) return true;
                    
                }
                
                return false;
                
            }
            
            bool WebSocketsClientImp::handshake() {
                UInt16 aport = _port;
                _session = new HTTPClientSession(_host, aport);
                
                HTTPRequest req(HTTPRequest::HTTP_POST,"/socket.io/1",HTTPMessage::HTTP_1_1);
                
                HTTPResponse res;
                
                try {
                    _session->sendRequest(req);
                } catch (std::exception &e) {
                    return false;
                }
                
                std::istream& rs = _session->receiveResponse(res);
                
                std::cout << res.getStatus() << " " << res.getReason() << std::endl;
                
                if (res.getStatus() != Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED)
                {
                    std::string temp;
                    
                    Poco::StreamCopier::copyToString(rs, temp);
                    
                    cout<<"response: %s\n",temp;
                    
                    Poco::StringTokenizer msg(temp, ":");
                    
                    cout<<"session: %s",msg[0];
                    cout<<"heartbeat: %s",msg[1];
                    cout<<"timeout: %s",msg[2];
                    cout<<"transports: %s",msg[3];
                    
                    _sid = msg[0];
                    _heartbeat_timeout = atoi(msg[1].c_str());
                    _timeout = atoi(msg[2].c_str());
                    
                    return true;
                }
                
                return false;
            }
            
            bool WebSocketsClientImp::openSocket() {
                
                UInt16 aport = _port;
                HTTPRequest req(HTTPRequest::HTTP_GET,"/socket.io/1/websocket/"+_sid,HTTPMessage::HTTP_1_1);
                HTTPResponse res;
                
                do {
                    try {
                        
                        _ws = new WebSocket(*_session, req, res);
                        
                    }
                    catch(NetException ne) {
                        std::cout << ne.displayText() << " : " << ne.code() << " - " << ne.what() << "\n";
                        
                        if(_ws) {
                            delete _ws;
                            _ws = NULL;
                        }
                        
                        Poco::Thread::sleep(2000);
                        
                    }
                } while(_ws == NULL);
                
                cout<<"WebSocket Created\n";
                
                _connected = true;
                
                int hbInterval = this->_heartbeat_timeout*.75*1000;
                _heartbeatTimer = new Poco::Timer(hbInterval, hbInterval);
                Poco::TimerCallback<WebSocketsClientImp> heartbeat(*this, &WebSocketsClientImp::heartbeat);
                _heartbeatTimer->start(heartbeat);
                
                _thread.start(*this);
                
                return _connected;
                
            }
            
            
            WebSocketsClientImp* WebSocketsClientImp::connect(std::string host, int port) {
                
                WebSocketsClientImp *s = new WebSocketsClientImp(host, port);
                
                if(s && s->init()) {
                    
                    return s;
                    
                }
                
                return NULL;
            }
            
            void WebSocketsClientImp::disconnect(std::string endpoint) {
                std::string s = "0::" + endpoint;
                
                if(endpoint == "") {
                    
                    _heartbeatTimer->stop();
                    _connected = false;
                    
                }
                
                _ws->sendFrame(s.data(), s.size());
            }
            
            void WebSocketsClientImp::connectToEndpoint(std::string endpoint) {
                
                std::string s = "1::" + endpoint;
                
                _ws->sendFrame(s.data(), s.size());
                
            }
            
            void WebSocketsClientImp::heartbeat(Poco::Timer& timer) {
                cout<<"heartbeat called\n";
                
                std::string s = "2::";
                
                _ws->sendFrame(s.data(), s.size());
                
            }
            
            void WebSocketsClientImp::run() {
                
                monitor();
                
            }
            
            void WebSocketsClientImp::monitor() {
                do
                {
                    receive();
                    
                } while (_connected);
            }
            
            void WebSocketsClientImp::send(std::string endpoint, std::string s) {
                //cout<<"sending message\n";
                
                std::stringstream pre;
                
                pre << "3::" << endpoint << ":" << s;
                
                std::string msg = pre.str();
                
                _ws->sendFrame(msg.data(), msg.size());
                
            }
            
            void WebSocketsClientImp::emit(std::string endpoint, std::string eventname, std::string args) {
                //cout<<"emitting event\n";
                
                std::stringstream pre;
                
                pre << "5::" << endpoint << ":{\"name\":\"" << eventname << "\",\"args\":" << args << "}";
                
                //cout<<"event data: %s\n", pre.str());
                
                std::string msg = pre.str();
                
                _ws->sendFrame(msg.data(), msg.size());
                
            }
            
            bool WebSocketsClientImp::receive() {
                
                char buffer[1024];
                int flags;
                int n;
                
                n = _ws->receiveFrame(buffer, sizeof(buffer), flags);
                //cout<<"bytes received: %d ",n);
                
                std::stringstream s;
                for(int i = 0; i < n; i++) {
                    s << buffer[i];
                }
                
                //cout<<"buffer received: \"%s\"\n",s.str());
                
                int control = atoi(&buffer[0]);
                Poco::StringTokenizer st(s.str(), ":");
                std::string endpoint = st[2];
                
                std::string uri = _uri;
                uri += endpoint;
                
                WebSocketsClient *c = ClientRegistry::instance()->getClient(uri);
                
                std::string payload = "";
                
                switch(control) {
                    case 0:
                        cout<<"Socket Disconnected\n";
                        break;
                    case 1:
                        cout<<"Connected to endpoint: %s \n", st[2];
                        break;
                    case 2:
                        cout<<"Heartbeat received\n";
                        break;
                    case 3:
                        cout<<"Message received\n";
                        
                        //MAKE OF EVENT
                        //c->getNCenter()->postNotification(new SIOMessage(st[3]));
                        break;
                    case 4:
                        cout<<"JSON Message Received\n";
                        
                        //MAKE OF EVENT
                        //c->getNCenter()->postNotification(new SIOJSONMessage(st[3]));
                        break;
                    case 5:
                        cout<<"Event Dispatched\n";
                        
                        for(int i = 3; i < st.count(); i++)
                        {
                            if(i != 3) payload += ":";
                            payload += st[i];
                        }
                        //MAKE OF EVENT
                        //c->getNCenter()->postNotification(new SIOEvent(c, payload));
                        break;
                    case 6:
                        cout<<"Message Ack\n";
                        break;
                    case 7:
                        cout<<"Error\n";
                        break;
                    case 8:
                        cout<<"Noop\n";
                        break;
                }
                
                return true;
                
            }
            
            void WebSocketsClientImp::addref() {
                _refCount++;
            }
            
            void WebSocketsClientImp::release() {
                if(--_refCount == 0) delete this;
            }
        }}}