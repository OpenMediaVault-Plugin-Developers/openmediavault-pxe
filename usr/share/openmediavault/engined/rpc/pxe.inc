<?php
/**
 * Copyright (C) 2014 OpenMediaVault Plugin Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
require_once "openmediavault/object.inc";
require_once "openmediavault/error.inc";
require_once "openmediavault/util.inc";
require_once "openmediavault/rpc.inc";
require_once "openmediavault/notify.inc";

class OMVRpcServicePxe extends OMVRpcServiceAbstract
{
    public function getName()
    {
        return "Pxe";
    }

    private function getPluginName()
    {
        return strtolower($this->getName());
    }

    private function getEventMessagePath()
    {
        return sprintf("org.openmediavault.services.%s", $this->getPluginName());
    }

    private function getEventMessagePathEntries()
    {
        return sprintf("%s.entries", $this->getEventMessagePath());
    }

    private function getEventMessagePathEntry()
    {
        return sprintf("%s.entry", $this->getEventMessagePathEntries());
    }

    private function getXPath()
    {
        return sprintf("//services/%s", $this->getPluginName());
    }

    private function getXPathEntries()
    {
        return sprintf("%s/entries", $this->getXPath());
    }

    private function getXPathEntry()
    {
        return sprintf("%s/entry", $this->getXPathEntries());
    }

    private function getPackageCache()
    {
        return "/var/cache/openmediavault/pxe_packages_cache";
    }

    public function initialize()
    {
        $this->registerMethod("getEntries");
        $this->registerMethod("getEntry");
        $this->registerMethod("setEntry");
        $this->registerMethod("deleteEntry");
        $this->registerMethod("getSettings");
        $this->registerMethod("setSettings");
        
        $this->registerMethod("getImageList")
        $this->registerMethod("updatePackageCache")
    }

    public function getSettings($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));

        // Get configuration object
        $object = $xmlConfig->get($this->getXPath());
        if (is_null($object)) {
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $this->getXPath());
        }

        // Modify result data
        $object["sharedfolderref"] = $object["sharedfolderref"];
        $object["enableWinPath"] = $object["enableWinPath"];

        // Remove useless fields from the result object.
        unset($object["entries"]);
        unset($object["menus"]);

        return $object;
    }

    public function setSettings($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams(
            $params,
            '{
                "type"       : "object",
                "properties" : {
                    "sharedfolderref": { '.$GLOBALS['OMV_JSONSCHEMA_UUID_NONE'].', "optional" : true },
                    "enableWinPath": {"type":"boolean"}
                }
            }'
        );

        // Get the existing configuration object.
        $oldObject = $xmlConfig->get( $this->getXPath() );
        if(is_null($oldObject)) {
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $this->getXPath() );
        }

        // Update the configuration object.
        $object = array(
            "sharedfolderref" => $params["sharedfolderref"],
            "enableWinPath"   => array_boolval($params,"enableWinPath"),
            "entries"         => isset($oldObject["entries"]) ? $oldObject["entries"] : array(),
            "menus"           => isset($oldObject["menus"]) ? $oldObject["menus"] : array()
        );

        // Set configuration object
        if (false === $xmlConfig->replace($this->getXPath(), $object)) {
            throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED, $this->getXPath());
        }

        // Notify configuration changes
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(OMV_NOTIFY_MODIFY, "org.openmediavault.services.pxe", $object);
    }


    public function getEntries($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "start"     : { "type" : "integer" },
                "limit"     : { '.$GLOBALS['OMV_JSONSCHEMA_COUNTFIELD'].' },
                "sortfield" : { '.$GLOBALS['OMV_JSONSCHEMA_SORTFIELD'].' },
                "sortdir"   : { '.$GLOBALS['OMV_JSONSCHEMA_SORTDIR'].' }
            }
        }');

        // Get configuration data.
        $objects = $xmlConfig->getList( $this->getXPathEntry() );
        if (is_null($objects))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPathEntry()
            );
        }

        // Filter the result.
        return $this->applyFilter($objects, $params['start'], $params['limit'],
            $params['sortfield'], $params['sortdir']);
    }

    public function getEntry($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "uuid" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID'].' }
            }
        }');

        // Get the configuration object.
        $xPath = sprintf("%s[uuid='%s']", $this->getXPathEntry(), $params['uuid']);
        $object = $xmlConfig->get($xPath);
        if (is_null($object))
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $xPath
            );
        }

        // Modify the result object.
        $object['enable'] = boolval($object['enable']);

        return $object;
    }

    public function setEntry($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams(
            $params,
            '{
                "type"        : "object",
                "properties"  : {
                    "uuid"   : { '.$GLOBALS['OMV_JSONSCHEMA_UUID_UNDEFINED'].' },
                    "enable" : { "type" : "boolean" },
                    "label"  : { "type" : "string" },
                    "menu"   : { "type" : "string", "optional" : true },
                    "kernel" : { "type" : "string" },
                    "append" : { "type" : "string" }
                }
            }'
        );

        // Prepare configuration data
        $object = array(
            "uuid"   => ($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) ? OMVUtil::uuid() : $params['uuid'],
            "enable" => array_boolval($params, "enable"),
            "label"  => $params['label'],
            "menu"   => $params['menu'],
            "kernel" => $params['kernel'],
            "append" => $params['append']
        );

        // Set the configuration data.
        $success = FALSE;
        if ($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED'])
        {
            // Append object to configuration.
            $success = $xmlConfig->set($this->getXPathEntries(), array("entry" => $object));
        }
        else
        {
            // Update existing configuration object.
            $xPath = sprintf("%s[uuid='%s']", $this->getXPathEntry(), $params['uuid']);
            $success = $xmlConfig->replace($xPath, $object);
        }

        if (FALSE === $success)
        {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED
            );
        }

        // Notify configuration changes.
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(
            ($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) ? OMV_NOTIFY_CREATE : OMV_NOTIFY_MODIFY,
            $this->getEventMessagePathEntry(),
            $object
        );

        // Return the configuration object.
        return $object;
    }

    public function deleteEntry($params, $context)
    {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "uuid" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID'].' }
            }
        }');

        // Delete the configuration object.
        $xPath = sprintf("%s[uuid='%s']", $this->getXPathEntry(), $params['uuid']);
        return $this->deleteConfigObjectByPath(
            $xPath,
            $this->getEventMessagePathEntry()
        );
    }

    public function getImageList($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Get current time
        $time  = time();

        // if the cache doesn't exist or is older than 1 day, download it
        if ( !file_exists($this->getPackageCache()) or
             ($time - filemtime($this->getPackageCache()) >= 60*60*24) )
        {
            $this->updatePackageCache($params, $context);
        }

        $objects = array();
        $packages = file($this->getPackageCache());

        // Loop through our array, show HTML source as HTML source; and line numbers too.
        foreach ($packages as $package)
        {
            if (empty($package))
                continue;

            $objects[] = array(
                "name" => $package
            );
        }

        // Filter the result.
        return $this->applyFilter($objects, $params['start'], $params['limit'],
            $params['sortfield'], $params['sortdir']);
    }

    public function updatePackageCache($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext(
            $context,
            array( "role" => OMV_ROLE_ADMINISTRATOR )
        );

        // Get packages file from web server and put in cache file
        file_put_contents(
            $this->getPackageCache(),
            file_get_contents("http://pxe.omv-extras.org/packages")
        );        
    }
}

// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance();
$rpcServiceMgr->registerService(new OMVRpcServicePxe());
