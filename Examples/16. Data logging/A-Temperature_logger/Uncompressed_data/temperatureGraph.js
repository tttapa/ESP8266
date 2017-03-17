var dataArray = [];

var maxDate = new Date();
var minDate = new Date();

var zoom = 6;
var zoomFactor = 24*60*60*1000; // 1 day
var maxZoom = 14; // ~ 8.4 months

loadCSV();

function drawChart() {

    var data = new google.visualization.DataTable();
    data.addColumn('datetime', 'UNIX');
    data.addColumn('number', 'Temperature');

    data.addRows(dataArray);

    var options = {
        curveType: 'function',

        height: 360,

        legend: { position: 'none' },

        hAxis: {
            viewWindow: {
                min: minDate,
                max: maxDate
            },
            gridlines: {
                count: -1,
                units: {
                    days: { format: ['MMM dd'] },
                    hours: { format: ['HH:mm', 'ha'] },
                }
            },
            minorGridlines: {
                units: {
                    hours: { format: ['hh:mm:ss a', 'ha'] },
                    minutes: { format: ['HH:mm a Z', ':mm'] }
                }
            }
        },
        vAxis: {
            title: "Temperature (Celsius)"
        }
    };

    var chart = new google.visualization.LineChart(document.getElementById('chart_div'));

    chart.draw(data, options);
}

function parseCSV(string) {
    var array = [];
    var lines = string.split("\n");
    for (var i = 0; i < lines.length; i++) {
        var data = lines[i].split(",");
        data[0] = new Date(parseInt(data[0]) * 1000);
        data[1] = parseFloat(data[1]);

        array.push(data);
    }
    return array;
}

function loadCSV() {
    var xmlhttp = new XMLHttpRequest();
    xmlhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            dataArray = parseCSV(this.responseText);
            google.charts.load('current', { 'packages': ['line', 'corechart'] });
            google.charts.setOnLoadCallback(setRange);
        }
    };
    xmlhttp.open("GET", "temp.csv", true);
    xmlhttp.send();
}

function updateDate() {
    var dateDiv = document.getElementById("date");
    var maxDay = maxDate.getDate();
    var maxMonth = maxDate.getMonth();
    var minDay = minDate.getDate();
    var minMonth = minDate.getMonth()
    if (maxDay == minDay && maxMonth == minMonth) {
        dateDiv.textContent = (maxDay).toString() + "/" + (maxMonth + 1).toString();
    } else {
        dateDiv.textContent = (minDay).toString() + "/" + (minMonth + 1).toString() + " - " + (maxDay).toString() + "/" + (maxMonth + 1).toString();

    }
}

updateDate();

document.getElementById("prev").onclick = function() {
    maxDate = new Date(maxDate.getTime() - getZoomTime()/3);
    setRange();
}
document.getElementById("next").onclick = function() {
    maxDate = new Date(maxDate.getTime() + getZoomTime()/3);
    setRange();
}

document.getElementById("zoomout").onclick = function() {
    zoom += 1;
    if(zoom > maxZoom) zoom = maxZoom;
    else setRange();
}
document.getElementById("zoomin").onclick = function() {
    zoom -= 1;
    if(zoom < 0) zoom = 0;
    else setRange();
}

document.getElementById("reset").onclick = function() {
    maxDate = new Date();
    zoom = 6;
    setRange();
}
document.getElementById("refresh").onclick = function() {
    maxDate = new Date();
    loadCSV();
}

function setRange() {
    minDate = new Date(maxDate.getTime() - getZoomTime());
    updateDate();
    drawChart();
}
function getZoomTime() {
    return zoomFactor*(2**(zoom-6));
}
document.body.onresize = drawChart;
