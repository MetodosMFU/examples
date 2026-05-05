# Cosmic generator example
Root file at [cosmics.root](generatorExample/cosmics.root) uses CRY to generate differential spectra, note units in `y` axis are counts/s/cm^2
<img width="789" height="542" alt="image" src="https://github.com/user-attachments/assets/f4fccc71-b4b4-4d72-8e18-7cde9d2640b0" />

Generator examples are provided for muons, protons and gammas.
```
       <if condition="${GENERATOR}==CosmicMuons">
           <generator type="cosmic">
            <source use="Cosmics" direction="(0,-1,0)" filename="cosmics.root"
                    particles="muon_plus,muon_minus"/>
        </generator>
        </if>
        <if condition="${GENERATOR}==CosmicProtons">
        <generator type="cosmic">
            <source use="Cosmics" direction="(0,-1,0)" filename="../cosmics.root"
                    particles="proton"/>
        </generator>
        </if>
        <if condition="${GENERATOR}==CosmicGammas">
        <generator type="cosmic">
            <source use="Cosmics" direction="(0,-1,0)" filename="../cosmics.root"
                    particles="gamma" energy="(1,10000)MeV"/>
        </generator>
        </if>
```

Note add as many sensitive volume as you need:
```
        <detector>
            <parameter name="energyRange" value="(1,1000000)keV"/>
            <volume name="veto1" sensitive="true" chance="1" maxStepSize="0.1mm"/>
            <volume name="veto2" sensitive="true" chance="1" maxStepSize="0.1mm"/>
            ...
        </detector>
```


# Analysis [Geant4ToSignal.C](generatorExample/Geant4ToSignal.C)

Note that now we have one signal per veto, this has been taken into account for the analysis. Update this field with the volume names used in the simulation:
```
const std::vector<std::string> sensitiveVolume={"veto1","veto2"};//Add as many as you want
```
Now you have one signal per veto, signalID is the volume ID:
<img width="511" height="320" alt="image" src="https://github.com/user-attachments/assets/20b0fb62-9883-40a8-8471-7f94992b77e9" />

Observables from the signal analysis are stored as a `std::map<int, value>`, where the first index is the signalID (volumeID), to get the info from the different observables check the macro [getObservable.C](generatorExample/getObservable.C), which provides examples on how to access the data using `TRestRun` and `RDataFrame`

Check generator info to obtain the surface of the generator used in the simulation (it should be always the same for a given world size), you should use this value to normalize the muon rate together with the differential spectra at [cosmics.root](generatorExample/cosmics.root).
```
RestGeant4Metadata* G4Metadata = static_cast<TRestGeant4Metadata*>(inputRun.GetMetadataClass("TRestGeant4Metadata"));
  std::cout<<"Generator surface "<<G4Metadata->GetGeant4PrimaryGeneratorInfo().GetSpatialGeneratorCosmicSurfaceTermCm2()<<" cm^2"<<endl;
```

