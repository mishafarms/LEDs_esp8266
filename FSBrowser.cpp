/* 
 FSWebServer - Example WebServer with SPIFFS backend for esp8266
 Copyright (c) 2015 Hristo Gochkov. All rights reserved.
 This file is part of the ESP8266WebServer library for Arduino environment.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 
 upload the contents of the data folder with MkSPIFFS Tool ("ESP8266 Sketch Data Upload" in Tools menu in Arduino IDE)
 or you can upload the contents of a folder if you CD in that folder and run the following command:
 for file in `ls -A1`; do curl -F "file=@$PWD/$file" esp8266fs.local/edit; done
 
 access the sample web page at http://esp8266fs.local
 edit the page by going to http://esp8266fs.local/edit
 */
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <ArduinoJson.h>

extern void ledsOff(void);

#define htonl(x) (((x & 0xff) << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8) | ((x & 0xff000000) >> 24))

#define DBG_OUTPUT_PORT Serial

typedef struct wifiMultEntry {
	String ssid;
	String password;
} WifiMultiEntry;

typedef struct wifiConfig {
	String ApHostname;
	String ApPassword;
	std::vector <WifiMultiEntry> multiEntries;
	String mdnsHostname;
} WifiConfig;

WifiConfig wifiConf;

ESP8266WiFiMulti wifiMulti;

char host[30];

ESP8266WebServer server(80);

//holds the current upload
File fsUploadFile;

//format bytes
String formatBytes(size_t bytes) {
	if (bytes < 1024) {
		return String(bytes) + "B";
	} else if (bytes < (1024 * 1024)) {
		return String(bytes / 1024.0) + "KB";
	} else if (bytes < (1024 * 1024 * 1024)) {
		return String(bytes / 1024.0 / 1024.0) + "MB";
	} else {
		return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
	}
}

String getContentType(String filename) {
	if (server.hasArg("download"))
		return "application/octet-stream";
	else if (filename.endsWith(".htm"))
		return "text/html";
	else if (filename.endsWith(".html"))
		return "text/html";
	else if (filename.endsWith(".css"))
		return "text/css";
	else if (filename.endsWith(".js"))
		return "application/javascript";
	else if (filename.endsWith(".png"))
		return "image/png";
	else if (filename.endsWith(".gif"))
		return "image/gif";
	else if (filename.endsWith(".jpg"))
		return "image/jpeg";
	else if (filename.endsWith(".ico"))
		return "image/x-icon";
	else if (filename.endsWith(".xml"))
		return "text/xml";
	else if (filename.endsWith(".pdf"))
		return "application/x-pdf";
	else if (filename.endsWith(".zip"))
		return "application/x-zip";
	else if (filename.endsWith(".gz"))
		return "application/x-gzip";
	return "text/plain";
}

bool handleFileRead(String path) {
  static uint32_t apSubnet = 0;
  uint32_t clIp = server.client().remoteIP();
  bool apClient = false;

  if (apSubnet == 0)
  {
    apSubnet = WiFi.softAPIP();
    apSubnet = htonl(apSubnet) & 0xffffff00;
  }
  
	DBG_OUTPUT_PORT.println("handleFileRead: " + path);

  // this says the remote is in the 192.168.4.0 subnet
  
  if ((htonl(clIp) & 0xffffff00) == apSubnet) {
    DBG_OUTPUT_PORT.println("From an AP client");
    apClient = true;
  }
  
	if (path.endsWith("/"))
		path += "index.htm";
	String contentType = getContentType(path);
	String pathWithGz = path + ".gz";

  if (apClient)
  {
    /* if we are talking to an AP client maybe give him different files if they exist.
     * prepend an '/ap' to the path and try with that. The '/ap' isn't really a directory 
     * it is just the name of the file.
     * If there is no file try without.
     */
     
    if (SPIFFS.exists("/ap" + pathWithGz) || SPIFFS.exists("/ap" + path)) {
      pathWithGz = "/ap" + pathWithGz;
      path = "/ap" + path;
    }
  }
  
	if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
		if (SPIFFS.exists(pathWithGz))
			path += ".gz";
		File file = SPIFFS.open(path, "r");
		size_t sent = server.streamFile(file, contentType);
		file.close();
		return true;
	}
	return false;
}

