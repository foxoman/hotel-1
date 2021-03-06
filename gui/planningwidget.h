#ifndef GUI_PLANNINGWIDGET_H
#define GUI_PLANNINGWIDGET_H

#include "gui/planningwidget/context.h"
#include "gui/planningwidget/datebarwidget.h"
#include "gui/planningwidget/planningboardlayout.h"
#include "gui/planningwidget/planningboardwidget.h"
#include "gui/planningwidget/roomlistwidget.h"

#include "persistence/datasource.h"
#include "persistence/sqlite/sqlitestorage.h"

#include "hotel/hotel.h"
#include "hotel/hotelcollection.h"
#include "hotel/planning.h"

#include <QObject>
#include <QScrollBar>
#include <QWidget>

#include <boost/date_time.hpp>
#include <boost/signals2.hpp>

#include <memory>
#include <vector>

namespace gui
{
  class PlanningWidget;

  template <class T>
  class CollectionObserverAdapter : public hotel::CollectionObserver<T>
  {
  public:
    // CollectionObserver<T> interface
    virtual void itemsAdded(const std::vector<T>& reservations) override { itemsAddedSignal(reservations); }
    virtual void itemsRemoved(const std::vector<T>& reservations) override { itemsRemovedSignal(reservations); }
    virtual void allItemsRemoved() override { allItemsRemovedSignal(); }

    // Public signals
    boost::signals2::signal<void(const std::vector<T>&)> itemsAddedSignal;
    boost::signals2::signal<void(const std::vector<T>&)> itemsRemovedSignal;
    boost::signals2::signal<void()> allItemsRemovedSignal;
  };

  /**
   * @brief The PlanningWidget class displays a list of reservations
   *
   * In a nutshell, the planning widget is composed of three main parts: A list of rooms, a date bar and a planning
   * board containing all of the reservations.
   */
  class PlanningWidget : public QWidget
  {
    Q_OBJECT
  public:
    PlanningWidget(persistence::DataSource& dataSource);

    void registerTool(const std::string& toolName, std::unique_ptr<gui::planningwidget::Tool> tool);
    void activateTool(const std::string& toolName);

  public slots:
    void setPivotDate(boost::gregorian::date pivotDate);

  signals:
    void pivotDateChanged(boost::gregorian::date pivotDate);

  protected:
    virtual void keyPressEvent(QKeyEvent* event) override;

  private:
    virtual void reservationsAdded(const std::vector<const hotel::Reservation*>& reservations);
    virtual void reservationsRemoved(const std::vector<const hotel::Reservation*>& reservations);
    virtual void allReservationsRemoved();
    virtual void hotelsAdded(const std::vector<const hotel::Hotel*>& hotels);
    virtual void hotelsRemoved(const std::vector<const hotel::Hotel*>& hotels);
    virtual void allHotelsRemoved();

    // Widgets
    QScrollBar* _verticalScrollbar;
    QScrollBar* _horizontalScrollbar;
    planningwidget::PlanningBoardWidget* _planningBoard;
    planningwidget::RoomListWidget* _roomList;
    planningwidget::DateBarWidget* _dateBar;

    // Shared widget state
    planningwidget::Context _context;

    CollectionObserverAdapter<const hotel::Reservation*> _planningObserver;
    CollectionObserverAdapter<const hotel::Hotel*> _hotelObserver;

    void updateDateRange();

    // Triggers a layout update of all the widgets
    void updateLayout();
  };

} // namespace gui

#endif // GUI_PLANNINGWIDGET_H
