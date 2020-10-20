import 'package:flutter/material.dart';



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
                Container(
                  width: 10,
                  height: 100,
                  child: CustomPaint(
                    painter: OpenPainter(),
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

//Drawing Pressure Circles
double rad = 17;
class OpenPainter extends CustomPainter {
  @override
  void paint(Canvas canvas, Size size) {
    var paint1 = Paint() //left thumb
      ..color = Colors.redAccent
      ..style = PaintingStyle.fill;
    var paint2 = Paint() //left top
      ..color = Colors.redAccent[100]
      ..style = PaintingStyle.fill;
    var paint3 = Paint() //left bottom
      ..color = Colors.redAccent
      ..style = PaintingStyle.fill;
    var paint4 = Paint() //right thumb
      ..color = Colors.redAccent
      ..style = PaintingStyle.fill;
    var paint5 = Paint() //right top
      ..color = Colors.redAccent
      ..style = PaintingStyle.fill;
    var paint6 = Paint() //right bottom
      ..color = Colors.redAccent
      ..style = PaintingStyle.fill;
    //a circle
    if(toggleVal) {
      canvas.drawCircle(Offset(leftThumbW, leftThumbH), rad, paint1);
      canvas.drawCircle(Offset(leftTopW, leftTopH), rad, paint2);
      canvas.drawCircle(Offset(leftBottomW, leftBottomH), rad, paint3);
      canvas.drawCircle(Offset(rightThumbW, rightThumbH), rad, paint4);
      canvas.drawCircle(Offset(rightTopW, rightTopH), rad, paint5);
      canvas.drawCircle(Offset(rightBottomW, rightBottomH), rad, paint6);
    }
  }

  @override
  bool shouldRepaint(CustomPainter oldDelegate) => true;
}