void handleFileUpload() {
	if (server.uri() != "/edit")
		return;
	HTTPUpload& upload = server.upload();
	if (upload.status == UPLOAD_FILE_START) {
		String filename = upload.filename;
		if (!filename.startsWith("/"))
			filename = "/" + filename;
		DBG_OUTPUT_PORT.print("handleFileUpload Name: ");
		DBG_OUTPUT_PORT.println(filename);
		fsUploadFile = SPIFFS.open(filename, "w");
		filename = String();
	} else if (upload.status == UPLOAD_FILE_WRITE) {
		//DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
		if (fsUploadFile)
			fsUploadFile.write(upload.buf, upload.currentSize);
	} else if (upload.status == UPLOAD_FILE_END) {
		if (fsUploadFile)
			fsUploadFile.close();
		DBG_OUTPUT_PORT.print("handleFileUpload Size: ");
		DBG_OUTPUT_PORT.println(upload.totalSize);
	}
}

void handleFileDelete() {
	if (server.args() == 0)
		return server.send(500, "text/plain", "BAD ARGS");
	String path = server.arg(0);
	DBG_OUTPUT_PORT.println("handleFileDelete: " + path);
	if (path == "/")
		return server.send(500, "text/plain", "BAD PATH");
	if (!SPIFFS.exists(path))
		return server.send(404, "text/plain", "FileNotFound");
	SPIFFS.remove(path);
	server.send(200, "text/plain", "");
	path = String();
}

void handleFileCreate() {
	if (server.args() == 0)
		return server.send(500, "text/plain", "BAD ARGS");
	String path = server.arg(0);
	DBG_OUTPUT_PORT.println("handleFileCreate: " + path);
	if (path == "/")
		return server.send(500, "text/plain", "BAD PATH");
	if (SPIFFS.exists(path))
		return server.send(500, "text/plain", "FILE EXISTS");
	File file = SPIFFS.open(path, "w");
	if (file)
		file.close();
	else
		return server.send(500, "text/plain", "CREATE FAILED");
	server.send(200, "text/plain", "");
	path = String();
}

void handleFileList() {
	if (!server.hasArg("dir")) {
		server.send(500, "text/plain", "BAD ARGS");
		return;
	}

	String path = server.arg("dir");
	DBG_OUTPUT_PORT.println("handleFileList: " + path);
	Dir dir = SPIFFS.openDir(path);
	path = String();

	String output = "[";
	while (dir.next()) {
		File entry = dir.openFile("r");
		if (output != "[")
			output += ',';
		bool isDir = false;
		output += "{\"type\":\"";
		output += (isDir) ? "dir" : "file";
		output += "\",\"name\":\"";
		output += String(entry.name()).substring(1);
		output += "\"}";
		entry.close();
	}

	output += "]";
	server.send(200, "text/json", output);
}

void handleSsidGet(void) {
	String output = "{\"SSID\":\"";
	output += WiFi.SSID();
	output += "\"}";
	server.send(200, "text/json", output);
}

void addMac(String &hostName, uint8_t *mac) {
	size_t index = 0;
	int x;
	
	for (x = 0 ; x < 6 ; x++)
	{
		char tmpBuf[10];
		char hexBuf[10];
		index = 0;
		
		sprintf(tmpBuf, "\%m%d", x);
		sprintf(hexBuf, "%02x", mac[x]);
		/* Make the replacement. */
		hostName.replace(tmpBuf, hexBuf);
	}
}

