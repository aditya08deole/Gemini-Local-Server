// Developed by Aditya Deole
// deoleadi@gmail.com
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <U8g2lib.h>

// Create display object for SH1106 128x64 I2C OLED
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

// Wi-Fi credentials
const char* ssid = "Aditya08";
const char* password = "heyheyhey";

// Your Gemini API key (replace with your actual key!)
const char* Gemini_Token = "Add your gemini API Key";
const char* Gemini_Max_Tokens = "50";

// System prompt for EDITH
const String SYSTEM_PROMPT = "You are EDITH (Enhanced Defense Intelligence Terminal Hub), an AI assistant integrated into smart glasses. Your responses must be professional, concise, and optimized for small screen display. Provide only the most important information in exactly 60 words or less. Be direct, helpful, and maintain a professional tone suitable for a high-tech assistant.";

void sendToGemini(String prompt); 
String truncateToWords(String text, int maxWords);
String filterSpecialCharacters(String text);
void displayOnOLED(String text, String type);
void displayWrappedText(String text, int startY);
void displayWelcomeScreen();
void displayProcessing();
void displayResponseInTwoParts(String response);
String enhanceUserQuery(String originalQuery);

WiFiServer server(80);
String gemini_Q;
String gemini_A;
String fullResponse = ""; // Store the full 30-word response
bool isNewQuestion = false;

// Display control variables
unsigned long lastDisplaySwitch = 0;
const unsigned long DISPLAY_INTERVAL = 5000; // 5 seconds
bool showingFirstPart = true;
String currentFirstPart = "";
String currentSecondPart = "";
bool hasResponse = false;

// OLED display variables - Simplified
const int MAX_CHARS_PER_LINE = 21; // Characters per line for smaller font
const int LINE_HEIGHT = 8; // Reduced height between lines for smaller font
const int MAX_LINES = 7; // Maximum lines on screen

