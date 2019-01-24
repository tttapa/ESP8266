var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);

connection.onopen = function () {
    connection.send('This is Connect ' + new Date());
};
connection.onerror = function (error) {
    console.log('WebSocket Error ', error);
};
connection.onmessage = function (e) {
    console.log('Server: ', e.data);

    try{
      var json = JSON.parse(e.data);
      if(json){
        var currentTemp =  json.temperature;//e.data.split(":")[1];
        var prevTemp = localStorage.getItem("currentTemp");

        localStorage.setItem("currentTemp", currentTemp);

        var tempTitle = currentTemp + "&#8451;"; //celsius
        if(prevTemp != currentTemp ){
          tempTitle += ( parseFloat(prevTemp) < parseFloat(currentTemp) ?   "&#8593;" : "&#8595;" )
        }

        var b = document.getElementById ("current_temperature");

        if(b != null)
          var dt = new Date(json.time*1000);
          var mess = dt.getHours() + ":" + dt.getMinutes() + ":" + dt.getSeconds();
          b.innerHTML = tempTitle + " at: " + mess;
      }
    }
    catch(e){
      console.log("This is propably not json object" + e );
    }
};

connection.onclose = function(){
    console.log('WebSocket connection closed');
};
