//
//  WebSocketClient.h
//  example_basic_client
//
//  Created by bacchus-house on 12/5/13.
//
//

#pragma once

#include "ofx/HTTP/Client/BaseClient.h"
#include "ofx/HTTP/Client/BaseClientSettings.h"
#include "ofx/HTTP/Server/WebSocket/WebSocketRoute.h"
#include "ofx/HTTP/Server/WebSocket/WebSocketRouteSettings.h"


namespace ofx {
    namespace HTTP {
        namespace Client {
            
            class BasicWebSocketClientSettings:
            public WebSocketRouteSettings,
            public BaseClientSettings
            {
            };
            
            
            class WebSocketsClient: public BaseClient
            {
            public:
                typedef std::shared_ptr<WebSocketsClient> SharedPtr;
                typedef std::weak_ptr<WebSocketsClient>   WeakPtr;
                typedef BasicWebSocketClientSettings          Settings;
                
                WebSocketsClient(const Settings& settings = Settings());
                
                WebSocketRoute::SharedPtr getWebSocketRoute();
                
                // this method is a hack replacement for std::make_shared<BasicServer>(...);
                static SharedPtr makeShared(const Settings& settings = Settings())
                {
                    return SharedPtr(new WebSocketsClient(settings));
                }
            private:
                WebSocketRoute::SharedPtr _webSocketRoute;
                
            };
            
            
        }
    }
} // namespace ofx::HTTP