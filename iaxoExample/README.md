# IAXO MICROMEGAS GEOMETRY

Only simulate the chamber: box square with cilynder hole with spider-web pattern cathode.

See dimensions below:
<img width="2509" height="1470" alt="Dimensiones simplificadas Cámara IAXO D1" src="https://github.com/user-attachments/assets/c1245384-845b-4873-8df2-0be7f65307a5" />

Cathode geometry example is at [geometry.gdml](iaxoExample/geometry.gdml), only cathode geometry is implemented, follow the sketch above for the dimensions of the chamber

The readout (copper) should be placed at  position (0,0,-15)mm, this is required because of the readout geometry implemented in the analysis

# GASES
From materials.xml:
- Argon2%Isobutane1.4bar
- XenonNeon2.3%Isobutane1.05bar

# Analysis config file [analysis.rml](iaxoExample/analysis.rml)

Note that the different gases has to be updated for the different gas simulations
```
<!--TRestDetectorGas name="Xenon-Neon 48.85Pct-Iso 2.3Pct 10-10E3Vcm" pressure="1.05" file="server"/-->
<TRestDetectorGas name="Argon-Isobutane 2.3Pct 10-10E3Vcm" pressure="1.4" file="server" />
```
To run the analysis `restManager --c analysis.rml --i RunXXXXX_restG4_xxxxx_xxxxx.root` or equivalent scripting.

For quantum efficiency studies use only central hole in the cathode (e.g. use TRestDataSet with cuts in position observables, see [summaryPlot.rml](iaxoExample/summaryPlot.rml) for more details)

For energy after full reconstruction use `tckAna_MaxTrackEnergy` observable to determine the efficiency, you can use other g4ana observables to determine initial direction and position of the gamma ray.