void setupAp(void) {
	String ApName;
	uint8_t mac[6];
	
	// I should have done this before so that we can support AP and STA mode together
	//	WiFi.mode(WIFI_AP);

	WiFi.softAPmacAddress(mac);

	if (wifiConf.ApHostname == "")
	{
		ApName = String("espLights-%m4%m5");
	}
	else
	{
		ApName = wifiConf.ApHostname;
	}
	addMac(ApName, mac);

	if (wifiConf.ApPassword != "")
	{
		DBG_OUTPUT_PORT.print("Setting up AP ");
		DBG_OUTPUT_PORT.print(ApName);
		DBG_OUTPUT_PORT.print(" password ");
		DBG_OUTPUT_PORT.println(wifiConf.ApPassword);
		
		WiFi.softAP(ApName.c_str(), wifiConf.ApPassword.c_str());		
	}
	else
	{
		WiFi.softAP(ApName.c_str());
	}
}

static const char *configFilename = "/wifi.json";
static const char *tmpConfigFilename = "/wifi.tmp";
static const char *configBackFilename = "/wifi.bak.json";

bool readWifiConfig(WifiConfig *pConfig) {
	bool status = false;
	int configSize = 0;

	if (!SPIFFS.exists(configFilename)) {
		// if it doesn't exist then return false

		DBG_OUTPUT_PORT.print("Config file ");
		DBG_OUTPUT_PORT.print(configFilename);
		DBG_OUTPUT_PORT.println(" doesn't exist");
		return false;
	}

	// file exists, open it

	File configFile = SPIFFS.open(configFilename, "r");

	if (!configFile) {
		// if we can't open it then return false
		DBG_OUTPUT_PORT.print("Config file ");
		DBG_OUTPUT_PORT.print(configFilename);
		DBG_OUTPUT_PORT.println(" open failed");
		return false;
	}

	// get the size of the file

	configSize = configFile.size();

	DynamicJsonBuffer jsonBuffer;

	// allocate space for the file

	char *configJson = (char *) calloc(configSize, 1);

	// read in the file

	if (configFile.readBytes(configJson, configSize) == 0) {
		DBG_OUTPUT_PORT.print("Config file ");
		DBG_OUTPUT_PORT.print(configFilename);
		DBG_OUTPUT_PORT.print(" read failed size ");
		DBG_OUTPUT_PORT.println(configSize);
		goto cleanup;
	} else {
		// we have hopefully a JSON string in memory, we need to parse it

		JsonObject& wifiJson = jsonBuffer.parseObject(configJson);

		if (wifiJson.success()) {
			// we should be able to do cool things here. But for now let's print it

			wifiJson.prettyPrintTo(Serial);
			Serial.println();
			
			// see if there is an ApHostname
			
			if (wifiJson.containsKey("ApHostname"))
			{
				// read it in.
				
				pConfig->ApHostname = wifiJson["ApHostname"].asString();
			}

			if (wifiJson.containsKey("ApPassword"))
			{
				// read it in.
				
				pConfig->ApPassword = wifiJson["ApPassword"].asString();
			}

			JsonArray& hosts = wifiJson["WifiMulti"];

			for (auto& jhost : hosts) {
				WifiMultiEntry entry;
				

				DBG_OUTPUT_PORT.print("The ssid = ");
				entry.ssid = jhost["ssid"].asString();

				DBG_OUTPUT_PORT.print(entry.ssid);
				DBG_OUTPUT_PORT.print(" and the password = ");

				entry.password = jhost["password"].asString();
				DBG_OUTPUT_PORT.println(entry.password);
				pConfig->multiEntries.push_back(entry);
			}

			if (wifiJson.containsKey("mdnsHostname"))
			{
				// read it in.
				
				pConfig->mdnsHostname = wifiJson["mdnsHostname"].asString();
			}

			// we parsed it and stored it in the passed in struct, return true

			status = true;
			goto cleanup;
		}
		else {
			// failed to parse, return false
			DBG_OUTPUT_PORT.print("Config file ");
			DBG_OUTPUT_PORT.print(configFilename);
			DBG_OUTPUT_PORT.println(" failed to parse JSON");
			goto cleanup;
		}
	}

cleanup:
	configFile.close();
	
	free(configJson);
	return status;
}

