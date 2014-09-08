<?php
require_once("openmediavault/object.inc");
require_once("openmediavault/config.inc");
require_once("openmediavault/error.inc");
require_once("openmediavault/util.inc");
require_once("openmediavault/rpcservice.inc");
require_once("openmediavault/notify.inc");
class OMVRpcServiceFirstPlugin extends OMVRpcServiceAbstract {
public function getName() { return "pxe";} // RPC Service name. Same as in .js files
/* Initialize the RPC service. Different methods of the RPC service are declared here*/
public function initialize() {
$this->registerMethod("getEntries"); // Method name. Same as in .js files
$this->registerMethod("setEntries"); // Method name. Same as in .js files
}
public function getSettings($params, $context) {
// Fill this out later
}
public function setSettings($params, $context) {
// Fill this out later
}
}
// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance(); // Get the "root" instance for the Services
$rpcServiceMgr->registerService(new OMVRpcServiceFirstPlugin()); // Register a new instance of the RPC service described above
?>
