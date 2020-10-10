import 'package:flutter/material.dart';

// This is the stateful widget that FirstScreen instantiates
class GloveApp extends StatefulWidget {
  GloveApp({Key key}) : super(key: key);
  @override
  _GloveApp createState() => _GloveApp();
}

// This is the private State class that goes with GloveApp
class _GloveApp extends State<GloveApp> {
  bool _heartbeatMode = false;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Gloves 1'),
        actions: <Widget>[
          FlatButton(
            textColor: Colors.white,
            onPressed: () {},
            child: Text('Set RTC'),
          )
        ],
      ),
      body: ListView(
        children: ListTile.divideTiles(
          context: context,
          tiles: [
            SwitchListTile(
              secondary: Icon(Icons.bluetooth),
              title: const Text('Heartbeat Mode'),
              value: _heartbeatMode,
              onChanged: (bool value) {
                setState(() {
                  _heartbeatMode = value;
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
    );
  }
}