bool writeWifiConfig(WifiConfig *pConfig) {
	// so now we have to build a jsonObject
	
	DynamicJsonBuffer jsonBuffer;
	JsonObject& wifiJson = jsonBuffer.createObject();
	
	if (pConfig->ApHostname != "")
	{
		wifiJson["ApHostname"] = pConfig->ApHostname;
		
		if (pConfig->ApPassword != "")
		{
			wifiJson["ApPassword"] = pConfig->ApPassword;			
		}
	}
	
	if (pConfig->multiEntries.size() != 0)
	{
		JsonArray& wifiMulti = wifiJson.createNestedArray("WifiMulti");
		for (auto entry : pConfig->multiEntries)
		{
			JsonObject& multiJson = wifiMulti.createNestedObject();
			
			multiJson["ssid"] = entry.ssid;
			multiJson["password"] = entry.password;
		}
	}
	
	if (pConfig->mdnsHostname != "")
	{
		wifiJson["mdnsHostname"] = pConfig->mdnsHostname;
	}
	
	wifiJson.prettyPrintTo(Serial);
	Serial.println("");
	
	// so now we should write this out to a file
	// then rename the original to name.bak
	// then rename our file to name
	
	// create the file or truncate it

	File configFile = SPIFFS.open(tmpConfigFilename, "w");

	if (!configFile) {
		// if we can't open it then return false
		DBG_OUTPUT_PORT.print("Write Config file ");
		DBG_OUTPUT_PORT.print(tmpConfigFilename);
		DBG_OUTPUT_PORT.println(" open failed");
		return false;
	}

	// write to the file
	
	wifiJson.printTo(configFile);
	configFile.close();

	// now I want to move the old file out of the way

	SPIFFS.rename(configFilename, configBackFilename);
	SPIFFS.rename(tmpConfigFilename, configFilename);
	
	DBG_OUTPUT_PORT.println("Write Config file OK");
	return true;
}

const char* updateIndex =
		"<form method='POST' action='/update' enctype='multipart/form-data'><input type='file'"
				"name='update'><input type='submit' value='Update'></form>";

