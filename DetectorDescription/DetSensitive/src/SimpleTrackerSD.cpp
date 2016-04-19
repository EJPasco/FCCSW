#include "SimpleTrackerSD.h"

// DD4hep
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

namespace det {
SimpleTrackerSD::SimpleTrackerSD(std::string aDetectorName,
  std::string aReadoutName,
  DD4hep::Geometry::Segmentation aSeg)
  :G4VSensitiveDetector(aDetectorName), m_seg(aSeg) {
  // add a name of the collection of hits
  collectionName.insert(aReadoutName);
  std::cout<<" Adding a collection with the name: "<<aReadoutName<<std::endl;
}

SimpleTrackerSD::~SimpleTrackerSD(){;}

void SimpleTrackerSD::Initialize(G4HCofThisEvent* aHitsCollections)
{
  // create a collection of hits and add it to G4HCofThisEvent
  // get id for collection
  static int HCID = -1;
  trackerCollection = new G4THitsCollection
    <DD4hep::Simulation::Geant4Hit>(SensitiveDetectorName,collectionName[0]);
  if(HCID<0)
    HCID = GetCollectionID(0);
  aHitsCollections->AddHitsCollection(HCID,trackerCollection);
}

G4bool SimpleTrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  // check if energy was deposited
  G4double edep = aStep->GetTotalEnergyDeposit();
  if(edep==0.)
    return false;

  // as in DD4hep::Simulation::Geant4GenericSD<Tracker>
  CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
  CLHEP::Hep3Vector postPos = aStep->GetPostStepPoint()->GetPosition();
  CLHEP::Hep3Vector medPos = 0.5*(prePos+postPos);
  DD4hep::Simulation::Position position(medPos.x(), medPos.y(), medPos.z());
  CLHEP::Hep3Vector direction = postPos - prePos;
  // create a hit and add it to collection
  const G4Track* track = aStep->GetTrack();
  DD4hep::Simulation::Geant4TrackerHit* hit = new  DD4hep::Simulation::Geant4TrackerHit(
    track->GetTrackID(), track->GetDefinition()->GetPDGEncoding(),edep, track->GetGlobalTime());
  if ( hit )  {
    hit->cellID  = getCellID(aStep);
    hit->energyDeposit = edep;
    hit->position = position;
    hit->momentum = direction;
    trackerCollection->insert(hit);
    return true;
  }
  return false;
}

uint64_t SimpleTrackerSD::getCellID(G4Step* aStep) {
  DD4hep::Simulation::Geant4VolumeManager volMgr =
    DD4hep::Simulation::Geant4Mapping::instance().volumeManager();
  DD4hep::Geometry::VolumeManager::VolumeID volID =
    volMgr.volumeID(aStep->GetPreStepPoint()->GetTouchable());
  if (m_seg.isValid() )  {
    G4ThreeVector global = 0.5 * (  aStep->GetPreStepPoint()->GetPosition()+
      aStep->GetPostStepPoint()->GetPosition());
    G4ThreeVector local  = aStep->GetPreStepPoint()->GetTouchable()->
      GetHistory()->GetTopTransform().TransformPoint(global);
    DD4hep::Simulation::Position loc(local.x()*MM_2_CM, local.y()*MM_2_CM, local.z()*MM_2_CM);
    DD4hep::Simulation::Position glob(global.x()*MM_2_CM, global.y()*MM_2_CM, global.z()*MM_2_CM);
    DD4hep::Geometry::VolumeManager::VolumeID cID = m_seg.cellID(loc,glob,volID);
    return cID;
  }
  return 0;
}
}
