const express = require("express");
const bodyParser = require('body-parser');
const serialport = require('serialport');
const SerialPort = serialport.SerialPort;
const { ReadlineParser } = require('@serialport/parser-readline');
const { json } = require("express");
const app = express();

var appPort = 8000;

var timeStamp = 0;
var acceleration = [0, 0, 0];
var orientation = [0, 0, 0];
var temperature = 0;
var ambientLight = 0;
var salinity = 0;
var dissolvedOxygen = 0;
var depth = 0;

const port = new SerialPort({
    path: 'COM10',
    baudRate: 9600
});
const parser = port.pipe(new ReadlineParser({ delimiter: '\r\n' }))

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

    var jsonString = JSON.stringify([timeStamp, acceleration, orientation, temperature, ambientLight, salinity, dissolvedOxygen, depth]);
    console.log('Sending: ', jsonString);
    res.end(jsonString);
});

parser.on('data', function (data) {
    // console.log('Received: ', data)
    var parsedData = JSON.parse(data);

    timeStamp = parsedData["#"];
    acceleration = parsedData.a;
    orientation = parsedData.o;
    temperature = parsedData.t;
    ambientLight = parsedData.l;
    salinity = parsedData.s;
    dissolvedOxygen = parsedData.d;
    depth = parsedData.p;
});