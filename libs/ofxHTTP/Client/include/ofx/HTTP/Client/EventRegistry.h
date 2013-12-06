#pragma once
#include "ofEvents.h"
#include "ofx/HTTP/Client/EventTarget.h"
#include <map>
#include <string>

#include "Poco/Delegate.h"


namespace ofx {
    namespace HTTP {
        namespace Client {
            typedef void (EventTarget::*callback)(const void*, string&);
            
            class WebSocketsClient;
            
            class EventRegistry
            {
            public:
                EventRegistry(void);
                ~EventRegistry(void);
                
                bool registerEvent(const char *name, EventTarget *target, callback c);
                void fireEvent(WebSocketsClient *client, const char *name, string data);
                
            private:
                
                std::map<std::string, ofEvent< string > *> mEventMap; //!< the map containing event names and handlers
            };
        }
    }
}
