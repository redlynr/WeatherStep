var currentVersion = "4.5";
var tempUnit;
var OPEN_WEATHER = 0;  
var WUNDERGROUND = 1;  
var YAHOO = 2;  

var sendError = function() {
    Pebble.sendAppMessage({'KEY_ERROR': true},
        function(e) {
            console.log('Sent empty state to Pebble successfully!');
        },
        function(e) {
            console.log('Error sending empty state to Pebble!');
        }
    );
};

var xhrRequest = function (url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        callback(this.responseText);
    };

    try {
        xhr.open(type, url);
        xhr.send();
    } catch (ex) {
        console.log(ex);
        sendError();
    }
};



var wu_iconToId = {
    'unknown': 0,
    'clear': 1,
    'sunny': 2,
    'partlycloudy': 3,
    'mostlycloudy': 4,
    'mostlysunny': 5,
    'partlysunny': 6,
    'cloudy': 7,
    'rain': 8,
    'snow': 9,
    'tstorms': 10,
    'sleat': 11,
    'flurries': 12,
    'hazy': 13,
    'chancetstorms': 14,
    'chancesnow': 15,
    'chancesleat': 16,
    'chancerain': 17,
    'chanceflurries': 18,
    'nt_unknown': 19,
    'nt_clear': 20,
    'nt_sunny': 21,
    'nt_partlycloudy': 22,
    'nt_mostlycloudy': 23,
    'nt_mostlysunny': 24,
    'nt_partlysunny': 25,
    'nt_cloudy': 26,
    'nt_rain': 27,
    'nt_snow': 28,
    'nt_tstorms': 29,
    'nt_sleat': 30,
    'nt_flurries': 31,
    'nt_hazy': 32,
    'nt_chancetstorms': 33,
    'nt_chancesnow': 34,
    'nt_chancesleat': 35,
    'nt_chancerain': 36,
    'nt_chanceflurries': 37,
    'fog': 38,
    'nt_fog': 39
};

var wu_IconToTiny = {
    'unknown': "system://images/TIMELINE_WEATHER",
    'clear': "system://images/TIMELINE_SUN",
    'sunny': "system://images/TIMELINE_SUN",
    'partlycloudy': "system://images/PARTLY_CLOUDY",
    'mostlycloudy': "system://images/CLOUDY_DAY",
    'mostlysunny': "system://images/PARTLY_CLOUDY",
    'partlysunny': "system://images/PARTLY_CLOUDY",
    'cloudy': "system://images/CLOUDY_DAY",
    'rain': "system://images/HEAVY_RAIN",
    'snow': "system://images/HEAVY_SNOW",
    'tstorms': "system://images/HEAVY_RAIN",
    'sleat': "system://images/LIGHT_SNOW",
    'flurries': "system://images/LIGHT_SNOW",
    'hazy': "system://images/TIMELINE_WEATHER",
    'chancetstorms': "system://images/HEAVY_RAIN",
    'chancesnow': "system://images/LIGHT_SNOW",
    'chancesleat': "system://images/LIGHT_SNOW",
    'chancerain': "system://images/LIGHT_RAIN",
    'chanceflurries': "system://images/LIGHT_SNOW",
    'nt_unknown': "system://images/TIMELINE_WEATHER",
    'nt_clear': "system://images/TIMELINE_SUN",
    'nt_sunny': "system://images/TIMELINE_SUN",
    'nt_partlycloudy': "system://images/PARTLY_CLOUDY",
    'nt_mostlycloudy': "system://images/CLOUDY_DAY",
    'nt_mostlysunny': "system://images/PARTLY_CLOUDY",
    'nt_partlysunny': "system://images/PARTLY_CLOUDY",
    'nt_cloudy': "system://images/CLOUDY_DAY",
    'nt_rain': "system://images/HEAVY_RAIN",
    'nt_snow': "system://images/HEAVY_SNOW",
    'nt_tstorms': "system://images/HEAVY_RAIN",
    'nt_sleat': "system://images/LIGHT_SNOW",
    'nt_flurries': "system://images/LIGHT_SNOW",
    'nt_hazy': "system://images/CLOUDY_DAY",
    'nt_chancetstorms': "system://images/HEAVY_RAIN",
    'nt_chancesnow': "system://images/LIGHT_SNOW",
    'nt_chancesleat': "system://images/LIGHT_SNOW",
    'nt_chancerain': "system://images/LIGHT_RAIN",
    'nt_chanceflurries': "system://images/LIGHT_SNOW",
    'fog': "system://images/CLOUDY_DAY",
    'nt_fog': "system://images/CLOUDY_DAY"
};



