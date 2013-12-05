//
//  BaseClientSettings.h
//  example_basic_client
//
//  Created by bacchus-house on 12/5/13.
//
//

// =============================================================================
//
// Copyright (c) 2013 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================
#include "ofx/HTTP/Client/BaseClientSettings.h"

namespace ofx {
    namespace HTTP {
        namespace Client {
            
            const std::string BaseClientSettings::DEFAULT_HOST = "127.0.0.1";
            const unsigned short BaseClientSettings::DEFAULT_PORT = 8080;
            const bool BaseClientSettings::DEFAULT_USE_SSL = false;
            
            
            //------------------------------------------------------------------------------
            BaseClientSettings::BaseClientSettings(const std::string& host,
                                                   const unsigned short port,
                                                   bool useSSL):
            _host(host),
            _port(port),
            _useSSL(useSSL)
            {
            }
            
            //------------------------------------------------------------------------------
            BaseClientSettings::~BaseClientSettings()
            {
            }
            
            
            BaseClientSettings::BaseClientSettings(){
                
            }
            
            //------------------------------------------------------------------------------
            void BaseClientSettings::setHost(const std::string& host)
            {
                _host = host;
            }
            
            //------------------------------------------------------------------------------
            std::string BaseClientSettings::getHost() const
            {
                return _host;
            }
            
            //------------------------------------------------------------------------------
            void BaseClientSettings::setPort(const unsigned short port)
            {
                _port = port;
            }
            
            //------------------------------------------------------------------------------
            unsigned short BaseClientSettings::getPort() const
            {
                return _port;
            }
            
            //------------------------------------------------------------------------------
            void BaseClientSettings::setUseSSL(bool useSSL)
            {
                _useSSL = useSSL;
            }
            
            //------------------------------------------------------------------------------
            bool BaseClientSettings::getUseSSL() const
            {
                return _useSSL;
            }
            
            //------------------------------------------------------------------------------
            Poco::URI BaseClientSettings::getURI() const
            {
                Poco::URI uri;
                
                uri.setScheme(_useSSL ? "https" : "http");
                uri.setHost(_host);
                uri.setPort(_port);
                
                return uri;
            }
            
            
        }
    }
} // namespace ofx::HTTP
