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


#include "Poco/Exception.h"
#include "Poco/NullStream.h"
#include "Poco/Thread.h"
#include "Poco/ThreadPool.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
#include "Poco/Net/HTTPAuthenticationParams.h"
#include "Poco/Net/NetException.h"
#include "ofEvents.h"
#include "ofx/HTTP/Client/ClientContext.h"
#include "ofx/HTTP/AbstractTypes.h"
#include "ofx/HTTP/Client/SessionSettings.h"
#include "ofx/HTTP/StreamUtils.h"
#include "ofx/HTTP/Client/BaseRequest.h"

using namespace Poco::Net;

namespace ofx {
namespace HTTP {
namespace Client {


class BaseClient
{
public:
    typedef std::shared_ptr<BaseClient> SharedPtr;
    typedef std::weak_ptr<BaseClient>   WeakPtr;

    BaseClient(ClientContext::SharedPtr context,
               Poco::ThreadPool& threadPoolRef = Poco::ThreadPool::defaultPool());

    virtual ~BaseClient();
    
    ResponseStream::SharedPtr execute(const BaseRequest& request);

//    void exectute(const Request::BaseRequest& _request,
//                  AbstractResponseStreamConsumer* consumer);

    ClientContext::SharedPtr getClientContext();
    
    Poco::ThreadPool& getThreadPoolRef(); // modifications to the thread pool can be made via the ref

    // convenience method
    
    void setCredentials(const AuthScope& scope, const Credentials& credentials);
    void setCredentials(const Poco::URI& uri,
                        const std::string& username,
                        const std::string& password);



    
protected:
    void update(ofEventArgs& args);

    static ResponseStream::SharedPtr openResponseStream(const BaseRequest& request, ClientContext::SharedPtr context);
        
//    Poco::Thread _syncThread; // thread for executing syncronous calls
//                              // must always be immediately joined

    ClientContext::SharedPtr _context;

    Poco::ThreadPool& _threadPoolRef; // thread pool for executing asynchronous calls
    

//    ofThreadErrorHandler errorHandler;
//    ErrorHandler* previousErrorHandler;
    
};


} } } // namespace ofx::HTTP::Client
