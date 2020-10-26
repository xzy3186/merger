# merger

This software merges two CERN ROOT TTree data into one tree by timestamp.
This project was originally developped for RIKEN2018 experiment.
This e19044 branch is dedicated for the specific experiment.

### Required dependencies

* CERN ROOT (root-project/root) v6.14.00+
* paass (pixie16/paass) latest e19044 branch
* yaml-cpp (jbeder/yaml-cpp) v0.6+
* cmake3
* c++11

### How to build


```
mkdir build
cd build
cmake [-DBUILD_ANAMERGER=ON|OFF] [-DBUILD_EVENT_SELECTOR=ON|OFF]
make install
```

The default install path is ${CMAKE_SOURCE_DIR}/install
ROOT dictionary will also be installed in install/lib

### How to run excutables

There will be a module file for Environment Modules installed in install/share/modulefiles/

#### TraceAnalyzer

TraceAnalyzer will rebuild events based on pspmt_vec_ in the PixTreeEvent class.
Channel event with the subtype="dynode_high" and tag="single" will be stored as one event in the output tree.
Other channles will be stored if it's within the time gate provided in the input yaml file.

Usage:
```
trace_analyzer_main [input rootfile] [output_rootfile] [config yaml file]
```

Example yaml file is found at config/config_trace_analyzer.yaml
* When TChainFlag is True, the program will chain all the root files that matches to [input_argument]*.root
* PrintFrequency sets how often the program prints progress.
* You can set InitalEvent and NumEvents to scan. It will scan all the events if these options are not provided.
* You can select which vectors in the paass output to output. When you want to output vandle_vec_ for example, set VANDLE option to True
* TimeWindow and TimeOffset are the time gate for pspmt channels relative to the dynode single.
* TimeWindowIon and TimeOffsetIon are the time gate for the front plastic relative to the dynode single.
* TimeWindowVeto and TimeOffsetVeto are the time gate for the veto detector relative to the dynode single.
* High/LowGainThreshold are the qdc threshold for the pspmt __anode__ channels.
* High/LowGainOverflow are the qdc upper limit for the pspmt __anode__ channels.
* You can apply calibration to the pspmt anode channels and calulated positions as a formular expression for the ROOT::TF1 class.
HighGainXA: "2.0*x+1.0" for example.

#### Mergers

There are two merger executables in this branch
* merger_main (for beta-implant correlation by pspmt)
* next_merger_main (for merging the NEXT events to VANDLE beta events)

Both programs takes one argument to run.
```
merger_main -c [config yaml file]
```

Example yaml file is found at config/config.yaml
* E19044BetaTSScannor is a timestamp scannor for the beta events. It takes the TraceAnalyzer output as an input.
	* MinHighGainDynEnergy and MaxHighGainDynEnergy defines the energy range to identify the pspmt event as a beta event
	* MinLowGainDynEnergy and MaxLowGainDynEnergy defines the energy range in low gain channel. Both the low gain and high gain ranges have to be satisfied to be filled.
	* OtherBranches is a list of branches to output to the merged root file. If there is no event in the branch, it will be ignored.
* ImplantTSScannor is a timestamp scannor for the implant events. It takes the same rootfile as E19044BetaTSScannor
	* MinLowGainDynEnergy and MaxLowGainDynEnergy defines the energy range to identify the pspmt event as an implant event
	* OtherBranches is a list of branches to output to the merged root file. If there is no event in the branch, it will be ignored.
* E19044BetaTreeMerger is a TreeMerger class for beta-ion correlation
	* TimeWindowLow and TimeWindowHigh defines the correlation window of T_beta - T_implant in ns.
* BetaTreeMerger is a base class of E19044BetaTreeMerger.
	* CorrelationRadius is the radius of the position gate for beta-implant correlation in YSO.

Following are the settings for next_merger_main.
* E19044NEXTTSScannor is a timestamp scannor for the NEXT events. It takes the paass output directly.
* E19044VANDLETSScannor is a timestamp scannor for the merged VANDLE events. It takes the merger_main output.
* NEXTMerger is a TreeMerger class for merging NEXT events to the VANDLE merged beta events.

It is required to specify the InputFileName, TreeName, and PixieBranchName of the input in the yaml file for all the TSScannor derrived classes.
OutputFileName, TreeName, and BranchName of the output are needed to be specified in the options for TreeMerger classes.

You can use config/config_gen.py to generate or change yaml files in a script.

