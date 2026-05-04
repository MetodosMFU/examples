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


# Analysis TBA
