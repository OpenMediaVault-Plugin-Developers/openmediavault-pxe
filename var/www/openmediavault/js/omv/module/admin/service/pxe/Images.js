/**
 * This file is part of OpenMediaVault.
 *
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @copyright Copyright (c) 2009-2014 Volker Theile
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
// require("js/omv/form/plugin/LinkedFields.js")

/**
 * @class OMV.module.admin.service.pxe.Settings
 * @derived OMV.workspace.form.Panel
 */
Ext.define("OMV.module.admin.service.pxe.Settings", {
        extend: "OMV.workspace.form.Panel",
        requires: [
                "OMV.form.field.SharedFolderComboBox"
        ],

/**        rpcService: "PXE",
*		rpcGetMethod: "getImagelist", // name for the function in the rpc that gets the settings
*		rpcSetMethod: "setImageDownload", // name for the function in the rpc that saves the settings
*/

        plugins: [{
                ptype: "linkedfields",
                correlations: [{
                        name: "combofield",
                        conditions: [
                                { name: "image", value: true }
                        ],
                        properties: "!allowBlank"
                }]
        }],

        getFormItems: function() {
                var me = this;
                return [{
                            xtype:    "fieldset",
                            title:    _("Image Installation"),
                            fieldDefaults:  {
                                              labelSeparator: ""
                                            },{
                            items: 
		xtype         : "combo",
                name          : "pxe_image",
                fieldLabel    : _("Distribution"),
                queryMode     : "local",
                store         : [],
                allowBlank    : false,
                editable      : false,
                triggerAction : "all",
                plugins       : [{
		ptype : "fieldinfo",
                text  : _("The Distribution you want to install.")
 
                                                }]
                                    }];
        }
});

OMV.WorkspaceManager.registerPanel({
        id: "Settings",
        path: "/service/pxe",
        text: _("Settings"),
        position: 35,
        className: "OMV.module.admin.service.pxe.Settings"
});
