
constexpr double photoElectronsPerkeV = 0.5;//Average number of photoelectrons per keV
constexpr double timeResolution = 0.04;//us
constexpr double timeOffset = 4;//us
constexpr int nTimeBins = 512;//Number of timeBins in the readout electronics
constexpr int shapingTime = 10;//units are timeBins
constexpr double gain = 10;
constexpr int Nr = 5 * shapingTime;
constexpr double noiseLevel = 2.;//ADCs
constexpr double ADCOffset = 250;//ADCs
const std::vector<std::string> sensitiveVolume={"veto1","veto2"};//Add as many as you want

std::vector<double> initShaper(){
  std::vector<double> response(Nr);
    for (int i = 0; i < Nr; i++) {
      const double coeff = ((Double_t)i) / shapingTime;
      response[i] = TMath::Exp(-3. * coeff) * coeff * coeff * coeff * sin(coeff);
    }
  double sum=0;
  for (int n = 0; n < Nr; n++) sum += response[n];
  for (int n = 0; n < Nr; n++) response[n] = response[n] * gain / sum;

  return response;
}

static std::vector<double> responseShaper = initShaper();

std::vector<int> GetSignalAfterShaping(const std::vector<int> data){

  const auto nBins = data.size();
  std::vector<int> result(nBins,0);
    for (int m = 0; m < nBins; m++) {
     if(data[m]==0)continue;
      for (int n = 0; m + n < nBins && n < Nr; n++){
        result[m + n] += responseShaper[n] * data[m];
      }
    }
  return result;
}