var ow_iconToId = {
    '01d': 1,
    '01d': 2,
    '02d': 3,
    '02d': 4,
    '02d': 5,
    '02d': 6,
    '03d': 7,
    '04d': 7,
    '09d': 8,
    '10d': 8,
    '13d': 9,
    '11d': 10,
    '50d': 13,
    '01n': 20,
    '01n': 21,
    '02n': 22,
    '02n': 23,
    '02n': 24,
    '02n': 25,
    '03n': 26,
    '04n': 26,
    '09n': 27,
    '10n': 27,
    '13n': 28,
    '11n': 29,
    '50n': 32,
};

var y_iconToId = {  
    '3200': 0,  
    '32': 1,  
    '34': 1,  
    '30': 5,  
    '28': 4,  
    '26': 7,  
    '44': 7,  
    '10': 8,  
    '11': 8,  
    '12': 8,  
    '40': 8,  
    '13': 9,  
    '14': 9,  
    '15': 9,  
    '16': 9,  
    '41': 9,  
    '42': 9,  
    '43': 9,  
    '46': 9,  
    '1': 10,  
    '3': 10,  
    '4': 10,  
    '37': 10,  
    '38': 10,  
    '39': 10,  
    '45': 10,  
    '47': 10,  
    '5': 11,  
    '6': 11,  
    '7': 11,  
    '8': 40,  
    '9': 40,  
    '18': 11,  
    '17': 41,  
    '35': 41,  
    '21': 13,  
    '19': 13,  
    '31': 20,  
    '33': 20,  
    '27': 23,  
    '29': 24,  
    '20': 38,  
    '25': 42,  
    '36': 43,  
    '23': 44,  
    '24': 44,  
    '0': 45,  
    '2': 46  
};  


// The timeline public URL root
var API_URL_ROOT = 'https://timeline-api.getpebble.com/';

/**
 * Send a request to the Pebble public web timeline API.
 * @param pin The JSON pin to insert. Must contain 'id' field.
 * @param type The type of request, either PUT or DELETE.
 * @param topics Array of topics if a shared pin, 'null' otherwise.
 * @param apiKey Timeline API key for this app, available from dev-portal.getpebble.com
 * @param callback The callback to receive the responseText after the request has completed.
 */
function timelineRequest(pin, type, topics, apiKey, callback) {
  // User or shared?
  var url = API_URL_ROOT + 'v1/' + ((topics !== null) ? 'shared/' : 'user/') + 'pins/' + pin.id;

  // Create XHR
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    console.log('timeline: response received: ' + this.responseText);
    callback(this.responseText);
  };
  xhr.open(type, url);

  // Set headers
  xhr.setRequestHeader('Content-Type', 'application/json');
  if(topics !== null) {
    xhr.setRequestHeader('X-Pin-Topics', '' + topics.join(','));
    xhr.setRequestHeader('X-API-Key', '' + apiKey);
  }

  // Get token
  Pebble.getTimelineToken(function(token) {
    // Add headers
    xhr.setRequestHeader('X-User-Token', '' + token);

    // Send
    xhr.send(JSON.stringify(pin));
    console.log('timeline: request sent.');
  }, function(error) { console.log('timeline: error getting timeline token: ' + error); });
}

/**
 * Insert a pin into the timeline for this user.
 * @param pin The JSON pin to insert.
 * @param callback The callback to receive the responseText after the request has completed.
 */
function insertUserPin(pin, callback) {
  timelineRequest(pin, 'PUT', null, null, callback);
}

/**
 * Delete a pin from the timeline for this user.
 * @param pin The JSON pin to delete.
 * @param callback The callback to receive the responseText after the request has completed.
 */

