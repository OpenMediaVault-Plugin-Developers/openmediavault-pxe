/**
 * @license     http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author      OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright   Copyright (c) 2013-2016 OpenMediaVault Plugin Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */
// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/grid/Panel.js")
// require("js/omv/workspace/window/Form.js")
// require("js/omv/workspace/window/plugin/ConfigObject.js")
// require("js/omv/Rpc.js")
// require("js/omv/data/Store.js")
// require("js/omv/data/Model.js")
// require("js/omv/data/proxy/Rpc.js")

/**
 * @class OMV.module.admin.service.pxe.Entry
 * @derived OMV.workspace.window.Form
 */
Ext.define("OMV.module.admin.service.pxe.Entry", {
    extend: "OMV.workspace.window.Form",
    requires: [
        "OMV.workspace.window.plugin.ConfigObject"
    ],

    rpcService: "PXE",
    rpcGetMethod: "getEntry",
    rpcSetMethod: "setEntry",
    plugins: [{
        ptype: "configobject"
    }],
    width: 700,
    height: 400,

    getFormItems: function() {
        var me = this;
        return [{
            xtype: "checkbox",
            name: "enable",
            fieldLabel: _("Enable"),
            checked: true
        },{
            xtype      : "textfield",
            name       : "group",
            fieldLabel : _("Group"),
            allowBlank : false
        },{
            xtype: "textfield",
            name: "label",
            fieldLabel: _("LABEL"),
            allowBlank: false
        },{
            xtype: "textfield",
            name: "menu",
            fieldLabel: _("MENU"),
            allowBlank: true
        },{
            xtype: "textfield",
            name: "kernel",
            fieldLabel: _("KERNEL"),
            allowBlank: false
        },{
            xtype: "textfield",
            name: "append",
            fieldLabel: _("APPEND"),
            allowBlank: false
        }];
    }
});

/**
 * @class OMV.module.admin.service.pxe.Entries
 * @derived OMV.workspace.grid.Panel
 */
Ext.define("OMV.module.admin.service.pxe.Entries", {
    extend: "OMV.workspace.grid.Panel",
    requires: [
        "OMV.Rpc",
        "OMV.data.Store",
        "OMV.data.Model",
        "OMV.data.proxy.Rpc"
    ],
    uses: [
        "OMV.module.admin.service.pxe.Entry"
    ],

    hidePagingToolbar: false,
    stateful: true,
    stateId: "ba17a72d-1104-1232-a31b-1148f4ea6dbe",
    columns: [{
        xtype: "booleaniconcolumn",
        text: _("Enabled"),
        sortable: true,
        dataIndex: "enable",
        stateID: "enable",
        align: "center",
        width: 80,
        resizeable: false,
        trueIcon: "switch_on.png",
        falseIcon: "switch_off.png"
    },{
        text      : _("Group"),
        sortable  : true,
        dataIndex : "group",
        stateId   : "group"
    },{
        text: _("LABEL"),
        sortable: true,
        dataIndex: "label",
        stateID: "label"
    },{
        text: _("MENU"),
        sortable: true,
        dataIndex: "menu",
        stateID: "menu"
    },{
        text: _("KERNEL"),
        sortable: true,
        dataIndex: "kernel",
        stateID: "kernel"
    },{
        text: _("APPEND"),
        sortable: true,
        dataIndex: "append",
        stateID: "append"
    }],

    initComponent: function() {
        var me = this;
        Ext.apply(me, {
            store: Ext.create("OMV.data.Store", {
                autoLoad: true,
                model: OMV.data.Model.createImplicit({
                    idProperty: "uuid",
                    fields: [
                        { name: "uuid", type: "string" },
                        { name: "enable", type: "boolean" },
                        { name: "group", type: "string" },
                        { name: "label", type: "string" },
                        { name: "menu", type: "string" },
                        { name: "kernel", type: "string" },
                        { name: "append", type: "string" }
                    ]
                }),
                proxy : {
                    type    : "rpc",
                    rpcData : {
                        service : "PXE",
                        method  : "getEntryList"
                    }
                }
            })
        });
        me.callParent(arguments);
    },

    onAddButton: function() {
        var me = this;
        Ext.create("OMV.module.admin.service.pxe.Entry", {
            title     : _("Add entry"),
            uuid      : OMV.UUID_UNDEFINED,
            listeners : {
                scope  : me,
                submit : function() {
                    this.doReload();
                }
            }
        }).show();
    },

    onEditButton : function() {
        var me = this;
        var record = me.getSelected();
        Ext.create("OMV.module.admin.service.pxe.Entry", {
            title     : _("Edit Entry"),
            uuid      : record.get("uuid"),
            listeners : {
                scope  : me,
                submit : function() {
                    this.doReload();
                }
            }
        }).show();
    },

    doDeletion : function(record) {
        var me = this;
        OMV.Rpc.request({
            scope   : me,
            callback : me.onDeletion,
            rpcData  : {
                service : "PXE",
                method  : "deleteEntry",
                params  : {
                    uuid : record.get("uuid")
                }
            }
        });
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "entries",
    path      : "/service/pxe",
    text      : _("Entries"),
    position  :  30,
    className : "OMV.module.admin.service.pxe.Entries"
});
