void viewGeometry(const std::string &geomFile="setup.gdml"){
  TGeoManager *geo = TGeoManager::Import(geomFile.c_str());
    if(geo){
      geo->GetTopVolume()->Draw("ogl");
      geo->CheckOverlaps(0.001);
    }
}
