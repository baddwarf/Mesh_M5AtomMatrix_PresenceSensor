#include "painlessMesh.h"
#include <FastLED.h>
#include "meshRule.h"

// LEDS
#define SENSOR_PIN 32
#define LED_PIN     27
#define NUM_LEDS    25
#define BRIGHTNESS  20
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

// MESH
#define MESH_ROOT false
#define MESH_ROOT_PRIORITY 0
#define MESH_PREFIX "homeMesh"
#define MESH_PASSWORD "MeshP4Ssw0rd"
#define MESH_PORT 5555
Scheduler userScheduler;
painlessMesh  mesh;
uint32_t meshRootNode;
uint8_t meshNodeType;
bool meshConnected;
uint8_t meshDeviceType = DEVTYPE_SENSOR_PRESENCE;

// PIR SENSOR
bool motionDetected;

void checkPIRSensor();
Task taskcheckPIRSensor(500, TASK_FOREVER, &checkPIRSensor);

void setup() {
  
  // put your setup code here, to run once:
  Serial.println("[INIT] M5 Atom");
  Serial.begin(115200);
  while (!Serial)
  {}
  delay(500);
  motionDetected = false;
  Serial.println("[INIT] LEDS");
  setupLeds();
  Serial.println("[INIT] MESH");
  setupMesh();
  Serial.println("[INIT] TASKS");
  setupTasks();
}

void loop() {
  mesh.update();
}