function deleteUserPin(pin, callback) {
  timelineRequest(pin, 'DELETE', null, null, callback);
}

/**
 * Insert a pin into the timeline for these topics.
 * @param pin The JSON pin to insert.
 * @param topics Array of topics to insert pin to.
 * @param apiKey Timeline API key for this app, available from dev-portal.getpebble.com
 * @param callback The callback to receive the responseText after the request has completed.
 */

function insertSharedPin(pin, topics, apiKey, callback) {
  timelineRequest(pin, 'PUT', topics, apiKey, callback);
}

/**
 * Delete a pin from the timeline for these topics.
 * @param pin The JSON pin to delete.
 * @param topics Array of topics to delete pin from.
 * @param apiKey Timeline API key for this app, available from dev-portal.getpebble.com
 * @param callback The callback to receive the responseText after the request has completed.
 */

function deleteSharedPin(pin, topics, apiKey, callback) {
  timelineRequest(pin, 'DELETE', topics, apiKey, callback);
}


Pebble.addEventListener("ready",
    function(e) {
        console.log("Pebble Ready!");
        if (localStorage['weatherEnabled'] && parse(localStorage['weatherEnabled'].toLowerCase())) {
/* KAH 2/26/2016 
            getWeather(localStorage['weatherKey'], parse(localStorage['useCelsius'].toLowerCase()), localStorage['overrideLocation']);
*/
        } else {
            sendError();
        }
    }
);

Pebble.addEventListener('appmessage',
    function(e) {
        console.log('AppMessage received!');
        if (e.payload.KEY_HASUPDATE) {
//            console.log('Checking for updates...');
//            checkForUpdates();
        } else {
            console.log('Fetching weather info...');
//            getWeather(localStorage['weatherKey'], parse(localStorage['useCelsius'].toLowerCase()), localStorage['overrideLocation']);
            var weatherKey = localStorage.weatherKey;  
            var provider = weatherKey ? 1 : 0;  
            if (localStorage.weatherProvider) {  
                provider = parseInt(localStorage.weatherProvider, 10);  
                switch (provider) {  
                    case WUNDERGROUND:  
                        weatherKey = localStorage.weatherKey;  
                        break;  
                    default:  
                        weatherKey = '';  
                }  
            }  
            getWeather(provider, weatherKey, parse(localStorage.useCelsius.toLowerCase()), localStorage.overrideLocation);  
          
        }
    }                     
);

Pebble.addEventListener('showConfiguration', function(e) {
    Pebble.openURL('http://www.actulife.com/WeatherStep/v4.5');
});

Pebble.addEventListener('webviewclosed', function(e) {
    var configData = JSON.parse(decodeURIComponent(e.response));
    console.log(JSON.stringify(configData));

    var dict = {};

    for (var item in configData) {
        var key = 'KEY_' + item.toUpperCase();
        var value = configData[item];
        if (String(value).indexOf('0x') !== -1) {
            value = parseInt(value, 16);
        }
        if (String(value).indexOf('|') !== -1) {
            var newValue = value.split('|')[1].split(':')[0];
            dict[key + 'CODE'] = value.split('|')[0];
            dict[key + 'MINUTES'] = parseInt(value.split('|')[1].split(':')[1], 10);
            value = parseInt(newValue, 10);
        }
        if (key === 'KEY_FONTTYPE' || key === 'KEY_DATEFORMAT' || key === 'KEY_LOCALE' || key == 'KEY_WEATHERPROVIDER') {
            value = parseInt(value, 10);
        }
        dict[key] = value;
    }

    localStorage['weatherEnabled'] = dict['KEY_ENABLEWEATHER'];
    localStorage['useCelsius'] = dict['KEY_USECELSIUS'];
    localStorage['leadingZero'] = dict['KEY_LEADINGZERO'];
    localStorage['useBigTemp'] = dict['KEY_USEBIGTEMP'];
    localStorage['weatherKey'] = dict['KEY_WEATHERKEY'];
    localStorage['overrideLocation'] = dict['KEY_OVERRIDELOCATION'];
    localStorage.weatherProvider = dict.KEY_WEATHERPROVIDER;  
    localStorage.yahooKey = dict.KEY_YAHOOKEY;  
    delete dict.KEY_WEATHERKEY;  
    delete dict.KEY_WEATHERPROVIDER;  
    delete dict.KEY_OVERRIDELOCATION;  
    delete dict.KEY_YAHOOKEY;  


    Pebble.sendAppMessage(dict, function() {
	console.log('Send config successful: ' + JSON.stringify(dict));
    }, function() {
	console.log('Send failed!');
    }); 
});

