import 'dart:async';
import 'dart:math';

import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';

import './GloveApp.dart';
import './heartbeat.dart';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      color: Colors.lightBlue,
      home: StreamBuilder<BluetoothState>(
          stream: FlutterBlue.instance.state,
          initialData: BluetoothState.unknown,
          builder: (c, snapshot) {
            final state = snapshot.data;
            if (state == BluetoothState.on) {
              return FirstScreen();
            }
            return BluetoothOffScreen(state: state);
          }),
    );
  }
}

class FirstScreen extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Gloves Nearby'),
      ),
      body: ListView(
        children: ListTile.divideTiles(
          context: context,
          tiles: [
            ListTile(
              title: Text('Gloves 1'),
              trailing: Icon(Icons.keyboard_arrow_right),
              onTap: () {
                //Navigator.of(context).pushNamed('/GloveApp');
                Navigator.of(context).pushNamed('/heartbeat');
              },
            ),
            ListTile(
              title: Text('Gloves 2'),
              trailing: Icon(Icons.keyboard_arrow_right),
              onTap: () {
                //Navigator.of(context).pushNamed('/GloveApp');
                Navigator.of(context).pushNamed('/heartbeat');
              },
            ),
            ListTile(
              title: Text('Gloves 3'),
              trailing: Icon(Icons.keyboard_arrow_right),
              onTap: () {
                //Navigator.of(context).pushNamed('/GloveApp');
                Navigator.of(context).pushNamed('/heartbeat');
              },
            ),
          ],
        ).toList(),
      ),
    );
  }
}

// Show this when Bluetooth cannot be used
class BluetoothOffScreen extends StatelessWidget {
  const BluetoothOffScreen({Key key, this.state}) : super(key: key);

  final BluetoothState state;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.lightBlue,
      body: Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: <Widget>[
            Icon(
              Icons.bluetooth_disabled,
              size: 200.0,
              color: Colors.white54,
            ),
            Text(
              'Bluetooth Adapter is ${state != null ? state.toString().substring(15) : 'not available'}.',
              style: Theme.of(context)
                  .primaryTextTheme
                  .subhead
                  .copyWith(color: Colors.white),
            ),
          ],
        ),
      ),
    );
  }
}
