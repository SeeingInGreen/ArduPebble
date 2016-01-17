//create UI
var UI = require('ui');
var IP = "http://172.20.10.9";
var lockState = "Unknown";
var lockCommand = "/?LN";
var unlockCommand = "/?LF";
var lightState = "Unknown";
var lightOnCommand = "/?FN";
var lightOffCommand = "/?FF";
var states = "On/Off";
var menuItems;
var menu;

//create menu items
function setMenuItems() {
 menuItems = [ 
  {
    title: "Lock",
    subtitle: lockState
  },
  {
    title: "Light",
    subtitle: lightState
  }];
}

function setMenu() {
menu = new UI.Menu({
  sections: [{
    title: 'Connected Devices',
    items: menuItems
  }]
});
}
setMenuItems();
setMenu();
menu.show();




//when a menu item is clicked, we want its option to be presented.
menu.on('select', function(event) { 
  var choices = states.split('/');
  var choiceItems = [
    {
      title: choices[0],
      subtitle: ""
    },
    {
      title: choices[1],
      subtitle: ""
    }
  ];
  var choiceMenu = new UI.Menu ({
    sections: [{
      title: menuItems[event.itemIndex].title,
      items: choiceItems
    }]
  });
  choiceMenu.show();
  
  //when a choice is selected the pebble should send an http request
  choiceMenu.on('select', function(event2) {
    if (event2.itemIndex === 0) { //0 indicates the on position
         switch (menuItems[event.itemIndex].title) {
           case "Lock" :
             if (Boolean(HTTPGET(IP + lockCommand))) {
               lockState = "On";
               menu.item(0,0, {title: "Lock", subtitle: lockState});
               menu.show();
             }
             break;
           case "Light":
             if (Boolean(HTTPGET(IP + lightOnCommand))) {
               lightState = "On";
               menu.item(0,1, {title: "Light", subtitle: lightState});
               menu.show();
             }
             break;
           default:
         }
    }
    else {  //itemIndex must then equal 1, indicating the off position
         switch (menuItems[event.itemIndex].title) {
           case "Lock" :
             if (Boolean(HTTPGET(IP + unlockCommand))) {
               lockState = "Off";
               menu.item(0,0, {title: "Lock", subtitle: lockState});
               menu.show();
             }
             break;
           case "Light":
             if (Boolean(HTTPGET(IP + lightOffCommand))) {
               lightState = "Off";
               menu.item(0,1, {title: "Light", subtitle: lightState});
               menu.show();
             }
             break;
           default:
       }
    } 
  }
);
});

function HTTPGET(url){
  var req = new XMLHttpRequest();
  req.open("GET", url, false);
  req.send();
  return req.status();
}