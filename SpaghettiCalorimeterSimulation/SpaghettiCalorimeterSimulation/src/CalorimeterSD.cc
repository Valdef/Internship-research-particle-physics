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
/// \file B4/B4c/src/CalorimeterSD.cc
/// \brief Implementation of the B4c::CalorimeterSD class

#include "CalorimeterSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

namespace B4c
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CalorimeterSD::CalorimeterSD(const G4String& name,
                             const G4String& hitsCollectionName,
                             G4int nofCells)
 : G4VSensitiveDetector(name),
   fNofCells(nofCells)
{
  collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CalorimeterSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection
  fHitsCollection
    = new CalorHitsCollection(SensitiveDetectorName, collectionName[0]);

  // Add this collection in hce
  auto hcID
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection );

  // Create hits
  // fNofCells for cells + one more for total sums
  for (G4int i=0; i<fNofCells+1; i++ ) {
    fHitsCollection->insert(new CalorHit());
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool CalorimeterSD::ProcessHits(G4Step* step,
                                     G4TouchableHistory*)
{
  // energy deposit
  auto edep = step->GetTotalEnergyDeposit() * MeV;
  
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  auto eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  auto incomingX = G4RunManager::GetRunManager()->GetCurrentEvent()->GetPrimaryVertex()->GetX0();
  auto incomingY = G4RunManager::GetRunManager()->GetCurrentEvent()->GetPrimaryVertex()->GetY0();

  auto beamE = G4RunManager::GetRunManager()->GetCurrentEvent()->GetPrimaryVertex()->GetPrimary()->GetPz()/CLHEP::GeV;
  
  //G4cout << "eventID = " << eventID << "beamX = " << incomingX << "beamY = " << incomingY << G4endl;

  //G4cout << "I got here!!! CalorimeterSD1" << G4endl;


  const G4ThreeVector p1 = step->GetPreStepPoint()->GetPosition();
  const G4ThreeVector p2 = step->GetPostStepPoint()->GetPosition();

  const G4String& material1 = step->GetPreStepPoint()->GetMaterial()->GetName();
  const G4String& material2 = step->GetPostStepPoint()->GetMaterial()->GetName();

  const G4int replicanumber0 = step->GetPreStepPoint()->GetTouchable()->GetReplicaNumber(0);
  const G4int replicanumber1 = step->GetPreStepPoint()->GetTouchable()->GetReplicaNumber(1);
  const G4int replicanumber2 = step->GetPreStepPoint()->GetTouchable()->GetReplicaNumber(2);
  const G4int replicanumber3 = step->GetPreStepPoint()->GetTouchable()->GetReplicaNumber(3);
  const G4int replicanumber4 = step->GetPreStepPoint()->GetTouchable()->GetReplicaNumber(4);


  //G4cout << replicanumber1 << ", " << replicanumber3 << G4endl;


  const G4double x1 = p1.x();
  const G4double y1 = p1.y();
  const G4double z1 = p1.z();
    
  const G4double x2 = p2.x();
  const G4double y2 = p2.y();
  const G4double z2 = p2.z();

  const G4double x0 = 0.5*(x1 + x2);
  const G4double y0 = 0.5*(y1 + y2);
  const G4double z0 = 0.5*(z1 + z2);

  G4double zBase = (z0 + 90.0) - (18 * replicanumber4); //Value of z within each group of X and Y layers, width of two layers

  if (0.01 < edep)
  {

  analysisManager->FillNtupleDColumn(1, 0, edep);
  analysisManager->FillNtupleDColumn(1, 1, x0);
  analysisManager->FillNtupleDColumn(1, 2, y0);
  analysisManager->FillNtupleDColumn(1, 3, z0);
  analysisManager->AddNtupleRow(1);


  if (material1 == "G4_POLYSTYRENE" && material2 == "G4_POLYSTYRENE") {
    analysisManager->FillNtupleDColumn(2, 0, eventID);
    analysisManager->FillNtupleDColumn(2, 1, beamE);
    analysisManager->FillNtupleDColumn(2, 2, incomingX);
    analysisManager->FillNtupleDColumn(2, 3, incomingY);
    analysisManager->FillNtupleDColumn(2, 4, edep);
    analysisManager->FillNtupleDColumn(2, 5, x0);
    analysisManager->FillNtupleDColumn(2, 6, y0);
    analysisManager->FillNtupleDColumn(2, 7, z0);


    //G4cout << "Replica Numbers (0-4): " << replicanumber0 << " " << replicanumber1 << " " << replicanumber2 << " " << replicanumber3 << " " << replicanumber4 << " " << G4endl;

    if (zBase > 9) {

    analysisManager->FillNtupleDColumn(2, 8, std::floor(replicanumber2/3));
    analysisManager->FillNtupleDColumn(2, 9, -50.0);
    analysisManager->FillNtupleDColumn(2, 10, std::floor(replicanumber1/3) + 3*(2*replicanumber4+1));
    }
    else {

    analysisManager->FillNtupleDColumn(2, 8, -50.0);
    analysisManager->FillNtupleDColumn(2, 9, std::floor((179 - replicanumber2)/3)); // Number of fibres in X or Y -1
    analysisManager->FillNtupleDColumn(2, 10, std::floor(replicanumber1/3) + 3*(2*replicanumber4));
    }

    analysisManager->AddNtupleRow(2);

  /*
    G4int siPM_ID = int( 360*replicanumber4 + std::floor(replicanumber1/3) * 60 + std::floor(replicanumber2/3));
    analysisManager->FillNtupleDColumn(3, siPM_ID, edep);
  */
  }

  }

  // step length
  G4double stepLength = 0.;
  if ( step->GetTrack()->GetDefinition()->GetPDGCharge() != 0. ) {
    stepLength = step->GetStepLength();
  }

  if ( edep==0. && stepLength == 0. ) return false;

  auto touchable = (step->GetPreStepPoint()->GetTouchable());

  // Get calorimeter cell id
  auto sumCopyNumber = 0;

  if (zBase > 9) {

    G4double xyCopyNumber = std::floor(replicanumber2/3);
    G4double zCopyNumber = (2*replicanumber4+1);
    auto sumCopyNumber = xyCopyNumber + 60*zCopyNumber;
  }
  else {

    G4double xyCopyNumber =  std::floor((179 - replicanumber2)/3); // Number of fibres in X or Y -1
    G4double zCopyNumber = (2*replicanumber4);
    auto sumCopyNumber = xyCopyNumber + 60*zCopyNumber;
  }

  // Get hit accounting data for this cell
  auto hit = (*fHitsCollection)[sumCopyNumber];
  if ( ! hit ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hit " << sumCopyNumber;
    G4Exception("CalorimeterSD::ProcessHits()",
      "MyCode0004", FatalException, msg);
  }

  // Get hit for total accounting
  auto hitTotal
    = (*fHitsCollection)[fHitsCollection->entries()-1];

  // Add values
  hit->Add(edep, stepLength);
  hitTotal->Add(edep, stepLength);

  //G4cout << "I got here!!! CalorimeterSD2" << G4endl;

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CalorimeterSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) {
     auto nofHits = fHitsCollection->entries();
     G4cout
       << G4endl
       << "-------->Hits Collection: in this event they are " << nofHits
       << " hits in the tracker chambers: " << G4endl;
     for ( std::size_t i=0; i<nofHits; ++i ) (*fHitsCollection)[i]->Print();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
