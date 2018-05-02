#ifndef ALICA_VIEWER_ALICA_PLAN_PARSER_H
#define ALICA_VIEWER_ALICA_PLAN_PARSER_H

#include <SystemConfig.h>
#include <engine/PlanRepository.h>
#include <engine/SimplePlanTree.h>
#include <engine/Types.h>
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

typedef std::unordered_map<const supplementary::AgentID*, std::shared_ptr<SimplePlanTree>, supplementary::AgentIDHash, supplementary::AgentIDEqualsComparator>
    SimplePlanTreeMap;

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
typedef std::unordered_map<const supplementary::AgentID*, AgentInfo, supplementary::AgentIDHash, supplementary::AgentIDEqualsComparator> AgentInfoMap;

class CombinedSimplePlanTree
{
  public:
    CombinedSimplePlanTree(const std::shared_ptr<SimplePlanTree> spt)
        : _spt(spt)
    {
        addRobot(spt->getRobotId());
    }

    const std::unordered_set<std::shared_ptr<CombinedSimplePlanTree>>& getChildren() const { return _children; }

    std::unordered_set<std::shared_ptr<CombinedSimplePlanTree>>& editChildren() { return _children; }

    const std::shared_ptr<SimplePlanTree> getSpt() const { return _spt; }

    const AgentGrp& getRobotsSorted()
    {
        std::sort(_robotIds.begin(), _robotIds.end(), supplementary::AgentIDComparator());
        return _robotIds;
    }

    void addRobot(const supplementary::AgentID* robotId)
    {
        if (robotId) {
            _robotIds.push_back(robotId);
        }
    }

  private:
    const std::shared_ptr<SimplePlanTree> _spt;
    std::unordered_set<std::shared_ptr<CombinedSimplePlanTree>> _children;
    AgentGrp _robotIds;
};

struct StateHash
{
    std::size_t operator()(const State* const obj) const { return std::hash<int64_t>{}(obj->getId()); }
};

struct StateEqualsComparator
{
    bool operator()(const State* a, const State* b) const { return a->getId() == b->getId(); }
};

typedef std::unordered_map<const State*, std::shared_ptr<CombinedSimplePlanTree>, StateHash, StateEqualsComparator> CombinedSimplePlanTreeMap;

class AlicaPlan
{
  public:
    AlicaPlan(int argc, char* argv[])
        : _planParser(&_planRepository)
        , _agentIDManager(new supplementary::AgentIDFactory())
        , _rootState(new State(-1))
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

    ~AlicaPlan() { delete _rootState; }

    void addSptToCspt(std::shared_ptr<CombinedSimplePlanTree> parent, const std::shared_ptr<SimplePlanTree> spt)
    {
        const State* state = spt->getState();
        std::shared_ptr<CombinedSimplePlanTree> cur;
        CombinedSimplePlanTreeMap::iterator csptEntry = _combinedSimplePlanTree.find(state);
        if (csptEntry == _combinedSimplePlanTree.end()) {
            cur = std::make_shared<CombinedSimplePlanTree>(spt);
            _combinedSimplePlanTree.emplace(state, cur);
            parent->editChildren().insert(cur);
        } else {
            cur = csptEntry->second;
            cur->addRobot(spt->getRobotId());
        }
        for (const std::shared_ptr<SimplePlanTree>& child_spt : spt->getChildren()) {
            addSptToCspt(cur, child_spt);
        }
    }

    const CombinedSimplePlanTreeMap& getCombinedSimplePlanTree()
    {
        _combinedSimplePlanTree.clear();
        std::shared_ptr<CombinedSimplePlanTree> root = std::make_shared<CombinedSimplePlanTree>(std::make_shared<SimplePlanTree>());
        _combinedSimplePlanTree.emplace(_rootState, root);
        for (const auto& sptMapPair : _simplePlanTrees) {
            addSptToCspt(root, sptMapPair.second);
        }
        return _combinedSimplePlanTree;
    }

    void handlePlanTreeInfo(const PlanTreeInfo& incoming)
    {
        std::shared_ptr<SimplePlanTree> spt = sptFromMessage(incoming.senderID, incoming.stateIDs);
        if (spt != nullptr) {
            SimplePlanTreeMap::iterator sptEntry = _simplePlanTrees.find(incoming.senderID);
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

    const State* getRootState() const { return _rootState; }

    const SimplePlanTreeMap& getSimplePlanTrees() const { return _simplePlanTrees; }

    const AgentInfo* getAgentInfo(const supplementary::AgentID* agentID) const
    {
        AgentInfoMap::const_iterator agentInfoEntry = _agentInfos.find(agentID);
        if (agentInfoEntry != _agentInfos.end()) {
            return &agentInfoEntry->second;
        }
        return nullptr;
    }

  private:
    const State* _rootState;
    PlanRepository _planRepository;
    PlanParser _planParser;
    SimplePlanTreeMap _simplePlanTrees;
    CombinedSimplePlanTreeMap _combinedSimplePlanTree;
    AgentInfoMap _agentInfos;
    supplementary::AgentIDManager _agentIDManager;
};

} // namespace alica

#endif // ALICA_VIEWER_ALICA_PLAN_PARSER_H
