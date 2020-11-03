import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';

import 'circle_painter.dart';

class LeftHand extends StatefulWidget { //reloads changes when saved
  LeftHand({Key key}) : super(key: key);
  @override
  _LeftHand createState() => _LeftHand();
}
/*Global Variables*/
bool toggleVal = false;
double leftThumbH = 0; double leftThumbW = 0;
double leftTopH = 0; double leftTopW = 0;
double leftBottomH = 0; double leftBottomW = 0;
double radVal = 0; int colVal = 100;
class _LeftHand extends State<LeftHand> {

  toggleValSwitch(bool newVal){
    setState(() {
      toggleVal = newVal;
    });
  }
  //status
  var status_type = 'not recording';
  /*function here for receiving status of glove*/

  //battery
  int battery_per = 0; //initially zero
  /*update battery percentage*/
  // state variable
  double forceVal = 0;

  // change circle size when button pressed
  _changeSize() {
    setState(() {
      if (forceVal == 0) {
        forceVal = 20;
        convertRadIntensity(forceVal);
      } else if (forceVal == 20) {
        forceVal = 50;
        convertRadIntensity(forceVal);
      } else if (forceVal == 50) {
        forceVal = 100;
        convertRadIntensity(forceVal);
      } else {
        forceVal = 0;
        convertRadIntensity(forceVal);
      }
    });
  }

  @override
  Widget build(BuildContext context){
    return Scaffold(
      appBar: AppBar(
        title: Text(
            'Left Hand',
            style: TextStyle(
              color: Colors.black,
            )
        ),
        centerTitle: true,
        backgroundColor: Colors.blueGrey, //ctrl+Q to view color intensity
        leading: GestureDetector( //back button
          onTap: () {
            Navigator.pop(context);
          },
          child: Icon(
            Icons.arrow_back,
          ),
        ),
        actions: <Widget>[
          FlatButton(
            textColor: Colors.white,
            onPressed: () {},
            child: Text('Set RTC'),
          ),
          Container(
            child: RaisedButton(
              child: Text('Test'),
              color: Colors.blueGrey,
              onPressed: _changeSize,
            ),
          ),
        ],
      ),
      body: Column(
        children: <Widget>[
          Stack(
              children:<Widget> [
                Row(
                  mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                  crossAxisAlignment: CrossAxisAlignment.center,
                  children:<Widget>[
                    Container( //left hand
                        child: Image.asset(
                          'assets/transparent-hand-left.png',
                          height: MediaQuery.of(context).size.height*0.6,
                          width: MediaQuery.of(context).size.width*0.9,
                          fit: BoxFit.fitWidth,
                        )
                    ),
                  ],),
                toggleVal ? Positioned( //left thumb circle
                  top: MediaQuery.of(context).size.height*0.29,
                  left: MediaQuery.of(context).size.width*0.08,
                  //height: 50.0,
                  //width: 50.0,
                  child: Image.asset(
                    'assets/transparent-circle.png',
                    height: MediaQuery.of(context).size.height*0.17,
                    width: MediaQuery.of(context).size.width*0.17,
                    fit: BoxFit.fitWidth,
                  ),
                ): Container(height:0, width: 0),
                toggleVal ? Positioned( //left top circle
                  top: MediaQuery.of(context).size.height*0.24,
                  left: MediaQuery.of(context).size.width*0.44,
                  //height: 50.0,
                  //width: 50.0,
                  child: Image.asset(
                    'assets/transparent-circle.png',
                    height: MediaQuery.of(context).size.height*0.17,
                    width: MediaQuery.of(context).size.width*0.17,
                    fit: BoxFit.fitWidth,
                  ),
                ):Container(height:0, width: 0),
                toggleVal ? Positioned(//left bottom circle
                  top: MediaQuery.of(context).size.height*0.37,
                  left: MediaQuery.of(context).size.width*0.60,
                  //height: 50.0,
                  //width: 50.0,
                  child: Image.asset(
                    'assets/transparent-circle.png',
                    height: MediaQuery.of(context).size.height*0.17,
                    width: MediaQuery.of(context).size.width*0.17,
                    fit: BoxFit.fitWidth,
                  ),
                ):Container(height:0, width: 0),
                Container(
                  width: 400,
                  height: 400,
                  child: Circle(
                    center: {"x": leftThumbW, "y": leftThumbH},
                    radius: radVal,
                    intensity: colVal,
                    mode: toggleVal,
                  ),
                ),
                Container(
                  width: 400,
                  height: 400,
                  child: Circle(
                    center: {"x": leftTopW, "y": leftTopH},
                    radius: radVal,
                    intensity: colVal,
                    mode: toggleVal,
                  ),
                ),
                Container(
                  width: 400,
                  height: 400,
                  child: Circle(
                    center: {"x": leftBottomW, "y": leftBottomH},
                    radius: radVal,
                    intensity: colVal,
                    mode: toggleVal,
                  ),
                ),
              ]
          ),
          Divider( thickness: 2),
          Expanded(
            child: ListView(
              children: ListTile.divideTiles(
                context: context,
                tiles:[
                  SwitchListTile(
                    secondary: Icon(Icons.bluetooth),
                    activeColor: Colors.blueGrey,
                    title: const Text('Active Monitoring Mode'),
                    value: toggleVal,
                    onChanged:(newVal) {
                      setState(() {
                        toggleValSwitch(newVal);
                        /*Get height and width*/
                        leftThumbH = MediaQuery.of(context).size.height*0.375;
                        leftThumbW = MediaQuery.of(context).size.width*0.16;
                        leftTopH = MediaQuery.of(context).size.height*0.325;
                        leftTopW = MediaQuery.of(context).size.width*0.52;
                        leftBottomH = MediaQuery.of(context).size.height*0.455;
                        leftBottomW = MediaQuery.of(context).size.width*0.68;

                      });
                    },
                  ),
                  ListTile(
                    leading: Icon(Icons.info_outline),
                    title: Text('Status'),
                    trailing: Icon(Icons.keyboard_arrow_right),
                  ),
                  ListTile(
                    leading: Icon(Icons.battery_charging_full),
                    title: Text('Battery'),
                    trailing: Icon(Icons.keyboard_arrow_right),
                  ),
                ],
              ).toList(),
            ),
          ),
        ],),
    );
  }
}
void convertRadIntensity(double forceVal) {
  /*Convert Force Value to Radius and Intensity*/
  //Assume Force Value Range is 0 - 100
  //Radius Range is 3,4,5,..., 35
  //Intensity Range is 100, 200, ..., 900
  radVal = (forceVal - 0) * 32/100 + 3;
  double intens = (forceVal - 0) * 800/100 + 100;
  colVal = ((intens.toInt() + 99) ~/ 100 ) * 100; //rounds to nearest hundredth

}


