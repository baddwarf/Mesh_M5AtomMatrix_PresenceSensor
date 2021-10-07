void meshSendCapabilities() //TODO
{
  String msg;
  DynamicJsonDocument jsonBuffer(1024);
  JsonObject root = jsonBuffer.to<JsonObject>();
  JsonObject capabilities = jsonBuffer.createNestedObject("capabilities");
  root["type"] = "capabilities";
  root["device_type"] = meshDeviceType;
  serializeJson(root, msg);
  mesh.sendSingle(meshRootNode, msg);
}

void meshCommand(uint8_t cmd_class, String cmd_param)
{

}

void meshSendCommand(uint32_t to, uint8_t device_type, uint8_t cmd_class, String cmd_param)
{
  String msg;
  DynamicJsonDocument jsonBuffer(1024);
  JsonObject root = jsonBuffer.to<JsonObject>();
  JsonObject capabilities = jsonBuffer.createNestedObject("capabilities");
  root["type"] = "command";
  root["device_type"] = device_type;
  root["cmd_class"] = cmd_class;
  root["cmd_param"] = cmd_param;
  serializeJson(root, msg);
  if (to != 0)
    mesh.sendSingle(to, msg);
  else
    mesh.sendBroadcast(msg);
}

void meshSendEvent(uint32_t to, uint8_t evt_devtype, uint8_t evt_class, String evt_param)
{
  String msg;
  DynamicJsonDocument jsonBuffer(1024);
  JsonObject root = jsonBuffer.to<JsonObject>();
  JsonObject capabilities = jsonBuffer.createNestedObject("capabilities");
  root["type"] = "event";
  root["device_type"] = evt_devtype;
  root["evt_class"] = evt_class;
  root["evt_param"] = evt_param;
  serializeJson(root, msg);
  mesh.sendSingle(to, msg);
}

#ifdef MESH_RULE_MANAGER
void meshCheckEvent(uint32_t from, uint8_t evt_devtype, uint8_t evt_class, String evt_param)
{
  String event;
  int ruleCount = 0;
  int ruleidx;

  event = "[MESH] " + String(from) + " : " + String(evt_devtype) + " |" + String(evt_class) + "|" + evt_param;
  Serial.println(event);
  ruleCount = rules.getActionFromEvent(from, evt_devtype, evt_class, evt_param);
  Serial.println("[MESH] Event check - Action count : " + String(ruleCount));
  for (int i = 0; i < ruleCount; i++)
  {
    ruleidx = rules.getMatchIndex(i);
    if (ruleidx >= 0);
    {
      rules[ruleidx]->print();
      meshSendCommand(rules[ruleidx]->getActionTo(), 
        rules[ruleidx]->getActionDeviceType(), 
        rules[ruleidx]->getActionClass(), 
        rules[ruleidx]->getActionParams());
    }
  }
}
#endif


void meshStatus()
{
  uint8_t count = mesh.getNodeList().size() + 1;

  if (count > 1)
  {
    Serial.println("[MESH] connected with " + String(count) + " nodes");
    meshConnected = true;
  }
  else
  {
    Serial.println("[MESH] disconnected or node is alone");
    meshConnected = false;
  }
  if (MESH_ROOT)
    Serial.println("[MESH] this node is root");
  else if ((count > 1) && (meshRootNode != 0))
  {
    if (mesh.isConnected(meshRootNode))
    {
      Serial.println("[MESH] root connected");
    }
    else
    {
      Serial.println("[MESH] root disconnected");
    }
  }

}

void meshRootElection()
{
  // send root elect message
  // create new task to elect root after receiving candidates
}

/*
** MESH Callback
**
*/

void meshReceiveCallback(uint32_t from, String &msg)
{
  DynamicJsonDocument jsonBuffer(1024 + msg.length());
  deserializeJson(jsonBuffer, msg);
  JsonObject root = jsonBuffer.as<JsonObject>();
  if (root.containsKey("type"))
  {
    String messageType = root["type"].as<String>();
    if (messageType == "message")
    {
      //consoleLog("Message from " + String(from));
      String hname = root["name"].as<String>();
      size_t group = root["group"];
      String message = root["message"].as<String>();
      Serial.printf("[MESH] Node %u has name %s and is part of group %i\n", from, hname.c_str(), group);
      Serial.println("[MESH] Msg: " + message);
    }
    else if (messageType == "newroot")
    {
      //consoleLog("New root " + String(from));
      Serial.println("[MESH] New root node : " + String(from));
      meshRootNode = from;
    }
    else if (messageType == "newnode")
    {
      //consoleLog("New root " + String(from));
      Serial.println("[MESH] New node : " + String(from));
    }
    else if (messageType == "healtcheck") // TODO : is it a good way?
    {
      uint32_t msgRoot = from;
      if (meshRootNode != msgRoot)
      {
        Serial.println("[MESH] New root : " + String(from));
      }
      else
        Serial.println("[MESH] Root still alive");
    }
    else if (messageType == "event")
    {
      String hname = root["name"].as<String>();
      size_t group = root["group"];
      uint8_t evt_class = root["evt_class"];
      uint8_t evt_devtype = root["device_type"];
      String evt_param = root["evt_param"].as<String>();
      Serial.println("[MESH] Event received from " + hname + "(" + String(from) + ")");
      //consoleLog("Event [" + hname + "]: " + evt_msg);
      #ifdef MESH_RULE_MANAGER
      meshCheckEvent(from, evt_devtype, evt_class, evt_param);
      #endif
    }
    else if (messageType == "command")
    {
      String hname = root["name"].as<String>();
      size_t group = root["group"];
      uint8_t cmd_class = root["cmd_class"];
      String cmd_param = root["cmd_param"].as<String>();
      Serial.println("[MESH] Command received from " + hname + "(" + String(from) + ")");
      Serial.println("[MESH] Command : [" + String(cmd_class) + "] " + cmd_param);
      meshCommand(cmd_class, cmd_param);
    }
    else
    {
      Serial.println("[MESH] Invalid message type");
    }
  }
  else
  {
    Serial.println("[MESH] Invalid message");
  }
}

void meshNewConnectionCallback(uint32_t nodeId)
{
  Serial.println("[MESH] New node: " + String(nodeId));
  String msg;
  DynamicJsonDocument jsonBuffer(1024);
  JsonObject root = jsonBuffer.to<JsonObject>();
  if(MESH_ROOT)
    root["type"] = "newroot";    
  else
    root["type"] = "newnode";
  serializeJson(root, msg);
  mesh.sendSingle(nodeId, msg);
}

void meshChangedConnectionCallback()
{
  Serial.println("[MESH] Connection changed");
}

void meshNodeTimeAdjustedCallback(int32_t offset)
{
  Serial.println("[MESH] Time adjustment");
}