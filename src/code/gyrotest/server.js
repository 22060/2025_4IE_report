const dgram = require('dgram');
var os = require('os');
const WebSocketServer = require('websocket').server;
const http = require('http')
var clients = [];

console.log(new Date().getTime());
const PORT = 6000
function getLocalAddress() {
    var ifacesObj = {}
    var result = ""
    ifacesObj.ipv4 = [];
    ifacesObj.ipv6 = [];
    var interfaces = os.networkInterfaces();

    for (var dev in interfaces) {
        interfaces[dev].forEach(function(details){
            if (!details.internal){
                switch(details.family){
                    case "IPv4":
                        ifacesObj.ipv4.push({name:dev, address:details.address});
                        result = details.address
                    break;
                    case "IPv6":
                        ifacesObj.ipv6.push({name:dev, address:details.address})
                    break;
                }
            }
        });
    }
    result = "127.0.0.1"
    return result;
};
const socket = dgram.createSocket('udp4');
socket.on('listening',() => {
  const address = socket.address();
  console.log("UDP socket listening on" + address.address + ":" + address.port);
})

socket.on("message",(message,remote)=>{
    var data = message.toString().slice(0,-1)
    data = data.split(",")
    console.log(data)
    clients.forEach((client) => {
      client.send(JSON.stringify(data))
    }
    )
})
socket.bind(PORT,getLocalAddress())

const PORT2 = 8080
const server1 = http.createServer((request, response) => {
  const interval = setInterval(() => {
    response.writeHead(200, { 'Content-Type': 'image/png;charset=buffer' })
    response.end(c_array,"binary")
  }, 100);
})

server1.listen(PORT2, () => {
        // console.log(` サーバ起動 http://localhost:${PORT2}`)
})
const wsServer = new WebSocketServer({
  httpServer: server1,
  autoAcceptConnections: false
})

const originIsAllowed = (origin) => {
  return true
}

var result

wsServer.on('request', (request) => {
  // console.log(request.remoteAddress)
  if (!originIsAllowed(request.origin)) {
    // console.log(`${request.origin} からのアクセスが拒否されました`)
  }

  const connection = request.accept('', request.origin)
  // console.log(`接続が許可されました`)
  var index = clients.push(connection) - 1;
  connection.on('message', message => {
      switch (message.type) {
        case 'utf8':
          // console.log(message.utf8Data)
          break
        case 'binary':
          // console.log(message.binaryData)
          // 中略
      }
    })

  connection.on('close', (reasonCode, description) => {
    // console.log(`${reasonCode} によって切断されました`)
    clients.splice(index, 1);
  })
})