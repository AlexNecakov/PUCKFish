const express = require("express");
const bodyParser = require('body-parser');
const serialport = require('serialport');
const SerialPort = serialport.SerialPort;
const { ReadlineParser } = require('@serialport/parser-readline')
const app = express();

var appPort = 6969;
var current_speed = 0;
var current_Error = 0;
var current_Front = 0;

const port = new SerialPort({
    path: 'COM10',
    baudRate: 9600
});
const parser = port.pipe(new ReadlineParser({ delimiter: '\r\n' }))

//web client communication
app.listen(process.env.PORT || appPort, () => {
    console.log(`Application started and Listening on port ${appPort}`);
});

app.use(express.static(__dirname));
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());
app.use(bodyParser.raw());

app.get("/", (req, res) => {
    res.sendFile(__dirname + "/index.html");
    res.sendFile(__dirname + "/source.js");
    res.sendFile(__dirname + "/styles.css");
});

app.get("/data", async (req, res) => {
    res.sendFile(__dirname + "/index.html");
    res.sendFile(__dirname + "/source.js");
    res.sendFile(__dirname + "/styles.css");

    res.end(JSON.stringify([current_speed, current_Error, current_Front]));
});

// // Read data that is available but keep the stream in "paused mode"
// port.on('readable', function () {
//     console.log('Data:', port.read())
// })

// Switches the port into "flowing mode"
// port.on('data', function (data) {
//     console.log('Data:', data)
// })
parser.on('data', console.log)

// server.on('message', function (msg, info) {
//     console.log(typeof (msg), msg.length)
//     var msgString = msg.toString();
//     console.log(msg[0], msg[2], msg[4]);
//     current_speed = ((msg[0] << 8) | msg[1]) / 1000;
//     current_Error = ((msg[2] << 8) | msg[3]);
//     current_Front = ((msg[4] << 8) | msg[5]);
//     console.log(current_speed, current_Error, current_Front)
//     // response = Buffer.from('g')
//     console.log(info.address)
//     message = new Buffer(stop);
//     console.log(info.port, info.address);
//     portOfESP = info.port;
//     server.send(JSON.stringify(stop), info.port, info.address, (err) => {
//         console.log("no conection")
//     });

// });

// server.on('error', function (error) {
//     console.log('Error: ' + error);
//     server.close();
// });