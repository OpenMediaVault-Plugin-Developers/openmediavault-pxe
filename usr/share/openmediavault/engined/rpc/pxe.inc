<?php
/**
 * Copyright (C) 2014-2016 OpenMediaVault Plugin Developers
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
class OMVRpcServicePxe extends \OMV\Rpc\ServiceAbstract
{
    private $pxePackageCache = "/var/cache/openmediavault/pxe_packages_cache";

    public function getName()
    {
        return "Pxe";
    }

    public function initialize()
    {
        $this->registerMethod("getSettings");
        $this->registerMethod("setSettings");

        $this->registerMethod("getEntryList");
        $this->registerMethod("getEntry");
        $this->registerMethod("setEntry");
        $this->registerMethod("deleteEntry");

        $this->registerMethod("getImageList");
        $this->registerMethod("updatePackageCache");
        $this->registerMethod("setImageDownload");
        $this->registerMethod("getIPAddresses");
    }

    public function getSettings($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ["role" => OMV_ROLE_ADMINISTRATOR]);
        // Get the configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get("conf.service.pxe");
        // Remove useless properties from the object.
        $object->remove("entries");
        return $object->getAssoc();
    }

    public function setSettings($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ["role" => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, "rpc.pxe.setsettings");
        // Get the existing configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get("conf.service.pxe");
        $object->setAssoc($params);
        $db->set($object);
        // Remove useless properties from the object.
        $object->remove("entries");
        // Notify configuration changes.
        $dispatcher = \OMV\Engine\Notify\Dispatcher::getInstance();
        $dispatcher->notify(
            OMV_NOTIFY_MODIFY,
            "org.openmediavault.conf.service.pxe",
            $object->getAssoc()
        );
        // Return the configuration object.
        return $object->getAssoc();
    }

    public function getEntryList($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ["role" => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, "rpc.common.getlist");
        // Get the list of configuration objects.
        $db = \OMV\Config\Database::getInstance();
        $objects = $db->getAssoc("conf.service.pxe.entry");
        // Filter the result objects.
        return $this->applyFilter($objects, $params['start'], $params['limit'],
            $params['sortfield'], $params['sortdir']);
    }

    public function getEntry($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ["role" => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, "rpc.common.objectuuid");
        // Get the configuration object.
        $db = \OMV\Config\Database::getInstance();
        return $db->getAssoc("conf.service.pxe.entry", $params['uuid']);
    }

    public function setEntry($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ["role" => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, "rpc.pxe.setentry");
        // Prepare the configuration object.
        $object = new \OMV\Config\ConfigObject("conf.service.pxe.entry");
        $object->setAssoc($params);
        // Set the configuration object.
        $isNew = $object->isNew();
        $db = \OMV\Config\Database::getInstance();
        $db->set($object);
        // Notify configuration changes.
        $dispatcher = \OMV\Engine\Notify\Dispatcher::getInstance();
        $dispatcher->notify(
            $isNew ? OMV_NOTIFY_CREATE : OMV_NOTIFY_MODIFY,
            "org.openmediavault.conf.service.pxe.entry",
            $object->getAssoc()
        );
        // Return the configuration object.
        return $object->getAssoc();
    }

    public function deleteEntry($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ["role" => OMV_ROLE_ADMINISTRATOR]);
        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, "rpc.common.objectuuid");
        // Delete the configuration object.
        return $this->deleteConfigObjectByUuid(
            "conf.service.pxe.entry",
            $params['uuid'],
            "org.openmediavault.conf.service.pxe.entry"
        );
    }

    public function getImageList($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ["role" => OMV_ROLE_ADMINISTRATOR]);
        // If the cache doesn't exist or is older than 1 day, download it
        if ( !file_exists($this->pxePackageCache) or
             (time() - filemtime($this->pxePackageCache) >= 60*60*24) ) {
            $this->updatePackageCache($params, $context);
        }
        $objects = [];
        $packages = file($this->pxePackageCache);

        // Loop through our array, show HTML source as HTML source; and line numbers too.
        foreach ($packages as $package) {
            if (empty($package))
                continue;
            $objects[] = [
                "name" => trim($package)
            ];
        }
        // Filter the result.
        return $objects;
    }

    public function updatePackageCache($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ["role" => OMV_ROLE_ADMINISTRATOR]);
        // Get packages file from web server and put in cache file
        file_put_contents(
            $this->pxePackageCache,
            file_get_contents("http://pxe.omv-extras.org/packages")
        );
    }

    public function setImageDownload($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ["role" => OMV_ROLE_ADMINISTRATOR]);
        // Get the configuration object.
        $db = \OMV\Config\Database::getInstance();
        $object = $db->get("conf.service.pxe");
        // Remove useless properties from the object.
        $object->remove("entries");

        // Create a background process.
        $bgStatusFilename = $this->createBgProcStatus();
        $pid = $this->fork();
        if($pid > 0) { // Parent process.
            $this->initializeBgProcStatus($bgStatusFilename, $pid);
            return $bgStatusFilename;
        }
        // Child process.
        try {
            $bgOutputFilename = $this->createBgProcOutput();
            $this->updateBgProcStatus($bgStatusFilename, "outputfilename", $bgOutputFilename);
            $cmd = sprintf("omv-mkconf pxe package %s 2>&1", $params['package']);
            $this->exec($cmd, $output, $bgOutputFilename);
            $this->finalizeBgProcStatus($bgStatusFilename, $output);
            exit(0);
        } catch(\Exception $e) {
            $this->finalizeBgProcStatus($bgStatusFilename, "", $e);
            exit(1);
        }

        // add info from config.txt to entries
        $configTxt = "/tmp/pxeinstall/config.txt";
        $file = explode(PHP_EOL, file_get_contents($configTxt));
        $label  = trim(preg_replace("/LABEL/", "", $file[0], 1));
        $kernel = trim(preg_replace("/KERNEL/", "", $file[1], 1));
        $append = trim(preg_replace("/APPEND/", "", $file[2], 1));

        // Replace <Your_IP_Address>:<Your_NFS_Export> with select IP address and nfs export dir
        $append = str_replace("<Your_IP_Address>", $params['address'], $append);

        $newEntry = [
            "uuid"   => $GLOBALS['OMV_UUID_UNDEFINED'],
            "enable" => true,
            "grp"    => "Default",
            "label"  => $label,
            "menu"   => "",
            "kernel" => $kernel,
            "append" => $append
        ];
        $this->setEntry($newEntry, $context);

        // remove config.txt
        unlink($configTxt);

        $cmd = "omv-mkconf pxe update 2>&1";
        exec($cmd, $output);
    }

    public function getIPAddresses($params, $context)
    {
        // Validate the RPC caller context.
        $this->validateMethodContext($context, ["role" => OMV_ROLE_ADMINISTRATOR]);

        // Get IP addresses
        $cmd = "ifconfig -a | grep 'inet' | cut -d: -f2 | awk {'print $1'}";
        exec($cmd, $result);

        $objects = [];
        foreach ($result as $address) {
            if (empty($address))
                continue;

            if (filter_var(trim($address), FILTER_VALIDATE_IP, FILTER_FLAG_IPV4) === false)
                continue;

            if (substr(trim($address), 0, 3) == "127")
                continue;

            $objects[] = [
                "address" => trim($address)
            ];
        }
        return $objects;
    }
}
