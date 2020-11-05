import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'dart:convert';

import 'circle_painter.dart';

class RightHand extends StatefulWidget {
  //reloads changes when saved
  RightHand({Key key, this.device}) : super(key: key);
  final BluetoothDevice device;
  @override
  _RightHand createState() => _RightHand();
}

/*Global Variables*/

class _RightHand extends State<RightHand> {
  bool toggleVal = false;
  double rightThumbH = 0;
  double rightThumbW = 0;
  double rightTopH = 0;
  double rightTopW = 0;
  double rightBottomH = 0;
  double rightBottomW = 0;
  double radValThumb = 0;
  double radValPalm = 0;
  int colValThumb = 100;
  int colValPalm = 100;
  BluetoothCharacteristic rtcCharacteristic;

  toggleValSwitch(bool newVal) {
    setState(() {
      toggleVal = newVal;
    });
  }

  //status
  var status_type = 'Not connected';
  /*function here for receiving status of glove*/

  //battery
  int battery_per = 0; //initially zero

  void sendResetTimeRTC() {
    print(new DateTime.now().toIso8601String());
    // Send current time to glove
    rtcCharacteristic.write(utf8.encode(new DateTime.now().toIso8601String()));
  }

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
              if (status_type != "Recording") {
                toggleVal = false;
              }
              battery_per = parsedJson['battery'];
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
        if (c.uuid.toString() == "81600d69-4d48-4d19-b299-7ef5e3b21f69") // RTC Characteristic
        {
          print("Found RTC char");
          rtcCharacteristic = c;
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
        title: Text('Gloves (Right)',
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
            onPressed:
                (status_type == "Recording") ? null : () => sendResetTimeRTC(),
            child: Text('Reset RTC'),
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
                    //right hand
                    child: Image.asset(
                  'assets/transparent-hand-right.png',
                  height: MediaQuery.of(context).size.height * 0.6,
                  width: MediaQuery.of(context).size.width * 0.9,
                  fit: BoxFit.fitWidth,
                )),
              ],
            ),
            toggleVal
                ? Positioned(
                    //right thumb circle
                    top: MediaQuery.of(context).size.height * 0.29,
                    left: MediaQuery.of(context).size.width * 0.755,

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
                    //right top circle
                    top: MediaQuery.of(context).size.height * 0.24,
                    left: MediaQuery.of(context).size.width * 0.38,

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
                center: {"x": rightThumbW, "y": rightThumbH},
                radius: radValThumb,
                intensity: colValThumb,
                mode: toggleVal,
              ),
            ),
            Container(
              width: 400,
              height: 400,
              child: Circle(
                center: {"x": rightTopW, "y": rightTopH},
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
                    onChanged: (status_type != "Recording")
                        ? null
                        : (newVal) {
                            setState(() {
                              toggleValSwitch(newVal);
                              /*Get height and width*/
                              rightThumbH =
                                  MediaQuery.of(context).size.height * 0.375;
                              rightThumbW =
                                  MediaQuery.of(context).size.width * 0.84;
                              rightTopH =
                                  MediaQuery.of(context).size.height * 0.325;
                              rightTopW =
                                  MediaQuery.of(context).size.width * 0.465;
                              rightBottomH =
                                  MediaQuery.of(context).size.height * 0.455;
                              rightBottomW =
                                  MediaQuery.of(context).size.width * 0.31;
                            });
                          },
                  ),
                  ListTile(
                    leading: Icon(Icons.info_outline),
                    title: Text('Status: ' + status_type),
                    // trailing: Icon(Icons.keyboard_arrow_right),
                  ),
                  ListTile(
                    leading: Icon(Icons.battery_charging_full),
                    title: Text('Battery'),
                    trailing: Text(battery_per.toString() + "%"),
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
