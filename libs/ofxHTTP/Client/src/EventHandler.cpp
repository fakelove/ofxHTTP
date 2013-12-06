#include "ofx/HTTP/Client/EventHandler.h"
#include "Poco/Observer.h"
#include "ofx/HTTP/Client/ClientNotifications.h"
#include "Poco/NotificationCenter.h"
#include "ofx/HTTP/Client/EventRegistry.h"
#include "ofx/HTTP/Client/WebSocketsClient.h"

namespace ofx{
    namespace HTTP{
        namespace Client{
            
            EventHandler::EventHandler(void)
            {
            }
            
            EventHandler::EventHandler(Poco::NotificationCenter* nc)
            {
                _nCenter = nc;
                registerCallbacks(_nCenter);
                
            }
            
            EventHandler::~EventHandler(void)
            {
                _nCenter->removeObserver(
                                         Poco::Observer<EventHandler, ClientMessage>(*this, &EventHandler::handleMessage)
                                         );
                _nCenter->removeObserver(
                                         Poco::Observer<EventHandler, SIOJSONMessage>(*this, &EventHandler::handleJSONMessage)
                                         );
                _nCenter->removeObserver(
                                         Poco::Observer<EventHandler, ClientEvent>(*this, &EventHandler::handleEvent)
                                         );
            }
            
            void EventHandler::handleMessage(ClientMessage* pNf)
            {
                _logger->information("handling message, message received: %s",pNf->getMsg());
                pNf->release();
            }
            
            void EventHandler::handleJSONMessage(SIOJSONMessage* pNf)
            {
                _logger->information("handling JSON message");
                pNf->release();
            }
            
            void EventHandler::handleEvent(SIOEvent* pNf)
            {
                _logger->information("handling Event");
                _logger->information("data: %s", pNf->_data);
                
                ParseHandler::Ptr pHandler = new ParseHandler(true);
                Parser parser(pHandler);
                
                Var result = parser.parse(pNf->_data);
                Object::Ptr object = result.extract<Object::Ptr>();
                Var temp = object->get("name");
                
                std::string eventName = temp.convert<std::string>();
                
                Array::Ptr arr = object->getArray("args");
                Object::Ptr args = arr->getObject(0);
                
                pNf->_client->fireEvent(eventName.c_str(), args);
                
                pNf->release();
            }
            
            void EventHandler::registerCallbacks(NotificationCenter* nc)
            {
                _nCenter = nc;
                
                _nCenter->addObserver(
                                      Observer<EventHandler, SIOMessage>(*this, &EventHandler::handleMessage)
                                      );
                _nCenter->addObserver(
                                      Observer<EventHandler, SIOJSONMessage>(*this, &EventHandler::handleJSONMessage)
                                      );
                _nCenter->addObserver(
                                      Observer<EventHandler, SIOEvent>(*this, &EventHandler::handleEvent)
                                      );
            }
            
            void EventHandler::setNCenter(NotificationCenter* nc)
            {
                _nCenter = nc;
                registerCallbacks(_nCenter);
            }
        }
    }
}