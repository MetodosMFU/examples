void getObservable(const std::string &fileName, const std::string &obsString){

TRestRun inputRun(fileName);

TRestAnalysisTree* anaTree = inputRun.GetAnalysisTree();

std::cout<<"Entries "<<anaTree->GetEntries()<<std::endl;
   for(int i=0;i<inputRun.GetEntries();i++){
      inputRun.GetEntry(i);
      std::map<int, double> obsVal;
      obsVal.clear();
      auto obsID = anaTree->GetObservableID(obsString);
      auto a = anaTree->GetObservable(obsID);
      a >> obsVal;
      
      for (const auto& [id, value] : obsVal)cout<<i<<" "<<id<<" "<<value<<endl;

  }

}

void getObservableRDataFrame(const std::string &fileName, const std::string &obsString){

ROOT::RDataFrame df("AnalysisTree", fileName);

auto df_with_val = df.Define("amp_ch0", [](const std::map<int, double>& m) {
    if (m.count(0)) return m.at(0);
    return 0.0;
}, {obsString});

df_with_val = df_with_val.Define("amp_ch1", [](const std::map<int, double>& m) {
    if (m.count(1)) return m.at(1);
    return 0.0;
}, {obsString});

auto h = df_with_val.Histo1D("amp_ch0");
h->DrawClone();

auto h1 = df_with_val.Histo1D("amp_ch1");
h1->DrawClone("SAME");

}

