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
/// \file B4/B4c/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the B4::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "stdlib.h"
#include "ctime"

namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  G4int nofParticles = 1;
  fParticleGun = new G4ParticleGun(nofParticles);

  // default particle kinematic
  //
  auto particleDefinition
    = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
  fParticleGun->SetParticleDefinition(particleDefinition);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
//  fParticleGun->SetParticleEnergy(18.0*GeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // This function is called at the begining of event

  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get world volume
  // from G4LogicalVolumeStore
  //
 
  G4double worldXHalfLength = 0.;
  G4double worldYHalfLength = 0.;
  G4double worldZHalfLength = 0.;
  auto worldLV = G4LogicalVolumeStore::GetInstance()->GetVolume("World");

  // Check that the world volume has box shape
  G4Box* worldBox = nullptr;
  if (  worldLV ) {
    worldBox = dynamic_cast<G4Box*>(worldLV->GetSolid());
  

  if ( worldBox ) {
    worldXHalfLength = worldBox->GetXHalfLength();
    worldYHalfLength = worldBox->GetYHalfLength();
    worldZHalfLength = worldBox->GetZHalfLength();
  }
  else  {
    G4ExceptionDescription msg;
    msg << "World volume of box shape not found." << G4endl;
    msg << "Perhaps you have changed geometry." << G4endl;
    msg << "The gun will be place in the center.";
    G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
      "MyCode0002", JustWarning, msg);
  }

  // Set gun position
  // Define radius of circle 

  G4double gaussianNumber1 = G4RandGauss::shoot(0.0, 0.026);
  G4double gaussianNumber2 = G4RandGauss::shoot(0.0, 0.026);

  G4double thetax = gaussianNumber1 * deg; //angle theta 
  G4double thetay = gaussianNumber2 * deg;

  //G4cout<<"thetax"<<thetax<<G4endl;
  //G4cout<<"thetay"<<thetay<<G4endl;

  G4double beamX = 45 * deg * thetax;
  G4double beamY = 45 * deg * thetay;

  // x-vector, y-vector, z-vector
  G4double xcomponent = std::sin(thetax) * std::cos(thetay);
  G4double ycomponent = std::sin(thetay);
  G4double zcomponent = std::cos(thetax) * std::cos(thetay);
 
  //G4cout<<"xcomponent"<<xcomponent<<G4endl;
  //G4cout<<"ycomponent"<<ycomponent<<G4endl;
  //G4cout<<"zcomponent"<<zcomponent<<G4endl;

  G4ThreeVector beamVector(xcomponent, ycomponent, zcomponent); 

  G4ThreeVector beamPosition(beamX, beamY, -worldXHalfLength); 
  fParticleGun->SetParticlePosition((beamPosition));
  fParticleGun->SetParticleMomentumDirection(beamVector);

  srand(time(0));
  G4double beamE = (rand() % 41 + 20) * GeV;
  //G4double beamE = 10 * GeV;
  //G4cout<<beamE/GeV<<"GeV"<<G4endl;
  fParticleGun->SetParticleEnergy(beamE);
  fParticleGun->GeneratePrimaryVertex(anEvent);

}

}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.......