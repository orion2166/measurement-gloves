import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';

import 'circle_painter.dart';

class Heartbeat extends StatefulWidget { //reloads changes when saved
  Heartbeat({Key key}) : super(key: key);
  @override
  _Heartbeat createState() => _Heartbeat();
}
/*Global Variables*/
bool toggleVal = false;
double leftThumbH = 0; double leftThumbW = 0;
double leftTopH = 0; double leftTopW = 0;
double leftBottomH = 0; double leftBottomW = 0;
double rightThumbH = 0; double rightThumbW = 0;
double rightTopH = 0; double rightTopW = 0;
double rightBottomH = 0; double rightBottomW = 0;
var pos_list = List<Coordinate>(6); //list of position of circles

class Coordinate {
  final double h; //height
  final double w; //width
  const Coordinate(this.h, this.w);
}
class _Heartbeat extends State<Heartbeat> {

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
        forceVal = 10;
      } else if (forceVal == 10) {
        forceVal = 20;
      } else if (forceVal == 20) {
        forceVal = 30;
      } else {
        forceVal = 0;
      }
    });
  }

  @override
  Widget build(BuildContext context){
    return Scaffold(
      appBar: AppBar(
        title: Text(
            'Gloves',
            style: TextStyle(
              color: Colors.black,
            )
        ),
        centerTitle: true,
        backgroundColor: Colors.blueAccent, //ctrl+Q to view color intensity
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
              color: Colors.blueAccent,
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
                          width: MediaQuery.of(context).size.width*0.5,
                          fit: BoxFit.fitWidth,
                        )
                    ),
                    Container( //right hand
                        child: Image.asset(
                          'assets/transparent-hand-right.png',
                          height: MediaQuery.of(context).size.height*0.6,
                          width: MediaQuery.of(context).size.width*0.5,
                          fit: BoxFit.fitWidth,
                        )
                    ),
                  ],),
                toggleVal ? Positioned( //left thumb circle
                  top: MediaQuery.of(context).size.height*0.6*0.5,
                  left: MediaQuery.of(context).size.width*0.5*0.046,

                  child: Image.asset(
                    'assets/transparent-circle.png',
                    height: MediaQuery.of(context).size.height*0.6*0.1417,
                    width: MediaQuery.of(context).size.width*0.5*0.17,
                    fit: BoxFit.fitWidth,
                  ),
                ): Container(height:0, width: 0),
                toggleVal ? Positioned( //left top circle
                  top: MediaQuery.of(context).size.height*0.6*0.45,
                  left: MediaQuery.of(context).size.width*0.5*0.44,

                  child: Image.asset(
                    'assets/transparent-circle.png',
                    height: MediaQuery.of(context).size.height*0.6*0.1417,
                    width: MediaQuery.of(context).size.width*0.5*0.17,
                    fit: BoxFit.fitWidth,
                  ),
                ):Container(height:0, width: 0),
                toggleVal ? Positioned(//left bottom circle
                  top: MediaQuery.of(context).size.height*0.6*0.58333,
                  left: MediaQuery.of(context).size.width*0.5*0.64,

                  child: Image.asset(
                    'assets/transparent-circle.png',
                    height: MediaQuery.of(context).size.height*0.6*0.1417,
                    width: MediaQuery.of(context).size.width*0.5*0.17,
                    fit: BoxFit.fitWidth,
                  ),
                ):Container(height:0, width: 0),
                toggleVal ? Positioned( //right thumb circle
                  top: MediaQuery.of(context).size.height*0.6*0.5,
                  left: MediaQuery.of(context).size.width*0.5*1.78,

                  child: Image.asset(
                    'assets/transparent-circle.png',
                    height: MediaQuery.of(context).size.height*0.6*0.1417,
                    width: MediaQuery.of(context).size.width*0.5*0.17,
                    fit: BoxFit.fitWidth,
                  ),
                ):Container(height:0, width: 0),
                toggleVal ? Positioned( //right top circle
                  top: MediaQuery.of(context).size.height*0.6*0.45,
                  left: MediaQuery.of(context).size.width*0.5*1.38,

                  child: Image.asset(
                    'assets/transparent-circle.png',
                    height: MediaQuery.of(context).size.height*0.6*0.1417,
                    width: MediaQuery.of(context).size.width*0.5*0.17,
                    fit: BoxFit.fitWidth,
                  ),
                ):Container(height:0, width: 0),
                toggleVal ? Positioned( //right bottom circle
                  top: MediaQuery.of(context).size.height*0.6*0.58333,
                  left: MediaQuery.of(context).size.width*0.5*1.2,

                  child: Image.asset(
                    'assets/transparent-circle.png',
                    height: MediaQuery.of(context).size.height*0.6*0.1417,
                    width: MediaQuery.of(context).size.width*0.5*0.17,
                    fit: BoxFit.fitWidth,
                  ),
                ):Container(height:0, width: 0),
                Container(
                  width: 400,
                  height: 400,
                  child: Circle(
                    center: {"x": leftThumbW, "y": leftThumbH},
                    radius: forceVal,
                    mode: toggleVal,
                  ),
                ),
                Container(
                  width: 400,
                  height: 400,
                  child: Circle(
                    center: {"x": leftTopW, "y": leftTopH},
                    radius: forceVal,
                    mode: toggleVal,
                  ),
                ),
                Container(
                  width: 400,
                  height: 400,
                  child: Circle(
                    center: {"x": leftBottomW, "y": leftBottomH},
                    radius: forceVal,
                    mode: toggleVal,
                  ),
                ),
                Container(
                  width: 400,
                  height: 400,
                  child: Circle(
                    center: {"x": rightThumbW, "y": rightThumbH},
                    radius: forceVal,
                    mode: toggleVal,
                  ),
                ),
                Container(
                  width: 400,
                  height: 400,
                  child: Circle(
                    center: {"x": rightTopW, "y": rightTopH},
                    radius: forceVal,
                    mode: toggleVal,
                  ),
                ),
                Container(
                  width: 400,
                  height: 400,
                  child: Circle(
                    center: {"x": rightBottomW, "y": rightBottomH},
                    radius: forceVal,
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
                    title: const Text('Active Monitoring Mode'),
                    value: toggleVal,
                    onChanged:(newVal) {
                      setState(() {
                        toggleValSwitch(newVal);
                        /*Get height and width*/
                        leftThumbH = MediaQuery.of(context).size.height*0.34;
                        leftThumbW = MediaQuery.of(context).size.width*0.063;
                        leftTopH = MediaQuery.of(context).size.height*0.31;
                        leftTopW = MediaQuery.of(context).size.width*0.26;
                        leftBottomH = MediaQuery.of(context).size.height*0.395;
                        leftBottomW = MediaQuery.of(context).size.width*0.365;
                        rightThumbH = MediaQuery.of(context).size.height*0.34;
                        rightThumbW = MediaQuery.of(context).size.width*(1-0.063);
                        rightTopH = MediaQuery.of(context).size.height*0.31;
                        rightTopW = MediaQuery.of(context).size.width*(1-0.265);
                        rightBottomH = MediaQuery.of(context).size.height*0.395;
                        rightBottomW = MediaQuery.of(context).size.width*(1-0.36);

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