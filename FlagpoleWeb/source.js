var timeStamp = [];
var acceleration = [[], [], []];
var orientation = [[], [], []];
var temperature = [];
var ambientLight = [];
var salinity = [];
var dissolvedOxygen = [];
var depth = [];

var queryTime = 1000;
var clearTime = 500;

window.onload = async function () {
    drawAcceleration();
    drawOrientation();
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

    timeStamp.push(vals[0] / 1000);
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

    drawAcceleration();
    drawOrientation();
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

function drawAcceleration() {
    var dataX = [];
    for (var i = 0; i < timeStamp.length; i++) {
        var obj = { x: timeStamp[i], y: acceleration[0][i] };
        dataX.push(obj)
    }
    dataX.sort();
    var dataY = [];
    for (var i = 0; i < timeStamp.length; i++) {
        var obj = { x: timeStamp[i], y: acceleration[1][i] };
        dataY.push(obj)
    }
    dataY.sort();
    var dataZ = [];
    for (var i = 0; i < timeStamp.length; i++) {
        var obj = { x: timeStamp[i], y: acceleration[2][i] };
        dataZ.push(obj)
    }
    dataZ.sort();

    var chart = new CanvasJS.Chart("graphA", {
        animationEnabled: false,
        title: {
            text: "Acceleration"
        },
        axisX: {
            title: "Time (seconds)"
        },
        axisY: {
            title: "Acceleration (m/s^2)",
        },
        data: [{
            type: "scatter",
            toolTipContent: "<b>Time: </b>{x} sec<br/><b>Acceleration X: </b>{y} m/s^2",
            dataPoints: dataX,
            color: "mediumseagreen"
        }, {
            type: "scatter",
            toolTipContent: "<b>Time: </b>{x} sec<br/><b>Acceleration Y: </b>{y} m/s^2",
            dataPoints: dataY,
            color: "mediumturquoise"
        }, {
            type: "scatter",
            toolTipContent: "<b>Time: </b>{x} sec<br/><b>Acceleration Z: </b>{y} m/s^2",
            dataPoints: dataZ,
            color: "mediumblue"
        }]
    });
    chart.render();
}

function drawOrientation() {
    var dataX = [];
    for (var i = 0; i < timeStamp.length; i++) {
        var obj = { x: timeStamp[i], y: orientation[0][i] };
        dataX.push(obj)
    }
    dataX.sort();
    var dataY = [];
    for (var i = 0; i < timeStamp.length; i++) {
        var obj = { x: timeStamp[i], y: orientation[1][i] };
        dataY.push(obj)
    }
    dataY.sort();
    var dataZ = [];
    for (var i = 0; i < timeStamp.length; i++) {
        var obj = { x: timeStamp[i], y: orientation[2][i] };
        dataZ.push(obj)
    }
    dataZ.sort();

    var chart = new CanvasJS.Chart("graphO", {
        animationEnabled: false,
        title: {
            text: "Orientation"
        },
        axisX: {
            title: "Time (seconds)"
        },
        axisY: {
            title: "Orientation (rad/s)",
        },
        data: [{
            type: "scatter",
            toolTipContent: "<b>Time: </b>{x} sec<br/><b>Orientation X: </b>{y} rad/s",
            dataPoints: dataX,
            color: "lightseagreen"
        }, {
            type: "scatter",
            toolTipContent: "<b>Time: </b>{x} sec<br/><b>Orientation Y: </b>{y} rad/s",
            dataPoints: dataY,
            color: "lightskyblue"
        }, {
            type: "scatter",
            toolTipContent: "<b>Time: </b>{x} sec<br/><b>Orientation Z: </b>{y} rad/s",
            dataPoints: dataZ,
            color: "mediumaquamarine"
        }]
    });
    chart.render();
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
            toolTipContent: "<b>Time: </b>{x} sec<br/><b>Temperature: </b>{y} C",
            dataPoints: data,
            color: "dodgerblue"
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
            toolTipContent: "<b>Time: </b>{x} sec<br/><b>Ambient Light: </b>{y} lux",
            dataPoints: data,
            color: "darkturquoise"
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
            toolTipContent: "<b>Time: </b>{x} sec<br/><b>Salinity: </b>{y} ppt",
            dataPoints: data,
            color: "cornflowerblue"
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
            title: "Oxygen Saturation (%)",
        },
        data: [{
            type: "scatter",
            toolTipContent: "<b>Time: </b>{x} sec<br/><b>Oxygen Saturation: </b>{y}%",
            dataPoints: data,
            color: "royalblue"
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
            toolTipContent: "<b>Time: </b>{x} sec<br/><b>Depth: </b>{y} m",
            dataPoints: data,
            color: "midnightblue"
        }]
    });
    chart.render();
}
