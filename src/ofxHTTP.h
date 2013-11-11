#pragma once


#include "Poco/StreamCopier.h"

#include "ofx/SSLManager.h"
#include "Poco/LocalDateTime.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTime.h"
#include "Poco/Net/VerificationErrorArgs.h"

#include "ofx/HTTP/Server/FileSystem/BasicFileSystemServer.h"
#include "ofx/HTTP/Server/IPVideo/BasicIPVideoServer.h"

#include "ofx/HTTP/Authentication.h"

#include "ofx/HTTP/Client/BaseClient.h"
#include "ofx/HTTP/Client/BaseRequest.h"
#include "ofx/HTTP/Client/Context.h"
#include "ofx/HTTP/Client/GetRequest.h"
#include "ofx/HTTP/Client/PostRequest.h"
