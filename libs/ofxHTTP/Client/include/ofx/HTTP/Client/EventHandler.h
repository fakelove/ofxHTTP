#pragma once
#include "Poco/NotificationCenter.h"
#include "ofx/HTTP/Client/ClientNotifications.h"

namespace ofx{
    namespace HTTP{
        namespace Client{
            class EventHandler
            {
            public:
                EventHandler(void);
                EventHandler(Poco::NotificationCenter* nc);
                ~EventHandler(void);
                
                void handleMessage(ClientMessage* pNf);
                void handleEvent(ClientEvent* pNf);
                
                void registerCallbacks(Poco::NotificationCenter* nc);
                
                Poco::NotificationCenter* _nCenter;
                
                
                void setNCenter(NotificationCenter* nc);
            };

        }
    }
}


