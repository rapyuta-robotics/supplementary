#ifndef ALICA_VIEWER_ALICA_PLAN_PARSER_H
#define ALICA_VIEWER_ALICA_PLAN_PARSER_H

#include <SystemConfig.h>
#include <engine/PlanRepository.h>
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

class PlanTree
{
  public:
    static bool compare(const PlanTree* a, const PlanTree* b)
    {
        if (a == nullptr || b == nullptr) {
            return false;
        }
        if (a->getState() == nullptr || b->getState() == nullptr) {
            return false;
        }
        return a->getState() == b->getState();
    }

    PlanTree()
        : _parent(nullptr)
        , _state(nullptr)
        , _entryPoint(nullptr)
        , _x(0)
        , _y(0)
    {
    }

    explicit PlanTree(const PlanTree* other, PlanTree* parent)
        : _state(other->_state)
        , _entryPoint(other->_entryPoint)
        , _robotIds(other->_robotIds)
        , _x(other->_x)
        , _y(other->_y)
    {
        setParent(parent);
        for (PlanTree* child : other->_children) {
            addChildren(new PlanTree(child, this));
        }
    }

    ~PlanTree()
    {
        if (!_children.empty()) {
            for (PlanTree* child : _children) {
                delete child;
            }
        }
    }

    void setParent(PlanTree* parent)
    {
        _parent = parent;
        if (_parent != nullptr) {
            _x = parent->_x;
            _y = parent->_y;
            if (_entryPoint != nullptr && parent->_entryPoint != nullptr && _entryPoint->getPlan()->getId() == parent->_entryPoint->getPlan()->getId()) {
                _x++;
            } else {
                _y++;
                _x += static_cast<int>(parent->_children.size());
            }
        }
        if (_state)
            std::cout << _state->getName() << " ( " << _x << " " << _y << " )";
        if (parent->_state)
            std::cout << " ::Parent:: " << parent->_state->getName() << " ( " << parent->_x << " " << parent->_y << " )";
        std::cout << std::endl;
    }

    const std::vector<PlanTree*>& getChildren() const { return _children; }

    void addChildren(PlanTree* child)
    {
        if (child != nullptr) {
            _children.push_back(child);
        }
    }

    void getRobotsSorted(AgentGrp& robotIds) const
    {
        robotIds = _robotIds;
        std::sort(robotIds.begin(), robotIds.end(), supplementary::AgentIDComparator());
    }

    const AgentGrp& getRobots() const { return _robotIds; }

    void mergeRobots(const AgentGrp& robotIds)
    {
        // Here we assume that no duplicate robotID are present
        for (const supplementary::AgentID* robotId : robotIds) {
            addRobot(robotId);
        }
    }

    void addRobot(const supplementary::AgentID* robotId)
    {
        if (robotId != nullptr) {
            _robotIds.push_back(robotId);
        }
    }

    const PlanTree* getParent() const { return _parent; }

    const EntryPoint* getEntryPoint() const { return _entryPoint; }

    const State* getState() const { return _state; }

    bool setState(const State* state)
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
        _state = state;
        _entryPoint = entryPoint;
        return true;
    }

    void mergePlanTree(const PlanTree* src)
    {
        if (src != nullptr && src->getState() != nullptr) {
            if (!_children.empty()) {
                std::vector<PlanTree*>::iterator iter =
                    std::find_if(_children.begin(), _children.end(), [&](PlanTree* pt) { return PlanTree::compare(pt, src); });
                if (iter != _children.end()) {
                    (*iter)->mergeRobots(src->getRobots());
                    for (PlanTree* child : src->getChildren()) {
                        (*iter)->mergePlanTree(child);
                    }
                    return;
                }
            }
            addChildren(new PlanTree(src, this));
        }
    }

    int getX() const { return _x; }
    int getY() const { return _y; }

  private:
    PlanTree* _parent;
    int _x;
    int _y;
    std::vector<PlanTree*> _children;
    const State* _state;
    const EntryPoint* _entryPoint;
    AgentGrp _robotIds;
};

