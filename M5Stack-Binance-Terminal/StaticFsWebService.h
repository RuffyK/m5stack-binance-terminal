#ifndef RUFFY_STATIC_FS_WEB_SERVICE_H
#define RUFFY_STATIC_FS_WEB_SERVICE_H

#include <SPIFFS.h>
#include <FS.h>
#include <WebServer.h>

#define ROOT_PATH "/wwwroot"

class StaticFsWebService{
public:
	StaticFsWebService(WebServer *server);
	virtual ~StaticFsWebService();
	void begin(String rootDir);

private:
    WebServer *server;
	String getMimeType(String fileName);
};

#endif