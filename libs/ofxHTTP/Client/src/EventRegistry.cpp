#include "ofx/HTTP/Client/EventRegistry.h"

namespace ofx {
    namespace HTTP {
        namespace Client {
            
            EventRegistry::EventRegistry(void)
            {
            }
            
            
            EventRegistry::~EventRegistry(void)
            {
            }
            
            bool EventRegistry::registerEvent(const char *name, EventTarget *target, callback c)
            {
                std::map<std::string,ofEvent<string> *>::iterator it= mEventMap.find(std::string(name));
                if(it != mEventMap.end())
                {
                    
                    ofEvent<string> *e = it->second;
                    
                    //*e += Poco::delegate(target, c);
                    
                }else
                {
                    
                    ofEvent<string> *e = new ofEvent<string>();
                    
                    //*e += Poco::delegate(target, c);
                    
                    mEventMap[std::string(name)] = e;
                    
                }
                
                return true;
            }
            
            void EventRegistry::fireEvent(WebSocketsClient *client, const char *name, string data)
            {
                
                std::map<std::string, ofEvent<string> *>::iterator it= mEventMap.find(std::string(name));
                if(it != mEventMap.end())
                {
                    ofEvent<string> *e = it->second;
                    e->notify(client, data);
                    
                }else
                {
                    cout<<"no event handler"<<endl;
                    
                }
                
            }
        }
    }
}