typedef std::unordered_map<const supplementary::AgentID*, PlanTree*, supplementary::AgentIDHash, supplementary::AgentIDEqualsComparator> PlanTreeMap;

class AlicaPlan
{
  public:
    AlicaPlan(int argc, char* argv[])
        : _planParser(&_planRepository)
        , _combinedPlanTree(nullptr)
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
                masterPlanName = argv[++i];
            }
            if (std::string(argv[i]) == "-rd" || std::string(argv[i]) == "-rolesetdir") {
                roleSetDir = argv[++i];
            }
            if (std::string(argv[i]) == "-r" || std::string(argv[i]) == "-roleset") {
                roleSetName = argv[++i];
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

    ~AlicaPlan()
    {
        if (_combinedPlanTree) {
            delete _combinedPlanTree;
        }
        for (const auto& ptMapPair : _planTrees) {
            delete ptMapPair.second;
        }
    }

    const PlanTree* getCombinedPlanTree()
    {
        if (_combinedPlanTree) {
            delete _combinedPlanTree;
        }
        _combinedPlanTree = new PlanTree();
        for (const auto& ptMapPair : _planTrees) {
            _combinedPlanTree->mergePlanTree(ptMapPair.second);
        }
        return _combinedPlanTree;
    }

    void handlePlanTreeInfo(const PlanTreeInfo& incoming)
    {
        PlanTreeMap::iterator ptEntry = _planTrees.find(incoming.senderID);
        if (ptEntry != _planTrees.end()) {
            return;
        }
        PlanTree* pt = planTreeFromMessage(incoming.senderID, incoming.stateIDs);
        if (pt != nullptr) {
            PlanTreeMap::iterator ptEntry = _planTrees.find(incoming.senderID);
            if (ptEntry != _planTrees.end()) {
                ptEntry->second = pt;
            } else {
                _planTrees.emplace(incoming.senderID, pt);
            }
        }
    }

    /**
     * Constructs a PlanTree from a received message
     * @param robotId The id of the robot.
     * @param ids The list of long encoding of a robot's plantree as received in a PlanTreeInfo message.
     * @return PlanTree*
     */
    PlanTree* planTreeFromMessage(const supplementary::AgentID* robotId, const std::list<int64_t>& ids)
    {
        if (ids.size() == 0) {
            std::cerr << "Empty state list for robot " << robotId << std::endl;
            return nullptr;
        }

        PlanTree* root = new PlanTree();
        root->addRobot(robotId);

        std::list<int64_t>::const_iterator iter = ids.begin();
        const PlanRepository::Accessor<State>& validStates = _planRepository.getStates();
        if (!root->setState(validStates.find(*iter))) {
            std::cout << "Unable to add State (" << *iter << ") received from " << robotId << std::endl;
            return nullptr;
        }

        if (ids.size() > 1) {
            ++iter;
            PlanTree* curParent;
            PlanTree* cur = root;
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
                    cur = new PlanTree();
                    cur->addRobot(robotId);
                    if (!cur->setState(validStates.find(*iter))) {
                        std::cout << "Unable to add State (" << *iter << ") received from " << robotId << std::endl;
                        return nullptr;
                    }
                    cur->setParent(curParent);
                    curParent->addChildren(cur);
                }
            }
        }
        return root;
    }

    const PlanTreeMap& getPlanTrees() const { return _planTrees; }

    const AgentInfoMap& getAgentInfoMap() const { return _agentInfos; }

    const AgentInfo* getAgentInfo(const supplementary::AgentID* agentID) const
    {
        AgentInfoMap::const_iterator agentInfoEntry = _agentInfos.find(agentID);
        if (agentInfoEntry != _agentInfos.end()) {
            return &agentInfoEntry->second;
        }
        return nullptr;
    }

  private:
    PlanRepository _planRepository;
    PlanParser _planParser;
    PlanTreeMap _planTrees;
    PlanTree* _combinedPlanTree;
    AgentInfoMap _agentInfos;
    supplementary::AgentIDManager _agentIDManager;
};

} // namespace alica

#endif // ALICA_VIEWER_ALICA_PLAN_PARSER_H