function parse(type) {
    return typeof type == 'string' ? JSON.parse(type) : type;
}

function locationSuccess(pos, provider, weatherKey, useCelsius, overrideLocation) {
  console.log(overrideLocation);
  console.log("Retrieving weather info");
  
switch (provider) {  
        case OPEN_WEATHER:  
            fetchOpenWeatherMapData(pos, useCelsius, overrideLocation);  
            break;  
        case WUNDERGROUND:  
            fetchWeatherUndergroundData(pos, weatherKey, useCelsius, overrideLocation);  
            break;  
        case YAHOO:  
            fetchYahooData(pos, useCelsius, overrideLocation);  
            break;  
    }  
}  



function locationError(err) {
  console.log('location error on the JS side! Failure #' + currentFailures);
  //if we fail, try using the cached location
  if(currentFailures <= failureRetryAmount) {
    // reset cache time
    window.localStorage.setItem('weather_loc_cache_time', (new Date().getTime() / 1000));

    getWeather();
    currentFailures++;
  } else {
    // until we get too many failures, at which point give up
    currentFailures = 0;
  }
}

function executeYahooQuery(pos, useCelsius, woeid, overrideLocation) { 
     var url = 'https://query.yahooapis.com/v1/public/yql?format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys&q='; 
     var woeidQuery = ''; 
     if (overrideLocation) { 
         //woeidQuery = 'select woeid from geo.places where text="' + overrideLocation + '"'; 
         // KAH 3/4/2016
         woeidQuery = 'select woeid from geo.places(1) where text="' + overrideLocation + '"'; 
     } else { 
         woeidQuery = '' + woeid; 
     } 
           var date = new Date();
          date.setHours(date.getHours());
 
     if (woeidQuery) { 
         var query = 'select * from weather.forecast where woeid in (' + woeidQuery + ')'; 
         url += encodeURIComponent(query); 
         console.log(url); 
 
 
         xhrRequest(url, 'GET', function(responseText) { 
             try { 
                 var resp = JSON.parse(responseText); 
               // KAH 3/4/2016
                 var now = new Date();  

                 var resultIndex = (now.getDate() === now.getUTCDate() ? 1 : (now.getTimezoneOffset() > 0 ? 0 : 2));  

               
                 var results = resp.query.results.channel.item; 
                 var temp = Math.round(useCelsius ? fahrenheitToCelsius(results.condition.temp) : results.condition.temp); 
//                 var min = Math.round(useCelsius ? fahrenheitToCelsius(results.forecast['1'].low) : results.forecast['1'].low); 
//                 var max = Math.round(useCelsius ? fahrenheitToCelsius(results.forecast['1'].high) : results.forecast['1'].high);
// KAH 3/4/2016
                 var min = Math.round(useCelsius ? fahrenheitToCelsius(results.forecast[resultIndex].low) : results.forecast[resultIndex].low);  
                 var max = Math.round(useCelsius ? fahrenheitToCelsius(results.forecast[resultIndex].high) : results.forecast[resultIndex].high);
               
                 var condition = y_iconToId[results.condition.code]; 
 
                 if (typeof(condition) === 'undefined') { 
                    condition = 0; 
                 } 
 
            var city = resp.query.results.channel.location.city;
            
            var feelslike =  Math.round(useCelsius ? fahrenheitToCelsius(resp.query.results.channel.wind.chill) : resp.query.results.channel.wind.chill);
            
            var unitsSpeed = resp.query.results.channel.units.speed;
            var wind = resp.query.results.channel.wind.speed + unitsSpeed;
            
            var desc = results.condition.text;
            
var headings = ["Powered by"];
var current = 'Hi/Lo: ' + max + '°/' + min + '°\nWind Chill: ' + feelslike + '°\nWind Speed: ' + wind +  '\n';
var paragraphs = ["Yahoo"];
          
                    
               
 // Create the pin
          var pin = {
            "id": "weather-pin-0",
            "time": date.toISOString(),
            "layout": {
              "type": "weatherPin",
              "title": desc,
              "backgroundColor": "#FFAA55",
              //"subtitle" : max + '/' + min,
              "subtitle": temp + '°',
              "locationName": city,
              "tinyIcon": "system://images/TIMELINE_WEATHER",
              //"body": 'Hi/Lo: ' + max + '°/' + min + '°\n\nWind Chill: ' + feelslike + '°\n\nWind Speed: ' + wind +  '\n\n' + desc + '\n\n' + 'Weather Data provided by Yanoo \n\n'
              "body":current,
              "headings":headings,
              "paragraphs":paragraphs
            }
          };
          
          
          
          console.log('Inserting pin ' + JSON.stringify(pin));


          insertUserPin(pin, function(responseText) { 
            console.log('Result: ' + responseText);
          });              
               
               
               
               
 
                 sendData(temp, max, min, condition); 
             } catch (ex) { 
                 console.log(ex); 
                 console.log('Yahoo weather failed, falling back to open weather'); 
                 fetchOpenWeatherMapData(pos, useCelsius, overrideLocation); 
             } 
         }); 
     } else { 
         console.log('No woeid found, falling back to open weather'); 
         fetchOpenWeatherMapData(pos, useCelsius, overrideLocation); 
     } 
 } 




