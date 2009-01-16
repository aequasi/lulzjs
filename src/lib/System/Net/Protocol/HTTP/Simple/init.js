/****************************************************************************
* This file is part of lulzJS                                               *
* Copyleft meh.                                                             *
*                                                                           *
* lulzJS is free software: you can redistribute it and/or modify            *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation, either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* lulzJS is distributed in the hope that it will be useful.                 *
* but WITHOUT ANY WARRANTY; without even the implied warranty o.            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See th.             *
* GNU General Public License for more details.                              *
*                                                                           *
* You should have received a copy of the GNU General Public License         *
* along with lulzJS.  If not, see <http://www.gnu.org/licenses/>.           *
****************************************************************************/

require("Net/Net.so");

require(["Net/Socket/Socket.so", "Net/Socket/Socket.js"]);

require("Net/Ports/Ports.js");

require("Net/Protocol/Protocol.so");

require([
    "Net/Protocol/HTTP/HTTP.so", "Net/Protocol/HTTP/HTTP.js",
    "Net/Protocol/HTTP/Request.js", "Net/Protocol/HTTP/Response.js",
    "Net/Protocol/HTTP/Client.js"
]);

require("Simple.js");

if (!Program.HTTP) {
    Program.HTTP = new Object;
}

Program.HTTP = Object.extend(Program.HTTP, {
    Get:  System.Net.Protocol.HTTP.Simple.Get,
    Post: System.Net.Protocol.HTTP.Simple.Post
});

