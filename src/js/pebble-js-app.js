var currentVersion = "4.0";
var tempUnit;
var OPEN_WEATHER = 0;  
var WUNDERGROUND = 1;  
var YAHOO = 2;  


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
            console.log('Checking for updates...');
            checkForUpdates();
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
    Pebble.openURL('http://www.actulife.com/WeatherStep/v4.0');
});

Pebble.addEventListener('webviewclosed', function(e) {
    var configData = JSON.parse(decodeURIComponent(e.response));
    console.log(JSON.stringify(configData));

    var dict = {};

    for (item in configData) {
        var key = 'KEY_' + item.toUpperCase();
        var value = configData[item];
        if (String(value).indexOf('0x') !== -1) {
            value = parseInt(value, 16);
        }
        if (String(value).indexOf('|') !== -1) {
            newValue = value.split('|')[1].split(':')[0];
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
};

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
         woeidQuery = 'select woeid from geo.places where text="' + overrideLocation + '"'; 
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
                 var results = resp.query.results.channel.item; 
                 var temp = Math.round(useCelsius ? fahrenheitToCelsius(results.condition.temp) : results.condition.temp); 
                 var min = Math.round(useCelsius ? fahrenheitToCelsius(results.forecast['1'].low) : results.forecast['1'].low); 
                 var max = Math.round(useCelsius ? fahrenheitToCelsius(results.forecast['1'].high) : results.forecast['1'].high); 
                 var condition = y_iconToId[results.condition.code]; 
 
                 if (typeof(condition) === 'undefined') { 
                    condition = 0; 
                 } 
 
            var city = resp.query.results.channel.location.city;
            
            var feelslike =  Math.round(useCelsius ? fahrenheitToCelsius(resp.query.results.channel.wind.chill) : resp.query.results.channel.wind.chill);
            
            var unitsSpeed = resp.query.results.channel.units.speed;
            var wind = resp.query.results.channel.wind.speed + unitsSpeed;
            
            var desc = results.condition.text;
            
               
               
 // Create the pin
          var pin = {
            "id": "weather-pin-0",
            "time": date.toISOString(),
            "layout": {
              "type": "weatherPin",
              "title": "Update",
              "backgroundColor": "#FFAA55",
              //"subtitle" : max + '/' + min,
              "subtitle": temp + '°',
              "locationName": city,
              "tinyIcon": "system://images/TIMELINE_WEATHER",
              "body": 'Hi/Lo: ' + max + '°/' + min + '°\n\nWind Chill: ' + feelslike + '°\n\nWind Speed: ' + wind +  '\n\n' + desc + '\n\n' + 'Weather Data provided by Yanoo \n\n'
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
    var url = 'http://api.wunderground.com/api/' + weatherKey + '/conditions/forecast/q/'
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
 
            var desc = resp.forecast.txt_forecast.forecastday[0].fcttext;            
            var city = resp.current_observation.display_location.city;
            var dewpoint_c = resp.current_observation.dewpoint_c;
            var dewpoint_f = resp.current_observation.dewpoint_f;
            var dewpoint = Math.round((useCelsius ? dewpoint_c : dewpoint_f));
            var feelslike_c = resp.current_observation.feelslike_c;
            var feelslike_f = resp.current_observation.feelslike_f;
            var feelslike = Math.round((useCelsius ? feelslike_c : feelslike_f));
            var wind = resp.current_observation.wind_string;
            var precip = resp.current_observation.precip_today_string;
            
            console.log(desc);
            console.log(city);     
          
          var date = new Date();
          date.setHours(date.getHours());
         
// Create the pin
//var pin = {
//  "id": "pin-generic-1",
//  "time": date.toISOString(),
//  "layout": {
//    "type": "genericPin",
//    "title": "This is a genericPin!",
//    "tinyIcon": "system://images/NOTIFICATION_FLAG",

//  }
//}
          
tempUnit = (useCelsius ? 'C' : 'F');        
          
// Create the pin
          var pin = {
            "id": "weather-pin-0",
            "time": date.toISOString(),
            "layout": {
              "type": "weatherPin",
              "title": "Update",
              "backgroundColor": "#FFAA55",
              //"subtitle" : max + '/' + min,
              "subtitle": temp + '°',
              "locationName": city,
              "tinyIcon": "system://images/TIMELINE_WEATHER",
              //"body": 'Temp: ' + temp + '°' + tempUnit + '\n Feels like: ' + feelslike + '°' + tempUnit + '\n Wind: ' + wind + '\n Dewpoint: ' + dewpoint + tempUnit + '\n\n Today\'s Forecast: \n' + desc   
              "body": 'Hi/Lo: ' + max + '°/' + min + '°\nFeels like: ' + feelslike + '°' + '\nDewpoint: ' + dewpoint + '°' + '\n\nWind: ' + wind + '\n\nPrecip:' + precip + '\n\nForecast: \n' + desc + '\n\n' + 'Weather Data provided by Weather Underground \n\n'
            }
          };
          
          
          
          console.log('Inserting pin ' + JSON.stringify(pin));


          insertUserPin(pin, function(responseText) { 
            console.log('Result: ' + responseText);
          });
    
          
          
          
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
    
    if (!overrideLocation) {
        url += '&lat=' + pos.coords.latitude + '&lon=' + pos.coords.longitude;
    } else {
        url += '&q=' + encodeURIComponent(overrideLocation);
    }

  
    console.log(url);

    xhrRequest(url, 'GET', function(responseText) {
        try {
            var resp = JSON.parse(responseText);
            var temp = useCelsius ? kelvinToCelsius(resp.main.temp) : kelvinToFahrenheit(resp.main.temp);
            var max = useCelsius ? kelvinToCelsius(resp.main.temp_max) : kelvinToFahrenheit(resp.main.temp_max);
            var min = useCelsius ? kelvinToCelsius(resp.main.temp_min) : kelvinToFahrenheit(resp.main.temp_min);
            var condition = ow_iconToId[resp.weather[0].icon];
            var location = resp.name;
            var wind = resp.wind.speed;

            if (typeof(condition) === 'undefined') {
                condition = 0;
            }
          
          var date = new Date();
          date.setHours(date.getHours());      
          
          
     // Create the pin
          var pin = {
            "id": "weather-pin-0",
            "time": date.toISOString(),
            "layout": {
              "type": "weatherPin",
              "title": "Update",
              "locationName": location,
              "backgroundColor": "#FFAA55",
              //"subtitle" : max + '/' + min,
              "subtitle": temp + '°',
              "tinyIcon": "system://images/TIMELINE_WEATHER",
              //"body": 'Temp: ' + temp + '°' + tempUnit + '\n Feels like: ' + feelslike + '°' + tempUnit + '\n Wind: ' + wind + '\n Dewpoint: ' + dewpoint + tempUnit + '\n\n Today\'s Forecast: \n' + desc   
              "body": 'Hi/Lo: ' + max + '°/' + min + '°\nWindSpeed: ' + wind + '\n' + 'Weather Data provided by Open Weather \n\n'
            }
          };
          
          
          
          console.log('Inserting pin ' + JSON.stringify(pin));


          insertUserPin(pin, function(responseText) { 
            console.log('Result: ' + responseText);
          });
         
            
            sendData(temp, max, min, condition);

        } catch (ex) {
            console.log(ex.stack);
        }
    });
}



function sendUpdateData(updateAvailable) {
    console.log(updateAvailable ? 'Update available!' : 'No updates.');
    Pebble.sendAppMessage({'KEY_HASUPDATE': updateAvailable},
        function(e) {
            console.log('Sent update data to Pebble successfully!');
        },
        function(e) {
            console.log('Error sending update data to Pebble!');
        }
    );
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
    }

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

var sendError = function() {
    Pebble.sendAppMessage({'KEY_ERROR': true},
        function(e) {
            console.log('Sent empty state to Pebble successfully!');
        },
        function(e) {
            console.log('Error sending empty state to Pebble!');
        }
    );
}

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