function fetchYahooData(pos, useCelsius, overrideLocation) {  
    if (!overrideLocation) {  
        getWoeidAndExecuteQuery(pos, useCelsius);  
    } else {  
        executeYahooQuery(pos, useCelsius, '', overrideLocation);  
    }  
  
}  
  
function fahrenheitToCelsius(temp) {  
    return (temp - 32)/1.8;  
}  
  
function getWoeidAndExecuteQuery(pos, useCelsius) {  
    var truncLat = pos.coords.latitude.toFixed(4);  
    var truncLng = pos.coords.longitude.toFixed(4);  
    var latLng = truncLat + ',' + truncLng;  
  
    if (localStorage[latLng]) {  
        console.log('Got woeid from storage. ' + latLng + ': ' + localStorage[latLng]);  
        executeYahooQuery(pos, useCelsius, localStorage[latLng]);  
        return;  
    }  
  
    var url = 'http://gws2.maps.yahoo.com/findlocation?pf=1&locale=en_US&offset=15&flags=J&q=' + truncLat + '%2c' + truncLng + '&gflags=R&start=0&count=100';  
  
    console.log(url);  
    xhrRequest(url, 'GET', function(responseText) {  
        try {  
            var resp = JSON.parse(responseText);  
            if (resp.ResultSet.Error === 0) {  
                var woeid = resp.ResultSet.Results[0].woeid;  
                console.log('Got woeid from API. ' + latLng + ': ' + woeid);  
                localStorage[latLng] = woeid;  
                executeYahooQuery(pos, useCelsius, woeid);  
            } else {  
                console.log('woeid query failed: ' + resp.ResultSet.Error);  
                executeYahooQuery(pos, useCelsius, '');  
            }  
        } catch (ex) {  
            console.log(ex.stack);  
            console.log('woeid query failed');  
            executeYahooQuery(pos, useCelsius, '');  
        }  
    });  
}  
  





