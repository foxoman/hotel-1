#include "planningwidget.h"

#include <QGridLayout>
#include <QKeyEvent>

namespace gui
{
  PlanningWidget::PlanningWidget(persistence::DataSource& dataSource)
  {
    // Assign the data
    _context.setDataSource(&dataSource);

    // Initialize the layout object with the above data
    _context.setPivotDate(boost::gregorian::day_clock::local_day());
    _context.initializeLayout(planningwidget::PlanningBoardLayout::GroupedByHotel);

    updateDateRange();

    // Create UI
    auto grid = new QGridLayout;
    grid->setMargin(0);
    grid->setSpacing(0);

    _verticalScrollbar = new QScrollBar(Qt::Vertical);
    _horizontalScrollbar = new QScrollBar(Qt::Horizontal);
    _planningBoard = new planningwidget::PlanningBoardWidget(&_context);
    _roomList = new planningwidget::RoomListWidget(&_context);
    _dateBar = new planningwidget::DateBarWidget(&_context);

    // Wire up the scroll bars
    _planningBoard->setHorizontalScrollBar(_horizontalScrollbar);
    _planningBoard->setVerticalScrollBar(_verticalScrollbar);
    _roomList->setVerticalScrollBar(_verticalScrollbar);
    _dateBar->setHorizontalScrollBar(_horizontalScrollbar);

    // Wire up events
    connect(_dateBar, SIGNAL(dateClicked(boost::gregorian::date)), this, SLOT(setPivotDate(boost::gregorian::date)));
    _planningObserver.itemsAddedSignal.connect(boost::bind(&PlanningWidget::reservationsAdded, this, boost::placeholders::_1));
    _planningObserver.itemsRemovedSignal.connect(boost::bind(&PlanningWidget::reservationsRemoved, this, boost::placeholders::_1));
    _planningObserver.allItemsRemovedSignal.connect(boost::bind(&PlanningWidget::allReservationsRemoved, this));
    _hotelObserver.itemsAddedSignal.connect(boost::bind(&PlanningWidget::hotelsAdded, this, boost::placeholders::_1));
    _hotelObserver.itemsRemovedSignal.connect(boost::bind(&PlanningWidget::hotelsRemoved, this, boost::placeholders::_1));
    _hotelObserver.allItemsRemovedSignal.connect(boost::bind(&PlanningWidget::allHotelsRemoved, this));

    grid->addWidget(_dateBar, 0, 1);
    grid->addWidget(_roomList, 1, 0);
    grid->addWidget(_planningBoard, 1, 1);
    grid->addWidget(_verticalScrollbar, 1, 2);
    grid->addWidget(_horizontalScrollbar, 2, 1);
    setLayout(grid);

    dataSource.planning().addObserver(&_planningObserver);
    dataSource.hotels().addObserver(&_hotelObserver);
  }

  void PlanningWidget::registerTool(const std::string& toolName, std::unique_ptr<gui::planningwidget::Tool> tool)
  {
    _context.registerTool(toolName, std::move(tool));
  }

  void PlanningWidget::activateTool(const std::string& toolName) { _context.activateTool(toolName); }

  void PlanningWidget::setPivotDate(boost::gregorian::date pivotDate)
  {
    // TODO: This should not update the whole layout!
    _context.setPivotDate(pivotDate);
    updateLayout();
    emit pivotDateChanged(pivotDate);
  }

  void PlanningWidget::keyPressEvent(QKeyEvent* event)
  {
    if (event->key() == Qt::Key_F1)
      _context.initializeLayout(planningwidget::PlanningBoardLayout::GroupedByHotel);
    if (event->key() == Qt::Key_F2)
      _context.initializeLayout(planningwidget::PlanningBoardLayout::GroupedByRoomCategory);

    if (event->key() == Qt::Key_Delete)
    {
      persistence::op::Operations removals;
      for (auto reservation : _context.selectedReservations())
        removals.push_back(persistence::op::DeleteReservation{reservation->id()});
      _context.dataSource().queueOperations(std::move(removals));
    }

    if (event->key() == Qt::Key_F1 || event->key() == Qt::Key_F2)
      updateLayout();

    auto tool = _context.activeTool();
    if (tool)
      tool->keyPressEvent(event);
  }

  void PlanningWidget::reservationsAdded(const std::vector<const hotel::Reservation*>& reservations)
  {
    _planningBoard->addReservations(reservations);
    updateDateRange();
  }

  void PlanningWidget::reservationsRemoved(const std::vector<const hotel::Reservation*>& reservations)
  {
    _planningBoard->removeReservations(reservations);
  }

  void PlanningWidget::allReservationsRemoved() { _planningBoard->removeAllReservations(); }

  void PlanningWidget::hotelsAdded(const std::vector<const hotel::Hotel*>& hotels)
  {
    _roomList->clear();
    for (auto room : _context.hotelCollection().allRooms())
      _roomList->addRoomItem(room);
    _context.initializeLayout(planningwidget::PlanningBoardLayout::GroupedByHotel);
    updateLayout();
  }

  void PlanningWidget::hotelsRemoved(const std::vector<const hotel::Hotel*>& hotels)
  {

    _roomList->clear();
    for (auto room : _context.hotelCollection().allRooms())
      _roomList->addRoomItem(room);
    _context.initializeLayout(planningwidget::PlanningBoardLayout::GroupedByHotel);
    updateLayout();
  }

  void PlanningWidget::allHotelsRemoved()
  {
    _roomList->clear();
    updateLayout();
  }

  void PlanningWidget::updateDateRange()
  {
    auto pivotDate = _context.layout().pivotDate();
    auto planningExtent = boost::gregorian::date_period(pivotDate, pivotDate);
    planningExtent = _context.planning().getPlanningExtent();

    // Extend the date range so that it starts one week prior to the first reservation and extends for at least one year
    auto dateRange = planningExtent.merge(boost::gregorian::date_period(
        planningExtent.begin() + boost::gregorian::days(-7), planningExtent.begin() + boost::gregorian::days(365)));

    // Apply the scene size
    auto& layout = _context.layout();
    auto left = layout.getDatePositionX(dateRange.begin()) - 10;
    auto right = layout.getDatePositionX(dateRange.end()) + 10;
    layout.setSceneRect(QRectF(left, 0, right - left, layout.getHeight()));
  }

  void PlanningWidget::updateLayout()
  {
    _planningBoard->updateLayout();
    _roomList->updateLayout();
    _dateBar->updateLayout();

    auto tool = _context.activeTool();
    if (tool)
      tool->updateLayout();
  }

} // namespace gui
