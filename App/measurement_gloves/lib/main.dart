import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';

import './GloveApp.dart';
import './heartbeat.dart';
import './leftHand.dart';
import './rightHand.dart';
import './widgets.dart';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  @override
  // Widget build(BuildContext context) {
  //   return MaterialApp(
  //     initialRoute: '/',
  //     routes: {
  //       '/': (context) => FirstScreen(),
  //       //'/GloveApp': (context) => GloveApp(),
  //       // '/heartbeat': (context) => Heartbeat(),
  //       '/leftHand': (context) => LeftHand(),
  //       '/rightHand': (context) => RightHand(),
  //     },
  //   );
  // }
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
      body: RefreshIndicator(
        onRefresh: () =>
            FlutterBlue.instance.startScan(timeout: Duration(seconds: 4), withServices: [Guid("139d09c1-b45a-4c76-b4bd-778dc82a5d67"), Guid("26548447-3cd0-4460-b683-43b332274c2b")]),

        child: SingleChildScrollView(
          child: Column(
            children: <Widget>[
              // StreamBuilder<List<BluetoothDevice>>(
              //   stream: Stream.periodic(Duration(seconds: 2))
              //       .asyncMap((_) => FlutterBlue.instance.connectedDevices),
              //   initialData: [],
              //   builder: (c, snapshot) => Column(
              //     children: snapshot.data
              //         .map((d) => ListTile(
              //               title: Text(d.name),
              //             ))
              //         .toList(),
              //   ),
              // ),
              StreamBuilder<List<ScanResult>>(
                stream: FlutterBlue.instance.scanResults,
                initialData: [],
                builder: (c, snapshot) => Column(
                  children: snapshot.data
                      .map(
                        (r) =>
                        // ListTile(
                        //   title: Text(r.device.name),
                        //   trailing: Icon(Icons.keyboard_arrow_right),
                        // // onTap:
                        // )
                        ScanResultTile(
                          result: r,
                          onTap: () => Navigator.of(context)
                              .push(MaterialPageRoute( builder: (context) {
                            if(r.advertisementData.localName.contains("Left"))
                            {
                              return LeftHand(device: r.device);
                            }
                            else
                            {
                              print("trying to go to right hand");
                              return RightHand(device: r.device);
                            }
                          })),
                        ),
                      )
                      .toList(),
                ),
              ),
            ],
          ),
        ),
      ),
      floatingActionButton: StreamBuilder<bool>(
        stream: FlutterBlue.instance.isScanning,
        initialData: false,
        builder: (c, snapshot) {
          if (snapshot.data) {
            return FloatingActionButton(
              child: Icon(Icons.stop),
              onPressed: () => FlutterBlue.instance.stopScan(),
              backgroundColor: Colors.red,
            );
          } else {
            return FloatingActionButton(
                child: Icon(Icons.search),
                onPressed: () => FlutterBlue.instance
                    .startScan(timeout: Duration(seconds: 4), withServices: [Guid("139d09c1-b45a-4c76-b4bd-778dc82a5d67"), Guid("26548447-3cd0-4460-b683-43b332274c2b")]));
          }
        },
      ),
    );
  }
}

// class FirstScreen extends StatelessWidget {
//   @override
//   Widget build(BuildContext context) {
//     return Scaffold(
//       appBar: AppBar(
//         title: Text('Gloves Nearby'),
//         backgroundColor: Colors.blueGrey,
//       ),
//       body: ListView(
//         children: ListTile.divideTiles(
//           context: context,
//           tiles: [
//             ListTile(
//               title: Text('Gloves 1'),
//               trailing: Icon(Icons.keyboard_arrow_right),
//               onTap: () {
//                 //Navigator.of(context).pushNamed('/GloveApp');
//                 Navigator.of(context).pushNamed('/leftHand');
//               },
//             ),
//             ListTile(
//               title: Text('Gloves 2'),
//               trailing: Icon(Icons.keyboard_arrow_right),
//               onTap: () {
//                 //Navigator.of(context).pushNamed('/GloveApp');
//                 Navigator.of(context).pushNamed('/rightHand');
//               },
//             ),
//             ListTile(
//               title: Text('Gloves 3'),
//               trailing: Icon(Icons.keyboard_arrow_right),
//               onTap: () {
//                 //Navigator.of(context).pushNamed('/GloveApp');
//                 Navigator.of(context).pushNamed('/heartbeat');
//               },
//             ),
//           ],
//         ).toList(),
//       ),
//     );
//   }
// }

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
