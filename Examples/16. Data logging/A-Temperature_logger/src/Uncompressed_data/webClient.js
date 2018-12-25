var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);

connection.onopen = function () {
    connection.send('This is Connect ' + new Date());
};
connection.onerror = function (error) {
    console.log('WebSocket Error ', error);
};
connection.onmessage = function (e) {
    console.log('Server: ', e.data);

    if(e.data == "updateTemp"){

    }
};
connection.onclose = function(){
    console.log('WebSocket connection closed');
};

function sendRGB() {
    var rgbstr = '# 23'
    console.log('RGB: ' + rgbstr);
    connection.send(rgbstr);
}
