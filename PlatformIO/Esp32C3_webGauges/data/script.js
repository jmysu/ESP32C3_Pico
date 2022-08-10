// Get current sensor readings when the page loads  
window.addEventListener('load', getReadings);
var gauge_val = 0;
 

// Function to get current readings on the webpage when it loads for the first time
function getReadings(){
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var myObj = JSON.parse(this.responseText);
      console.log(myObj); 
      var donut = myObj.donut; 
      gauge_val = donut;
    }
  }; 
  xhr.open("GET", "/readings", true);
  xhr.send();
}

if (!!window.EventSource) {
  var source = new EventSource('/events');
  
  source.addEventListener('open', function(e) {
    console.log("Events Connected");
  }, false);

  source.addEventListener('error', function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Events Disconnected");
    }
  }, false);
  
  source.addEventListener('message', function(e) {
    console.log("message", e.data);
  }, false);
  
  source.addEventListener('new_readings', function(e) {
    console.log("new_readings", e.data);
    var myObj = JSON.parse(e.data);
    console.log(myObj); 
    gauge_val = myObj.donut;
  }, false);
}

 
      var pad = function(tar) {}

      var gauge0 = Gauge(document.getElementById("gauge0"));

      var gauge1 = Gauge(
        document.getElementById("gauge1"),
            {
          max: 100,
          dialStartAngle: -90,
          dialEndAngle: -90.001,
          value: 100,
          label: function(value) {
            return (Math.round(value * 100) / 100) + "/" + this.max;
          }
        }
      );

      var gauge2 = Gauge(
        document.getElementById("gauge2"),
            {
          min: -100,
          max: 100,
          dialStartAngle: 180,
          dialEndAngle: 0,
          value: 50,
          viewBox: "0 0 100 57",
          color: function(value) {
            if(value < 20) {
              return "#5ee432";
            }else if(value < 40) {
              return "#fffa50";
            }else if(value < 60) {
              return "#f7aa38";
            }else {
              return "#ef4655";
            }
          }
        }
      );

      var gauge3 = Gauge(
        document.getElementById("gauge3"),
            {
          max: 100,
          value: 50
        }
      );

      var gauge4 = Gauge(
        document.getElementById("gauge4"),
            {
          max: 100,
          dialStartAngle: 180,
          dialEndAngle: -90,
          viewBox: "0 0 60 60",
          value: 50
        }
      );

      var gauge5 = Gauge(
        document.getElementById("gauge5"),
            {
          max: 200,
          dialStartAngle: 0,
          dialEndAngle: -180,
          viewBox: "0 35 100 100",
          value: 50
        }
      );

      var gauge6 = Gauge(
        document.getElementById("gauge6"),
            {
          max: 100,
          dialStartAngle: 90.01,
          dialEndAngle: 89.99,
          dialRadius: 15,
          showValue: false,
          value: 100
        }
      );
      (function loop() {
        gauge0.setValue(gauge_val, 1);
        gauge1.setValueAnimated(gauge_val, 1);
        gauge2.setValueAnimated(50 - gauge_val, 3);
        gauge3.setValueAnimated(gauge_val, 1.5);
        gauge4.setValueAnimated(gauge_val, 2);
        gauge5.setValueAnimated(gauge_val, 1);
        gauge6.setValueAnimated(gauge_val, 1);
        
        window.setTimeout(loop, 1000);
      })();
      
     