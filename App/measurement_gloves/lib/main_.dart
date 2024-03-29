import 'package:flutter/material.dart';

import './GloveApp.dart';
import './heartbeat.dart';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      initialRoute: '/',
      routes: {
        '/': (context) => FirstScreen(),
        //'/GloveApp': (context) => GloveApp(),
        '/heartbeat': (context) => Heartbeat(),
      },
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
