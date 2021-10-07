void checkPIRSensor()
{
  DynamicJsonDocument jsonBuffer(1024);
  JsonObject root = jsonBuffer.to<JsonObject>();
  String msg;
  uint8_t sensorState = digitalRead(SENSOR_PIN);
  if (sensorState)
  {
    if (!motionDetected)
    {
      Serial.println("[SENSOR] State change : Motion");
      leds[1] = CRGB::Red;
      FastLED.show();
      root["type"] = "event";
      root["name"] = "PIR Sensor";
      root["device_type"] = meshDeviceType;
      root["evt_class"] = EVTCLASS_PRESENCE_ON; // motion sensor detection
      root["evt_param"] = "";
      serializeJson(root, msg);
      mesh.sendBroadcast(msg);
      motionDetected = true;
    }
  }
  else
  {
    if (motionDetected)
    {
      Serial.println("[SENSOR] State change : No motion");
      leds[1] = CRGB::Black;
      FastLED.show();
      root["type"] = "event";
      root["name"] = "PIR Sensor";
      root["device_type"] = meshDeviceType;
      root["evt_class"] = EVTCLASS_PRESENCE_OFF; // motion sensor detection
      root["evt_param"] = "";
      serializeJson(root, msg);
      mesh.sendBroadcast(msg);
      motionDetected = false;
    }
  }
}