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
    drawAmbientLight();
    drawSalinity();
    drawDissolvedOxygen();
    drawPressure();
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

    timeStamp.push(vals[0]/1000);
    acceleration[0].push(vals[1][0]);
    acceleration[1].push(vals[1][1]);
    acceleration[2].push(vals[1][2]);
    orientation[0].push(vals[2][0]);
    orientation[1].push(vals[2][1]);
    orientation[2].push(vals[2][2]);
    temperature.push(vals[3]);
    ambientLight.push(vals[4]);
    salinity.push(vals[5]);
    dissolvedOxygen.push(vals[6]);
    depth.push(vals[7]);
    
    drawTemperature();
    drawAmbientLight();
    drawSalinity();
    drawDissolvedOxygen();
    drawPressure();

}, queryTime);

async function getData() {
    const response = await fetch("/data");
    return response.json();
}

function drawTemperature() {
    var data = [];
    for (var i = 0; i < timeStamp.length; i++) {
        var obj = { x: timeStamp[i], y: temperature[i] };
        data.push(obj)
    }
    data.sort();

    var chart = new CanvasJS.Chart("graphT", {
        animationEnabled: false,
        title: {
            text: "Temperature"
        },
        axisX: {
            title: "Time (seconds)"
        },
        axisY: {
            title: "Temperature (C)",
        },
        data: [{
            type: "scatter",
            dataPoints: data,
        }]
    });
    chart.render();
}

function drawAmbientLight() {
    var data = [];
    for (var i = 0; i < timeStamp.length; i++) {
        var obj = { x: timeStamp[i], y: ambientLight[i] };
        data.push(obj)
    }
    data.sort();

    var chart = new CanvasJS.Chart("graphL", {
        animationEnabled: false,
        title: {
            text: "Ambient Light"
        },
        axisX: {
            title: "Time (seconds)"
        },
        axisY: {
            title: "Ambient Light (Lux)",
        },
        data: [{
            type: "scatter",
            dataPoints: data,
        }]
    });
    chart.render();
}

function drawSalinity() {
    var data = [];
    for (var i = 0; i < timeStamp.length; i++) {
        var obj = { x: timeStamp[i], y: salinity[i] };
        data.push(obj)
    }
    data.sort();

    var chart = new CanvasJS.Chart("graphS", {
        animationEnabled: false,
        title: {
            text: "Salinity"
        },
        axisX: {
            title: "Time (seconds)"
        },
        axisY: {
            title: "Salinity (ppt)",
        },
        data: [{
            type: "scatter",
            dataPoints: data,
        }]
    });
    chart.render();
}

function drawDissolvedOxygen() {
    var data = [];
    for (var i = 0; i < timeStamp.length; i++) {
        var obj = { x: timeStamp[i], y: dissolvedOxygen[i] };
        data.push(obj)
    }
    data.sort();

    var chart = new CanvasJS.Chart("graphD", {
        animationEnabled: false,
        title: {
            text: "Dissolved Oxygen"
        },
        axisX: {
            title: "Time (seconds)"
        },
        axisY: {
            title: "Percent Saturation (%)",
        },
        data: [{
            type: "scatter",
            dataPoints: data,
        }]
    });
    chart.render();
}

function drawPressure() {
    var data = [];
    for (var i = 0; i < timeStamp.length; i++) {
        var obj = { x: timeStamp[i], y: depth[i] };
        data.push(obj)
    }
    data.sort();

    var chart = new CanvasJS.Chart("graphP", {
        animationEnabled: false,
        title: {
            text: "Depth"
        },
        axisX: {
            title: "Time (seconds)"
        },
        axisY: {
            title: "Depth (m)",
        },
        data: [{
            type: "scatter",
            dataPoints: data,
        }]
    });
    chart.render();
}
