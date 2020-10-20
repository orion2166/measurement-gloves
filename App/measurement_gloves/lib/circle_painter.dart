//circle_painter.dart
import 'package:flutter/material.dart';
import 'heartbeat.dart';
class Circle extends StatefulWidget {
  final Map<String, double> center;
  final double radius;
  final bool mode;
  Circle({this.center, this.radius, this.mode});
  @override
  _CircleState createState() => _CircleState();
}

class _CircleState extends State<Circle> with SingleTickerProviderStateMixin {
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
      painter: DrawCircle(center: widget.center, radius: widget.radius, mode: widget.mode),
    );
  }
}
//Drawing Pressure Circles
class DrawCircle extends CustomPainter {
  Map<String, double> center;
  double radius;
  bool mode;
  DrawCircle({this.center, this.radius, this.mode});
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

    if(mode) { //if monitoring mode
      canvas.drawCircle(Offset(center["x"], center["y"]), radius, paint1); //change rad to radius later
    }
  }

  @override
  bool shouldRepaint(CustomPainter oldDelegate) => true;
}