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

System.Net.Protocol.HTTP.Request = Class.create({
    initialize: function (url, options) {
        if (!url) {
            throw "You have to pass at least a url.";
        }

        this.options = Object.extend({
            method : "GET",
            port   : System.Net.Ports.HTTP,
            timeout: 10,
            ssl    : false,
            requestHeaders: {}
        }, options);

        this.setDefaultHeaders(this.options.requestHeaders);

        // Implement sockopts for the timeout also ssl stuff.

        var port;
        if (port = /^[^:]:(\d+)\//.exec(url)) {
            this.options.port = port[1];
        }
        else {
            this.options.port = this.options.port;
        }

        if (url.match(/^https/)) {
            this.options.ssl = true;
        }

        var data;
        if (data = /^(http(s)?:\/\/)?([^:\/]*)(:\d+)?(\/.*)?$/.exec(url)) {
            this.options.host = data[3];
            this.options.page = data[5] || "/";
        }
        else {
            throw "The url isn't a valid url, probably.";
        }

        this.socket = new System.Net.Socket;
        if (!this.socket.connect(this.options.host, this.options.port)) {
            throw "Couldn't connect to the host.";
        }

        this.socket.sendLine([
            "{0} {1} HTTP/1.1".format([this.options.method.toUpperCase(), this.options.page]),
            "Host: {0}".format([this.options.host]),
        ].concat(this.getRequestHeadersArray()).concat([""]));

        var answer  = System.Net.Protocol.HTTP.parseResponse(this.socket.receiveLine());
        var headers = '';
        var content;

        var line;
        while (line = this.socket.receiveLine()) {
            headers += line+"\n";
        }
        headers = System.Net.Protocol.HTTP.parseHeaders(headers);
        content = this.socket.receive(headers["Content-Length"]);

        this.response = new System.Net.Protocol.HTTP.Response(answer, headers, content);
    },

    setDefaultHeaders: function (headers) {
        if (Object.is(headers, Array)) {
            if (headers.length % 2) {
                throw "The array has a odd length.";
            }

            var headerz = headers;
            headers     = new Object;

            for (var i = 0; i < headerz.length; i+=2) {
                headers[headerz[i]] = headerz[i+1];
            }
        }

        this.options.requestHeaders = {
            'Connection': 'close',
            'User-Agent': 'lulzJS/'+__VERSION__
        };

        Object.extend(this.options.requestHeaders, headers);
    },

    getRequestHeadersArray: function () {
        var headerz = [];

        for (var header in this.options.requestHeaders) {
            headerz.push("{name}: {value}".format({
                name: header,
                value: this.options.requestHeaders[header]
            }));
        }

        return headerz;
    },
});
