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

#ifndef RSUMANAGER_H_
#define RSUMANAGER_H_

#include <cmodule.h>
#include <TraCIScenarioManager.h>


class RSUManager: public cSimpleModule {
public:
    RSUManager();
    virtual ~RSUManager();
    std::vector<std::pair<Coord, std::string> > getRandomRsusList();
protected:
  virtual int numInitStages() const { return 3; }
  virtual void initialize(int stage);
  virtual void handleMessage(cMessage *msg);
private:
  std::vector<std::pair<Coord, std::string> > rsusList;
  std::vector<std::pair<Coord, std::string> > randomRsusList;
  TraCIScenarioManager *manager;
  uint nRandomRsu;
  std::string namePrefix;
  cMessage *start;
  bool rsuInitialized;
  simsignal_t nTotalRSUsSignal;
  void parseRsu();
  void createRsu(Coord pos, std::string name);
  void generateRandomRsus(uint n);
  void placeRsu();
};

#endif /* RSUMANAGER_H_ */
