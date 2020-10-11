import 'package:flutter/material.dart';

void main() => runApp(MaterialApp(
    home: Home()
));

class Home extends StatefulWidget { //reloads changes when saved
  @override
  _HomeState createState() => _HomeState();
}

class _HomeState extends State<Home> {
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
        backgroundColor: Colors.grey[100], //ctrl+Q to view color intensity
        leading: GestureDetector( //back button
          onTap: () {},
          child: Icon(
            Icons.arrow_back,
          ),
        ),
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
                        height: MediaQuery.of(context).size.height*0.7,
                        width: MediaQuery.of(context).size.width*0.5,
                        fit: BoxFit.fitWidth,
                      )
                  ),
                  Container( //right hand
                      child: Image.asset(
                        'assets/transparent-hand-right.png',
                        height: MediaQuery.of(context).size.height*0.7,
                        width: MediaQuery.of(context).size.width*0.5,
                        fit: BoxFit.fitWidth,
                      )
                  ),
                ],),
              toggleVal ? Positioned( //left thumb circle
                top: MediaQuery.of(context).size.height*0.35,
                left: MediaQuery.of(context).size.width*0.03,
                //height: 50.0,
                //width: 50.0,
                child: Image.asset(
                  'assets/transparent-circle.png',
                  height: MediaQuery.of(context).size.height*0.085,
                  width: MediaQuery.of(context).size.width*0.085,
                  fit: BoxFit.fitWidth,
                ),
              ): Container(height:0, width: 0),
              toggleVal ? Positioned( //left top circle
                top: MediaQuery.of(context).size.height*0.33,
                left: MediaQuery.of(context).size.width*0.22,
                //height: 50.0,
                //width: 50.0,
                child: Image.asset(
                  'assets/transparent-circle.png',
                  height: MediaQuery.of(context).size.height*0.085,
                  width: MediaQuery.of(context).size.width*0.085,
                  fit: BoxFit.fitWidth,
                ),
              ):Container(height:0, width: 0),
              toggleVal ? Positioned(//left bottom circle
                top: MediaQuery.of(context).size.height*0.40,
                left: MediaQuery.of(context).size.width*0.32,
                //height: 50.0,
                //width: 50.0,
                child: Image.asset(
                  'assets/transparent-circle.png',
                  height: MediaQuery.of(context).size.height*0.085,
                  width: MediaQuery.of(context).size.width*0.085,
                  fit: BoxFit.fitWidth,
                ),
              ):Container(height:0, width: 0),
              toggleVal ? Positioned( //right thumb circle
                top: MediaQuery.of(context).size.height*0.35,
                left: MediaQuery.of(context).size.width - 55.0,
                //height: 50.0,
                //width: 50.0,
                child: Image.asset(
                  'assets/transparent-circle.png',
                  height: MediaQuery.of(context).size.height*0.085,
                  width: MediaQuery.of(context).size.width*0.085,
                  fit: BoxFit.fitWidth,
                ),
              ):Container(height:0, width: 0),
              toggleVal ? Positioned( //right top circle
                top: MediaQuery.of(context).size.height*0.33,
                left: MediaQuery.of(context).size.width - 127.0,
                //height: 50.0,
                //width: 50.0,
                child: Image.asset(
                  'assets/transparent-circle.png',
                  height: MediaQuery.of(context).size.height*0.085,
                  width: MediaQuery.of(context).size.width*0.085,
                  fit: BoxFit.fitWidth,
                ),
              ):Container(height:0, width: 0),
              toggleVal ? Positioned( //right bottom circle
                top: MediaQuery.of(context).size.height*0.40,
                left: MediaQuery.of(context).size.width - 167.0,
                //height: 50.0,
                //width: 50.0,
                child: Image.asset(
                  'assets/transparent-circle.png',
                  height: MediaQuery.of(context).size.height*0.085,
                  width: MediaQuery.of(context).size.width*0.085,
                  fit: BoxFit.fitWidth,
                ),
              ):Container(height:0, width: 0),
            ]
          ),
          Row( //heartbeat mode bar
            mainAxisAlignment: MainAxisAlignment.start,
            crossAxisAlignment: CrossAxisAlignment.center,
            children:<Widget>[
              Expanded(
                flex: 2,
                child: Icon(
                  Icons.timeline_sharp,
                ),
              ),
              Expanded(
                flex: 6,
                child: Text(
                  'Monitor Mode',
                  textAlign: TextAlign.center,
                ),
              ),
              Expanded(
                flex: 2,
                child: Switch(
                  value: toggleVal,
                  onChanged: (newVal) {
                    toggleValSwitch(newVal);
                  },
                ),
              ),

            ],),
          Row(//status bar
            //mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            crossAxisAlignment: CrossAxisAlignment.center,
            children:<Widget>[
              Expanded(
                flex: 2,
                child: Icon(
                  Icons.schedule,
                ),
              ),
              Expanded(
                flex: 6,
                child: Text(
                  'Status',
                  textAlign: TextAlign.center,
                ),
              ),
              Expanded(
                flex: 2,
                child: Text(
                  '$status_type',
                  textAlign: TextAlign.center,
                ),
              ),
            ],),
          Row( //battery bar
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            crossAxisAlignment: CrossAxisAlignment.center,
            children:<Widget>[
              Expanded(
                flex: 2,
                child: Icon(
                  Icons.battery_charging_full,
                ),
              ),
              Expanded(
                flex: 6,
                child: Text(
                  'Battery',
                  textAlign: TextAlign.center,
                ),
              ),
              Expanded(
                flex: 2,
                child: Text(
                  '$battery_per %',
                  textAlign: TextAlign.center,
                ),
              ),
            ],),
        ],),
    );
  }
}