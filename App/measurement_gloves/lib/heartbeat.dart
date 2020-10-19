import 'package:flutter/material.dart';
/*
void main() => runApp(MaterialApp(
    home: Home()
));
*/
class Heartbeat extends StatefulWidget { //reloads changes when saved
  Heartbeat({Key key}) : super(key: key);
  @override
  _Heartbeat createState() => _Heartbeat();
}

class _Heartbeat extends State<Heartbeat> {
  //data
  double hand_height = 0;
  double hand_width= 0;
  //heartbeat mode
  bool toggleVal = false;

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
          )
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
                    title: const Text('Acitive Monitoring Mode'),
                    value: toggleVal,
                    onChanged:(newVal) {
                      setState(() {
                        toggleValSwitch(newVal);
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