function fetchWeatherUndergroundData(pos, weatherKey, useCelsius, overrideLocation) {
    var url = 'http://api.wunderground.com/api/' + weatherKey + '/conditions/forecast/hourly/q/';
    if (!overrideLocation) {
        url += pos.coords.latitude + ',' + pos.coords.longitude + '.json';
    } else {
        url += encodeURIComponent(overrideLocation) + '.json';
    }

    console.log(url);

    xhrRequest(url, 'GET', function(responseText) {
        try {
            var resp = JSON.parse(responseText);
            var temp = Math.round((useCelsius ? resp.current_observation.temp_c : resp.current_observation.temp_f));
            var highTemp = resp.forecast.simpleforecast.forecastday[0].high;
            var lowTemp = resp.forecast.simpleforecast.forecastday[0].low;
            var max = Math.round((useCelsius ? highTemp.celsius : highTemp.fahrenheit));
            var min = Math.round((useCelsius ? lowTemp.celsius : lowTemp.fahrenheit));
            var icon = resp.current_observation.icon_url.match(/\/([^.\/]*)\.gif/)[1];
            var condition = wu_iconToId[icon];
            if (typeof(condition) === 'undefined') {
                condition = 0;
            }
 
            var desc = ' ';
            if (useCelsius) {
              desc = resp.forecast.txt_forecast.forecastday[0].fcttext_metric;            
            }
            else {
              desc = resp.forecast.txt_forecast.forecastday[0].fcttext;            
            }
          
            var city = resp.current_observation.display_location.city;
            var dewpoint_c = resp.current_observation.dewpoint_c;
            var dewpoint_f = resp.current_observation.dewpoint_f;
            var dewpoint = Math.round((useCelsius ? dewpoint_c : dewpoint_f));
            var feelslike_c = resp.current_observation.feelslike_c;
            var feelslike_f = resp.current_observation.feelslike_f;
            var feelslike = Math.round((useCelsius ? feelslike_c : feelslike_f));
            var wind = resp.current_observation.wind_string;
            var precip = resp.current_observation.precip_today_string;
            
            //console.log('hourly forecast \n');
          
             var date = new Date();
            date.setHours(date.getHours());         
            

          
          
     
          
var tinyIcon = wu_IconToTiny[icon];
         
var backgroundcolor = "#FFAA55";
          
        

tempUnit = (useCelsius ? 'C' : 'F');        
          
         
var headings = ["Forecast", "Powered by"];
var current = 'Hi/Lo: ' + max + '°/' + min + '°\nFeels like: ' + feelslike + '°' + '\nDewpoint: ' + dewpoint + '°' + '\n\nWind: ' + wind + '\n\nPrecip:' + precip;
var paragraphs = [desc,"Weather Underground"];
          
          
          
// Create the regular update pin
          var pin = {
            "id": "weather-pin-0",
            "time": date.toISOString(),
            "layout": {
              "type": "weatherPin",
              "title": resp.current_observation.weather,
              "backgroundColor": backgroundcolor,
              //"subtitle" : max + '/' + min,
              "subtitle": temp + '°',
              "locationName": city,
              //"tinyIcon": "system://images/TIMELINE_WEATHER",
              "tinyIcon": tinyIcon,
              //"body": 'Temp: ' + temp + '°' + tempUnit + '\n Feels like: ' + feelslike + '°' + tempUnit + '\n Wind: ' + wind + '\n Dewpoint: ' + dewpoint + tempUnit + '\n\n Today\'s Forecast: \n' + desc   
              //"body": 'Hi/Lo: ' + max + '°/' + min + '°\nFeels like: ' + feelslike + '°' + '\nDewpoint: ' + dewpoint + '°' + '\n\nWind: ' + wind + '\n\nPrecip:' + precip + '\n\nForecast: \n' + desc + '\n\n' + 'Weather Data provided:\nby Weather Underground \n\n'
              "body":current,
              "headings": headings,
              "paragraphs": paragraphs
            }
          };
          
          
          
          console.log('Inserting pin #1 ' + JSON.stringify(pin));


          insertUserPin(pin, function(responseText){
            console.log('Result: ' + responseText);
          });
    

// create a pin if precip will occur in future hours
          /*
           var rain_pin = {
                  "id": "rain-pin-0",
                  "time": date.toISOString(),
                  "layout": {
                  "type": "weatherPin",
                  "title": resp.hourly_forecast[0].condition,
                  "backgroundColor": "#AAFFAA",
                  "subtitle": temp + '°',
                  "locationName": city,
                  "tinyIcon": tinyIcon,
                    "body":  "temporary"
                  }    
          };
          console.log('Inserting pin #2 ' + JSON.stringify(rain_pin));
          insertUserPin(rain_pin, function(responseText){
            
          });
 */         
        
/*          
          var rain_pin = "";
          var i = 1;
          var rain_found = false;
          if (resp.hourly_forecast[0].pop <30){
            for (i=1; i<resp.hourly_forecast.length; i++){
              //console.log(resp.hourly_forecast[i].FCTTIME.hour + ' ' + resp.hourly_forecast[i].condition);
              if (resp.hourly_forecast[i].pop >= 30) {
                date = new Date();
                date.setMinutes(0);
                date.setHours(date.getHours() + 1 + i);
                
                temp = Math.round((useCelsius ? resp.hourly_forecast[i].temp.metric : resp.hourly_forecast[i].temp.english));
                icon = resp.hourly_forecast[i].icon_url.match(/\/([^.\/]*)\.gif/)[1];
                tinyIcon = wu_IconToTiny[icon];

                backgroundcolor = "#AAFFFF";
                rain_pin = {
                  "id": "rain-pin-0",
                  "time": date.toISOString(),
                  "layout": {
                  "type": "weatherPin",
                  "title": resp.hourly_forecast[i].condition,
                  "backgroundColor": backgroundcolor,
                  "subtitle": temp + '°',
                  "locationName": city,
                  "tinyIcon": tinyIcon,
                  "body":  resp.hourly_forecast[i].condition + '\nPop: ' + resp.hourly_forecast[i].pop + '% ' + '\n\n' + 'Weather Data provided by:\nWeather Underground \n\n'
                  }                
              };
              console.log('Inserting pin #3' + JSON.stringify(rain_pin));
              insertUserPin(rain_pin, function(responseText) { 
                console.log('Result: ' + responseText);
              });
              rain_found = true;
              break;
            }              
          } 
            if (rain_found !== true) {
                console.log('deleting future pin #1 ' + JSON.stringify(rain_pin));
                deleteUserPin(rain_pin, function(responseText) { 
                console.log('Result: ' + responseText);
              }); 
            }
        } 
      else
        {
          console.log('deleting future pin #2 ' + JSON.stringify(rain_pin));
         deleteUserPin(rain_pin, function(responseText) { 
                console.log('Result: ' + responseText);
              }); 
        }
*/          
            sendData(temp, max, min, condition);

        } catch(ex) {
            //console.log(ex);
            //console.log('Falling back to OpenWeatherMap');
            //fetchOpenWeatherMapData(pos, useCelsius);
          console.log(ex.stack);  
          console.log('Falling back to Yahoo');  
          fetchYahooData(pos, useCelsius, overrideLocation);  
  

          
        }
    });
}

