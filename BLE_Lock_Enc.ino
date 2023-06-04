#include <esp_now.h>
#include <WiFi.h>

// Define and initialize an int relay to 26. GPIO26 will be used to control the relay.
int relay = 26;

uint8_t masterMacAddress[] = {0x08, 0xB6, 0x1F, 0x33, 0x36, 0xAC};

static const char* PMK_KEY = "123hjk9i0ii67bte";
static const char* LMK_KEY = "0ii67bte123hjk9i";

char pwd[64] = "Password";

typedef struct struct_message {
    char key[64];
} struct_message;

struct_message message;

void printMAC(const uint8_t * mac_addr){
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
}

bool comp(char key1[64], char key2[64])
{
  for (int i = 0; i < 64; i++)
  {
    if (key1[i] != key2[i])
    {
      return false;
    }
  }
  return true;
}


void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
 
  Serial.print("Packet received from: ");
  printMAC(mac_addr);
  
  memcpy(&message, incomingData, sizeof(message));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Key  received: ");
  Serial.println(message.key);
  if (strcmp(pwd, message.key)==0)
  {
    // Unlock the soleniod lock.
    digitalWrite(relay, LOW);
    
    // Print logs to the Serial monitor.
    Serial.println("Lock Unlocked");
    
    // Wait for 2 seconds and momentarily close the lock before we check if the key is still in the proximity.
    delay(5000);
    digitalWrite(relay, HIGH);
  }
  else{
    Serial.println("Password doesn't match!!");
  }

  
  Serial.println("-------------------");
}

esp_now_peer_info_t peerInfo;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("There was an error initializing ESP-NOW");
    return;
  }
 
  esp_now_set_pmk((uint8_t *)PMK_KEY);
  
  memcpy(peerInfo.peer_addr, masterMacAddress, 6);
  peerInfo.channel = 0;

  for (uint8_t i = 0; i < 16; i++) {
    peerInfo.lmk[i] = LMK_KEY[i];
  }

  peerInfo.encrypt = true;
       
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);
}
void loop() {
  
}
