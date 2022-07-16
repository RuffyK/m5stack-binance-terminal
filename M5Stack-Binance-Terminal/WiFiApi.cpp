#include "WiFiApi.h"
StaticJsonDocument<200> jsonBuf;
void useWiFiApi(WebServer   *server) {
	server->on("/api/wifi/status", HTTP_GET, [server]() {
    Serial.println("[WiFiApi] Get WiFi Status");
		server->send(200, "application/json", getWiFiStatus());
	});

	server->on("/api/wifi/scan", HTTP_GET, [server]() {
		bool hidden = server->hasArg("hidden");
    Serial.println("[WiFiApi] Get Available Networks");
		if (server->hasArg("hidden")) {
			hidden = server->arg("hidden") == "true";
		}
		server->send(200, "application/json", getAvailableNetworks(hidden));
	});
	server->on("/api/wifi/connect", HTTP_POST, [server]() {
    Serial.print("[WiFiApi] Connect to Network, ");
		String json = server->arg("plain");
    deserializeJson(jsonBuf, json);

		String ssid = jsonBuf["ssid"];
		String pw = jsonBuf["password"];

		Serial.print("SSID: ");
		Serial.println(ssid);

		WiFi.setAutoReconnect(false);
		WiFi.setAutoConnect(false);

		if (pw == "") {
			WiFi.begin(ssid.c_str());
		}
		else {
			WiFi.begin(ssid.c_str(), pw.c_str());
		}

		delay(500);

		bool success = false;

		for (int i = 0; i < 10; i++) {
			if (WiFi.status() == WL_CONNECTED) {
				success = true;
				break;
			}
			delay(1000);
		}

		if (success) {
			WiFi.setAutoReconnect(true);
			WiFi.setAutoConnect(true);
		}
		Serial.print("Success: ");
		Serial.println(success);

		server->send(200, "application/json", "{\"success\": " + String(success) + "}");

	});
}

String getWiFiStatus(void) {
	String json = "{\n  \"status\": \"";
	switch (WiFi.status()) {
	case WL_CONNECTED:
		json += "connected";
		break;
	case WL_CONNECTION_LOST:
		json += "connection lost";
		break;
	case WL_CONNECT_FAILED:
		json += "connecting failed";
		break;
	case WL_DISCONNECTED:
		json += "disconnected";
		break;
	case WL_SCAN_COMPLETED:
		json += "scanning completed";
		break;
	case WL_NO_SSID_AVAIL:
		json += "no ssid available";
		break;
	case WL_NO_SHIELD:
		json += "no shield";
		break;
	case WL_IDLE_STATUS:
		json += "idle";
		break;
	default:
		json += "unknown";
		break;
	}
	json += "\",\n";
	json += "  \"ip\": \"" + WiFi.localIP().toString() + "\",\n";
	json += "  \"ssid\": \"" + WiFi.SSID() + "\",\n";
	json += "  \"rssi\": " + String(WiFi.RSSI()) + "\n";
	json += "}";
	return json;
}

String getAvailableNetworks(bool showHidden) {
	String json = "[";
	delay(50);
	int n = WiFi.scanNetworks(false, showHidden);
	if (n > 0)
	{
		json += "\n";
		for (int i = 0; i < n; i++)
		{
			json += "  {\n    \"ssid\": \"" + WiFi.SSID(i) + "\",\n";
			json += "    \"rssi\": " + String(WiFi.RSSI(i)) + ",\n";
			char* encType;
			switch (WiFi.encryptionType(i))
			{
			case 0:
				encType = "none";
        break;
      case 1:
        encType = "wep";
        break;
      case 2:
        encType = "wpa_psk";
        break;
      case 3:
        encType = "wpa2_psk";
        break;
      case 4:
        encType = "wpa_wpa2_psk";
        break;
      case 5:
        encType = "wpa2_enterprise";
        break;
			default:
				encType = "unknown";
				break;
			}
			json += "    \"encryption\": \"" + String(encType) + "\"\n  }";
			if (i + 1 < n) {
				json += ",";
			}
			json += "\n";
		}
	}
	json += "]";
	WiFi.scanDelete();
	return json;
}
