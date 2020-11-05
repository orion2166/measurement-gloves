//circle_painter.dart
import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'leftHand.dart';
import 'rightHand.dart';
// import 'LeftHandTest.dart';

//Global Variable
//double radius = 0;
//double intensity = 0;

class Circle extends StatefulWidget {
  final Map<String, double> center;
  final double radius;
  final int intensity;
  final bool mode;
  Circle({this.center, this.radius, this.intensity, this.mode});
  @override
  _CircleState createState() => _CircleState();
}

class _CircleState extends State<Circle> with SingleTickerProviderStateMixin {
/*
  void convertRadIntensity(double forceVal) {
    /*Convert Force Value to Radius and Intensity*/
    //Assume Force Value Range is 0 - 100
    //Radius Range is 3 - 35
    //Intensity Range is 100 - 900
    radius = (forceVal - 0) * 100/32 + 3;
    intensity = (forceVal - 0) * 900/32 + 100;

  }
*/
  AnimationController _controller;
  @override
  void initState() {
    super.initState();
    _controller = AnimationController(vsync: this);
  }

  @override
  void dispose() {
    super.dispose();
    _controller.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return CustomPaint(
      size: Size(MediaQuery.of(context).size.width,
          MediaQuery.of(context).size.height),
      painter: DrawCircle(center: widget.center, radius: widget.radius, intensity: widget.intensity, mode: widget.mode),
    );
  }
}
//Drawing Pressure Circles
class DrawCircle extends CustomPainter {
  Map<String, double> center;
  double radius;
  int intensity;
  bool mode;
  DrawCircle({this.center, this.radius, this.intensity, this.mode});


  @override
  void paint(Canvas canvas, Size size) {
    var paint1 = Paint() //left thumb
      ..color = Colors.red[intensity]
      ..style = PaintingStyle.fill;

    if(mode) { //if monitoring mode
      canvas.drawCircle(Offset(center["x"], center["y"]), radius, paint1); //change rad to radius later
    }
  }

  @override
  bool shouldRepaint(CustomPainter oldDelegate) => true;

}
