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


#include "Poco/BasicEvent.h"
#include "Poco/Delegate.h"
#include "Poco/File.h"
#include "Poco/SharedPtr.h"
#include "Poco/SingletonHolder.h"
#include "Poco/Net/Context.h"
#include "Poco/Net/NetSSL.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/PrivateKeyPassphraseHandler.h"
#include "Poco/Net/InvalidCertificateHandler.h"
#include "ofUtils.h"
#include "ofEvents.h"


namespace ofx {


class SSLManager
{
public:
    static void initializeClient(Poco::Net::Context::Ptr pContext = 0);
    static void initializeServer(Poco::Net::Context::Ptr pContext = 0);

    template<class ListenerClass>
    static void registerAllEvents(ListenerClass* listener)
    {
        SSLManager::instance(); // make sure it is instantiated
        Poco::Net::SSLManager& manager = Poco::Net::SSLManager::instance();

        manager.ServerVerificationError      += Poco::delegate(listener, &ListenerClass::onServerVerificationError);
        manager.ClientVerificationError      += Poco::delegate(listener, &ListenerClass::onClientVerificationError);
        manager.PrivateKeyPassphraseRequired += Poco::delegate(listener, &ListenerClass::onPrivateKeyPassphraseRequired);
    }

    template<class ListenerClass>
    static void unregisterAllEvents(ListenerClass* listener)
    {
        SSLManager::instance(); // make sure it is instantiated
        Poco::Net::SSLManager& manager = Poco::Net::SSLManager::instance();

        manager.ServerVerificationError      -= Poco::delegate(listener, &ListenerClass::onServerVerificationError);
        manager.ClientVerificationError      -= Poco::delegate(listener, &ListenerClass::onClientVerificationError);
        manager.PrivateKeyPassphraseRequired -= Poco::delegate(listener, &ListenerClass::onPrivateKeyPassphraseRequired);
    }

    template<class ListenerClass>
    static void registerClientEvents(ListenerClass* listener)
    {
        SSLManager::instance(); // make sure it is instantiated
        Poco::Net::SSLManager& manager = Poco::Net::SSLManager::instance();

        manager.ClientVerificationError      += Poco::delegate(listener, &ListenerClass::onClientVerificationError);
        manager.PrivateKeyPassphraseRequired += Poco::delegate(listener, &ListenerClass::onPrivateKeyPassphraseRequired);
    }

    template<class ListenerClass>
    static void unregisterClientEvents(ListenerClass* listener)
    {
        SSLManager::instance(); // make sure it is instantiated
        Poco::Net::SSLManager& manager = Poco::Net::SSLManager::instance();

        manager.ClientVerificationError      -= Poco::delegate(listener, &ListenerClass::onClientVerificationError);
        manager.PrivateKeyPassphraseRequired -= Poco::delegate(listener, &ListenerClass::onPrivateKeyPassphraseRequired);
    }

    template<class ListenerClass>
    static void registerServerEvents(ListenerClass* listener)
    {
        SSLManager::instance(); // make sure it is instantiated
        Poco::Net::SSLManager& manager = Poco::Net::SSLManager::instance();

        manager.ServerVerificationError      += Poco::delegate(listener, &ListenerClass::onServerVerificationError);
        manager.PrivateKeyPassphraseRequired += Poco::delegate(listener, &ListenerClass::onPrivateKeyPassphraseRequired);
    }

    template<class ListenerClass>
    static void unregisterServerEvents(ListenerClass* listener)
    {
        SSLManager::instance(); // make sure it is instantiated
        Poco::Net::SSLManager& manager = Poco::Net::SSLManager::instance();

        manager.ServerVerificationError      -= Poco::delegate(listener, &ListenerClass::onServerVerificationError);
        manager.PrivateKeyPassphraseRequired -= Poco::delegate(listener, &ListenerClass::onPrivateKeyPassphraseRequired);
    }

private:
    SSLManager();
    SSLManager(const SSLManager&);
	SSLManager& operator = (const SSLManager&);

	~SSLManager();

    static SSLManager& instance();

    void onServerVerificationError(const void* pSender, Poco::Net::VerificationErrorArgs& args);
    void onClientVerificationError(const void* pSender, Poco::Net::VerificationErrorArgs& args);
    void onPrivateKeyPassphraseRequired(const void* pSender, std::string& args);

    bool _clientContextInitialized;
    bool _serverContextInitialized;

    static const std::string DEFAULT_CA_LOCATION;
    static const std::string DEFAULT_PRIVATE_KEY_FILE;
    static const std::string DEFAULT_CERTIFICATE_FILE;

    ///       <openSSL>
	///          <server|client>
	///            <privateKeyFile>mycert.key</privateKeyFile>
	///            <certificateFile>mycert.crt</certificateFile>
	///            <caConfig>rootcert.pem</caConfig>
	///            <verificationMode>none|relaxed|strict|once</verificationMode>
	///            <verificationDepth>1..9</verificationDepth>
	///            <loadDefaultCAFile>true|false</loadDefaultCAFile>
	///            <cipherList>ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH</cipherList>
	///            <privateKeyPassphraseHandler>
	///                <name>KeyFileHandler</name>
	///                <options>
	///                    <password>test</password>
	///                </options>
	///            </privateKeyPassphraseHandler>
	///            <invalidCertificateHandler>
	///                 <name>ConsoleCertificateHandler</name>
	///            </invalidCertificateHandler>
	///            <cacheSessions>true|false</cacheSessions>
	///            <sessionIdContext>someString</sessionIdContext> <!-- server only -->
	///            <sessionCacheSize>0..n</sessionCacheSize>       <!-- server only -->
	///            <sessionTimeout>0..n</sessionTimeout>           <!-- server only -->
	///            <extendedVerification>true|false</extendedVerification>
	///            <requireTLSv1>true|false</requireTLSv1>
	///          </server|client>
	///          <fips>false</fips>
	///       </openSSL>

    friend class Poco::SingletonHolder<SSLManager>;

};


} // namespace ofx
