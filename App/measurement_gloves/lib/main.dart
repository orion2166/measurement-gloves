import 'package:flutter/material.dart';

//import './GloveApp.dart';
//import './heartbeat.dart';
import './leftHand.dart';
import './rightHand.dart';
import './LeftHandTest.dart';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      initialRoute: '/',
      routes: {
        '/': (context) => FirstScreen(),
        '/leftHand': (context) => LeftHand(),
        '/rightHand': (context) => RightHand(),
        '/LeftHandTest': (context) => LeftHandTest(),
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
        backgroundColor: Colors.blueGrey,
      ),
      body: ListView(
        children: ListTile.divideTiles(
          context: context,
          tiles: [
            ListTile(
              title: Text('Gloves 1'),
              trailing: Icon(Icons.keyboard_arrow_right),
              onTap: () {
                //add some codes here to determine which page to navigate (left or right)
                Navigator.of(context).pushNamed('/leftHand');
              },
            ),
            ListTile(
              title: Text('Gloves 2'),
              trailing: Icon(Icons.keyboard_arrow_right),
              onTap: () {
                Navigator.of(context).pushNamed('/rightHand');
              },
            ),
            ListTile(
              title: Text('Gloves 3'),
              trailing: Icon(Icons.keyboard_arrow_right),
              onTap: () {
                Navigator.of(context).pushNamed('/LeftHandTest');
              },
            ),
          ],
        ).toList(),
      ),
    );
  }
}
