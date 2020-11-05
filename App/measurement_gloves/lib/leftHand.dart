import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'dart:convert';

import 'circle_painter.dart';

class LeftHand extends StatefulWidget {
  //reloads changes when saved
  LeftHand({Key key, this.device}) : super(key: key);
  final BluetoothDevice device;
  @override
  _LeftHand createState() => _LeftHand();
}

class _LeftHand extends State<LeftHand> {
  bool toggleVal = false;
  double leftThumbH = 0;
  double leftThumbW = 0;
  double leftTopH = 0;
  double leftTopW = 0;
  double leftBottomH = 0;
  double leftBottomW = 0;
  double radValThumb = 0;
  double radValPalm = 0;
  int colValThumb = 100;
  int colValPalm = 100;

  toggleValSwitch(bool newVal) {
    setState(() {
      toggleVal = newVal;
    });
  }

  //status
  var status_type = 'Not connected';

  //battery
  int battery_per = 0; //initially zero

  void setupGloveDevice() async {
    await widget.device.connect();
    List<BluetoothService> services = await widget.device.discoverServices();
    services.forEach((service) {
      var characteristics = service.characteristics;
      for (BluetoothCharacteristic c in characteristics) {
        if (c.uuid.toString() ==
            "b106d600-3ee1-4a10-8dd7-260074535086") // Status characteristic
        {
          c.setNotifyValue(true);
          c.value.listen((value) {
            var parsedJson = json.decode(new String.fromCharCodes(value));
            setState(() {
              status_type = parsedJson['state'];
              battery_per = parsedJson['battery'];
              if (status_type != "Recording")
              {
                toggleVal = false;
              }
            });
          });
        }
        if (c.uuid.toString() ==
            "43b513cf-08aa-4bd9-bc58-3f626a4248d8") // Monitoring characteristic
        {
          c.setNotifyValue(true);
          c.value.listen((value) {
            if (toggleVal) // Active Monitoring Mode is ON
            {
              var parsedJson = json.decode(new String.fromCharCodes(value));
              setState(() {
                convertRadIntensityThumb(parsedJson["Thumb"]);
                convertRadIntensityPalm(parsedJson["Palm"]);
              });
            }
          });
        }
      }
    });
  }

  @override
  void initState() {
    setupGloveDevice();
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Gloves (Left)',
            style: TextStyle(
              color: Colors.black,
            )),
        centerTitle: true,
        backgroundColor: Colors.blueGrey, //ctrl+Q to view color intensity
        leading: GestureDetector(
          //back button
          onTap: () {
            widget.device.disconnect();
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
        ],
      ),
      body: Column(
        children: <Widget>[
          Stack(children: <Widget>[
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
              crossAxisAlignment: CrossAxisAlignment.center,
              children: <Widget>[
                Container(
                    //left hand
                    child: Image.asset(
                  'assets/transparent-hand-left.png',
                  height: MediaQuery.of(context).size.height * 0.6,
                  width: MediaQuery.of(context).size.width * 0.9,
                  fit: BoxFit.fitWidth,
                )),
              ],
            ),
            toggleVal
                ? Positioned(
                    //left thumb circle
                    top: MediaQuery.of(context).size.height * 0.29,
                    left: MediaQuery.of(context).size.width * 0.08,
                    //height: 50.0,
                    //width: 50.0,
                    child: Image.asset(
                      'assets/transparent-circle.png',
                      height: MediaQuery.of(context).size.height * 0.17,
                      width: MediaQuery.of(context).size.width * 0.17,
                      fit: BoxFit.fitWidth,
                    ),
                  )
                : Container(height: 0, width: 0),
            toggleVal
                ? Positioned(
                    //left top circle
                    top: MediaQuery.of(context).size.height * 0.24,
                    left: MediaQuery.of(context).size.width * 0.44,
                    //height: 50.0,
                    //width: 50.0,
                    child: Image.asset(
                      'assets/transparent-circle.png',
                      height: MediaQuery.of(context).size.height * 0.17,
                      width: MediaQuery.of(context).size.width * 0.17,
                      fit: BoxFit.fitWidth,
                    ),
                  )
                : Container(height: 0, width: 0),
            Container(
              width: 400,
              height: 400,
              child: Circle(
                center: {"x": leftThumbW, "y": leftThumbH},
                radius: radValThumb,
                intensity: colValThumb,
                mode: toggleVal,
              ),
            ),
            Container(
              width: 400,
              height: 400,
              child: Circle(
                center: {"x": leftTopW, "y": leftTopH},
                radius: radValPalm,
                intensity: colValPalm,
                mode: toggleVal,
              ),
            ),
          ]),
          Divider(thickness: 2),
          Expanded(
            child: ListView(
              children: ListTile.divideTiles(
                context: context,
                tiles: [
                  SwitchListTile(
                    secondary: Icon(Icons.bluetooth),
                    activeColor: Colors.blueGrey,
                    title: const Text('Active Monitoring Mode'),
                    value: toggleVal,
                    onChanged: (status_type != "Recording") ? null : (newVal) {
                      setState(() {
                        toggleValSwitch(newVal);
                        /*Get height and width*/
                        leftThumbH = MediaQuery.of(context).size.height * 0.375;
                        leftThumbW = MediaQuery.of(context).size.width * 0.16;
                        leftTopH = MediaQuery.of(context).size.height * 0.325;
                        leftTopW = MediaQuery.of(context).size.width * 0.52;
                        leftBottomH =
                            MediaQuery.of(context).size.height * 0.455;
                        leftBottomW = MediaQuery.of(context).size.width * 0.68;
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
        ],
      ),
    );
  }

  void convertRadIntensityThumb(int forceVal) {
    /*Convert Force Value to Radius and Intensity*/
    //Assume Force Value Range is 0 - 100
    //Radius Range is 3,4,5,..., 35
    //Intensity Range is 100, 200, ..., 900
    radValThumb = (forceVal - 0) * 32 / 2500 + 3;
    double intens = (forceVal - 0) * 800 / 2500 + 100;
    colValThumb =
        ((intens.toInt() + 99) ~/ 100) * 100; //rounds to nearest hundredth
  }

  void convertRadIntensityPalm(int forceVal) {
    /*Convert Force Value to Radius and Intensity*/
    //Assume Force Value Range is 0 - 100
    //Radius Range is 3,4,5,..., 35
    //Intensity Range is 100, 200, ..., 900
    radValPalm = (forceVal - 0) * 32 / 3500 + 3;
    double intens = (forceVal - 0) * 800 / 3500 + 100;
    colValPalm =
        ((intens.toInt() + 99) ~/ 100) * 100; //rounds to nearest hundredth
  }
}