function fetchOpenWeatherMapData(pos, useCelsius, overrideLocation) {
    var url = 'http://api.openweathermap.org/data/2.5/weather?appid=979cbf006bf67bc368a54af240d15cf3';
    var urlForecast = 'http://api.openweathermap.org/data/2.5/forecast/daily?appid=979cbf006bf67bc368a54af240d15cf3&format=json&cnt=3';

    if (!overrideLocation) {
        url += '&lat=' + pos.coords.latitude + '&lon=' + pos.coords.longitude;
        urlForecast += '&lat=' + pos.coords.latitude + '&lon=' + pos.coords.longitude;
    } else {
        url += '&q=' + encodeURIComponent(overrideLocation);
        urlForecast += '&q=' + encodeURIComponent(overrideLocation);
    }

    console.log(url);
    console.log(urlForecast);

    xhrRequest(url, 'GET', function(responseText) {
        try {
            console.log('Retrieving current weather from OpenWeatherMap');
            var resp = JSON.parse(responseText);
            var temp = useCelsius ? kelvinToCelsius(resp.main.temp) : kelvinToFahrenheit(resp.main.temp);
            var condition = ow_iconToId[resp.weather[0].icon];
            var day = new Date(resp.dt * 1000);
            if (typeof(condition) === 'undefined') {
                condition = 0;
            }

            xhrRequest(urlForecast, 'GET', function(forecastRespText) {
                try {
                    console.log('Retrieving forecast data from OpenWeatherMap');
                    var fResp = JSON.parse(forecastRespText);

                    var max = useCelsius ? kelvinToCelsius(fResp.list[0].temp.max) : kelvinToFahrenheit(fResp.list[0].temp.max);
                    var min = useCelsius ? kelvinToCelsius(fResp.list[0].temp.min) : kelvinToFahrenheit(fResp.list[0].temp.min);
                    var location = resp.name;
                    var wind = resp.wind.speed;
                    var date = new Date();
                    date.setHours(date.getHours());  
                  
                    for (var fIndex in fResp.list) {
                        var fDay = new Date(fResp.list[fIndex].dt * 1000);
                        if (day.getUTCDate() === fDay.getUTCDate()) {
                            console.log(JSON.stringify(fResp.list[fIndex]));
                            max = useCelsius ? kelvinToCelsius(fResp.list[fIndex].temp.max) : kelvinToFahrenheit(fResp.list[fIndex].temp.max);
                            min = useCelsius ? kelvinToCelsius(fResp.list[fIndex].temp.min) : kelvinToFahrenheit(fResp.list[fIndex].temp.min);
                        }
                    }
var headings = ["Powered by"];
var current = 'Hi/Lo: ' + max + '°/' + min + '°\nWindSpeed: ' + wind + '\n';
var paragraphs = ["Open Weather"];
                  // Create the pin
          var pin = {
            "id": "weather-pin-0",
            "time": date.toISOString(),
            "layout": {
              "type": "weatherPin",
              "title": resp.weather[0].main,
              "locationName": location,
              "backgroundColor": "#FFAA55",
              //"subtitle" : max + '/' + min,
              "subtitle": temp + '°',
              "tinyIcon": "system://images/TIMELINE_WEATHER",
              //"body": 'Temp: ' + temp + '°' + tempUnit + '\n Feels like: ' + feelslike + '°' + tempUnit + '\n Wind: ' + wind + '\n Dewpoint: ' + dewpoint + tempUnit + '\n\n Today\'s Forecast: \n' + desc   
              //"body": 'Hi/Lo: ' + max + '°/' + min + '°\nWindSpeed: ' + wind + '\n' + 'Weather Data provided by Open Weather \n\n'
              "body": current + "\n\n",
              "headings": headings,
              "paragraphs": paragraphs
            }
          };
          
          
          
          console.log('Inserting pin ' + JSON.stringify(pin));


          insertUserPin(pin, function(responseText) { 
            console.log('Result: ' + responseText);
          });

                    sendData(temp, max, min, condition);
                } catch (ex) {
                    console.log('Failure requesting forecast data from OpenWeatherMap');
                    console.log(ex.stack);
                }
            });

        } catch (ex) {
            console.log('Failure requesting current weather from OpenWeatherMap');
            console.log(ex.stack);
        }
    });
}



