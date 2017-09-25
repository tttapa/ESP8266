var dataArray = [];

var defaultZoomTime = 24*60*60*1000; // 1 day
var minZoom = -6; // 22 minutes 30 seconds
var maxZoom = 8; // ~ 8.4 months

var zoomLevel = 0;
var viewportEndTime = new Date();
var viewportStartTime = new Date();

loadCSV(); // Download the CSV data, load Google Charts, parse the data, and draw the chart


/*
Structure:

    loadCSV
        callback:
        parseCSV
        load Google Charts (anonymous)
            callback:
            updateViewport
                displayDate
                drawChart
*/

/*
               |                    CHART                    |
               |                  VIEW PORT                  |
invisible      |                   visible                   |      invisible
---------------|---------------------------------------------|--------------->  time
       viewportStartTime                              viewportEndTime

               |______________viewportWidthTime______________|

viewportWidthTime = 1 day * 2^zoomLevel = viewportEndTime - viewportStartTime
*/

function loadCSV() {
    var xmlhttp = new XMLHttpRequest();
    xmlhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            dataArray = parseCSV(this.responseText);
            google.charts.load('current', { 'packages': ['line', 'corechart'] });
            google.charts.setOnLoadCallback(updateViewport);
        }
    };
    xmlhttp.open("GET", "temp.csv", true);
    xmlhttp.send();
    var loadingdiv = document.getElementById("loading");
    loadingdiv.style.visibility = "visible";
}

function parseCSV(string) {
    var array = [];
    var lines = string.split("\n");
    for (var i = 0; i < lines.length; i++) {
        var data = lines[i].split(",", 2);
        data[0] = new Date(parseInt(data[0]) * 1000);
        data[1] = parseFloat(data[1]);
        array.push(data);
    }
    return array;
}

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
                min: viewportStartTime,
                max: viewportEndTime
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

    var dateselectdiv = document.getElementById("dateselect");
    dateselectdiv.style.visibility = "visible";

    var loadingdiv = document.getElementById("loading");
    loadingdiv.style.visibility = "hidden";
}

function displayDate() { // Display the start and end date on the page
    var dateDiv = document.getElementById("date");

    var endDay = viewportEndTime.getDate();
    var endMonth = viewportEndTime.getMonth();
    var startDay = viewportStartTime.getDate();
    var startMonth = viewportStartTime.getMonth()
    if (endDay == startDay && endMonth == startMonth) {
        dateDiv.textContent = (endDay).toString() + "/" + (endMonth + 1).toString();
    } else {
        dateDiv.textContent = (startDay).toString() + "/" + (startMonth + 1).toString() + " - " + (endDay).toString() + "/" + (endMonth + 1).toString();
    }
}

document.getElementById("prev").onclick = function() {
    viewportEndTime = new Date(viewportEndTime.getTime() - getViewportWidthTime()/3); // move the viewport to the left for one third of its width (e.g. if the viewport width is 3 days, move one day back in time)
    updateViewport();
}
document.getElementById("next").onclick = function() {
    viewportEndTime = new Date(viewportEndTime.getTime() + getViewportWidthTime()/3); // move the viewport to the right for one third of its width (e.g. if the viewport width is 3 days, move one day into the future)
    updateViewport();
}

document.getElementById("zoomout").onclick = function() {
    zoomLevel += 1; // increment the zoom level (zoom out)
    if(zoomLevel > maxZoom) zoomLevel = maxZoom;
    else updateViewport();
}
document.getElementById("zoomin").onclick = function() {
    zoomLevel -= 1; // decrement the zoom level (zoom in)
    if(zoomLevel < minZoom) zoomLevel = minZoom;
    else updateViewport();
}

document.getElementById("reset").onclick = function() {
    viewportEndTime = new Date(); // the end time of the viewport is the current time
    zoomLevel = 0; // reset the zoom level to the default (one day)
    updateViewport();
}
document.getElementById("refresh").onclick = function() {
    viewportEndTime = new Date(); // the end time of the viewport is the current time
    loadCSV(); // download the latest data and re-draw the chart
}

document.body.onresize = drawChart;

function updateViewport() {
    viewportStartTime = new Date(viewportEndTime.getTime() - getViewportWidthTime());
    displayDate();
    drawChart();
}
function getViewportWidthTime() {
    return defaultZoomTime*(2**zoomLevel); // exponential relation between zoom level and zoom time span
                                           // every time you zoom, you double or halve the time scale
}

