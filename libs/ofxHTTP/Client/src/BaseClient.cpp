/*==============================================================================

 Copyright (c) 2013 - Christopher Baker <http://christopherbaker.net>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 =============================================================================*/


#include "ofx/HTTP/Client/BaseClient.h"


namespace ofx {
namespace HTTP {
namespace Client {

    
//------------------------------------------------------------------------------
ResponseStream* BaseClient::openResponseStream(const BaseRequest& request,
                                           ClientContext::SharedPtr context) {

    Poco::URI uri(request.getURI());
    
    SessionSettings& sessionSettings = context.get()->getSessionSettingsRef();  // get a copy of the session settings
    CredentialStore& credentials     = context.get()->getCredentialStoreRef();
    CookieStore&     cookies         = context.get()->getCookieStoreRef();
    
    int redirects = 0;
    
    Poco::Net::HTTPResponse pResponse;
    
    // ofLogVerbose("Client::open") << "Beginning redirect loop. Max redirects = " << requestSettings.maxRedirects ;
    
    try {
        
        while (redirects < sessionSettings.getMaxRedirects()) {
//            URI resolvedURI(targetURI);
            
            Poco::URI redirectedProxyUri;
            Poco::Net::HTTPClientSession* pSession = NULL;
            bool proxyRedirectRequested = false;
            
            bool authenticationRequested = false;
//            bool authenticationAttempted = false;
            
            
            ofLogVerbose("Client::open") << "Beginning retry loop : redirect # " << redirects;
            
            do {
                
                // psession will be deleted if proxy is redirected
                if (!pSession) {
                    
                    pSession = new Poco::Net::HTTPClientSession(uri.getHost(), uri.getPort());
                    
                    ofLogVerbose("Client::open") << "New session created - host: " << pSession->getHost() << " port: " << pSession->getPort();
                    
                    if(sessionSettings.isProxyEnabled()) {
                        if(redirectedProxyUri.empty()) {
                            ofLogVerbose("Client::open") << "Using proxy " << sessionSettings.getProxyHost() << " @ " << sessionSettings.getProxyPort();

                            pSession->setProxyHost(sessionSettings.getProxyHost());
                            pSession->setProxyPort(sessionSettings.getProxyPort());
                        } else {
                            ofLogVerbose("Client::open") << "Using redirected proxy " << redirectedProxyUri.getHost() << " @ " << redirectedProxyUri.getPort();

                            // use the redirected proxy url, rather than the one provided in settings
                            pSession->setProxyHost(redirectedProxyUri.getHost());
                            pSession->setProxyPort(redirectedProxyUri.getPort());
                        }

                        if(sessionSettings.hasProxyCredentials()) {
                            ofLogVerbose("Client::open") << "With proxy username:password : " << sessionSettings.getProxyUsername() << ":" << sessionSettings.getProxyPassword();
                            pSession->setProxyUsername(sessionSettings.getProxyUsername());
                            pSession->setProxyPassword(sessionSettings.getProxyPassword());
                        } else {
                            ofLogVerbose("Client::open") << "Without proxy credentials.";
                        }
                    }
                    
                } else {
                    ofLogVerbose("Client::open") << "Using existing session - host: " << pSession->getHost() << " port: " << pSession->getPort();
                }
                
                std::string path = uri.getPathAndQuery();
                
                if(path.empty()) path = "/";
                
                ofLogVerbose("Client::open") << "Using path: " << path;
                
                Poco::Net::HTTPRequest pRequest(request.getMethod(), path, request.getVersion());
                
                // apply defaults from the session first
                if(sessionSettings.hasDefaultHeaders()) {
                    ofLogVerbose("Client::open") << "Writing default headers:";
                    Poco::Net::NameValueCollection defaultHeaders = sessionSettings.getDefaultHeaders();
                    Poco::Net::NameValueCollection::ConstIterator iter = defaultHeaders.begin();
                    while(iter != defaultHeaders.end()) {
                        ofLogVerbose("Client::open") << (*iter).first << "=" << (*iter).second;
                        pRequest.set((*iter).first, (*iter).second);
                    }
                }
                
                
                if(!sessionSettings.getUserAgent().empty()) {
                    ofLogVerbose("Client::open") << "Setting user agent to: " << sessionSettings.getUserAgent();
                    pRequest.set("User-Agent", sessionSettings.getUserAgent());
                }
                
                ////// ALL OF THIS NEEDS TO GO ELSEWHERE -- maybe inside the context?
                if(authenticationRequested) {
                    credentials.authenticate(*pSession, pRequest, pResponse);
                } else {
                    credentials.authenticate(*pSession, pRequest);//
                }
                
                
                // call back into the request to pull the request data
                ofLogVerbose("Client::open") << "Preparing request.";
                request.prepareRequest(pRequest);
                
                
                //////////////////////////////////////////////////////////////////
                /////////////////////// -- SEND REQUEST -- ///////////////////////
                //////////////////////////////////////////////////////////////////
                ofLogVerbose("Client::open") << "Sending request.";
                std::ostream& requestStream = pSession->sendRequest(pRequest);
                
                ofLogVerbose("Client::open") << "Sending request body.";
                request.sendRequestBody(requestStream); // upload, put etc
                //////////////////////////////////////////////////////////////////
                ///////////////////////// -- RESPONSE -- /////////////////////////
                //////////////////////////////////////////////////////////////////
                ofLogVerbose("Client::open") << "Receiving response.";
                std::istream& responseStream = pSession->receiveResponse(pResponse);
                
                
                cookies.store(pResponse);
                
                
                if (pResponse.getStatus() == Poco::Net::HTTPResponse::HTTP_MOVED_PERMANENTLY ||
                    pResponse.getStatus() == Poco::Net::HTTPResponse::HTTP_FOUND ||
                    pResponse.getStatus() == Poco::Net::HTTPResponse::HTTP_SEE_OTHER ||
                    pResponse.getStatus() == Poco::Net::HTTPResponse::HTTP_TEMPORARY_REDIRECT) {
                    
                    uri.resolve(pResponse.get("Location"));
                    
                    ++redirects;
                    delete pSession;
                    pSession = NULL;
                    
                    ofLogVerbose("Client::open") << "Redirecting to: " << uri.toString();
                    
                } else if (pResponse.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
                    ofLogVerbose("Client::open") << "Got valid stream, returning.";
                    
                    return new ResponseStream(pResponse, new Poco::Net::HTTPResponseStream(responseStream, pSession));
                
                } else if (pResponse.getStatus() == Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED &&
                           !authenticationRequested) {

                    authenticationRequested = true;
                    
                    std::streamsize n = StreamUtils::consume(responseStream);
                    
                    ofLogVerbose("Client::open") << "Got HTTP_UNAUTHORIZED, trying to authorize with response this time. (" << n << " bytes consumed)";
                    
                } else if (pResponse.getStatus() == Poco::Net::HTTPResponse::HTTP_USEPROXY &&
                           !proxyRedirectRequested) {
                    // The requested resource MUST be accessed through the proxy
                    // given by the Location field. The Location field gives the
                    // URI of the proxy. The recipient is expected to repeat this
                    // single request via the proxy. 305 responses MUST only be generated by origin servers.
                    // only use for one single request!
                    redirectedProxyUri.resolve(pResponse.get("Location"));
                    delete pSession;
                    pSession = NULL;
                    proxyRedirectRequested = true; // only allow useproxy once

                    std::streamsize n = StreamUtils::consume(responseStream);
                    
                    ofLogVerbose("Client::open") << "Got HTTP_USEPROXY, trying to use redirected proxy. (" << n << " bytes consumed)";
                    
                } else {
                    ofLogVerbose("Client::open") << "Got other response " << pResponse.getStatus() << " b/c " << pResponse.getReason();
                    return new ResponseStream(pResponse,
                                              new Poco::Net::HTTPResponseStream(responseStream, pSession),
                                              new Poco::Exception(pResponse.getReason(), uri.toString()));
                }
                
            } while (authenticationRequested || proxyRedirectRequested);
            
        }
    } catch(const Poco::Net::HostNotFoundException& exc) {
        ofLogVerbose("Client::open") << "Got exception " << exc.displayText();
        return new ResponseStream(pResponse, NULL, exc.clone());
    } catch(const Poco::Exception& exc) {
        ofLogVerbose("Client::open") << "Got exception " << exc.displayText();
        return new ResponseStream(pResponse, NULL, exc.clone());
    } catch(...) {
        ofLogVerbose("Client::open") << "Got unknown exception";
        return new ResponseStream(pResponse, NULL, new Poco::Exception("Unknown exception"));
    }
    
    return new ResponseStream(pResponse,
                              NULL,
                              new Poco::IOException("Too many redirects while opening URI", request.getURI().toString()));
}

//    if(requestSettings.bHandleRedirects && ofxHTTPResponse::isRedirectRequested(pResponse)) {
//        
//        if(request.redirectCount >= requestSettings.maxRedirects) {
//            ofLogError("Client::getRedirectionURI") << "Maximum redirects were exceeded.";
//            
//        } else if(pResponse.has("Location")) {
//            string location = pResponse.get("Location");
//            
//            URI redirectURI(location);
//            
//            
//            if(requestSettings.bRejectRelativeRedirects &&
//               redirectURI.isRelative()) {
//                ofLogError("Client::getRedirectionURI") << "Relative Redirects are Prohibited.";
//                // then fail becaues we don't allow them
//            } else {
//                redirectURI = uri;
//                redirectURI.resolve(location);
//            }
//            
//            if(requestSettings.bRequireHTTPRedirects &&
//               redirectURI.getScheme() != "http" &&
//               redirectURI.getScheme() != "https") {
//                ofLogError("Client::getRedirectionURI") << "Invalid redirect scheme: " << redirectURI.getScheme();
//            }
//            
//            if(!requestSettings.bAllowCircularRedirects && request.redirectCount > 0) {
//                vector<URI>::const_iterator iter = request.redirectHistory.begin();
//                while(iter != request.redirectHistory.end()) {
//                    URI hURI = (*iter);
//                    if(hURI == redirectURI) {
//                        ofLogError("Client::getRedirectionURI") << "Circular Redirect Detected.";
//                    }
//                    ++iter;
//                }
//            }
//



//------------------------------------------------------------------------------
BaseClient::BaseClient(ClientContext::SharedPtr context,
               Poco::ThreadPool& threadPoolRef) :
    _context(context),
    _threadPoolRef(threadPoolRef)
{
    ofAddListener(ofEvents().update,this,&BaseClient::update);
    
    cout << "MAXR=" << _context.get()->getSessionSettingsRef().getMaxRedirects() << endl;
}

//------------------------------------------------------------------------------
BaseClient::~BaseClient()
{
    ofRemoveListener(ofEvents().update,this,&BaseClient::update);
}

//------------------------------------------------------------------------------
void BaseClient::update(ofEventArgs& args)
{
    // keep an eye on the queue
    
//    bool threadPoolAvailable = true;
//    while(!executorQueue.empty() && threadPoolAvailable) {
//        try
//        {
//
//            std::shared_ptr< ofxHTTPRequestExecutor >& task = executorQueue.front();
//            ofxThreadSettings settings = task->getThreadSettings();
//
//            threadPool.startWithPriority(settings.getPriority(), *task.get(), settings.getName());
//
//            // if successful, then we will pop it off.
//            executorQueue.pop();
//            
//        } catch (const NoThreadAvailableException& exc) {
//            threadPoolAvailable = false;
//        }
//    }
}
        
////------------------------------------------------------------------------------
//ResponseStream* BaseClient::exectute(const Request::BaseRequest& request)
//{
//    return openResponseStream(request,context);
//}

////------------------------------------------------------------------------------
//void BaseClient::exectute(const Request::BaseRequest& _request,
//                      AbstractResponseStreamConsumer* consumer)
//{
//    if(consumer != NULL)
//    {
//        consumer->consume(openResponseStream(_request));
//    }
//    else
//    {
//        ofLogError("Client::exectute") << "consumer was NULL";
//    }
//}

//------------------------------------------------------------------------------
Poco::ThreadPool& BaseClient::getThreadPoolRef()
{
    return _threadPoolRef;
}

//------------------------------------------------------------------------------
ClientContext::SharedPtr BaseClient::getClientContext()
{
    return _context;
}

//------------------------------------------------------------------------------
void BaseClient::setCredentials(const AuthScope& scope,
                                const Credentials& credentials)
{
    _context->getCredentialStoreRef().setCredentials(scope, credentials);
}

//------------------------------------------------------------------------------
void BaseClient::setCredentials(const Poco::URI& uri,
                                const std::string& username,
                                const std::string& password)
{
    _context->getCredentialStoreRef().setCredentials(uri, username, password);
}

//------------------------------------------------------------------------------
//void BaseClient::setClientContext(const ClientContext& _context) {
//    context = _context;
//}
//
////------------------------------------------------------------------------------
//ClientContext& BaseClient::getClientContextRef() {
//    return context;
//}
//
////------------------------------------------------------------------------------
//ClientContext BaseClient::getClientContext() const {
//    return context;
//}

} } } // namespace ofx::HTTP::Client
