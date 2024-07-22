//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B4/B4c/src/DetectorConstruction.cc
/// \brief Implementation of the B4c::DetectorConstruction class

#include "DetectorConstruction.hh"
#include "CalorimeterSD.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"
#include <G4Tubs.hh>
#include <G4SubtractionSolid.hh>

#include "G4SDManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

namespace B4c
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal
G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // Lead material defined using NIST Manager
  auto nistManagerPb = G4NistManager::Instance();
  nistManagerPb->FindOrBuildMaterial("G4_Pb");

  // Liquid argon material
  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;
  G4double density;
  new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density= 1.390*g/cm3);
         // The argon by NIST Manager is a gas with a different density

  // Vacuum
  new G4Material("Galactic", z=1., a=1.01*g/mole, density= universe_mean_density, kStateGas, 2.73*kelvin, 3.e-18*pascal);

  // Tungsten Powder
  new G4Material("tungstenPowder", z=74., a=183.84*g/mole, density= 11.25*g/cm3);

 // Tungsten Powder with epoxy
  new G4Material("tungPowderEpoxy", z=74., a=178.94*g/mole, density= 10.95*g/cm3);

  // Tungsten
  auto nistManagerW = G4NistManager::Instance();
  nistManagerW->FindOrBuildMaterial("G4_W");

  // Plastic Scintillator
  auto nistManagerPs = G4NistManager::Instance();
  nistManagerPs->FindOrBuildMaterial("G4_POLYSTYRENE");

  // Brass
  auto nistManagerBr = G4NistManager::Instance();
  nistManagerBr->FindOrBuildMaterial("G4_BRASS");

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  // Geometry parameters
  G4double absoThickness = 1.0*mm; //must divide evenly into 3mm - siPM size
  G4double gapThickness =  0.25*mm;
  G4double guideThickness = 0.75*mm;
  G4double layerThickness = 9.0*mm;

  G4double tungstenPlateThickness = 5*mm;

  G4double calorSizeXY  = 18.0*cm;
  G4double calorThickness = 18.0*cm;

  G4double worldSizeXY = 1.2 * calorSizeXY;
  G4double worldSizeZ  = 1.5 * calorThickness;

  fNofLayers = (2*(calorSizeXY/absoThickness)*(calorSizeXY/absoThickness));

  // Get materials
  auto defaultMaterial = G4Material::GetMaterial("Galactic");
  auto absorberMaterial = G4Material::GetMaterial("tungPowderEpoxy");
  auto gapMaterial = G4Material::GetMaterial("G4_POLYSTYRENE");
  auto guideMaterial = G4Material::GetMaterial("G4_BRASS");
  auto tungsten = G4Material::GetMaterial("G4_W");

  if ( ! defaultMaterial || ! absorberMaterial || ! gapMaterial ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined.";
    G4Exception("DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }

  //
  // 90 degree rotation matrices
  //

  G4RotationMatrix* rotationMatrixX = new G4RotationMatrix();
  rotationMatrixX->rotateX(90.*deg);

  G4RotationMatrix* rotationMatrixY = new G4RotationMatrix();
  rotationMatrixY->rotateY(90.*deg);

  G4RotationMatrix* rotationMatrixZ = new G4RotationMatrix();
  rotationMatrixZ->rotateZ(90.*deg);

  //
  // World
  //
  auto worldS
    = new G4Box("World",           // its name
                 worldSizeXY/2, worldSizeXY/2, worldSizeZ/2); // its size

  auto worldLV
    = new G4LogicalVolume(
                 worldS,           // its solid
                 defaultMaterial,  // its material
                 "World");         // its name

  auto worldPV = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),                         // at (0,0,0)
    worldLV,                                 // its logical volume
    "World",                                 // its name
    nullptr,                                 // its mother  volume
    false,                                   // no boolean operation
    0,                                       // copy number
    fCheckOverlaps);                         // checking overlaps

  /*

  //
  // Placing a tungsten plate in front of the calorimeter
  //
  
  G4Box* tungstenPlateS = new G4Box("tungstenplateS", calorSizeXY/2, calorSizeXY/2, tungstenPlateThickness/2);
  
  auto tungstenPlateLV
    = new G4LogicalVolume(
                 tungstenPlateS,           // its solid
                 tungsten,  // its material
                 "TungstenPlate");         // its name


  new G4PVPlacement(nullptr,                   // no rotation
    G4ThreeVector(0., 0., -(calorThickness + tungstenPlateThickness)/2),                  // its position
    tungstenPlateLV,                                // its logical volume
    "TungstenPlate",                                    // its name
    worldLV,                              // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    fCheckOverlaps); 
  
  */
  //
  // Calorimeter
  //
  auto calorimeterS
    = new G4Box("Calorimeter",     // its name
                 calorSizeXY/2, calorSizeXY/2, calorThickness/2); // its size

  auto calorLV
    = new G4LogicalVolume(
                 calorimeterS,     // its solid
                 defaultMaterial,  // its material
                 "Calorimeter");   // its name

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),          // at (0,0,0)
    calorLV,                  // its logical volume
    "Calorimeter",            // its name
    worldLV,                  // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    fCheckOverlaps);          // checking overlaps


  //
  // Building the Spaghetti Calorimeter
  //

  G4Box* tungBoxS = new G4Box( "tungstenS" , absoThickness/2, absoThickness/2,((calorSizeXY - 4*guideThickness)/3)/2);
  G4VSolid* gapS = new G4Tubs("gapS", 0., gapThickness*mm, calorSizeXY/2, 0., 2*M_PI*rad);
  G4Box* brassBoxS = new G4Box( "brassS" , absoThickness/2, absoThickness/2, guideThickness/2);
  
  G4VSolid* absorberS = new G4SubtractionSolid("sheathS", tungBoxS, gapS, 0, G4ThreeVector(0.,0.,0.));
  G4VSolid* guideS = new G4SubtractionSolid("guideS", brassBoxS, gapS, 0, G4ThreeVector(0.,0.,0.));

  auto moduleS = new G4Box("Module", absoThickness/2, calorSizeXY/2, absoThickness/2);

  auto layerXS = new G4Box("LayerX", absoThickness/2, calorSizeXY/2, layerThickness/2);

  auto layerYS = new G4Box("LayerY", calorSizeXY/2, calorSizeXY/2, layerThickness/2); 

  auto layerS = new G4Box("Layer", calorSizeXY/2, calorSizeXY/2, (2*layerThickness)/2);

  auto layerLV
    = new G4LogicalVolume(
                 layerS,           // its solid
                 defaultMaterial,  // its material
                 "Layer");         // its name


  new G4PVReplica(
                 "Layer",          // its name
                 layerLV,          // its logical volume
                 calorLV,          // its mother
                 kZAxis,           // axis of replication
                 (calorThickness/(layerThickness*2)),        // number of replica
                 layerThickness*2);  // witdth of replic

  auto layerYLV
    = new G4LogicalVolume(
                 layerYS,           // its solid
                 defaultMaterial,  // its material
                 "LayerY");         // its name

  new G4PVPlacement(nullptr,                   // no rotation
    G4ThreeVector(0., 0., layerThickness/2),                  // its position
    layerYLV,                                // its logical volume
    "LayerY",                                    // its name
    layerLV,                              // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    fCheckOverlaps);  

  new G4PVPlacement(rotationMatrixZ,                   // rotation
    G4ThreeVector(0., 0., -layerThickness/2),                  // its position
    layerYLV,                                // its logical volume
    "LayerY",                                    // its name
    layerLV,                              // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    fCheckOverlaps);  


  auto layerXLV
    = new G4LogicalVolume(
                 layerXS,           // its solid
                 defaultMaterial,  // its material
                 "layerX");         // its name

   new G4PVReplica(
                 "LayerX",          // its name
                 layerXLV,          // its logical volume
                 layerYLV,          // its mother
                 kXAxis,           // axis of replication
                 (calorSizeXY/absoThickness),        // number of replica
                 absoThickness);  // witdth of replica

  
  //
  //// Module - module is replicated across the width of the layer in Z
  //


  auto moduleLV
    = new G4LogicalVolume(
                 moduleS,           // its solid
                 defaultMaterial,  // its material
                 "module");         // its name
  
  new G4PVReplica(
                 "Module",          // its name
                 moduleLV,          // its logical volume
                 layerXLV,          // its mother
                 kZAxis,           // axis of replication
                 (layerThickness/absoThickness),        // number of replica
                 absoThickness);  // witdth of replica


  //
  //// Absorber - 3 absorbers placed along the fibre, seperated by brass guides
  //


  auto absorberLV
    = new G4LogicalVolume(
                 absorberS,        // its solid
                 absorberMaterial, // its material
                 "AbsoLV");        // its name

  new G4PVPlacement(rotationMatrixX,                   // no rotation
    G4ThreeVector(0., (calorSizeXY-guideThickness)/3, 0),                  // its position
    absorberLV,                                // its logical volume
    "Absorber1",                                    // its name
    moduleLV,                              // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    fCheckOverlaps);  

  new G4PVPlacement(rotationMatrixX,                   // no rotation
    G4ThreeVector(0., -(calorSizeXY-guideThickness)/3, 0.),                  // its position
    absorberLV,                                // its logical volume
    "Absorber3",                                    // its name
    moduleLV,                              // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    fCheckOverlaps);  


  new G4PVPlacement(rotationMatrixX,                   // no rotation
    G4ThreeVector(0., 0., 0),                  // its position
    absorberLV,                                // its logical volume
    "Absorber2",                                    // its name
    moduleLV,                              // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    fCheckOverlaps);  
  
  //
  //// Gap/Fibre - 1 fibre placed at the centre of the module
  //


  auto gapLV
    = new G4LogicalVolume(
                 gapS,             // its solid
                 gapMaterial,      // its material
                 "GapLV");         // its name

   new G4PVPlacement(rotationMatrixX,                   // no rotation
    G4ThreeVector(0., 0., 0),  // its position
    gapLV,                                     // its logical volume
    "Gap1",                                     // its name
    moduleLV,                                   // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    fCheckOverlaps);       

  //
  //// Guides - 4 brass guides places evenly though the module
  //

  auto guideLV
    = new G4LogicalVolume(
                 guideS,             // its solid
                 guideMaterial,      // its material
                 "GuideLV");         // its name

   new G4PVPlacement(rotationMatrixX,                   // no rotation
    G4ThreeVector(0., (calorSizeXY - guideThickness)/2, 0.),  // its position
    guideLV,                                     // its logical volume
    "Guide1",                                     // its name
    moduleLV,                                   // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    fCheckOverlaps);    

    new G4PVPlacement(rotationMatrixX,                   // no rotation
    G4ThreeVector(0., (((calorSizeXY - 4*guideThickness)/3) + guideThickness)/2, 0.),  // its position
    guideLV,                                     // its logical volume
    "Guide2",                                     // its name
    moduleLV,                                   // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    fCheckOverlaps);   

    new G4PVPlacement(rotationMatrixX,                   // no rotation
    G4ThreeVector(0., -(((calorSizeXY - 4*guideThickness)/3) + guideThickness)/2, 0.),  // its position
    guideLV,                                     // its logical volume
    "Guide3",                                     // its name
    moduleLV,                                   // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    fCheckOverlaps);  

    new G4PVPlacement(rotationMatrixX,                   // no rotation
    G4ThreeVector(0., -(calorSizeXY - guideThickness)/2, 0.),  // its position
    guideLV,                                     // its logical volume
    "Guide4",                                     // its name
    moduleLV,                                   // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    fCheckOverlaps); 



  //
  // print parameters
  //
  G4cout
    << G4endl
    << "------------------------------------------------------------" << G4endl
    << "---> The calorimeter is " << fNofLayers << " layers of: [ "
    << absoThickness/mm << "mm of " << absorberMaterial->GetName()
    << " + "
    << gapThickness/mm << "mm of " << gapMaterial->GetName() << " ] " << G4endl
    << "------------------------------------------------------------" << G4endl;

  //
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());

  auto simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  simpleBoxVisAtt->SetVisibility(true);
  calorLV->SetVisAttributes(simpleBoxVisAtt);

  //
  // Always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  // G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  //
  // Sensitive detectors
  //

  auto absoSD
    = new CalorimeterSD("AbsorberSD", "AbsorberHitsCollection", fNofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(absoSD);
  SetSensitiveDetector("AbsoLV",absoSD);

  auto gapSD
    = new CalorimeterSD("GapSD", "GapHitsCollection", fNofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(gapSD);
  SetSensitiveDetector("GapLV",gapSD);

  //
  // Magnetic field
  //
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue;
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