void FSBsetup(void) {
	DBG_OUTPUT_PORT.print("\n");
	DBG_OUTPUT_PORT.setDebugOutput(true);

	//SPIFFS.begin(); 
	{
		Dir dir = SPIFFS.openDir("/");
		DBG_OUTPUT_PORT.printf("about to read dir\n");
		while (dir.next()) {
			String fileName = dir.fileName();
			size_t fileSize = dir.fileSize();
			DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n", fileName.c_str(),
					formatBytes(fileSize).c_str());
		}
		DBG_OUTPUT_PORT.printf("\n");
	}

	if (readWifiConfig(&wifiConf) != true) {
		// I am not sure what I want to do here, I expect that going into AP mode
		// and letting the system be managed by the webpage at 192.168.4.1 is ok.
		DBG_OUTPUT_PORT.println("AP mode.");
		WiFi.mode(WIFI_AP);

		setupAp();
	}
	else
	{
		// look at what wifi mode we want
		if (wifiConf.ApHostname != "")
		{
			if (wifiConf.multiEntries.size() != 0)
			{
				// we are doing both

				DBG_OUTPUT_PORT.println("AP_STA mode.");
				WiFi.mode(WIFI_AP_STA);
			}
			else
			{
				// seems we only want AP mode
				
				DBG_OUTPUT_PORT.println("AP mode.");
				WiFi.mode(WIFI_AP);
			}
		}
		else if (wifiConf.multiEntries.size() != 0) {
			// seems we only want STA  mode
			
			DBG_OUTPUT_PORT.println("STA mode.");
			WiFi.mode(WIFI_STA);
		}
		
		if (wifiConf.multiEntries.size() != 0) {
			// process the multiEntries
			
			if (wifiConf.ApHostname != "")
			{
				// We also want an AP
				
				setupAp();
			}
			
			for( auto entry : wifiConf.multiEntries ) {
				// add each entry into the WiFi.multi
			    wifiMulti.addAP(entry.ssid.c_str(), entry.password.c_str());	
			}

			DBG_OUTPUT_PORT.println("Connecting Wifi...");

			while (wifiMulti.run() != WL_CONNECTED) {
				delay(500);
				DBG_OUTPUT_PORT.print(".");
			}

			DBG_OUTPUT_PORT.print("Connected to SSID ");
			DBG_OUTPUT_PORT.println(WiFi.SSID());
			DBG_OUTPUT_PORT.print("IP address: ");
			DBG_OUTPUT_PORT.println(WiFi.localIP());

			String mdnsName;
			
			if (wifiConf.mdnsHostname == "")
			{
				mdnsName = "espLights";
			}
			else
			{
				mdnsName = wifiConf.mdnsHostname;
			}
			
			uint8_t mac[6];
			
			WiFi.macAddress(mac);

			addMac(mdnsName, mac);

			if (MDNS.begin(mdnsName.c_str())) {
				MDNS.addService("http", "tcp", 80);
			}

      DBG_OUTPUT_PORT.print("Open http://");
      DBG_OUTPUT_PORT.print(mdnsName);
      DBG_OUTPUT_PORT.println(".local/edit to see the file browser");
		}
		else {
			// we have no SSIDs to connect to, What to do? I know setup in Ap mode
			
			DBG_OUTPUT_PORT.println("No SSIDs where in the config, starting as an AP");
			setupAp();
		}
	}
	
	// either way we should either have an AP or an STA that can do web stuff

	//SERVER INIT
	//list directory
	server.on("/list", HTTP_GET, handleFileList);
	//load editor
	server.on("/edit", HTTP_GET,
			[]() {
				if(!handleFileRead("/edit.htm")) server.send(404, "text/plain", "FileNotFound");
			});
	//create file
	server.on("/edit", HTTP_PUT, handleFileCreate);
	//delete file
	server.on("/edit", HTTP_DELETE, handleFileDelete);
	//first callback is called after the request has ended with all parsed arguments
	//second callback handles file uploads at that location
	server.on("/edit", HTTP_POST, []() {server.send(200, "text/plain", "");},
			handleFileUpload);

	server.on("/ssid", HTTP_GET, handleSsidGet);

	//called when the url is not defined here
	//use it to load content from SPIFFS
	server.onNotFound([]() {
		if(!handleFileRead(server.uri()))
		server.send(404, "text/plain", "FileNotFound");
	});

  server.on("/restart", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "OK");
    ledsOff();
    ESP.restart();
  });
  
	// setup to get program updates as well.
	server.on("/up", HTTP_GET, []() {
		server.sendHeader("Connection", "close");
		server.sendHeader("Access-Control-Allow-Origin", "*");
		server.send(200, "text/html", updateIndex);
	});
 
	server.on("/update", HTTP_POST, []() {
		server.sendHeader("Connection", "close");
		server.sendHeader("Access-Control-Allow-Origin", "*");
		server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
		ESP.restart();
	}, []() {
		HTTPUpload& upload = server.upload();
		if(upload.status == UPLOAD_FILE_START) {
			Serial.setDebugOutput(true); 
			// turn off leds
			ledsOff();
			WiFiUDP::stopAll();
			Serial.printf("Update: %s\n", upload.filename.c_str());
			uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
			if(!Update.begin(maxSketchSpace)) { //start with max available size
				Update.printError(Serial);
			}
		} else if(upload.status == UPLOAD_FILE_WRITE) {
			if(Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
				Update.printError(Serial);
			}
		} else if(upload.status == UPLOAD_FILE_END) {
			if(Update.end(true)) { //true to set the size to the current progress
				Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
			} else {
				Update.printError(Serial);
			}
			Serial.setDebugOutput(false);
		}
		yield();
	});
	server.begin();
	DBG_OUTPUT_PORT.println("HTTP server started");

}

void FSBloop(void) {
	server.handleClient();
}

