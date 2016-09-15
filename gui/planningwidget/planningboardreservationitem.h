#ifndef GUI_PLANNINGBOARDRESERVATIONITEM_H
#define GUI_PLANNINGBOARDRESERVATIONITEM_H

#include "gui/planningwidget/planningboardlayout.h"

#include "hotel/reservation.h"

#include <QGraphicsItem>

namespace gui
{
  namespace planningwidget
  {

    /**
     * @brief The PlanningBoardAtomItem class is the graphical equivalent to a ReservationAtom object
     *
     * It displays a single reserved room over a well defined date range. Multiple instances are grouped together in a
     * PlanningBoardReservationItem object.
     *
     * @see PlanningBoardWidget
     * @see PlanningBoardReservationItem
     */
    class PlanningBoardAtomItem : public QGraphicsRectItem
    {
    public:
      PlanningBoardAtomItem(const PlanningBoardLayout* layout, const hotel::ReservationAtom* atom,
                            QGraphicsItem* parent = nullptr);

      // QGraphicsRectItem interface
      virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

      void updateLayout();

    protected:
      //! @brief Reacts on selection changes and propagates them to the parent PlanningBoardReservationItem
      virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    private:
      QString getDisplayedText() const;
      QColor getItemColor() const;
      QColor getItemTextColor() const;

      const PlanningBoardLayout* _layout;
      const hotel::ReservationAtom* _atom;
    };

    /**
     * @brief The PlanningBoardReservationItem class is the graphical equivalent to a Reservation object
     *
     * It displays all of the reservation atoms. It also displays connecting lines between the reservations when they
     * are
     * selected, so that the user can visually follow change of rooms.
     *
     * @see PlanningBoardWidget
     * @see PlanningBoardAtomItem
     */
    class PlanningBoardReservationItem : public QGraphicsItem
    {
    public:
      PlanningBoardReservationItem(const PlanningBoardLayout* layout, const hotel::Reservation* reservation,
                                   QGraphicsItem* parent = nullptr);

      // QGraphicsItem interface
      virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
      virtual QRectF boundingRect() const override;

      /**
       * @brief setReservationSelected marks this item and all of its children as selected
       */
      void setReservationSelected(bool select);

      void updateLayout();

    private:
      const PlanningBoardLayout* _layout;
      const hotel::Reservation* _reservation;
      bool _isSelected;
      /**
       * @brief Set to true during a setReservationSelected() call to avoid reentrance and infinite recursion.
       *
       * This is necessary, since the individual atoms can trigger the selection of the whole reservation, and the
       * reservation can trigger the selection of all of the atoms.
       */
      bool _isUpdatingSelection;
    };

  } // namespace planningwidget
} // namespace gui

#endif // GUI_PLANNINGBOARDRESERVATIONITEM_H