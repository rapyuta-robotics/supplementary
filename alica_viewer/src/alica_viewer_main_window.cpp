#include "alica_viewer/alica_viewer_main_window.h"

#include "elastic_nodes/block.h"
#include "elastic_nodes/edge.h"
#include "elastic_nodes/node.h"

namespace alica
{

AlicaViewerMainWindow::AlicaViewerMainWindow(int argc, char* argv[], QWidget* parent)
    : QMainWindow(parent)
    , _scene(new QGraphicsScene(this))
    , _interfaceNode(argc, argv)
    , _alicaPlan(argc, argv)
    , _prevPlanId(0)
{
    _ui.setupUi(this);

    _scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    _zoom = new GraphicsViewZoom(_ui.graphicsView);
    _zoom->setModifiers(Qt::NoModifier);

    _ui.graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    _ui.graphicsView->setScene(_scene);
    _ui.graphicsView->fitInView(_scene->sceneRect());

    // fill combo box
    const AgentInfoMap& aiMap = _alicaPlan.getAgentInfoMap();
    for (const auto& agentInfo : aiMap) {
        _agentIdVector.push_back(agentInfo.first);
    }
    std::sort(_agentIdVector.begin(), _agentIdVector.end(), supplementary::AgentIDComparator());

    _ui.agentIdComboBox->addItem("Combined");
    _ui.agentIdComboBox->addItem("All");
    for (const supplementary::AgentID* agentId : _agentIdVector) {
        const AgentInfo* ai = _alicaPlan.getAgentInfo(agentId);
        if (ai) {
            _ui.agentIdComboBox->addItem(QString::fromStdString(ai->name));
        }
    }

    // Connect the signals and slots between interface to main window
    QObject::connect(&_interfaceNode, &AlicaViewerInterface::shutdown, this, &AlicaViewerMainWindow::close);
    QObject::connect(&_interfaceNode, &AlicaViewerInterface::alicaEngineInfoUpdate, this, &AlicaViewerMainWindow::alicaEngineInfoUpdate);
    QObject::connect(&_interfaceNode, &AlicaViewerInterface::alicaPlanInfoUpdate, this, &AlicaViewerMainWindow::alicaPlanInfoUpdate);
}

elastic_nodes::Node* AlicaViewerMainWindow::addStateToScene(const PlanTree* planTreeNode)
{
    if (planTreeNode->getX() == 0 && planTreeNode->getY() == 0) {
        _prevPlanId = 0;
        elastic_nodes::Block::count = -1;
    }

    AgentGrp robotIds;
    planTreeNode->getRobotsSorted(robotIds);
    std::string robotIdList = "[ ";
    for (const supplementary::AgentID* robotId : robotIds) {
        robotIdList += _alicaPlan.getAgentInfo(robotId)->name + std::string(", ");
    }
    robotIdList.erase(robotIdList.end() - 2, robotIdList.end());
    robotIdList += std::string(" ]");
    std::string stateName = planTreeNode->getState()->getName();
    std::string entrypointName = planTreeNode->getEntryPoint()->getTask()->getName();
    std::string planName = planTreeNode->getEntryPoint()->getPlan()->getName();
    elastic_nodes::Node* parentNode = new elastic_nodes::Node(stateName, entrypointName, planName, robotIdList);
    _scene->addItem(parentNode); // addItem() takes ownership of the arguement
    int x = planTreeNode->getX() * 300;
    int y = planTreeNode->getY() * 300;
    parentNode->setPos(x, y);

    for (const PlanTree* child : planTreeNode->getChildren()) {
        elastic_nodes::Node* childNode = addStateToScene(child);
        elastic_nodes::Edge::Direction direction;
        if (child->getX() != planTreeNode->getX()) {
            direction = elastic_nodes::Edge::Direction::RIGHT;
        } else {
            direction = elastic_nodes::Edge::Direction::DOWN;
        }
        _scene->addItem(new elastic_nodes::Edge(parentNode, childNode, direction)); // addItem() takes ownership of the arguement
    }

    // Draw a bounding rectangle for every plan
    // int64_t currentPlanId = planTreeNode->getEntryPoint()->getPlan()->getId();
    // if (_prevPlanId == 0) {
    //     _startBlockNode = parentNode;
    //     _prevPlanId = currentPlanId;
    // }
    // if (numOfChildren == 0) {
    //     _endBlockNode = parentNode;
    // }
    // if (_prevPlanId != currentPlanId) {
    //     elastic_nodes::Block* block = new elastic_nodes::Block(_startBlockNode, _endBlockNode);
    //     _scene->addItem(block);
    //     _startBlockNode = parentNode;
    //     _prevPlanId = currentPlanId;
    // }
    // _endBlockNode = parentNode;

    return parentNode;
}

void AlicaViewerMainWindow::updateNodes()
{
    _scene->clear();
    int indexSelected = _ui.agentIdComboBox->currentIndex();
    const PlanTreeMap& ptMap = _alicaPlan.getPlanTrees();
    if (indexSelected == 0) { // Combined
        const PlanTree* planTree = _alicaPlan.getCombinedPlanTree();
        for (PlanTree* child : planTree->getChildren()) {
            addStateToScene(child);
        }
    } else if (indexSelected == 1) { // All
        for (const auto& ptMapPair : ptMap) {
            addStateToScene(ptMapPair.second);
        }
    } else { // individual agents
        PlanTreeMap::const_iterator planTreeEntry = ptMap.find(_agentIdVector[indexSelected - 2]);
        if (planTreeEntry != ptMap.end()) {
            addStateToScene(planTreeEntry->second);
        }
    }
}

AlicaViewerMainWindow::~AlicaViewerMainWindow()
{
    delete _scene;
}

void AlicaViewerMainWindow::alicaEngineInfoUpdate(const AlicaEngineInfo& msg) {}

void AlicaViewerMainWindow::alicaPlanInfoUpdate(const PlanTreeInfo& msg)
{
    _alicaPlan.handlePlanTreeInfo(msg);
    updateNodes();
}

} // namespace alica