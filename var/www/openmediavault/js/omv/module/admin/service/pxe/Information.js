/**
 * Copyright (C) 2013 OpenMediaVault Plugin Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/form/Panel.js")

/**
 * @class OMV.module.admin.service.vdr.Info
 * @derived OMV.workspace.form.Panel
 */
Ext.define("OMV.module.admin.service.pxe.Information", {
    extend : "OMV.workspace.form.Panel",

    autoLoadData    : false,
    hideOkButton    : true,
    hideResetButton : true,
    mode            : "local",

    getFormItems : function() {
        var me = this;

        return [{
            /* VDR info */
            xtype  : "fieldset",
            title  : _("Information"),
            layout : "fit",
            items  : [{
                border  : false,
                html    : '<h3 style="margin-top: 5px;">TFTP</h3>' +
                        'You need to enable TFTP and set the shared Folder for it. It has to be the same as the one you use in the PXE Plugin. It will also the shared folder where you store your PXE boot images.' +                  
                        '<h3>Dnsmasq Settings</h3>' +
                        'You need to set the following in the dnsmasq Plugin.' +
                        '<ul>' +
                        '<li>' +
                        '<b>General / Enable</b><br>' +
						'Put in the checkmark.' +
                        '</li>' +
						'<br>' +
                        '<li>' +
                        '<b>Domain Name</b> Choose what you want. In example<br>' +
						'local.domain<br>' +
                        '</li>' +
						'<br>' +
                        '<li>' +
						'<b>DHCP Settings / Enable</b><br>' +
						'Put in the checkmark.' +
                        '</li>' +
						'<br>' +
                        '<li>' +
						'<b>DNS Servers</b> Put in your DNS Servers. In example<br>' +
						'192.168.1.1,8.8.8.8' +
                        '</li>' +
						'<br>' +
                        '<li>' +
						'<b>DHCP Boot</b><br>' +
						'/pxelinux.0,0.0.0.0' +
						'</li>' +
						'<br>' +
                        '<li>' +
						'<b>Extra Options</b> Replace 192.168.178.0 with your Network Range.<br>' + 
						'port=0<br>' +
'dhcp-range=192.168.178.0,proxy<br>' +
'pxe-service=x86PC,"PXE Boot-Server",pxelinux<br>' +
'resolv-file=/etc/resolv.conf<br>' +
						'</li>' +
                        '</ul>' 
            }]
        }];
    }

});

OMV.WorkspaceManager.registerPanel({
    id        : "information",
    path      : "/service/pxe",
    text      : _("Setup Info"),
    position  : 30,
    className : "OMV.module.admin.service.pxe.Information"
});
