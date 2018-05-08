#include "alica_viewer/alica_viewer_main_window.h"

#include "elastic_nodes/edge.h"
#include "elastic_nodes/node.h"

namespace alica
{

AlicaViewerMainWindow::AlicaViewerMainWindow(int argc, char* argv[], QWidget* parent)
    : QMainWindow(parent)
    , _scene(new QGraphicsScene(this))
    , _interfaceNode(argc, argv)
    , _alicaPlan(argc, argv)
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
    _ui.agentIdComboBox->addItem("All");
    for (const auto& agentInfo : aiMap) {
        _ui.agentIdComboBox->addItem(QString::fromStdString(agentInfo.second.name));
    }

    // Connect the signals and slots between interface to main window
    QObject::connect(&_interfaceNode, &AlicaViewerInterface::shutdown, this, &AlicaViewerMainWindow::close);
    QObject::connect(&_interfaceNode, &AlicaViewerInterface::alicaEngineInfoUpdate, this, &AlicaViewerMainWindow::alicaEngineInfoUpdate);
    QObject::connect(&_interfaceNode, &AlicaViewerInterface::alicaPlanInfoUpdate, this, &AlicaViewerMainWindow::alicaPlanInfoUpdate);
}

elastic_nodes::Node* AlicaViewerMainWindow::addStateToScene(const PlanTree* planTreeNode, int x, int y)
{
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
    parentNode->setPos(x, y);
    y += 300;
    int numOfChildren = static_cast<int>(planTreeNode->getChildren().size());
    if (numOfChildren > 0) {
        x = (numOfChildren == 1) ? x : x - (300 / numOfChildren);
        for (const PlanTree* child : planTreeNode->getChildren()) {
            elastic_nodes::Node* childNode = addStateToScene(child, x, y);
            x += (300 / numOfChildren);
            _scene->addItem(new elastic_nodes::Edge(parentNode, childNode)); // addItem() takes ownership of the arguement
        }
    }
    return parentNode;
}

void AlicaViewerMainWindow::updateNodes()
{
    _scene->clear();
    int x = 0;

    const PlanTree* planTree = _alicaPlan.getCombinedPlanTree();
    for (PlanTree* child : planTree->getChildren()) {
        int y = 0;
        addStateToScene(child, x, y);
        x += 300;
    }

    // const PlanTreeMap& ptMap = _alicaPlan.getPlanTrees();
    // for (const auto& ptMapPair : ptMap) {
    //     const AgentInfo* agentInfo = _alicaPlan.getAgentInfo(ptMapPair.first);
    //     addStateToScene(ptMapPair.second, x, 0);
    //     x += 300;
    // }
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