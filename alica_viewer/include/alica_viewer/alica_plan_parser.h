#ifndef ALICA_VIEWER_ALICA_PLAN_PARSER_H
#define ALICA_VIEWER_ALICA_PLAN_PARSER_H

#include <SystemConfig.h>
#include <engine/PlanRepository.h>
#include <engine/SimplePlanTree.h>
#include <engine/containers/PlanTreeInfo.h>
#include <engine/model/EntryPoint.h>
#include <engine/model/Plan.h>
#include <engine/model/State.h>
#include <engine/model/Task.h>
#include <engine/parser/PlanParser.h>
#include <supplementary/AgentID.h>
#include <supplementary/AgentIDManager.h>

namespace alica
{

typedef std::map<const supplementary::AgentID*, std::shared_ptr<SimplePlanTree>, supplementary::AgentIDComparator> SimplePlanTreeMap;

class AgentInfo
{
  public:
    AgentInfo() {}

    AgentInfo(int tmp_id, const std::string& tmp_name)
        : id(tmp_id)
        , name(tmp_name)
    {
    }

    int id;
    std::string name;
};
typedef std::map<const supplementary::AgentID*, AgentInfo, supplementary::AgentIDComparator> AgentInfoMap;

class AlicaPlan
{
  public:
    AlicaPlan(int argc, char* argv[])
        : _planParser(&_planRepository)
        , _agentIDManager(new supplementary::AgentIDFactory())
    {
        if (argc < 2) {
            std::cout << "Usage: Base -m [Masterplan] -rd [rolesetdir] -r [roleset]" << std::endl;
            exit(1);
        }

        std::string masterPlanName = "";
        std::string roleSetName = "";
        std::string roleSetDir = "";

        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "-m" || std::string(argv[i]) == "-masterplan") {
                masterPlanName = argv[i + 1];
                ++i;
            }

            if (std::string(argv[i]) == "-rd" || std::string(argv[i]) == "-rolesetdir") {
                roleSetDir = argv[i + 1];
                ++i;
            }
            if (std::string(argv[i]) == "-r" || std::string(argv[i]) == "-roleset") {
                roleSetName = argv[i + 1];
                ++i;
            }
        }

        if (masterPlanName.size() == 0 || roleSetDir.size() == 0) {
            std::cout << "Usage: Base -m [Masterplan] -rd [rolesetdir] -r [roleset]" << std::endl;
            exit(1);
        }
        std::cout << "Masterplan is: " << masterPlanName << std::endl;
        std::cout << "Rolset Directory is: " << roleSetDir << std::endl;
        std::cout << "Rolset is: " << roleSetName << std::endl;

        _planParser.parsePlanTree(masterPlanName);

        ///@todo remove this once AgentID becomes independent of Globals.conf
        std::shared_ptr<std::vector<std::string>> agentNames = (*supplementary::SystemConfig::getInstance())["Globals"]->getSections("Globals.Team", NULL);
        for (const std::string& agentName : *agentNames) {
            int id = (*supplementary::SystemConfig::getInstance())["Globals"]->tryGet<int>(-1, "Globals", "Team", agentName.c_str(), "ID", NULL);
            _agentInfos.emplace(_agentIDManager.getID(id), AgentInfo(id, agentName));
        }
    }

    void handlePlanTreeInfo(const PlanTreeInfo& incoming)
    {
        std::shared_ptr<SimplePlanTree> spt = sptFromMessage(incoming.senderID, incoming.stateIDs);
        if (spt != nullptr) {
            auto sptEntry = _simplePlanTrees.find(incoming.senderID);
            if (sptEntry != _simplePlanTrees.end()) {
                sptEntry->second = spt;
            } else {
                _simplePlanTrees.emplace(incoming.senderID, spt);
            }
        }
    }

    bool addState(std::shared_ptr<SimplePlanTree> spt, const State* state) const
    {
        if (state == nullptr) {
            std::cout << "Unknown state." << std::endl;
            return false;
        }

        const EntryPoint* entryPoint = nullptr;
        for (const EntryPoint* ep : state->getInPlan()->getEntryPoints()) {
            if (std::find(ep->getReachableStates().begin(), ep->getReachableStates().end(), state) != ep->getReachableStates().end()) {
                entryPoint = ep;
            }
        }

        if (entryPoint == nullptr) {
            std::cout << "Entrypoint unknown for state (" << state << ")." << std::endl;
            return false;
        }

        spt->setState(state);
        spt->setEntryPoint(entryPoint);
        return true;
    }

    /**
     * Constructs a SimplePlanTree from a received message
     * @param robotId The id of the other robot.
     * @param ids The list of long encoding another robot's plantree as received in a PlanTreeInfo message.
     * @return shared_ptr of a SimplePlanTree
     */
    std::shared_ptr<SimplePlanTree> sptFromMessage(const supplementary::AgentID* robotId, const std::list<int64_t>& ids)
    {
        if (ids.size() == 0) {
            std::cerr << "Empty state list for robot " << robotId << std::endl;
            return nullptr;
        }

        std::shared_ptr<SimplePlanTree> root = std::make_shared<SimplePlanTree>();
        root->setRobotId(robotId);
        root->setStateIds(ids);

        std::list<int64_t>::const_iterator iter = ids.begin();
        const PlanRepository::Accessor<State>& validStates = _planRepository.getStates();
        if (!addState(root, validStates.find(*iter))) {
            std::cout << "Unable to add State (" << *iter << ") received from " << robotId << std::endl;
            return nullptr;
        }

        if (ids.size() > 1) {
            ++iter;
            std::shared_ptr<SimplePlanTree> curParent;
            std::shared_ptr<SimplePlanTree> cur = root;
            for (; iter != ids.end(); ++iter) {
                if (*iter == -1) {
                    curParent = cur;
                    cur = nullptr;
                } else if (*iter == -2) {
                    cur = curParent;
                    if (cur == nullptr) {
                        std::cout << "Malformed SptMessage from " << robotId << std::endl;
                        return nullptr;
                    }
                } else {
                    cur = std::make_shared<SimplePlanTree>();
                    cur->setRobotId(robotId);
                    curParent->editChildren().insert(cur);
                    if (!addState(cur, validStates.find(*iter))) {
                        std::cout << "Unable to add State (" << *iter << ") received from " << robotId << std::endl;
                        return nullptr;
                    }
                }
            }
        }
        return root;
    }

    const SimplePlanTreeMap& getSimplePlanTrees() const { return _simplePlanTrees; }

    const AgentInfo* getAgentInfo(const supplementary::AgentID* agentID) const
    {
        auto agentInfoEntry = _agentInfos.find(agentID);
        if (agentInfoEntry != _agentInfos.end()) {
            return &agentInfoEntry->second;
        }
        return nullptr;
    }

  private:
    PlanRepository _planRepository;
    PlanParser _planParser;
    SimplePlanTreeMap _simplePlanTrees;
    AgentInfoMap _agentInfos;
    supplementary::AgentIDManager _agentIDManager;
};

} // namespace alica

#endif // ALICA_VIEWER_ALICA_PLAN_PARSER_H
