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

    // Connect the signals and slots between interface to main window
    QObject::connect(&_interfaceNode, &AlicaViewerInterface::shutdown, this, &AlicaViewerMainWindow::close);
    QObject::connect(&_interfaceNode, &AlicaViewerInterface::alicaEngineInfoUpdate, this, &AlicaViewerMainWindow::alicaEngineInfoUpdate);
    QObject::connect(&_interfaceNode, &AlicaViewerInterface::alicaPlanInfoUpdate, this, &AlicaViewerMainWindow::alicaPlanInfoUpdate);
}

elastic_nodes::Node* AlicaViewerMainWindow::addStateToScene(const std::shared_ptr<SimplePlanTree>& spt, int x, int y)
{
    elastic_nodes::Node* parentNode =
        new elastic_nodes::Node(spt->getState()->getName(), spt->getEntryPoint()->getTask()->getName(), spt->getEntryPoint()->getPlan()->getName(), "");
    _scene->addItem(parentNode); // addItem() takes ownership of the arguement
    parentNode->setPos(x, y);
    y += 300;
    int numOfChildren = static_cast<int>(spt->getChildren().size());
    if (numOfChildren > 0) {
        x = (numOfChildren == 1) ? x : x - (300 / numOfChildren);
        for (const std::shared_ptr<SimplePlanTree>& child_spt : spt->getChildren()) {
            elastic_nodes::Node* childNode = addStateToScene(child_spt, x, y);
            x += (300 / numOfChildren);
            _scene->addItem(new elastic_nodes::Edge(parentNode, childNode)); // addItem() takes ownership of the arguement
        }
    }
    return parentNode;
}

elastic_nodes::Node* AlicaViewerMainWindow::addStateToScene(const std::shared_ptr<CombinedSimplePlanTree>& cspt, int x, int y)
{
    // std::cout << cspt->getSpt()->getState()->getName() << std::endl;
    // std::cout << cspt->getChildren().size() << " :: ";
    const AgentGrp& robotIds = cspt->getRobotsSorted();
    std::string robotIdList = "[ ";
    for (const supplementary::AgentID* robotId : robotIds) {
        robotIdList += _alicaPlan.getAgentInfo(robotId)->name + std::string(", ");
    }
    robotIdList.erase(robotIdList.end() - 2, robotIdList.end());
    robotIdList += std::string(" ]");
    std::cout << robotIdList << std::endl;
    elastic_nodes::Node* parentNode = new elastic_nodes::Node(cspt->getSpt()->getState()->getName(), cspt->getSpt()->getEntryPoint()->getTask()->getName(),
                                                              cspt->getSpt()->getEntryPoint()->getPlan()->getName(), robotIdList);
    _scene->addItem(parentNode); // addItem() takes ownership of the arguement
    parentNode->setPos(x, y);
    y += 300;
    int numOfChildren = static_cast<int>(cspt->getChildren().size());
    if (numOfChildren > 0) {
        x = (numOfChildren == 1) ? x : x - (300 / numOfChildren);
        for (const std::shared_ptr<CombinedSimplePlanTree>& child_spt : cspt->getChildren()) {
            elastic_nodes::Node* childNode = addStateToScene(child_spt, x, y);
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

    const CombinedSimplePlanTreeMap& csptMap = _alicaPlan.getCombinedSimplePlanTree();
    const CombinedSimplePlanTreeMap::const_iterator root = csptMap.find(_alicaPlan.getRootState());
    if (root != csptMap.end()) {
        for (const std::shared_ptr<CombinedSimplePlanTree>& cspt : root->second->getChildren()) {
            int y = 0;
            addStateToScene(cspt, x, y);
            x += 300;
        }
    } else {
        std::cout << "No robots available in state map." << std::endl;
    }

    // const SimplePlanTreeMap& sptMap = _alicaPlan.getSimplePlanTrees();
    // for (const auto& sptMapPair : sptMap) {
    //     const AgentInfo* agentInfo = _alicaPlan.getAgentInfo(sptMapPair.first);
    //     if (agentInfo != nullptr) {
    //         std::cout << agentInfo->name << " ";
    //     }
    //     std::cout << std::endl;
    //     addStateToScene(sptMapPair.second, x, 0);
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