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
/// \file B4/B4c/src/RunAction.cc
/// \brief Implementation of the B4::RunAction class

#include "RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction()
{
  // set printing event number per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);

  // Create analysis manager
  // The choice of the output format is done via the specified
  // file extension.
  auto analysisManager = G4AnalysisManager::Instance();

  // Create directories
  //analysisManager->SetHistoDirectoryName("histograms");
  //analysisManager->SetNtupleDirectoryName("ntuple");
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);
    // Note: merging ntuples is available only with Root output

  // Book histograms, ntuple
  //

  // Creating histograms
  analysisManager->CreateH1("Eabs","Edep in tungsten", 100, 0., 15*GeV);
  analysisManager->CreateH1("Egap","Edep in scintillator", 100, 0., 3*GeV);
  analysisManager->CreateH1("Labs","trackL in tungsten", 100, 0., 8.5*m);
  analysisManager->CreateH1("Lgap","trackL in scintillator", 100, 0., 7.5*m);

  // Creating ntuple
  //

  analysisManager->CreateNtuple("B4_TotalData", "Edep and TrackL");
  analysisManager->CreateNtupleDColumn("Eabs");
  analysisManager->CreateNtupleDColumn("Egap");
  analysisManager->CreateNtupleDColumn("Labs");
  analysisManager->CreateNtupleDColumn("Lgap");
  analysisManager->CreateNtupleDColumn("beamE");
  analysisManager->FinishNtuple();

  analysisManager->CreateNtuple("B4_SpacialData", "Edep spacial distribution");
  analysisManager->CreateNtupleDColumn("Edep");
  analysisManager->CreateNtupleDColumn("X");
  analysisManager->CreateNtupleDColumn("Y");
  analysisManager->CreateNtupleDColumn("Z");
  analysisManager->FinishNtuple();

  analysisManager->CreateNtuple("B4_ScintillatorData", "Edep spacial distribution in scintillator");
  analysisManager->CreateNtupleDColumn("eventID");
  analysisManager->CreateNtupleDColumn("beamE");
  analysisManager->CreateNtupleDColumn("beamX");
  analysisManager->CreateNtupleDColumn("beamY");
  analysisManager->CreateNtupleDColumn("Edep");
  analysisManager->CreateNtupleDColumn("X");
  analysisManager->CreateNtupleDColumn("Y");
  analysisManager->CreateNtupleDColumn("Z");
  analysisManager->CreateNtupleDColumn("copyNumberX");
  analysisManager->CreateNtupleDColumn("copyNumberY");
  analysisManager->CreateNtupleDColumn("copyNumberZ");
  analysisManager->FinishNtuple();
  
  /*
  analysisManager->CreateNtuple("B4_ReadoutData", "Edep in each siPM for each event");
  analysisManager->CreateNtupleDColumn("eventID");
  analysisManager->CreateNtupleDColumn("beamE");
  analysisManager->CreateNtupleDColumn("beamX");
  analysisManager->CreateNtupleDColumn("beamY");
  for (G4double siPM_Number = 0.0; siPM_Number < 3600.0; siPM_Number++) {
    G4String columnName = std::to_string(siPM_Number);
    analysisManager->CreateNtupleDColumn(columnName);
  }
  analysisManager->FinishNtuple();
  */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* /*run*/)
{
  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);

  //G4cout << "I got here!!! BeginOfRunAction1" << G4endl;

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  G4String fileName = "/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/GEANT4/output/B4_latest1.root";
  // Other supported output types:
  // G4String fileName = "B4.csv";
  // G4String fileName = "B4.hdf5";
  // G4String fileName = "B4.xml";
  analysisManager->OpenFile(fileName);
  G4cout << "Using " << analysisManager->GetType() << G4endl;

  //G4cout << "I got here!!! BeginOfRunAction2" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* /*run*/)
{

  //G4cout << "I got here!!! EndOfRunAction1" << G4endl;
  // print histogram statistics
  //
  auto analysisManager = G4AnalysisManager::Instance();
  if ( analysisManager->GetH1(1) ) {
    G4cout << G4endl << " ----> print histograms statistic ";
    if(isMaster) {
      G4cout << "for the entire run " << G4endl << G4endl;
    }
    else {
      G4cout << "for the local thread " << G4endl << G4endl;
    }

    G4cout << " EAbs : mean = "
       << G4BestUnit(analysisManager->GetH1(0)->mean(), "Energy")
       << " rms = "
       << G4BestUnit(analysisManager->GetH1(0)->rms(),  "Energy") << G4endl;

    G4cout << " EGap : mean = "
       << G4BestUnit(analysisManager->GetH1(1)->mean(), "Energy")
       << " rms = "
       << G4BestUnit(analysisManager->GetH1(1)->rms(),  "Energy") << G4endl;

    G4cout << " LAbs : mean = "
      << G4BestUnit(analysisManager->GetH1(2)->mean(), "Length")
      << " rms = "
      << G4BestUnit(analysisManager->GetH1(2)->rms(),  "Length") << G4endl;

    G4cout << " LGap : mean = "
      << G4BestUnit(analysisManager->GetH1(3)->mean(), "Length")
      << " rms = "
      << G4BestUnit(analysisManager->GetH1(3)->rms(),  "Length") << G4endl;
  }

  // save histograms & ntuple
  //
  analysisManager->Write();
  analysisManager->CloseFile();

  //G4cout << "I got here!!! EndOfRunAction2" << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
