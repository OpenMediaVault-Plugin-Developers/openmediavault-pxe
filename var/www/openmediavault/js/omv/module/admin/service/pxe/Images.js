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
 * @class OMV.module.admin.service.pxe.Images
 * @derived OMV.workspace.form.Panel
 */
Ext.define("OMV.module.admin.service.pxe.Images", {
    extend : "OMV.workspace.form.Panel",
    uses   : [
        "OMV.data.Model",
        "OMV.data.Store"
    ],

    autoLoadData    : false,
    hideOkButton    : true,
    hideResetButton : true,
    mode            : "local",

    getFormItems : function() {
        var me = this;
        return [{
            xtype    : "fieldset",
            title    : _("PXE Images"),
            defaults : {
                labelSeparator : ""
            },
            items : [{
                xtype         : "combo",
                allowBlank    : false,
                editable      : false,
                triggerAction : "all",
                displayField  : "name",
                valueField    : "name",
                store         : Ext.create("OMV.data.Store", {
                    autoLoad : true,
                    model    : OMV.data.Model.createImplicit({
                        idProperty : "name",
                        fields     : [
                            { name : "name", type : "string" }
                        ]
                    }),
                    proxy : {
                        type    : "rpc",
                        rpcData : {
                            service : "Pxe",
                            method  : "getImageList"
                        }
                    }
                })
            }]
        }];
    }
});

OMV.WorkspaceManager.registerPanel({
    id: "images",
    path: "/service/pxe",
    text: _("Images"),
    position: 35,
    className: "OMV.module.admin.service.pxe.Images"
});
