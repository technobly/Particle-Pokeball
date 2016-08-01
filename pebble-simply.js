// Particle Pokeball - Pebble Simply.js App - Technobly 2016
// Using accelerometer functions from demo here https://gist.github.com/Meiguro/9030503

var DEVICE_ID = 'YOUR_DEVICE_ID';
var ACCESS_TOKEN = 'YOUR_ACCESS_TOKEN';
var center = 90;
var right = 150;
var left = 30;
var deadband = 100; // won't send data until accel > deadband

var map = function(value, fromStart, fromEnd, toStart, toEnd)
{
  return Math.floor((value - fromStart) * (toEnd - toStart) / (fromEnd - fromStart) + toStart);
}

simply.setText({
  title: 'Particle Pokeball',
  body: 'Press buttons to control the bot!',
}, true);

/**
 * Use simply.accelConfig({ rate: 100, samples: 25 }) to configure
 * the hertz and accel data per batch for the accelData events.
 * See the API reference at simplyjs.io for more information.
 */
simply.accelConfig({ rate: 2, samples: 10 });

var onAccelData = function(e) {
  var dir = e.accel.x;
  var spd = e.accel.y;


  if (dir > deadband) {
    if (dir > 800) {
      dir = left;
    }
    else {
      dir = map(dir, deadband, 800, center, left);
    }
  }
  else if (dir < -deadband) {
    if (dir < -800) {
      dir = right;
    }
    else {
      dir = map(dir, -deadband, -800, center, right);
    }
  }
  else {
    dir = center;
  }

  if (spd > deadband) {
    if (spd > 800) {
      spd = deadband;
    }
    else {
      spd = map(spd, deadband, 800, 0, deadband);
    }
  }
  else if (spd < -deadband) {
    if (spd < -800) {
      spd = -deadband;
    }
    else {
      spd = map(spd, -deadband, -800, 0, -deadband);
    }
  }
  else {
    spd = 0;
  }

  var command = ',' + dir + ',' + spd + ',';
  simply.body('dir: ' + dir + '\nspd:' + spd);

  ajax(
    {
      url: 'https://api.particle.io/v1/devices/' + DEVICE_ID + '/cmd?access_token=' + ACCESS_TOKEN,
      method: 'post',
      type: 'json',
      data: { a: command }
    },
    function(e){ console.log('ajax success: ' + e); },
    function(e){ console.log('ajax failure: ' + e); }
  );
};

// Press up to begin accelData streaming
// Pressing up multiple times will register the handler more than once so be careful.
simply.on('singleClick', 'up', function(e) {
  simply.on('accelData', onAccelData);
});

// Press down until all accelData handlers are removed and you can accelPeek again
simply.on('singleClick', 'down', function(e) {
  simply.off('accelData', onAccelData);
  onAccelData({"accel":{"x":0,"y":0}});
});

// Press select to accelPeek
simply.on('singleClick', 'select', function(e) {
  if (simply.accelConfig().subscribe) {
    // accelData and accelPeek can't happen simultaneously
    return;
  }
  simply.accelPeek(function(e) {
    onAccelData(e);
  });
});
