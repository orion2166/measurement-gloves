import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';

import 'circle_painter.dart';

class RightHand extends StatefulWidget { //reloads changes when saved
  RightHand({Key key}) : super(key: key);
  @override
  _RightHand createState() => _RightHand();
}
/*Global Variables*/
bool toggleVal = false;
double rightThumbH = 0; double rightThumbW = 0;
double rightTopH = 0; double rightTopW = 0;
double rightBottomH = 0; double rightBottomW = 0;

class _RightHand extends State<RightHand> {

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
        forceVal = 35;
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
                    Container( //right hand
                        child: Image.asset(
                          'assets/transparent-hand-right.png',
                          height: MediaQuery.of(context).size.height*0.6,
                          width: MediaQuery.of(context).size.width*0.9,
                          fit: BoxFit.fitWidth,
                        )
                    ),
                  ],),
                toggleVal ? Positioned( //right thumb circle
                  top: MediaQuery.of(context).size.height*0.29,
                  left: MediaQuery.of(context).size.width*0.755,

                  child: Image.asset(
                    'assets/transparent-circle.png',
                    height: MediaQuery.of(context).size.height*0.17,
                    width: MediaQuery.of(context).size.width*0.17,
                    fit: BoxFit.fitWidth,
                  ),
                ):Container(height:0, width: 0),
                toggleVal ? Positioned( //right top circle
                  top: MediaQuery.of(context).size.height*0.24,
                  left: MediaQuery.of(context).size.width*0.38,

                  child: Image.asset(
                    'assets/transparent-circle.png',
                    height: MediaQuery.of(context).size.height*0.17,
                    width: MediaQuery.of(context).size.width*0.17,
                    fit: BoxFit.fitWidth,
                  ),
                ):Container(height:0, width: 0),
                toggleVal ? Positioned( //right bottom circle
                  top: MediaQuery.of(context).size.height*0.37,
                  left: MediaQuery.of(context).size.width*0.23,

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
                    activeColor: Colors.blueGrey,
                    title: const Text('Active Monitoring Mode'),
                    value: toggleVal,
                    onChanged:(newVal) {
                      setState(() {
                        toggleValSwitch(newVal);
                        /*Get height and width*/
                        rightThumbH = MediaQuery.of(context).size.height*0.375;
                        rightThumbW = MediaQuery.of(context).size.width*0.84;
                        rightTopH = MediaQuery.of(context).size.height*0.325;
                        rightTopW = MediaQuery.of(context).size.width*0.465;
                        rightBottomH = MediaQuery.of(context).size.height*0.455;
                        rightBottomW = MediaQuery.of(context).size.width*0.31;

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