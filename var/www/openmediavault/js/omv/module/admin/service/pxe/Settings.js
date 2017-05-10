/**
*
* @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
* @author    Volker Theile <volker.theile@openmediavault.org>
* @copyright Copyright (c) 2009-2014 Volker Theile
* @copyright Copyright (c) 2013-2017 OpenMediaVault Plugin Developers
*
* OpenMediaVault is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* OpenMediaVault is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with OpenMediaVault. If not, see <http://www.gnu.org/licenses/>.
*/
// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/form/Panel.js")
// require("js/omv/form/field/SharedFolderComboBox.js")

/**
* @class OMV.module.admin.service.pxe.Settings
* @derived OMV.workspace.form.Panel
*/
Ext.define("OMV.module.admin.service.pxe.Settings", {
    extend: "OMV.workspace.form.Panel",
    requires: [
        "OMV.form.field.SharedFolderComboBox"
    ],

    rpcService: "PXE",
    rpcGetMethod: "getSettings",
    rpcSetMethod: "setSettings",

    plugins: [{
        ptype: "linkedfields",
        correlations: [{
            name: "sharedfolderref",
            conditions: [{
                name: "enable", value: true
            }],
            properties: "!allowBlank"
        }]
    }],

    getFormItems: function() {
        var me = this;
        return [{
            xtype:    "fieldset",
            title:    _("Settings"),
            fieldDefaults:  {
                labelSeparator: ""
            },
            items:  [{
                xtype: "sharedfoldercombo",
                name: "sharedfolderref",
                fieldLabel: _("Shared folder"),
                allowNone: true,
                plugins: [{
                    ptype: "fieldinfo",
                    text: _("The location of your PXE files.")
                }]
            },{
                xtype         : "numberfield",
                name          : "port",
                fieldLabel    : _("HTTP Port"),
                vtype         : "port",
                minValue      : 1,
                maxValue      : 65535,
                allowDecimals : false,
                allowNegative : false,
                allowBlank    : false,
                value         : 8001,
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("Port to listen on for http downloads.")
                }]
            },{
                xtype:  "checkbox",
                name:   "enableWinPath",
                fieldLabel: _("Enable Windows Path Support"),
                checked: false,
                plugins: [{
                    ptype: "fieldinfo",
                    text: _("If enabled, add -m /etc/tftp_remap.conf to the Extra options box in the tftp server tab.")
                }]
            }]
        }];
    }
});

OMV.WorkspaceManager.registerPanel({
    id: "Settings",
    path: "/service/pxe",
    text: _("Settings"),
    position: 10,
    className: "OMV.module.admin.service.pxe.Settings"
});
