void setupLeds()
{
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness( BRIGHTNESS );
  leds[0] = CRGB::Green;
  FastLED.show();
}

void setupMesh()
{
  meshConnected = false;
  meshRootNode = 0;
  meshNodeType = DEVTYPE_SENSOR_PRESENCE;
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  Serial.println("[INIT] Mesh node id " + String(mesh.getNodeId()));
  // setup callback functions
  mesh.onReceive(&meshReceiveCallback);
  mesh.onNewConnection(&meshNewConnectionCallback);
  mesh.onChangedConnections(&meshChangedConnectionCallback);
  mesh.onNodeTimeAdjusted(&meshNodeTimeAdjustedCallback);
}

void setupTasks()
{
  pinMode(SENSOR_PIN, INPUT);
  userScheduler.addTask(taskcheckPIRSensor);
  taskcheckPIRSensor.enable();
}