// Enhanced EDITH Sci-Fi HTML Page
const char html_page[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta charset="UTF-8">
  <title>EDITH - Enhanced Defense Intelligence Terminal Hub</title>
  <link rel="icon" href="data:image/svg+xml,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24' fill='%2300ffff'><circle cx='12' cy='12' r='10'/><path d='M12 6v6l4 2'/></svg>" type="image/svg+xml">
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }
    
    body {
      font-family: 'Courier New', monospace;
      background: linear-gradient(45deg, #FBFBFB, #E8F9FF, #C4D9FF);
      color: #2D3748;
      min-height: 100vh;
      overflow-x: hidden;
      animation: backgroundShift 10s ease-in-out infinite alternate;
    }
    
    @keyframes backgroundShift {
      0% { background: linear-gradient(45deg, #FBFBFB, #E8F9FF, #C4D9FF); }
      100% { background: linear-gradient(45deg, #C4D9FF, #FBFBFB, #E8F9FF); }
    }
    
    .container {
      max-width: 1000px;
      margin: 0 auto;
      padding: 20px;
      position: relative;
    }
    
    .header {
      text-align: center;
      margin-bottom: 40px;
      position: relative;
    }
    
    .logo {
      font-size: 3em;
      font-weight: bold;
      color: #5A67D8;
      text-shadow: 0 0 20px #C5BAFF, 0 0 40px #C5BAFF, 0 0 60px #C5BAFF;
      animation: pulse 2s ease-in-out infinite;
      margin-bottom: 10px;
    }
    
    @keyframes pulse {
      0%, 100% { opacity: 1; transform: scale(1); }
      50% { opacity: 0.8; transform: scale(1.05); }
    }
    
    .subtitle {
      font-size: 1.2em;
      color: #5A67D8;
      text-transform: uppercase;
      letter-spacing: 3px;
      margin-bottom: 5px;
    }
    
    .tagline {
      font-size: 0.9em;
      color: #718096;
      font-style: italic;
    }
    
    .terminal-window {
      background: rgba(248, 251, 251, 0.9);
      border: 2px solid #C5BAFF;
      border-radius: 15px;
      box-shadow: 0 0 30px rgba(197, 186, 255, 0.3), inset 0 0 30px rgba(197, 186, 255, 0.1);
      padding: 30px;
      margin: 20px 0;
      position: relative;
      backdrop-filter: blur(10px);
    }
    
    .terminal-window::before {
      content: '';
      position: absolute;
      top: -2px;
      left: -2px;
      right: -2px;
      bottom: -2px;
      background: linear-gradient(45deg, #C5BAFF, #C4D9FF, #C5BAFF);
      border-radius: 15px;
      z-index: -1;
      animation: borderGlow 3s linear infinite;
    }
    
    @keyframes borderGlow {
      0%, 100% { opacity: 0.5; }
      50% { opacity: 1; }
    }
    
    .terminal-header {
      display: flex;
      align-items: center;
      margin-bottom: 25px;
      padding-bottom: 15px;
      border-bottom: 1px solid #C4D9FF;
    }
    
    .terminal-buttons {
      display: flex;
      gap: 8px;
      margin-right: 15px;
    }
    
    .terminal-button {
      width: 12px;
      height: 12px;
      border-radius: 50%;
      animation: buttonBlink 2s ease-in-out infinite;
    }
    
    .terminal-button.red { background: #ff5f56; }
    .terminal-button.yellow { background: #ffbd2e; }
    .terminal-button.green { background: #27ca3f; }
    
    @keyframes buttonBlink {
      0%, 90%, 100% { opacity: 1; }
      95% { opacity: 0.3; }
    }
    
    .terminal-title {
      color: #5A67D8;
      font-weight: bold;
      font-size: 1.1em;
    }
    
    .input-section {
      margin-bottom: 30px;
    }
    
    .input-label {
      display: block;
      color: #5A67D8;
      margin-bottom: 10px;
      font-weight: bold;
      text-transform: uppercase;
      letter-spacing: 1px;
    }
    
    .input-container {
      display: flex;
      gap: 10px;
      align-items: center;
    }
    
    .query-input {
      flex: 1;
      background: rgba(251, 251, 251, 0.9);
      border: 2px solid #C4D9FF;
      border-radius: 8px;
      padding: 15px 20px;
      color: #2D3748;
      font-family: 'Courier New', monospace;
      font-size: 16px;
      transition: all 0.3s ease;
    }
    
    .query-input:focus {
      outline: none;
      border-color: #C5BAFF;
      box-shadow: 0 0 15px rgba(197, 186, 255, 0.5);
      background: rgba(251, 251, 251, 1);
    }
    
    .submit-btn {
      background: linear-gradient(45deg, #C5BAFF, #5A67D8);
      border: none;
      border-radius: 8px;
      color: white;
      padding: 15px 25px;
      font-family: 'Courier New', monospace;
      font-weight: bold;
      font-size: 16px;
      cursor: pointer;
      transition: all 0.3s ease;
      text-transform: uppercase;
      letter-spacing: 1px;
    }
    
    .submit-btn:hover {
      background: linear-gradient(45deg, #5A67D8, #C5BAFF);
      transform: translateY(-2px);
      box-shadow: 0 5px 15px rgba(197, 186, 255, 0.4);
    }
    
    .response-section {
      margin-top: 30px;
    }
    
    .response-header {
      color: #5A67D8;
      font-weight: bold;
      margin-bottom: 15px;
      text-transform: uppercase;
      letter-spacing: 2px;
      display: flex;
      align-items: center;
      gap: 10px;
    }
    
    .response-indicator {
      width: 8px;
      height: 8px;
      background: #C5BAFF;
      border-radius: 50%;
      animation: responseIndicator 1s ease-in-out infinite;
    }
    
    @keyframes responseIndicator {
      0%, 100% { opacity: 1; transform: scale(1); }
      50% { opacity: 0.3; transform: scale(0.8); }
    }
    
    .response-content {
      background: rgba(196, 217, 255, 0.2);
      border-left: 4px solid #C5BAFF;
      padding: 20px;
      border-radius: 8px;
      color: #2D3748;
      font-size: 16px;
      line-height: 1.6;
      white-space: pre-wrap;
      word-wrap: break-word;
      animation: typewriter 0.5s ease-out;
    }
    
    @keyframes typewriter {
      from { opacity: 0; transform: translateY(10px); }
      to { opacity: 1; transform: translateY(0); }
    }
    
    .status-bar {
      position: fixed;
      bottom: 0;
      left: 0;
      right: 0;
      background: rgba(251, 251, 251, 0.95);
      border-top: 1px solid #C4D9FF;
      padding: 10px 20px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      font-size: 12px;
      color: #718096;
    }
    
    .status-item {
      display: flex;
      align-items: center;
      gap: 5px;
    }
    
    .status-dot {
      width: 6px;
      height: 6px;
      background: #68D391;
      border-radius: 50%;
      animation: statusBlink 2s ease-in-out infinite;
    }
    
    @keyframes statusBlink {
      0%, 100% { opacity: 1; }
      50% { opacity: 0.3; }
    }
    
    .grid-pattern {
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      background-image: 
        linear-gradient(rgba(196, 217, 255, 0.2) 1px, transparent 1px),
        linear-gradient(90deg, rgba(196, 217, 255, 0.2) 1px, transparent 1px);
      background-size: 50px 50px;
      pointer-events: none;
      z-index: -1;
      animation: gridMove 20s linear infinite;
    }
    
    @keyframes gridMove {
      0% { transform: translate(0, 0); }
      100% { transform: translate(50px, 50px); }
    }
    
    .oled-status {
      background: rgba(197, 186, 255, 0.1);
      border: 1px solid #C5BAFF;
      border-radius: 8px;
      padding: 15px;
      margin: 20px 0;
      text-align: center;
    }
    
    .oled-indicator {
      color: #5A67D8;
      font-weight: bold;
      margin-bottom: 5px;
    }
    
    @media (max-width: 768px) {
      .container { padding: 10px; }
      .logo { font-size: 2em; }
      .subtitle { font-size: 1em; letter-spacing: 2px; }
      .input-container { flex-direction: column; }
      .query-input, .submit-btn { width: 100%; }
      .status-bar { flex-direction: column; gap: 5px; text-align: center; }
    }
  </style>
</head>
<body>
  <div class="grid-pattern"></div>
  
  <div class="container">
    <div class="header">
      <div class="logo">EDITH</div>
      <div class="subtitle">Enhanced Interface For Transparent Holography</div>
      <div class="tagline">"Developed by Adi" - Powered by Gemini AI</div>
    </div>
    
    <div class="terminal-window">
      <div class="terminal-header">
        <div class="terminal-buttons">
          <div class="terminal-button red"></div>
          <div class="terminal-button yellow"></div>
          <div class="terminal-button green"></div>
        </div>
        <div class="terminal-title">Gemini AI Assistant + OLED Display v2.0</div>
      </div>
      
      <div class="oled-status">
        <div class="oled-indicator">📟 OLED Display Status: Active</div>
        <div style="font-size: 0.9em; color: #718096;">Questions and responses will be displayed on the 1.3" OLED screen in two 15-word parts</div>
      </div>
      
      <form method="POST" class="input-section">
        <label class="input-label">Enter Query:</label>
        <div class="input-container">
          <input type="text" name="geminitext" class="query-input" placeholder="Ask EDITH anything..." required autocomplete="off">
          <button type="submit" class="submit-btn">Execute</button>
        </div>
      </form>
      
      %RESPONSE%
    </div>
  </div>
  
  <div class="status-bar">
    <div class="status-item">
      <div class="status-dot"></div>
      <span>System Online</span>
    </div>
    <div class="status-item">
      <span>EDITH v2.0 | Gemini AI + OLED | Secure Connection</span>
    </div>
    <div class="status-item">
      <span>Ready for Commands</span>
    </div>
  </div>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  
  // Initialize OLED display
  u8g2.begin();
  displayWelcomeScreen();
  delay(3000);
  
  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  while (!Serial);
  
  // Show connecting status on OLED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x7_tr); // Smaller font
  u8g2.drawStr(0, 10, "EDITH Initializing...");
  u8g2.drawStr(0, 20, "Connecting to WiFi");
  u8g2.sendBuffer();
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Show connection success on OLED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x7_tr); // Smaller font
  u8g2.drawStr(0, 10, "EDITH Online!");
  u8g2.drawStr(0, 20, "WiFi Connected");
  u8g2.drawStr(0, 30, WiFi.localIP().toString().c_str());
  u8g2.drawStr(0, 40, "Ready for queries");
  u8g2.sendBuffer();

  server.begin();
  Serial.println("EDITH Server started successfully!");
  
  delay(2000);
  displayWelcomeScreen();
}

void loop() {
  // Handle OLED display cycling for responses
  handleDisplayCycling();
  
  // Handle web server
  WiFiClient client = server.available();

  if (client) {
    String request = "";
    int contentLength = 0;

    // Read the request headers
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;

        if (request.endsWith("\r\n\r\n")) break;
      }
    }

    // Handle POST request
    if (request.startsWith("POST")) {
      int contentIndex = request.indexOf("Content-Length: ");
      if (contentIndex != -1) {
        int endIndex = request.indexOf("\r\n", contentIndex);
        contentLength = request.substring(contentIndex + 16, endIndex).toInt();
      }

      // Wait for the body
      while (client.available() < contentLength) delay(1);

      String body = "";
      while (client.available()) body += (char)client.read();

      // Parse the form data
      int paramIndex = body.indexOf("geminitext=");
      if (paramIndex >= 0) {
        String rawQuery = body.substring(paramIndex + strlen("geminitext="));
        
        // URL decode
        rawQuery.replace('+', ' ');
        rawQuery.replace("%3F", "?");
        rawQuery.replace("%21", "!");
        rawQuery.replace("%2C", ",");
        rawQuery.replace("%27", "'");
        rawQuery.replace("%22", "\"");
        rawQuery.replace("%20", " ");
        rawQuery.replace("%40", "@");
        rawQuery.replace("%23", "#");
        rawQuery.replace("%24", "$");
        rawQuery.replace("%25", "%");
        rawQuery.replace("%26", "&");
        rawQuery.replace("%2B", "+");
        rawQuery.replace("%3D", "=");
        
        // Enhance the user query with context
        gemini_Q = enhanceUserQuery(rawQuery);
        
        isNewQuestion = true;
      }
    }

    // Process new question
    if (isNewQuestion) {
      // Display original question on OLED (not the enhanced version)
      String originalQuery = gemini_Q.substring(gemini_Q.indexOf("User Query: ") + 12);
      displayOnOLED(originalQuery, "QUESTION");
      
      // Show processing animation
      displayProcessing();
      
      // Send enhanced query to Gemini
      sendToGemini(gemini_Q);
      
      // Prepare answer for OLED display cycling
      if (fullResponse.length() > 0) {
        prepareResponseForDisplay(fullResponse);
        hasResponse = true;
        showingFirstPart = true;
        lastDisplaySwitch = millis();
      }
      
      isNewQuestion = false;
    }

    // Generate HTML Response
    String htmlResponse = html_page;
    if (gemini_A.length() > 0) {
      String responseHtml = "<div class=\"response-section\">";
      responseHtml += "<div class=\"response-header\">";
      responseHtml += "<div class=\"response-indicator\"></div>";
      responseHtml += "EDITH Response:";
      responseHtml += "</div>";
      responseHtml += "<div class=\"response-content\">" + gemini_A + "</div>";
      responseHtml += "</div>";
      htmlResponse.replace("%RESPONSE%", responseHtml);
    } else {
      htmlResponse.replace("%RESPONSE%", "");
    }

    // Send HTTP response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
    client.println(htmlResponse);
    client.stop();
  }
}

// ==========================
// Enhanced Query Function - NEW
// ==========================

String enhanceUserQuery(String originalQuery) {
  String enhancedQuery = SYSTEM_PROMPT + "\n\n";
  enhancedQuery += "Context: This query is being processed for display on smart glasses with a small OLED screen. ";
  enhancedQuery += "The response must be extremely concise and focused on essential information only.\n\n";
  enhancedQuery += "User Query: " + originalQuery + "\n\n";
  enhancedQuery += "Please provide a professional, concise response in exactly 30 words that directly answers the question.";
  
  return enhancedQuery;
}

// ==========================
// Display Management Functions - NEW
// ==========================

void handleDisplayCycling() {
  if (!hasResponse) {
    return; // No response to display
  }
  
  unsigned long currentTime = millis();
  
  // Check if it's time to switch display
  if (currentTime - lastDisplaySwitch >= DISPLAY_INTERVAL) {
    if (currentSecondPart.length() > 0) {
      showingFirstPart = !showingFirstPart; // Toggle between parts
    }
    lastDisplaySwitch = currentTime;
    
    // Update display
    if (showingFirstPart) {
      displayResponsePart(currentFirstPart, "1/2");
    } else {
      displayResponsePart(currentSecondPart, "2/2");
    }
  }
}

void prepareResponseForDisplay(String response) {
  // Split response into words
  int spaceCount = 0;
  int splitIndex = 0;

  // Find the 15th space (approximate midpoint)
  for (int i = 0; i < response.length(); i++) {
    if (response.charAt(i) == ' ') {
      spaceCount++;
      if (spaceCount == 15) {
        splitIndex = i;
        break;
      }
    }
  }

  if (splitIndex > 0) {
    currentFirstPart = response.substring(0, splitIndex);
    currentSecondPart = response.substring(splitIndex + 1); // Skip the space
  } else {
    currentFirstPart = response;
    currentSecondPart = "";
  }
  
  // Display the first part immediately
  displayResponsePart(currentFirstPart, currentSecondPart.length() > 0 ? "1/2" : "");
}

void displayResponsePart(String text, String partIndicator) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x7_tr);
  
  String header = "A: ";
  if (partIndicator.length() > 0) {
    header += "(" + partIndicator + ")";
  }
  u8g2.drawStr(0, 10, header.c_str());
  
  displayWrappedText(text, 20);
}

// ==========================
// OLED Display Functions - Modified
// ==========================

void displayWelcomeScreen() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(25, 15, "  E.D.I.T.H");
  u8g2.setFont(u8g2_font_5x7_tr); // Smaller font
  u8g2.drawStr(5, 28, "Enhanced Interface for");
  u8g2.drawStr(0, 38, "Transparent Holography");
  u8g2.drawStr(35, 50, "EDITH V2");
  u8g2.sendBuffer();
}

void displayProcessing() {
  for (int i = 0; i < 3; i++) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_5x7_tr); // Smaller font
    u8g2.drawStr(15, 20, "Processing");
    
    String dots = "";
    for (int j = 0; j <= i; j++) {
      dots += ".";
    }
    u8g2.drawStr(85, 20, dots.c_str());
    
    u8g2.drawStr(10, 35, "Consulting with EDITH...");
    u8g2.sendBuffer();
    delay(500);
  }
}

void displayOnOLED(String text, String type) {
  // Reset response display when showing a new question
  if (type == "QUESTION") {
    hasResponse = false;
  }
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x7_tr); // Smaller font
  
  // Display header
  String header = (type == "QUESTION") ? "Q: " : "A: ";
  u8g2.drawStr(0, 10, header.c_str());
  
  // Display text with word wrapping
  displayWrappedText(text, 20); // Start at line 20
}

// Remove the old displayResponseInTwoParts function as it's replaced by the new system

void displayWrappedText(String text, int startY) {
  int y = startY;
  int lineCount = 0;
  const int maxLines = 6; // Maximum lines that fit on screen after header
  const int maxCharsPerLine = 25; // More characters per line with smaller font
  
  while (text.length() > 0 && lineCount < maxLines && y <= 60) {
    int len = min(maxCharsPerLine, (int)text.length());
    
    // Find the best place to break the line (at a space)
    int spacePos = -1;
    if (len < text.length()) {
      spacePos = text.lastIndexOf(' ', len);
      if (spacePos <= len / 2) spacePos = -1; // Space too early, just cut
    }
    
    String line;
    if (spacePos == -1) {
      line = text.substring(0, len);
      text = text.substring(len);
    } else {
      line = text.substring(0, spacePos);
      text = text.substring(spacePos + 1); // Skip the space
    }
    
    // Draw the line
    u8g2.drawStr(0, y, line.c_str());
    y += LINE_HEIGHT;
    lineCount++;
  }
  
  u8g2.sendBuffer();
}

// ==========================
// Send to Gemini Function - MODIFIED
// ==========================

void sendToGemini(String prompt) {
  Serial.println("Enhanced Query: " + prompt);

  HTTPClient https;
  
  if (https.begin("https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + String(Gemini_Token))) {
    https.addHeader("Content-Type", "application/json");
    
    // Escape the enhanced prompt for JSON
    String escapedPrompt = prompt;
    escapedPrompt.replace("\"", "\\\"");
    escapedPrompt.replace("\n", "\\n");
    escapedPrompt = "\"" + escapedPrompt + "\"";
    
    String payload = String("{\"contents\": [{\"parts\":[{\"text\":" + escapedPrompt + "}]}],\"generationConfig\": {\"maxOutputTokens\": " + String(Gemini_Max_Tokens) + "}}");
    
    Serial.println("Sending request to Gemini...");
    int httpCode = https.POST(payload);

    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String response = https.getString();
      Serial.println("Received response from Gemini.");

      // Parse JSON using ArduinoJson
      const size_t capacity = 4096;
      DynamicJsonDocument doc(capacity);
      DeserializationError error = deserializeJson(doc, response);

      if (!error) {
        String reply = doc["candidates"][0]["content"]["parts"][0]["text"];
        
        // Clean and filter the response
        reply.trim();
        String filteredReply = filterSpecialCharacters(reply);
        
        // Truncate to exactly 30 words
        fullResponse = truncateToWords(filteredReply, 30);
        gemini_A = fullResponse; // Keep for web display

        Serial.println("EDITH Answer: " + gemini_A);
      } else {
        fullResponse = "Error: EDITH neural network unavailable. Please check connection and try again later.";
        gemini_A = fullResponse;
        Serial.println("JSON Parse Error: " + String(error.c_str()));
      }
    } else {
      fullResponse = "Error: Unable to establish secure connection with EDITH core systems. Retry recommended.";
      gemini_A = fullResponse;
      Serial.printf("HTTP Error Code: %d\n", httpCode);
      Serial.println("Error: " + https.errorToString(httpCode));
    }

    https.end();
  } else {
    fullResponse = "Error: EDITH AI core offline. Please check network connectivity.";
    gemini_A = fullResponse;
    Serial.println("HTTPS connection failed.");
  }
}

// ==========================
// Filter Special Characters
// ==========================

String filterSpecialCharacters(String text) {
  String filteredText = "";
  for (size_t i = 0; i < text.length(); i++) {
    char c = text[i];
    if (isalnum(c) || isspace(c) || c == '.' || c == ',' || c == '!' || c == '?' || c == '\'' || c == '"' || c == '-' || c == ':' || c == ';') {
      filteredText += c;
    } else {
      filteredText += ' ';
    }
  }
  return filteredText;
}

// ==========================
// Truncate to Word Limit (Modified for exactly 30 words)
// ==========================

String truncateToWords(String text, int maxWords) {
  if (text.length() == 0) return text;
  
  int wordCount = 0;
  String result = "";
  String currentWord = "";
  
  for (int i = 0; i < text.length(); i++) {
    char c = text.charAt(i);
    
    if (c == ' ' || i == text.length() - 1) {
      if (currentWord.length() > 0) {
        if (wordCount > 0) result += " ";
        result += currentWord;
        wordCount++;
        currentWord = "";
        
        if (wordCount >= maxWords) {
          break;
        }
      }
    } else {
      currentWord += c;
    }
  }
  
  return result;
}
