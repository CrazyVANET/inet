//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "RSUManager.h"
#include "MobilityBase.h"
#include "error.h"

Define_Module(RSUManager);

RSUManager::RSUManager() {
    // TODO Auto-generated constructor stub

}

RSUManager::~RSUManager() {
    // TODO Auto-generated destructor stub
}

void RSUManager::initialize(int stage) {
    if(stage != 2){
        return;
    }
    nRandomRsu = par("nRandomRsu");
    namePrefix = par("rsuPrefix").stringValue();
    rsuInitialized = false;
    manager = TraCIScenarioManagerAccess().get();
    start = new cMessage("initialize");
    scheduleAt(manager->getFirstStepAt(),start);
    nTotalRSUsSignal=registerSignal("nTotalRSUsSignal");
}

void RSUManager::handleMessage(cMessage* msg) {
    if(msg == start){
        if(!rsuInitialized){
            if( nRandomRsu > 0){
                generateRandomRsus(nRandomRsu);
            }
            parseRsu();
            placeRsu();
            rsuInitialized = true;
        }
        delete start;
    }
    else{
        ASSERT(false);
    }
}


void RSUManager::parseRsu()
{
    cXMLElement* rsuPlacement = par("rsuPlacement").xmlValue();
    std::string rootTag = rsuPlacement->getTagName();
    ASSERT(rootTag == "poas");
    cXMLElementList rsusList = rsuPlacement->getElementsByTagName("poa");

    for (cXMLElementList::const_iterator i = rsusList.begin(); i != rsusList.end(); ++i) {
        cXMLElement* e = *i;
        std::string id;

        double x;
        double y;

        ASSERT(e->getAttribute("id"));
        id = e->getAttribute("id");
        ASSERT(e->getAttribute("x"));
        x = atof(e->getAttribute("x"));
        ASSERT(e->getAttribute("y"));
        y = atof(e->getAttribute("y"));

        Coord omnetLoc = manager->traci2omnet(TraCIScenarioManager::TraCICoord(x,y));

        this->rsusList.push_back({omnetLoc, id});
    }
}

void RSUManager::createRsu(Coord pos, std::string name)
{
    cModule *parent = getParentModule();
    std::string type = par("rsuModule");
    cModuleType* moduleType = cModuleType::get(type.c_str());
    if (!moduleType) error(0,1,"Module Type \"%s\" not found", type.c_str());
    cModule *mod = moduleType->create(name.c_str(), parent);

    mod->finalizeParameters();
    mod->buildInside();

    // pre-initialize BaseMobility
    for (cModule::SubmoduleIterator iter(mod); !iter.end(); iter++) {
        cModule* submod = iter();
        MobilityBase* mm = dynamic_cast<MobilityBase*>(submod);
        if (!mm) continue;
        mm->par("initialX") = pos.x;
        mm->par("initialY") = pos.y;
        mm->par("initialZ") = 0;
    }

    mod->callInitialize();
}

void RSUManager::generateRandomRsus(uint n)
{
    std::vector<Coord> netBounds = manager->getNetBounds();
    std::list<std::string> junctionList = manager->commandGetJunctionIds();
    std::list<Coord> junctionLocations;
    for(std::list<std::string>::iterator i = junctionList.begin(); i != junctionList.end(); i++){
        junctionLocations.push_back(manager->commandGetJunctionPosition(*i));
    }

    for(uint i=0; i <n ; i++){
        Coord desiredCoord;
        desiredCoord.x = uniform(netBounds.at(0).x, netBounds.at(1).x);
        desiredCoord.y = uniform(netBounds.at(0).y, netBounds.at(1).y);
        Coord finalCoord = *junctionLocations.begin();
        double minDistance = desiredCoord.distance(*junctionLocations.begin());
        for(std::list<Coord>::iterator iter= ++junctionLocations.begin(); iter != junctionLocations.end(); iter++){
            double distance = desiredCoord.distance(*iter);
            if( distance< minDistance){
                minDistance= distance;
                finalCoord = *iter;
            }
        }
        char name[20];
        snprintf(name, sizeof(name), "randomRsu%d", i);
        rsusList.push_back({finalCoord,name});
        randomRsusList.push_back({finalCoord,name});
    }
}

std::vector<std::pair<Coord, std::string> > RSUManager::getRandomRsusList() {
    return randomRsusList;
}

void RSUManager::placeRsu()
{
    emit(nTotalRSUsSignal, rsusList.size());
    for(uint i = 0; i < rsusList.size(); i++){
        std::string prefix = namePrefix;
        createRsu(rsusList.at(i).first, prefix.append(rsusList.at(i).second));
    }
}