void Geant4ToSignal(const std::string &filePattern){

auto files = TRestTools::GetFilesMatchingPattern(filePattern);

for (const auto &inputFile : files){

  cout<<"Analyzing file "<<inputFile<<endl;

  TRestRun inputRun(inputFile);
  inputRun.PrintMetadata();

  TRestGeant4Metadata* G4Metadata = static_cast<TRestGeant4Metadata*>(inputRun.GetMetadataClass("TRestGeant4Metadata"));
  std::vector<int> sensitiveVolumeID;
  for (const auto& userVolume : sensitiveVolume) {
    //cout<<userVolume<<" "<<G4Metadata->GetActiveVolumeID(userVolume)<<endl;
    if (G4Metadata->GetActiveVolumeID(userVolume) >= 0) {
      sensitiveVolumeID.push_back(G4Metadata->GetActiveVolumeID(userVolume));
    } else
      cout << "TRestGeant4ToDetectorHitsProcess. volume name : " << userVolume
           << " not found and will not be added." << endl;
  }

  TRestGeant4Event* g4Event = static_cast<TRestGeant4Event*>(inputRun.GetInputEvent());
  
  TRestRawSignalEvent rawSignalEvent;
  TRestDetectorHitsEvent hitsEvent;

  TRestRun outRun (inputFile);
  outRun.SetRunNumber(inputRun.GetRunNumber());
  std::string runTag = inputRun.GetRunTag().Data();
  outRun.SetRunTag(runTag);
  outRun.SetRunType("RAWSIGNAL");
  outRun.SetOutputFileName("Run[fRunNumber]_[fRunType]_[fRunTag]_[fRunUser].root");
  outRun.FormOutputFile();
  outRun.AddEventBranch(g4Event);
  outRun.AddEventBranch(&rawSignalEvent);
  outRun.SetStartTimeStamp(inputRun.GetStartTimestamp());

  const int pulseStart = std::round(timeOffset/timeResolution);
  TVector2 baselineRange (0,pulseStart);
  TVector2 integralRange (pulseStart,nTimeBins-1);

  const int runEntries = inputRun.GetEntries();
  for(int i=0;i<runEntries;i++){
    inputRun.GetEntry(i);

    double firstHitTime = std::numeric_limits<double>::max();

    const auto nTracks = g4Event->GetNumberOfTracks();
    if(nTracks<=0)continue;
    for (unsigned int trackIndex = 0; trackIndex < nTracks; trackIndex++) {
      const auto trackG4 = g4Event->GetTrackPointer(trackIndex);

      const auto hits = trackG4->GetHits();
      const auto nHits = hits.GetNumberOfHits();
        
      //cout<<particleName<<" "<<nHits<<endl;
      
      if(nHits<=0)continue;
        for (unsigned int n = 0; n < nHits; n++) {
          const double energy = hits.GetEnergy(n);
          const double hitTime = hits.GetTime(n);
          const string volumeName = hits.GetVolumeName(i).Data();
          const auto volumeId = G4Metadata->GetActiveVolumeID(volumeName);
          if (std::find(sensitiveVolumeID.begin(), sensitiveVolumeID.end(), volumeId) == sensitiveVolumeID.end())continue;
          if (energy <= 0)continue;
          if(hitTime < firstHitTime)firstHitTime = hitTime;
        }
      }

    int triggerOffset =timeOffset/timeResolution;

    double totalPhotons =0;
    std::map<int, double> photons;
    std::map<int, std::vector<int> > rawSignalMap;
    std::map<int, double> totalEDepG4;
    std::map<int, double> thresholdIntegral;
    std::map<int, double> amplitude;
    std::map<int, double> averageTime;
    std::map<int, double> risetime;
    std::map<int, double> baseline;
    std::map<int, double> baselineSigma;
    std::map<int, double> width;

    double initialEnergyG4 = g4Event->GetPrimaryEventEnergy();

    for (unsigned int trackIndex = 0; trackIndex < nTracks; trackIndex++) {
      const auto trackG4 = g4Event->GetTrackPointer(trackIndex);
      const std::string particleName = trackG4->GetParticleName().Data();
      const auto hits = trackG4->GetHits();
      const auto nHits = hits.GetNumberOfHits();
        for (unsigned int n = 0; n < nHits; n++) {
          const string volumeName = hits.GetVolumeName(i).Data();
          const auto volumeId = G4Metadata->GetActiveVolumeID(volumeName);
          if (std::find(sensitiveVolumeID.begin(), sensitiveVolumeID.end(), volumeId) == sensitiveVolumeID.end())continue;
          const double energy = hits.GetEnergy(n);
          if (energy <= 0)continue;
          const TVector3& position = hits.GetPosition(n);
          const double hitTime = hits.GetTime(n);
          const int nPhotons = std::round(energy*photoElectronsPerkeV);
          //cout<<volumeId<<" "<<energy<<" "<<nPhotons<<endl;
          if(nPhotons<=1)continue;
          const double diffTime = hitTime - firstHitTime;
          int timeBin = std::round(diffTime/timeResolution + triggerOffset);
             if(timeBin< 0 || timeBin >= nTimeBins){
               std::cout<<"Warning time bin is "<<timeBin<<" but maximum bin is "<<nTimeBins<<" "<< firstHitTime<<" "<<hitTime<<" " << diffTime <<endl;
               continue;
             }
          auto rS = rawSignalMap.find(volumeId);
          if(rS == rawSignalMap.end())rawSignalMap[volumeId] = std::vector<int>(nTimeBins, 0);

          rawSignalMap[volumeId][timeBin] +=nPhotons;
          if(totalEDepG4.find(volumeId) == totalEDepG4.end()){
            totalEDepG4[volumeId] = energy;
            photons[volumeId] = nPhotons;
          } else {
            totalEDepG4[volumeId] += energy;
            photons[volumeId] += nPhotons;
          }
          totalPhotons +=nPhotons;
         }
        }

        if(totalPhotons<=0)continue;

        rawSignalEvent.Initialize();
        rawSignalEvent.SetID(g4Event->GetID());
        rawSignalEvent.SetSubID(g4Event->GetSubID());
        rawSignalEvent.SetTimeStamp(g4Event->GetTimeStamp());
        rawSignalEvent.SetSubEventTag(g4Event->GetSubEventTag());
        double res = 1./TMath::Sqrt(totalPhotons);
        TRandom3 smear;
        TRandom3 noise;
        const double smearFactor = smear.Gaus(1.0, res);
          for(const auto &[signalID, data] : rawSignalMap){
              for(auto val : data){
                val = std::round(smearFactor*val);
                val += noise.Gaus(0, noiseLevel);
              }
            auto shapedData = GetSignalAfterShaping(data);
            TRestRawSignal rawSignal;
            rawSignal.SetSignalID(signalID);
              for(const auto& val : shapedData){
                int ADC = val+noise.Gaus(0, noiseLevel) + ADCOffset;
                if(ADC<0 || ADC >std::numeric_limits<short>::max()) ADC =std::numeric_limits<short>::max();
                rawSignal.AddPoint((Short_t)ADC);
              }
            rawSignalEvent.AddSignal(rawSignal);
          }


        rawSignalEvent.SetBaseLineRange(baselineRange);
        rawSignalEvent.SetRange(integralRange);

        const int nSignals = rawSignalEvent.GetNumberOfSignals();
        if(nSignals<=0)continue;

          for (int s = 0; s < nSignals; s++) {
            TRestRawSignal* sgnl = rawSignalEvent.GetSignal(s);
            const int sID = sgnl->GetSignalID();
            sgnl->InitializePointsOverThreshold(TVector2(2., 1.),2);
            double integral = sgnl->GetThresholdIntegral();
            risetime[sID] = sgnl->GetRiseTime();
            width[sID] = sgnl->GetMaxPeakWidth();
            baseline[sID] = sgnl->GetBaseLine();
            baselineSigma[sID] = sgnl->GetBaseLineSigma();
            double amp = sgnl->GetMaxValue();
            thresholdIntegral[sID] = integral;
            amplitude[sID]= amp;
          }

        outRun.GetAnalysisTree()->SetObservableValue("thresholdIntegral", thresholdIntegral);
        outRun.GetAnalysisTree()->SetObservableValue("totalAmplitude", amplitude);
        outRun.GetAnalysisTree()->SetObservableValue("risetime", risetime);
        outRun.GetAnalysisTree()->SetObservableValue("width", width);
        outRun.GetAnalysisTree()->SetObservableValue("baseline", baseline);
        outRun.GetAnalysisTree()->SetObservableValue("baselineSigma", baselineSigma);
        outRun.GetAnalysisTree()->SetObservableValue("totalEDepG4",totalEDepG4);
        outRun.GetAnalysisTree()->SetObservableValue("initialEnergyG4",initialEnergyG4);

        outRun.GetAnalysisTree()->SetEventInfo(&rawSignalEvent);
        outRun.GetEventTree()->Fill();
        outRun.GetAnalysisTree()->Fill();

        if(i%1000==0)std::cout<<"Processed "<<i<<" events out of "<< runEntries<<endl;
  }

  outRun.SetEndTimeStamp(inputRun.GetEndTimestamp());
  outRun.UpdateOutputFile();
  G4Metadata->Write(G4Metadata->GetName());
  outRun.CloseFile();

}


}

