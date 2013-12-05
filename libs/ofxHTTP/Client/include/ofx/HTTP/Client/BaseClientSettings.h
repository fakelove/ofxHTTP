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


#pragma once


#include <string>
#include "Poco/Thread.h"
#include "Poco/Timespan.h"
#include "Poco/URI.h"


namespace ofx {
    namespace HTTP {
        namespace Client{
            
            class BaseClientSettings
            {
            public:
                BaseClientSettings(const std::string& host = DEFAULT_HOST,
                                   const unsigned short port = DEFAULT_PORT,
                                   bool useSSL = DEFAULT_USE_SSL);
                BaseClientSettings();
                ~BaseClientSettings();
                
                void setHost(const std::string& host);
                std::string getHost() const;
                
                void setPort(const unsigned short port);
                unsigned short getPort() const;
                
                void setUseSSL(bool useSSL);
                bool getUseSSL() const;
                
                Poco::URI getURI() const;
                
                const static std::string    DEFAULT_HOST;
                const static unsigned short DEFAULT_PORT;
                const static bool           DEFAULT_USE_SSL;
                
            private:
                std::string      _host;
                unsigned short   _port;
                bool             _useSSL;
                
            };
            
        }
    }
} // namespace ofx::HTTP
