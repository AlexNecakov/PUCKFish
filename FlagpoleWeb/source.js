var timeStamp = [];
var acceleration = [[], [], []];
var orientation = [[], [], []];
var temperature = [];
var ambientLight = [];
var salinity = [];
var dissolvedOxygen = [];
var depth = [];

var queryTime = 1000;
var clearTime = 1000;

window.onload = async function () {
    drawTemperature();
}

const fetchs = setInterval(async function () {
    vals = await getData();

    // keep x entries
    if (timeStamp.length > clearTime) {
        timeStamp = [];
        acceleration = [[], [], []];
        orientation = [[], [], []];
        temperature = [];
        ambientLight = [];
        salinity = [];
        dissolvedOxygen = [];
        depth = [];
    }

    timeStamp.push(vals[0]);
    // acceleration[0].push(vals["acceleration"][0]);
    // acceleration[1].push(vals["acceleration"][1]);
    // acceleration[2].push(vals["acceleration"][2]);
    // orientation[0].push(vals["orientation"][0]);
    // orientation[1].push(vals["orientation"][1]);
    // orientation[2].push(vals["orientation"][2]);
    temperature.push(vals[3]);
    // ambientLight.push(vals["ambientLight"]);
    // salinity.push(vals["salinity"]);
    // dissolvedOxygen.push(vals["dissolvedOxygen"]);
    // depth.push(vals["depth"]);
    console.log(timeStamp);
    drawTemperature();

}, queryTime);

async function getData() {
    const response = await fetch("/data");
    return response.json();
}

function drawTemperature() {
    var chart = new CanvasJS.Chart("graph1", {
        animationEnabled: false,
        title: {
            text: "Temperature"
        },
        axisX: {
            title: "Time"
        },
        axisY: {
            title: "Temperature",
        },
        data: [{
            type: "line",
            dataPoints: [
                { x: timeStamp },
                { y: temperature }
            ],
        }]
    });
    chart.render();
}