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


#include "ofApp.h"


//------------------------------------------------------------------------------
void ofApp::setup()
{
    ofSetLogLevel(OF_LOG_VERBOSE);

    ofx::SSLManager::registerAllEvents(this);

    HTTP::Client::Context::SharedPtr context = HTTP::Client::Context::makeShared();

//    std::string username = "username";
//    std::string password = "password";

//    std::string url = "https://httpbin.org/relative-redirect/19";
//    std::string url = "https://httpbin.org/basic-auth/" +  username +  "/" + password;
//    std::string url = "https://httpbin.org/hidden-basic-auth/" +  username +  "/" + password;
//    std::string url = "https://httpbin.org/digest-auth/auth/" +  username +  "/" + password;

//    context->getSessionSettingsRef().setUserAgent("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/32.0.1700.41 Safari/537.36");

    context->getSessionSettingsRef().setUserAgent("curl/7.30.0");


    std::string url = "https://www.google.com/calendar/ical/christopherbaker.net_91ul9n5dq2b6pkmin511q3bq14%40group.calendar.google.com/public/basic.ics";

    Poco::URI uri(url);

    HTTP::Client::GetRequest get(uri);


//
//    get.addCookie("pinkcookie");
//
//    get.addHeader("header","beader");

//    HTTP::AuthScope scope(uri.getScheme(),
//                          uri.getHost(),
//                          uri.getPort(),
//                          "me@kennethreitz.com",
//                          HTTP::DIGEST);
//
//    context->getCredentialStoreRef().setCredentials(scope,
//                                                    HTTP::Credentials(username,password));

    HTTP::Client::ResponseStream::SharedPtr response = HTTP::Client::ResponseStream::createResponseStream(get, context);

    if(!response->hasException())
    {
        Poco::StreamCopier::copyStream(response->getResponseStreamRef(), cout);
        cout << endl;
    } else {
        cout << response->getException()->message() << endl;
    }


//    url = "http://httpbin.org/hidden-basic-auth/" +  username +  "/" + password;
//
//    uri = Poco::URI(url);
//
//    get = ofx::HTTP::Client::GetRequest(uri);
//
//    context->getCredentialStoreRef().setCredentials(ofx::HTTP::AuthScope(get.getURI()),
//                                                    ofx::HTTP::Credentials(username,password));
//
//    response = ofx::HTTP::Client::ResponseStream::createResponseStream(get, context);
//
//    if(!response->hasException()) {
//        Poco::StreamCopier::copyStream(response->getResponseStreamRef(), cout);
//    } else {
//        cout << response->getException()->message() << endl;
//    }


//    url = "http://httpbin.org/digest-auth/" +  username +  "/" + password;
//
//    uri = Poco::URI(url);
//
//    get = ofx::HTTP::Client::GetRequest(uri);
//
//    context->getCredentialStoreRef().setCredentials(ofx::HTTP::AuthScope(get.getURI()),
//                                                    ofx::HTTP::Credentials(username,password));
//
//    response = ofx::HTTP::Client::ResponseStream::createResponseStream(get, context);
//
//    if(!response->hasException()) {
//        Poco::StreamCopier::copyStream(response->getResponseStreamRef(), cout);
//    } else {
//        cout << response->getException()->message() << endl;
//    }

}

void ofApp::exit()
{
    ofx::SSLManager::unregisterAllEvents(this);
}

//------------------------------------------------------------------------------
void ofApp::update()
{
}

//------------------------------------------------------------------------------
void ofApp::draw()
{
}

//------------------------------------------------------------------------------
void ofApp::keyPressed(int key)
{
}

//------------------------------------------------------------------------------
void ofApp::keyReleased(int key)
{
}

//------------------------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
}

//------------------------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
}

//------------------------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
}

//------------------------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
}

//------------------------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
}

//------------------------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
}

//------------------------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{
}

//------------------------------------------------------------------------------
void ofApp::onSSLServerVerificationError(Poco::Net::VerificationErrorArgs& args)
{
    ofLogVerbose("ofApp::onServerVerificationError") << args.errorMessage();

    // if you want to proceed, you should allow your user to inspect
    // the certificate and set:
    //     args.setIgnoreError(true);
    // if they approve
}

//------------------------------------------------------------------------------
void ofApp::onSSLClientVerificationError(Poco::Net::VerificationErrorArgs& args)
{
    ofLogVerbose("ofApp::onClientVerificationError") << args.errorMessage();

    std::stringstream ss;

    ss << "Error: " << args.errorMessage() << " #" << args.errorNumber() << " depth: " << args.errorDepth() << std::endl;

    ss << "Certificate: " << std::endl;

    ss << "Issued By: " << args.certificate().issuerName() << std::endl;
    ss << "Subject Name: " << args.certificate().issuerName() << std::endl;
    ss << "Common Name: " << args.certificate().issuerName() << std::endl;

    Poco::DateTime ldtValidFrom = args.certificate().validFrom();
    Poco::DateTime ldtExpiresOn = args.certificate().expiresOn();

    ss << "Valid From: " << Poco::DateTimeFormatter::format(ldtValidFrom, "%dd %H:%M:%S.%i") << std::endl;
    ss << "Expires On: " << Poco::DateTimeFormatter::format(ldtExpiresOn, "%dd %H:%M:%S.%i") << std::endl;

    ofLogVerbose("ofApp::onServerVerificationError") << ss.str();

    args.setIgnoreError(true);

    // if you want to proceed, you should allow your user to inspect
    // the certificate and set:
    //     args.setIgnoreError(true);
    // if they approve

    
}

//------------------------------------------------------------------------------
void ofApp::onSSLPrivateKeyPassphraseRequired(std::string& args)
{
    ofLogVerbose("ofApp::onPrivateKeyPassphraseRequired") << args;

    // if you want to proceed, you should allow your user set the
    // the certificate and set:
    //     args.setIgnoreError(true);
    // if they approve
}

