# include "StaticFsWebService.h"

StaticFsWebService::StaticFsWebService(WebServer *server) : server(server) { }

StaticFsWebService::~StaticFsWebService() { }

String StaticFsWebService::getMimeType(String fileName){
	if( fileName.endsWith(".html")
		|| fileName.endsWith(".htm")
		|| fileName.endsWith(".cshtml")) {
		return "text/html";
	}
	else if (fileName.endsWith(".js")) {
		return "application/javascript";
	}
	else if (fileName.endsWith("json")) {
		return "application/json";
	}
	else if (fileName.endsWith(".css")) {
		return "text/css";
	}
	return "text/plain";
}

void StaticFsWebService::begin(String rootDir) {
	Serial.println("Static FS WebService initializing...");
	Serial.print("Registering root directory: ");
	Serial.println(rootDir);

	File dir = SPIFFS.open(rootDir);
  
	if(!dir) {
		Serial.print("Failed to open directory ");
		Serial.println(rootDir);
		return;
	}
	if(!dir.isDirectory()){
		Serial.print(rootDir);
		Serial.println(" is not a directory!");
		return;
	}

	File file = dir.openNextFile();
  	while (file) {
		String path = file.path();
		String url = path;
		String mime = getMimeType(path);
		url.replace(rootDir, "");

		Serial.print(path);
		Serial.print("\t");
		Serial.println(url);

		server->serveStatic(url.c_str(), SPIFFS, path.c_str());

		if (mime == "text/html") {
			if (url.endsWith(".html")) {
				url = url.substring(0, url.length() - 5);
			} 
			else if (url.endsWith(".htm")) {
				url = url.substring(0, url.length() - 4);
			}
			else if (url.endsWith(".cshtml")) {
				url = url.substring(0, url.length() - 7);
			}
			server->serveStatic(url.c_str(), SPIFFS, path.c_str());
			Serial.print(path);
			Serial.print("\t");
			Serial.println(url);

			if (url.endsWith("index")
				|| url.endsWith("Index")) {
				url = url.substring(0, url.length() - 5);
				server->serveStatic(url.c_str(), SPIFFS, path.c_str());
				Serial.print(path);
				Serial.print("\t");
				Serial.println(url);
			}
		}
    	file = dir.openNextFile();
  	}
}