import 'dart:async';

import 'package:flutter/material.dart';
import 'package:pecb/models/models.dart';
import 'package:pecb/services/db_service.dart';
import 'package:pecb/ui/setup_snackbar_ui.dart';
import 'package:stacked/stacked.dart';
import 'package:stacked_services/stacked_services.dart';
import 'package:syncfusion_flutter_charts/charts.dart';

import '../../../app/app.locator.dart';
import '../../../app/app.logger.dart';
// import '../../setup_snackbar_ui.dart';

class HomeViewModel extends ReactiveViewModel {
  final log = getLogger('HomeViewModel');

  final _snackBarService = locator<SnackbarService>();
  // final _navigationService = locator<NavigationService>();
  final _dbService = locator<DbService>();

  DeviceReading? get data => _dbService.node;

  @override
  List<DbService> get reactiveServices => [_dbService];

  void onModelReady() async {
    DeviceData? deviceData = await _dbService.getDeviceData();
    if (deviceData != null) {
      _deviceData = DeviceData(
          ampPreset: deviceData.ampPreset, password: deviceData.password);
      log.i(_deviceData!.toJson());
      passwordController =
          TextEditingController(text: deviceData.password.toString());
      ampereController =
          TextEditingController(text: deviceData.ampPreset.toString());
      // passwordController.value =
      //     TextEditingValue(text: deviceData.password.toString());
      // ampereController.value =
      //     TextEditingValue(text: deviceData.ampPreset.toString());
      notifyListeners();
    } else {
      passwordController = TextEditingController(text: "");
      ampereController = TextEditingController(text: "");
    }
  }

  DeviceData? _deviceData;
  DeviceData? get deviceData => _deviceData;

  Future setDeviceData() {
    return _dbService.setDeviceData(_deviceData!);
  }

  late TextEditingController passwordController;
  late TextEditingController ampereController;

  bool _passwordVisible = false;
  bool get passwordVisible => _passwordVisible;

  void togglePasswordVisibility() {
    _passwordVisible = !_passwordVisible;
    notifyListeners();
  }

  Future<void> sendDataToFirebase() async {
    setBusy(true);
    String password = passwordController.text;
    String ampere = ampereController.text;
    _deviceData!.ampPreset = double.parse(ampere);
    _deviceData!.password = int.parse(password);
    setDeviceData();
    _snackBarService.showCustomSnackBar(
        message: "Updated", variant: SnackbarType.success);
    setBusy(false);
  }

  @override
  void dispose() {
    passwordController.dispose();
    ampereController.dispose();
    super.dispose();
  }
}
