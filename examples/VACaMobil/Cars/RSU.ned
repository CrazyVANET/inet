//
// Copyright (C) 2006-2012 Christoph Sommer <christoph.sommer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

package inet.examples.VACaMobil.Cars;

import inet.base.NotificationBoard;
import inet.networklayer.autorouting.ipv4.HostAutoConfigurator;
import inet.networklayer.common.InterfaceTable;
import inet.applications.udpapp.UDPBasicBurst;
import inet.mobility.models.StationaryMobility;
import inet.networklayer.ipv4.RoutingTable;
import inet.networklayer.manetrouting.DYMO;
import inet.transport.udp.UDP;
import inet.nodes.inet.NetworkLayer;
import inet.linklayer.ieee80211.Ieee80211Nic;

module RSU
{
    parameters:
        @node();

    gates:
        input radioIn @directIn;

    submodules:
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=60,160");
        }
        ac_wlan: HostAutoConfigurator {
            @display("p=60,401");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=60,240");
        }
        app: UDPBasicBurst {
            parameters:
                @display("p=304,56");
        }
        mobility: StationaryMobility {
            parameters:
                @display("p=60,459");
        }
        routingTable: RoutingTable {
            parameters:
                IPForward = true;
                routerId = "";
                routingFile = "";
                @display("p=60,326");
        }
        udp: UDP {
            parameters:
                @display("p=304,192");
        }
        networkLayer: NetworkLayer {
            parameters:
                proxyARP = false;
                @display("p=304,327;q=queue");
            gates:
                ifIn[1];
                ifOut[1];
        }
        wlan: Ieee80211Nic {
            parameters:
                @display("p=304,461;q=queue");
        }
        manetrouting: DYMO {
            @display("p=189,329");
        }
    connections allowunconnected:
        udp.appOut++ --> app.udpIn;
        udp.appIn++ <-- app.udpOut;

        udp.ipOut --> networkLayer.udpIn;
        udp.ipIn <-- networkLayer.udpOut;

        wlan.upperLayerOut --> networkLayer.ifIn[0];
        wlan.upperLayerIn <-- networkLayer.ifOut[0];
        
		networkLayer.manetOut --> manetrouting.from_ip;
		networkLayer.manetIn <-- manetrouting.to_ip;
		
        radioIn --> wlan.radioIn;
}

