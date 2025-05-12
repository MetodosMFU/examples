void getEfficiency (const std::string &filePattern){

const std::map<double, double> bRMap = {//Branching ratio
  {1460, 0.107}, //40K
  {238.63, 0.433}, //232Th series
  {727.18, 0.0658},
  {583.19, 0.02},
  {860.56, 0.1242},
  {2614.53, 0.026},
  {383.32, 0.1127},
  {911.02, 0.258},
  {968.97, 0.158}
  //Add as many as needed e.g. 238U series, 152Eu, and more...
};

struct events {int launchedEvents; int simEntries; int peakEvents; };

const int norm = 100000;

std::map<double, events> eventMap;

auto spectra = new TH1D("Spectra", "Spectra", 1024, 0, 8192);

auto files = TRestTools::GetFilesMatchingPattern(filePattern);
  for (const auto &inputFile : files){

  cout<<"Analyzing file "<<inputFile<<endl;
  
  TRestRun inputRun(inputFile);
  TRestGeant4Metadata* G4Metadata = static_cast<TRestGeant4Metadata*>(inputRun.GetMetadataClass("TRestGeant4Metadata"));
  const int launchedEvents = G4Metadata->GetNumberOfEvents();
  const double eInit =  G4Metadata->GetParticleSource(0)->GetEnergy();

  const int runEntries = inputRun.GetEntries();

  ROOT::RDataFrame df("AnalysisTree", inputFile);
  auto hist=df.Histo1D({"histo","histo",1024, 0, 8192},"totalAmplitude");

  auto it = bRMap.find(eInit);
    double br = 1;
    if(it ==  bRMap.end() ){
      cout<<"WARNING!!! Branching ratio not found for energy "<<eInit<<" keV assuming BR of "<<br<<endl;
    } else{
      br = it->second; 
    }
  spectra->Add(hist->DrawCopy(),br*norm/launchedEvents);

  auto peakEvents = df.Define("ERatio","totalEDepG4/initialEnergyG4").Filter("ERatio>0.95").Count();

  events ev ={launchedEvents, runEntries, (int)*peakEvents};
  
  auto it2 = eventMap.find(eInit);
    if(it2 ==  eventMap.end() ){
      eventMap[eInit] = ev;
    } else {
      it2->second.launchedEvents += launchedEvents;
      it2->second.simEntries += runEntries;
      it2->second.peakEvents += (int)*peakEvents;
    }

  }

auto gr = new TGraphErrors();

int c=0;
  for(const auto& [en, events] : eventMap){
    cout<<"Energy "<<en<<" keV: "<<" Launched events: "<<events.launchedEvents<<"; detected events: "<<events.simEntries<<"; peak events: "<<events.peakEvents<<endl;
    cout<<"Geometry efficiency: "<<double(events.simEntries)/events.launchedEvents<<"+/-"<<sqrt(events.simEntries)/events.launchedEvents<<"; Peak efficiency "<<double(events.peakEvents)/events.simEntries<<"+/-"<<sqrt(events.peakEvents)/events.simEntries<<"; Total efficiency :"<<double(events.peakEvents)/events.launchedEvents<<"+/-"<<sqrt(events.peakEvents)/events.launchedEvents<<endl;
    gr->SetPoint(c,en,double(events.peakEvents)/events.simEntries);
    gr->SetPointError(c,0,sqrt(events.peakEvents)/events.simEntries);
    c++;
  }

TCanvas *effCan = new TCanvas("effCan","effCan");
gr->GetXaxis()->SetTitle("Energy (keV)");
gr->GetYaxis()->SetTitle("Peak Efficiency");
gr->Draw();

TCanvas *spcCan = new TCanvas("spcCan","spcCan");
spectra->GetXaxis()->SetTitle("Amplitude (ADC)");
spectra->GetYaxis()->SetTitle("Counts");
spectra->Draw();

}

