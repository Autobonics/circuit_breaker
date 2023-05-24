/// Institution model
class DeviceReading {
  double maxAmp;
  double amp;
  double volt;
  bool isLoggedIn;
  DateTime lastSeen;

  DeviceReading({
    required this.isLoggedIn,
    required this.maxAmp,
    required this.amp,
    required this.volt,
    required this.lastSeen,
  });

  factory DeviceReading.fromMap(Map data) {
    return DeviceReading(
      isLoggedIn: data['isLoggedIn'] ?? false,
      amp: data['amp'] != null
          ? (data['amp'] % 1 == 0 ? data['amp'] + 0.1 : data['amp'])
          : 0,
      maxAmp: data['maxAmp'] != null
          ? (data['maxAmp'] % 1 == 0 ? data['maxAmp'] + 0.1 : data['maxAmp'])
          : 0,
      volt: data['volt'] != null
          ? (data['volt'] % 1 == 0 ? data['volt'] + 0.1 : data['volt'])
          : 0,
      lastSeen: DateTime.fromMillisecondsSinceEpoch(data['ts']),
    );
  }
}

/// Device control model
class DeviceData {
  double ampPreset;
  int password;

  DeviceData({
    required this.ampPreset,
    required this.password,
  });

  factory DeviceData.fromMap(Map data) {
    return DeviceData(
      ampPreset: data['ampPreset'] != null
          ? (data['ampPreset'] % 1 == 0
              ? data['ampPreset'] + 0.01
              : data['ampPreset'])
          : 0,
      password: data['password'] ?? 0,
    );
  }

  Map<String, dynamic> toJson() => {
        'ampPreset': ampPreset,
        'password': password,
      };
}
