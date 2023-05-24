import 'dart:ui';

import 'package:flutter/material.dart';
import 'package:pecb/ui/smart_widgets/online_status.dart';
import 'package:stacked/stacked.dart';

import 'package:syncfusion_flutter_charts/charts.dart';
import 'package:syncfusion_flutter_gauges/gauges.dart';
import 'package:syncfusion_flutter_gauges/gauges.dart' as g;

import 'package:lottie/lottie.dart';

import 'home_viewmodel.dart';

class HomeView extends StatelessWidget {
  const HomeView({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return ViewModelBuilder<HomeViewModel>.reactive(
      onViewModelReady: (model) => model.onModelReady(),
      builder: (context, model, child) {
        // print(model.node?.lastSeen);
        return Scaffold(
            appBar: AppBar(
              title: const Text('PECB'),
              centerTitle: true,
              actions: [IsOnlineWidget()],
            ),
            body: model.data != null && model.deviceData != null
                ? const _HomeBody()
                : Center(child: Text("No data")));
      },
      viewModelBuilder: () => HomeViewModel(),
    );
  }
}

class _HomeBody extends ViewModelWidget<HomeViewModel> {
  const _HomeBody({Key? key}) : super(key: key, reactive: true);

  @override
  Widget build(BuildContext context, HomeViewModel model) {
    return Stack(
      children: [
        Container(
          height: MediaQuery.of(context).size.height,
          child: SingleChildScrollView(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                Card(
                  child: Padding(
                    padding: const EdgeInsets.all(18.0),
                    child: Text(model.data!.isLoggedIn
                        ? "Logged in."
                        : "Logged out, waiting for password."),
                  ),
                ),
                // Card(
                //   child: Padding(
                //     padding: const EdgeInsets.all(18.0),
                //     child: Text("Volt: ${model.data!.volt} Ah"),
                //   ),
                // ),
                // Card(
                //   child: Padding(
                //     padding: const EdgeInsets.all(18.0),
                //     child: Text("Ampere reading: ${model.data!.amp} Ah"),
                //   ),
                // ),
                Card(
                  child: Padding(
                    padding: const EdgeInsets.all(18.0),
                    child: Text("Ampere reading: ${model.data!.maxAmp} Ah"),
                  ),
                ),
                MyInputView(),
              ],
            ),
          ),
        ),
        if (model.data!.maxAmp > model.deviceData!.ampPreset)
          Positioned.fill(child: Warning(value: model.data!.maxAmp))
      ],
    );
  }
}

class Warning extends StatelessWidget {
  final double value;
  const Warning({super.key, required this.value});

  @override
  Widget build(BuildContext context) {
    return BackdropFilter(
      filter: ImageFilter.blur(sigmaX: 10.0, sigmaY: 10.0),
      child: Center(
        child: Card(
          // elevation: 10,
          color: Colors.black.withOpacity(0.5),
          child: Container(
            height: 400,
            width: 250,
            child: Center(
              child: Column(
                mainAxisAlignment: MainAxisAlignment.center,
                crossAxisAlignment: CrossAxisAlignment.center,
                children: [
                  Lottie.network(
                      'https://assets2.lottiefiles.com/packages/lf20_Tkwjw8.json'),
                  SizedBox(height: 20),
                  Text(
                    'Overload $value Ah}',
                    textAlign: TextAlign.center,
                    style: TextStyle(
                      fontSize: 15,
                      color: Colors.white,
                    ),
                  ),
                ],
              ),
            ),
          ),
        ),
      ),
    );
  }
}

class MyInputView extends ViewModelWidget<HomeViewModel> {
  const MyInputView({Key? key}) : super(key: key, reactive: true);

  @override
  Widget build(BuildContext context, HomeViewModel viewModel) {
    return Padding(
      padding: const EdgeInsets.all(16.0),
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          if (viewModel.deviceData != null)
            TextFormField(
              controller: viewModel.passwordController,
              // initialValue: viewModel.deviceData!.password.toString(),
              keyboardType: TextInputType.number,
              maxLength: 4,
              obscureText: viewModel.passwordVisible,
              decoration: InputDecoration(
                border: OutlineInputBorder(),
                labelText: '4-Digit Password',
                suffixIcon: IconButton(
                  onPressed: () {
                    viewModel.togglePasswordVisibility();
                  },
                  icon: Icon(
                    viewModel.passwordVisible
                        ? Icons.visibility
                        : Icons.visibility_off,
                  ),
                ),
              ),
            ),
          SizedBox(height: 16.0),
          if (viewModel.deviceData != null)
            TextFormField(
              controller: viewModel.ampereController,
              // initialValue: viewModel.deviceData!.ampPreset.toString(),
              keyboardType: TextInputType.number,
              decoration: InputDecoration(
                border: OutlineInputBorder(),
                labelText: 'Cutoff Ampere',
              ),
            ),
          SizedBox(height: 16.0),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              ElevatedButton(
                onPressed: viewModel.isBusy
                    ? null
                    : () {
                        viewModel.sendDataToFirebase();
                      },
                child: viewModel.isBusy
                    ? CircularProgressIndicator()
                    : Text('Update data'),
              ),
            ],
          ),
        ],
      ),
    );
  }
}