function kelvinToCelsius(temp) {
    return Math.round(temp - 273.15);
}

function kelvinToFahrenheit(temp) {
    return Math.round(temp * 1.8 - 459.67);
}

function sendData(temp, max, min, condition) {
    var data = {
        'KEY_TEMP': temp,
        'KEY_MAX': max,
        'KEY_MIN': min,
        'KEY_WEATHER': condition
    };

    console.log(JSON.stringify(data));

    Pebble.sendAppMessage(data,
        function(e) {
            console.log('Weather info sent to Pebble successfully!');
        },
        function(e) {
            console.log('Error sending weather info to Pebble!');
        }
    );
}

function locationError(err) {
    console.log('Error requesting location!');
}

function getWeather(provider, weatherKey, useCelsius, overrideLocation) {
    console.log('Requesting weather: ' + provider + ', ' + weatherKey + ', ' + useCelsius + ', ' + overrideLocation);  
    provider = provider || 0;  

    weatherKey = weatherKey || '';
    useCelsius = useCelsius || false;
    overrideLocation = overrideLocation || '';
    if (overrideLocation) {
        locationSuccess(null, provider, weatherKey, useCelsius, overrideLocation);
    } else {
        navigator.geolocation.getCurrentPosition(
            function(pos) {
                locationSuccess(pos, provider, weatherKey, useCelsius, overrideLocation);
            },
            locationError,
            {timeout: 15000, maximumAge: 60000}
        );
    }
}


