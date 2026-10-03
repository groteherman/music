#include <Arduino.h>
#include <NimBLEDevice.h>

// ============================================================================
// --- CRITICAL FIX FOR 0x103 ERR_INVALID_STATE CRASH ---
// This native hook tells the Arduino core that Bluetooth is explicitly in use.
// It forces the system to preserve the Bluetooth hardware memory pool at boot!
// ============================================================================
extern "C" bool btInUse() { 
    return true; 
}
// ============================================================================

// Target device configurations
const char* TARGET_BLE_NAME = "TP-1"; 
static NimBLEAdvertisedDevice* targetDevice = nullptr;
static bool doConnect = false;
static bool connected = false;

// HID Report Map Characteristic UUID
static NimBLEUUID hidServiceUUID((uint16_t)0x1812);
static NimBLEUUID reportCharUUID((uint16_t)0x2A4D);

class MyAdvertisedDeviceCallbacks: public NimBLEAdvertisedDeviceCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        Serial.print("Discovered: ");
        Serial.println(advertisedDevice->getName().c_str());

        if (advertisedDevice->getName() == TARGET_BLE_NAME) {
            NimBLEDevice::getScan()->stop();
            targetDevice = advertisedDevice;
            doConnect = true;
        }
    }
};

void notifyCallback(NimBLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    // Optional: Uncomment the next lines if you still want to watch the raw streams
    
    Serial.print("Raw Stream: ");
    for (size_t i = 0; i < length; i++) { Serial.printf("%02X ", pData[i]); }
    Serial.println();
    

    // We only trigger actions on the final 'Release' packet to prevent multi-triggering
    // Based on your data: length is 8 bytes, and the release packet starts with 00
    if (length == 8 && pData[0] == 0x00) {
        
        // Let's look at byte index 2 and 3 (pData[2] and pData[3]) to identify which direction was swiped
        // For your 'UP' button, it registers: 70 07
        if (pData[2] == 0x70 && pData[4] == 0xAC) {
            Serial.println("[ACTION] -> 'UP' Button Clicked!");
            
            // PLACE YOUR CODE HERE
            // e.g., digitalWrite(MY_RELAY_PIN, !digitalRead(MY_RELAY_PIN)); 
        } 
        
        // --- CHANNELS FOR OTHER BUTTONS ---
        // When you press 'DOWN' or 'CENTER', check the values of pData[2] and pData[3] 
        // in their release packets and update these placeholders:
        else if (pData[2] == 0x70 && pData[4] == 0xC8) {
            Serial.println("[ACTION] -> 'DOWN' Button Clicked!");
        }
        else if (pData[2] == 0x70 && pData[4] == 0x70) {
            Serial.println("[ACTION] -> 'CENTER' Button Clicked!");
        }
    }
}


bool connectToRemote() {
    Serial.print("Forming a connection to ");
    Serial.println(targetDevice->getAddress().toString().c_str());
    
    NimBLEClient* pClient = NimBLEDevice::createClient();
    if(!pClient->connect(targetDevice)) {
        Serial.println("Failed to connect");
        return false;
    }
    Serial.println("Connected to remote hardware!");

    // Force secure bonding link
    if (!pClient->secureConnection()) {
        Serial.println("Secure handshake failed.");
    } else {
        Serial.println("Secure encryption link established!");
    }
    delay(500); 

    // --- NEW: FORCE HID REPORT MODE ---
    // The HID Service (0x1812) has a Protocol Mode characteristic (0x2A4E).
    // We must write 0x01 to it to tell the remote to start sending reports!
    NimBLERemoteService* pHidService = pClient->getService(NimBLEUUID((uint16_t)0x1812));
    if (pHidService != nullptr) {
        NimBLERemoteCharacteristic* pProtoChar = pHidService->getCharacteristic(NimBLEUUID((uint16_t)0x2A4E));
        if (pProtoChar != nullptr) {
            uint8_t mode = 0x01; // 0x01 = Report Protocol Mode
            pProtoChar->writeValue(&mode, 1, true);
            Serial.println("HID Protocol Mode successfully set to Report Mode!");
        }
    }
    delay(200);

    int totalSubscriptions = 0;
    std::vector<NimBLERemoteService*>* services = pClient->getServices(true);
    
    for (auto &service : *services) {
        std::vector<NimBLERemoteCharacteristic*>* characteristics = service->getCharacteristics(true);
        
        for (auto &chr : *characteristics) {
            if (chr->canNotify()) {
                // Subscribe with response parameters forced to true
                if (chr->subscribe(true, notifyCallback, true)) { 
                    Serial.printf("  Subscribed to: %s\n", chr->getUUID().toString().c_str());
                    totalSubscriptions++;
                    
                    // --- NEW: FORCE DIRECT DESCRIPTOR NOTIFICATION SIGNAL ---
                    // Explicitly tell the remote's internal client profile to begin streaming
                    NimBLERemoteDescriptor* pCccd = chr->getDescriptor(NimBLEUUID((uint16_t)0x2902));
                    if (pCccd != nullptr) {
                        uint8_t val[] = {0x01, 0x00};
                        pCccd->writeValue(val, 2, true);
                    }
                }
            }
        }
    }
    
    if (totalSubscriptions > 0) {
        Serial.printf("Success! Actively listening to %d channel(s).\n", totalSubscriptions);
        return true;
    } else {
        Serial.println("Error: No notification attributes found.");
        pClient->disconnect();
        return false;
    }
}

class MySecurityCallbacks : public NimBLESecurityCallbacks {
    // 1. Handles passkey requests (if the remote expects a default 000000 pin)
    uint32_t onPassKeyRequest() override {
        Serial.println("Passkey requested by remote...");
        return 0; 
    }

    // 2. Handles passkey notifications (Required by your library version)
    void onPassKeyNotify(uint32_t pass_key) override {
        Serial.printf("The passkey for this connection is: %d\n", pass_key);
    }
    
    // 3. Handles incoming security requests (Required by your library version)
    bool onSecurityRequest() override {
        Serial.println("Security upgrade requested by device. Approving...");
        return true; // Return true to accept the request
    }

    // 4. Handles confirmation pins for MITM protection
    bool onConfirmPIN(uint32_t pin) override {
        Serial.printf("Confirming PIN: %d\n", pin);
        return true;
    }

    // 5. Triggered once the encryption handshake is done
    void onAuthenticationComplete(ble_gap_conn_desc* desc) override {
        if (desc->sec_state.bonded) {
            Serial.println("Pairing / Bonding completed successfully!");
        } else {
            Serial.println("Pairing complete, but device is not bonded.");
        }
    }
};

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting ESP32 BLE Client...");
    
    // Now NimBLE can initialize safely since btInUse() protected the hardware memory pool
    NimBLEDevice::init("ESP32_Central");
    Serial.println("NimBLE initialized perfectly!");

    NimBLEDevice::setSecurityAuth(true, true, true); // Enable bonding, MITM protection, and secure connections
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT); // No screen/keyboard on ESP32
    NimBLEDevice::setSecurityCallbacks(new MySecurityCallbacks());

    NimBLEScan* pBLEScan = NimBLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(10, false); 
}

void loop() {
    if (doConnect) {
        doConnect = false;
        if (connectToRemote()) {
            connected = true;
        } else {
            Serial.println("Retry scanning...");
            NimBLEDevice::getScan()->start(10, false);
        }
    }
    delay(10);
}
