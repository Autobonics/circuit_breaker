// File generated by FlutterFire CLI.
// ignore_for_file: lines_longer_than_80_chars, avoid_classes_with_only_static_members
import 'package:firebase_core/firebase_core.dart' show FirebaseOptions;
import 'package:flutter/foundation.dart'
    show defaultTargetPlatform, kIsWeb, TargetPlatform;

/// Default [FirebaseOptions] for use with your Firebase apps.
///
/// Example:
/// ```dart
/// import 'firebase_options.dart';
/// // ...
/// await Firebase.initializeApp(
///   options: DefaultFirebaseOptions.currentPlatform,
/// );
/// ```
class DefaultFirebaseOptions {
  static FirebaseOptions get currentPlatform {
    if (kIsWeb) {
      return web;
    }
    switch (defaultTargetPlatform) {
      case TargetPlatform.android:
        return android;
      case TargetPlatform.iOS:
        return ios;
      case TargetPlatform.macOS:
        throw UnsupportedError(
          'DefaultFirebaseOptions have not been configured for macos - '
          'you can reconfigure this by running the FlutterFire CLI again.',
        );
      case TargetPlatform.windows:
        throw UnsupportedError(
          'DefaultFirebaseOptions have not been configured for windows - '
          'you can reconfigure this by running the FlutterFire CLI again.',
        );
      case TargetPlatform.linux:
        throw UnsupportedError(
          'DefaultFirebaseOptions have not been configured for linux - '
          'you can reconfigure this by running the FlutterFire CLI again.',
        );
      default:
        throw UnsupportedError(
          'DefaultFirebaseOptions are not supported for this platform.',
        );
    }
  }

  static const FirebaseOptions web = FirebaseOptions(
    apiKey: 'AIzaSyDa_etOGczVsI0JaHfRFFJfrAF3wuf4ftw',
    appId: '1:218335559348:web:c232f1829485f213b161e1',
    messagingSenderId: '218335559348',
    projectId: 'circuit-breaker-2de9d',
    authDomain: 'circuit-breaker-2de9d.firebaseapp.com',
    databaseURL: 'https://circuit-breaker-2de9d-default-rtdb.asia-southeast1.firebasedatabase.app',
    storageBucket: 'circuit-breaker-2de9d.appspot.com',
    measurementId: 'G-30GGYQ52XG',
  );

  static const FirebaseOptions android = FirebaseOptions(
    apiKey: 'AIzaSyBp6Xv4wJlazL1VxeifqS9uFF4hu3jiAWQ',
    appId: '1:218335559348:android:7055e90ddfc4ee07b161e1',
    messagingSenderId: '218335559348',
    projectId: 'circuit-breaker-2de9d',
    databaseURL: 'https://circuit-breaker-2de9d-default-rtdb.asia-southeast1.firebasedatabase.app',
    storageBucket: 'circuit-breaker-2de9d.appspot.com',
  );

  static const FirebaseOptions ios = FirebaseOptions(
    apiKey: 'AIzaSyDPl6i9GykBGXiBcNSJenriVR4IIVPzDxM',
    appId: '1:218335559348:ios:a6eb2a1273eff9e7b161e1',
    messagingSenderId: '218335559348',
    projectId: 'circuit-breaker-2de9d',
    databaseURL: 'https://circuit-breaker-2de9d-default-rtdb.asia-southeast1.firebasedatabase.app',
    storageBucket: 'circuit-breaker-2de9d.appspot.com',
    iosClientId: '218335559348-vffq0g6s8b10du0qsrsar5rvnpvr7t2b.apps.googleusercontent.com',
    iosBundleId: 'com.autobonics.hydropodHydroponics',
  );
}
