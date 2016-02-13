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

require_once "openmediavault/module.inc";
require_once "openmediavault/config.inc";
require_once "openmediavault/error.inc";
require_once "openmediavault/util.inc";

class OMVModulePxe extends OMVModuleServiceAbstract implements
    OMVINotifyListener, OMVIModuleServiceStatus
{
    /* Helpers */

    private function getEventMessagePath()
    {
        return "org.openmediavault.services.pxe";
    }

    private function getEventMessagePathEntry()
    {
        return sprintf("%s.entries.entry", $this->getEventMessagePath());
    }

    private function getXPath()
    {
        return "/config/services/pxe";
    }

    /* /Helpers */

    /**
     * Get the module name.
     */
    public function getName()
    {
        return "pxe";
    }

    /**
     * Get the status of the service
     */
    public function getStatus()
    {
        global $xmlConfig;

        $object = $xmlConfig->get($this->getXPath());

        if (is_null($object)) {
            throw new OMVException(
                OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPath()
            );
        }

        //$initd = new OMVSysVInitScript($this->getName());

        return [
            "name"    => $this->getName(),
            "title"   => gettext("PXE"),
            "enabled" => true,
            "running" => true
        ];
    }

    /**
     * Create and update the configuration
     */
    public function applyConfig()
    {
        $cmd = sprintf("omv-mkconf %s 2>&1", $this->getName());

        if (0 !== $this->exec($cmd, $output)) {
            throw new OMVException(
                OMVErrorMsg::E_EXEC_FAILED,
                $cmd,
                implode("\n", $output)
            );
        }

    }

    /**
     * Bind listeners.
     */
    public function bindListeners(OMVNotifyDispatcher $dispatcher)
    {
        $dispatcher->addListener(
            OMV_NOTIFY_CREATE | OMV_NOTIFY_MODIFY | OMV_NOTIFY_DELETE,
            $this->getEventMessagePath(),
            array($this, "setDirty")
        );

        $dispatcher->addListener(
            OMV_NOTIFY_CREATE | OMV_NOTIFY_MODIFY | OMV_NOTIFY_DELETE,
            $this->getEventMessagePathEntry(),
            array($this, "setDirty")
        );
    }
}

// Register module
$moduleMgr = &OMVModuleMgr::getInstance();
$moduleMgr->registerModule(new OMVModulePxe());
