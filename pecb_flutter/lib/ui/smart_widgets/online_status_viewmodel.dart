import 'dart:async';

import 'package:pecb/models/models.dart';
import 'package:pecb/services/db_service.dart';
import 'package:stacked/stacked.dart';

import '../../../app/app.locator.dart';
import '../../../app/app.logger.dart';

class HomeViewModel extends BaseViewModel {
  final log = getLogger('StatusWidget');

  final _dbService = locator<DbService>();

  DeviceReading? get node => _dbService.node;

  bool _isOnline = false;
  bool get isOnline => _isOnline;

  bool isOnlineCheck(DateTime? time) {
    if (time == null) return false;
    final DateTime now = DateTime.now();
    final difference = now.difference(time).inSeconds;
    // log.i("Status $difference");
    return difference.abs() <= 4;
  }

  late Timer timer;

  void setTimer() {
    const oneSec = const Duration(seconds: 1);
    timer = new Timer.periodic(
      oneSec,
      (Timer timer) {
        _isOnline = isOnlineCheck(node?.lastSeen);
        notifyListeners();
      },
    );
  }
}
