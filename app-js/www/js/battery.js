var battery = new Battery();

//Initialize google charts
//https://developers.google.com/chart/interactive/docs/gallery/annotationchart

// Load the Visualization API and the piechart package.
//google.load('visualization', '1.0', {'packages':['corechart']});
//google.load('visualization', '1', {'packages':['annotationchart']});

google.load('visualization', '1', {'packages':['annotationchart']});
//google.setOnLoadCallback(battery.draw);
//google.setOnLoadCallback(drawChart);

$(document).ready(function() {
    $('#battery-button').click(function(){
      setTimeout(battery.draw, 100);
    });
    
    setInterval(battery.draw, 1000);
});


function Battery(){

  var records = [];
/*
  var batteryValue = 4000;
  setInterval(function(){
    battery.add(batteryValue-= 100);
  }, 1000);
*/
  this.add = function(value){
    var now = new Date();
    records.push({date: now, value: parseInt(value) });

    //this.draw();
  };

  this.clear = function(){
    records = [];
  };

  this.draw = function(){

    if (records.length == 0) return;
    //log(TAG, "Drawing battery over "+records.length +" points");

    var data = new google.visualization.DataTable();
    data.addColumn('date', 'Date');
    data.addColumn('number', 'value');

    var rows = [];
    for(var i=0; i< records.length; i++){
      var r = records[i];
      rows.push([r.date, r.value]);
    }
    data.addRows(rows);

    var chart = new google.visualization.AnnotationChart(document.getElementById('chart_div'));
    chart.draw(data, {displayAnnotations: true, dateFormat: "HH:mm:ss", max: 4000, min:3000});
  };
}
