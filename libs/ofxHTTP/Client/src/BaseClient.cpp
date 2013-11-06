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


#include "ofx/HTTP/Client/BaseClient.h"


namespace ofx {
namespace HTTP {
namespace Client {


//------------------------------------------------------------------------------
BaseClient::BaseClient(ClientContext::SharedPtr context,
                       Poco::ThreadPool& threadPoolRef):
    _context(context),
    _threadPoolRef(threadPoolRef)
{
    ofAddListener(ofEvents().update,this,&BaseClient::update);

    cout << "MAXR=" << _context->getSessionSettingsRef().getMaxRedirects() << endl;
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
        
//------------------------------------------------------------------------------
ResponseStream::SharedPtr BaseClient::execute(const BaseRequest& request)
{
    return openResponseStream(request,_context);
}

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

//------------------------------------------------------------------------------
ResponseStream::SharedPtr BaseClient::openResponseStream(const BaseRequest& request,
                                                         ClientContext::SharedPtr context)
{
    HTTPResponse httpResponse;

    SessionSettings& sessionSettings = context->getSessionSettingsRef();  // get a copy of the session settings
    CredentialStore& credentials     = context->getCredentialStoreRef();
    CookieStore&     cookies         = context->getCookieStoreRef();

    int redirects = 0;

    try
    {
        Poco::URI uri(request.getURI());

        ofLogVerbose("BaseClient::openResponseStream") << "Beginning redirect loop. Max redirects = " <<
            context->getSessionSettingsRef().getMaxRedirects() ;

        while (redirects < sessionSettings.getMaxRedirects())
        {
            // URI resolvedURI(targetURI);
            Poco::URI redirectedProxyUri;
            HTTPClientSession* pClientSession; //
            bool proxyRedirectRequested = false;
            bool authenticationRequested = false;
            // bool authenticationAttempted = false;

            ofLogVerbose("BaseClient::openResponseStream") << "Beginning retry loop : redirect # " << redirects;

            do
            {
                // pClientSession will be deleted if proxy is redirected
                if (0 != pClientSession)
                {
                    pClientSession = new HTTPClientSession(uri.getHost(), uri.getPort());

                    ofLogVerbose("BaseClient::openResponseStream") << "New session created - host: " <<
                        pClientSession->getHost() << " port: " << pClientSession->getPort();

                    if(sessionSettings.isProxyEnabled())
                    {
                        if(redirectedProxyUri.empty())
                        {
                            ofLogVerbose("BaseClient::openResponseStream") << "Using proxy " <<
                                sessionSettings.getProxyHost() << " @ " << sessionSettings.getProxyPort();

                            pClientSession->setProxyHost(sessionSettings.getProxyHost());
                            pClientSession->setProxyPort(sessionSettings.getProxyPort());
                        }
                        else
                        {
                            ofLogVerbose("BaseClient::openResponseStream") << "Using redirected proxy " <<
                                redirectedProxyUri.getHost() << " @ " << redirectedProxyUri.getPort();

                            // use the redirected proxy url, rather than the one provided in settings
                            pClientSession->setProxyHost(redirectedProxyUri.getHost());
                            pClientSession->setProxyPort(redirectedProxyUri.getPort());
                        }

                        if(sessionSettings.hasProxyCredentials())
                        {
                            ofLogVerbose("BaseClient::openResponseStream") << "With proxy username:password : " <<
                                sessionSettings.getProxyUsername() << ":" << sessionSettings.getProxyPassword();

                            pClientSession->setProxyUsername(sessionSettings.getProxyUsername());
                            pClientSession->setProxyPassword(sessionSettings.getProxyPassword());
                        }
                        else
                        {
                            ofLogVerbose("BaseClient::openResponseStream") << "Without proxy credentials.";
                        }
                    }
                }
                else
                {
                    ofLogVerbose("BaseClient::openResponseStream") << "Using existing session - host: " <<
                        pClientSession->getHost() << " port: " << pClientSession->getPort();
                }

                std::string path = uri.getPathAndQuery();

                if(path.empty())
                {
                    path = "/";
                }

                ofLogVerbose("BaseClient::openResponseStream") << "Using path: " << path;

                Poco::Net::HTTPRequest httpRequest(request.getMethod(),
                                                   path,
                                                   request.getVersion());

                // apply defaults from the session first
                // TODO: default headers could also include useragent, etc ...
                if(sessionSettings.hasDefaultHeaders())
                {
                    ofLogVerbose("BaseClient::openResponseStream") << "Writing default headers:";
                    Poco::Net::NameValueCollection defaultHeaders = sessionSettings.getDefaultHeaders();
                    Poco::Net::NameValueCollection::ConstIterator iter = defaultHeaders.begin();

                    while(iter != defaultHeaders.end())
                    {
                        ofLogVerbose("BaseClient::openResponseStream") << (*iter).first << "=" << (*iter).second;
                        httpRequest.set((*iter).first, (*iter).second);
                        ++iter;
                    }
                }

                // TODO: this getting and setting user agent should be intelligently placed in the default headers
                if(!sessionSettings.getUserAgent().empty())
                {
                    ofLogVerbose("BaseClient::openResponseStream") << "Setting user agent to: " << sessionSettings.getUserAgent();
                    httpRequest.set("User-Agent", sessionSettings.getUserAgent());
                }

                ////// ALL OF THIS NEEDS TO GO ELSEWHERE -- maybe inside the context?
                if(authenticationRequested)
                {
                    credentials.authenticate(*pClientSession, httpRequest, httpResponse);
                }
                else
                {
                    credentials.authenticate(*pClientSession, httpRequest);
                }


                // call back into the request to pull the request data
                ofLogVerbose("BaseClient::openResponseStream") << "Preparing request.";

                request.prepareRequest(httpRequest);


                //////////////////////////////////////////////////////////////////
                /////////////////////// -- SEND REQUEST -- ///////////////////////
                //////////////////////////////////////////////////////////////////
                ofLogVerbose("BaseClient::openResponseStream") << "Sending request.";
                std::ostream& requestStream = pClientSession->sendRequest(httpRequest);

                ofLogVerbose("BaseClient::openResponseStream") << "Sending request body.";
                request.sendRequestBody(requestStream); // upload, put etc
                //////////////////////////////////////////////////////////////////
                ///////////////////////// -- RESPONSE -- /////////////////////////
                //////////////////////////////////////////////////////////////////
                ofLogVerbose("BaseClient::openResponseStream") << "Receiving response.";
                std::istream& responseStream = pClientSession->receiveResponse(pResponse);


                cookies.store(pResponse);


                if (pResponse.getStatus() == HTTPResponse::HTTP_MOVED_PERMANENTLY   ||
                    pResponse.getStatus() == HTTPResponse::HTTP_FOUND               ||
                    pResponse.getStatus() == HTTPResponse::HTTP_SEE_OTHER           ||
                    pResponse.getStatus() == HTTPResponse::HTTP_TEMPORARY_REDIRECT)
                {
                    uri.resolve(pResponse.get("Location"));

                    ++redirects;

                    delete pClientSession;
                    pClientSession = 0;

                    ofLogVerbose("BaseClient::openResponseStream") << "Redirecting to: " << uri.toString();

                }
                else if (pResponse.getStatus() == HTTPResponse::HTTP_OK)
                {
                    ofLogVerbose("BaseClient::openResponseStream") << "Got valid stream, returning.";
                    return ResponseStream::makeShared(pResponse, responseStream, pClientSession);
                }
                else if (pResponse.getStatus() == HTTPResponse::HTTP_UNAUTHORIZED &&
                         !authenticationRequested)
                {
                    authenticationRequested = true;

                    std::streamsize n = StreamUtils::consume(responseStream);

                    ofLogVerbose("BaseClient::openResponseStream") <<
                        "Got HTTP_UNAUTHORIZED, trying to authorize with response this time. (" << n << " bytes consumed)";

                }
                else if (pResponse.getStatus() == HTTPResponse::HTTP_USEPROXY &&
                         !proxyRedirectRequested)
                {
                    // The requested resource MUST be accessed through the proxy
                    // given by the Location field. The Location field gives the
                    // URI of the proxy. The recipient is expected to repeat this
                    // single request via the proxy. 305 responses MUST only be generated by origin servers.
                    // only use for one single request!
                    redirectedProxyUri.resolve(pResponse.get("Location"));

                    delete pClientSession;
                    pClientSession = 0;

                    proxyRedirectRequested = true; // only allow useproxy once

                    std::streamsize n = StreamUtils::consume(responseStream);

                    ofLogVerbose("BaseClient::openResponseStream") <<
                        "Got HTTP_USEPROXY, trying to use redirected proxy. (" << n << " bytes consumed)";

                }
                else
                {
                    ofLogVerbose("BaseClient::openResponseStream") << "Received unhandled response " <<
                        pResponse.getStatus() << " b/c " << pResponse.getReason();
                    return new ResponseStream(pResponse,
                                              new HTTPResponseStream(responseStream, pClientSession),
                                              new Poco::Exception(pResponse.getReason(), uri.toString()));
                }

            }
            while (authenticationRequested || proxyRedirectRequested);
        }
    }
    catch(const Poco::SyntaxException& exc)
    {
        ofLogVerbose("BaseClient::openResponseStream") << "Got Syntax exception " << exc.displayText();
        return new ResponseStream(pResponse, 0, exc.clone());
    }
    catch(const HostNotFoundException& exc)
    {
        ofLogVerbose("BaseClient::openResponseStream") << "Got exception " << exc.displayText();
        return new ResponseStream(pResponse, 0, exc.clone());
    }
    catch(const Poco::Exception& exc)
    {
        ofLogVerbose("BaseClient::openResponseStream") << "Got exception " << exc.displayText();
        return new ResponseStream(pResponse, 0, exc.clone());
    }
    catch(...)
    {
        ofLogVerbose("BaseClient::openResponseStream") << "Got unknown exception";
        return new ResponseStream(pResponse, 0, new Poco::Exception("Unknown exception"));
    }

    return new ResponseStream(pResponse,
                              0,
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



} } } // namespace ofx::HTTP::Client
