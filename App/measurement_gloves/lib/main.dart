import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';

import 'circle_painter.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: MyHomePage(),
      debugShowCheckedModeBanner: false,
    );
  }
}

class MyHomePage extends StatefulWidget {
  @override
  _MyHomePageState createState() => _MyHomePageState();
}

// state class
class _MyHomePageState extends State<MyHomePage> {
  // state variable
  double forceVal = 0;

  // The State class must include this method, which builds the widget
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Monitor mode - Test'),
        backgroundColor: Color(0xFF444444),
      ),
      body: ListView(children: <Widget>[
        Text(
          'Canvas',
          textAlign: TextAlign.center,
          style: TextStyle(fontSize: 20, height: 2),
        ),
        Container(
            width: 400,
            height: 400,
            child: Circle(
              center: {"x": 100, "y": 100},
              radius: forceVal,
            )),
        Container(
          child: RaisedButton(
            child: Text('Test'),
            color: Colors.blueAccent,
            onPressed: _changeSize,
          ),
        )
      ]),
    );
  }

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
}
